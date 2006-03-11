//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OpenAL ソース管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/utils/Singleton.h"
#include "sound/ALSource.h"
#include "base/utils/Singleton.h"
#include "base/utils/RisaThread.h"
#include "base/event/TickCount.h"
#include "base/event/IdleEvent.h"
#include "base/exception/RisaException.h"

RISSE_DEFINE_SOURCE_ID(51552,26074,48813,19041,30653,39645,11297,33602);




//---------------------------------------------------------------------------
//! @brief		デコード用スレッド
//---------------------------------------------------------------------------
class tRisaWaveDecodeThread : public tRisaThread, depends_on<tRisaTickCount>
{
	tRisaALSource * Source; //!< このスレッドに関連づけられているソース
	tRisaThreadEvent Event; //!< スレッドをたたき起こすため/スレッドを眠らせるためのイベント
	tRisaCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

public:
	tRisaWaveDecodeThread();
	~tRisaWaveDecodeThread();

	void SetSource(tRisaALSource * source);

protected:
	void Execute(void);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaWaveDecodeThread::tRisaWaveDecodeThread()
{
	Source = NULL;
	Run(); // スレッドの実行を開始
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaWaveDecodeThread::~tRisaWaveDecodeThread()
{
	Terminate(); // スレッドの終了を伝える
	Event.Signal(); // スレッドをたたき起こす
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ソースを設定する
//! @param		source ソース
//---------------------------------------------------------------------------
void tRisaWaveDecodeThread::SetSource(tRisaALSource * source)
{
	volatile tRisaCriticalSection::tLocker cs_holder(CS);
	Source = source;
	Event.Signal(); // スレッドをたたき起こす
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スレッドのエントリーポイント
//---------------------------------------------------------------------------
void tRisaWaveDecodeThread::Execute(void)
{
	while(!ShouldTerminate())
	{
		if(!Source)
		{
			// まだソースが割り当てられていない
			Event.Wait(60*1000); // 適当な時間待つ
		}
		else
		{
			risse_uint64 start_tick = tRisaTickCount::instance()->Get();
			// ソースが割り当てられている
			{
				volatile tRisaCriticalSection::tLocker cs_holder(CS);
				Source->FillBuffer(); // デコードを行う
			}

			// 眠るべき時間を計算する
			// ここでは大まかに FillBuffer が tRisaALBuffer::STREAMING_CHECK_SLEEP_MS
			// 周期で実行されるように調整する。
			// 楽観的なアルゴリズムなので Sleep 精度は問題にはならない。
			risse_uint64 end_tick = tRisaTickCount::instance()->Get();
			int sleep_ms = 
				tRisaALBuffer::STREAMING_CHECK_SLEEP_MS -
					static_cast<int>(end_tick - start_tick);
			if(sleep_ms < 0) sleep_ms = 1; // 0 を指定すると無限に待ってしまうので
			if(static_cast<unsigned int>(sleep_ms ) > tRisaALBuffer::STREAMING_CHECK_SLEEP_MS)
				sleep_ms = tRisaALBuffer::STREAMING_CHECK_SLEEP_MS;
			Event.Wait(sleep_ms); // 眠る
		}
	}
}
//---------------------------------------------------------------------------


























//---------------------------------------------------------------------------
//! @brief		デコード用スレッドのプール
//---------------------------------------------------------------------------
class tRisaWaveDecodeThreadPool :
								public singleton_base<tRisaWaveDecodeThreadPool>,
								manual_start<tRisaWaveDecodeThreadPool>,
								protected tRisaCompactEventDestination
{
	std::vector<tRisaWaveDecodeThread *> FreeThreads; //!< 使用していないスレッドのリスト

public:
	tRisaWaveDecodeThreadPool();
	~tRisaWaveDecodeThreadPool();

protected:
	void OnCompact(tCompactLevel level);

private:
	tRisaWaveDecodeThread * Acquire();
	void Unacquire(tRisaWaveDecodeThread * buffer);

public:
	static tRisaWaveDecodeThread * Get(tRisaALSource * source);
	static void Free(tRisaWaveDecodeThread * buffer);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaWaveDecodeThreadPool::tRisaWaveDecodeThreadPool()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaWaveDecodeThreadPool::~tRisaWaveDecodeThreadPool()
{
	// FreeThreads を解放する
	for(std::vector<tRisaWaveDecodeThread*>::iterator i = FreeThreads.begin();
		i != FreeThreads.end(); i++)
		delete (*i);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンパクトイベント
//---------------------------------------------------------------------------
void tRisaWaveDecodeThreadPool::OnCompact(tCompactLevel level)
{
	if(level >= clSlowBeat)
	{
		// FreeThreads を解放する
		for(std::vector<tRisaWaveDecodeThread*>::iterator i = FreeThreads.begin();
			i != FreeThreads.end(); i++)
			delete (*i);
		FreeThreads.clear();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スレッドを一つ得る
//! @return		スレッド
//---------------------------------------------------------------------------
tRisaWaveDecodeThread * tRisaWaveDecodeThreadPool::Acquire()
{
	// Free が空か？
	if(FreeThreads.size() == 0)
	{
		// 新しくスレッドを作成して返す
		return new tRisaWaveDecodeThread();
	}
	else
	{
		// Free から一つスレッドをとってきて返す
		tRisaWaveDecodeThread * th = FreeThreads.back();
		FreeThreads.pop_back();
		return th;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スレッドを一つ返す
//! @param		thread スレッド
//---------------------------------------------------------------------------
void tRisaWaveDecodeThreadPool::Unacquire(tRisaWaveDecodeThread * thread)
{
	// FreeThreads にスレッドを入れる
	try
	{
		FreeThreads.push_back(thread);
	}
	catch(...)
	{
		delete thread;
		throw;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スレッドを一つ得る
//! @param		source ソース
//! @return		スレッド
//---------------------------------------------------------------------------
tRisaWaveDecodeThread * tRisaWaveDecodeThreadPool::Get(tRisaALSource * source)
{
	tRisaWaveDecodeThread * th;
	if(!alive())
		th = new tRisaWaveDecodeThread();
	else
		th = instance()->Acquire();
	th->SetSource(source);
	return th;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スレッドを一つ返す
//! @param		thread スレッド
//---------------------------------------------------------------------------
void tRisaWaveDecodeThreadPool::Free(tRisaWaveDecodeThread * thread)
{
	thread->SetSource(NULL); // source を null に

	if(!alive())
		delete thread;
	else
		instance()->Unacquire(thread);
}
//---------------------------------------------------------------------------



































//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		buffer		OpenAL バッファを管理する tRisaALBuffer インスタンス
//---------------------------------------------------------------------------
tRisaALSource::tRisaALSource(boost::shared_ptr<tRisaALBuffer> buffer) :
	Buffer(buffer)
{
	Init(buffer);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ(ほかのtRisaALSourceとバッファを共有する場合)
//! @param		ref		コピー元ソース
//---------------------------------------------------------------------------
tRisaALSource::tRisaALSource(const tRisaALSource * ref)
{
	// 他のソースと Buffer を共有したい場合に使う。
	// Buffer は 非Streaming バッファでなければならない。
	if(ref->Buffer->GetStreaming())
		eRisaException::Throw(RISSE_WS_TR("Shared buffer must be a non-streaming buffer"));

	Init(ref->Buffer);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaALSource::~tRisaALSource()
{
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		オブジェクトを初期化する
//! @param		buffer		OpenAL バッファを管理する tRisaALBuffer インスタンス
//---------------------------------------------------------------------------
void tRisaALSource::Init(boost::shared_ptr<tRisaALBuffer> buffer)
{
	// フィールドの初期化
	SourceAllocated = false;
	DecodeThread = NULL;

	// スレッドプールを作成
	tRisaWaveDecodeThreadPool::ensure();

	try
	{
		volatile tRisaOpenAL::tCriticalSectionHolder cs_holder;

		// ソースの生成
		alGenSources(1, &Source);
		tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("alGenSources"));
		SourceAllocated = true;

		alSourcei(Source, AL_LOOPING, AL_FALSE);
		tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("alSourcei(AL_LOOPING)"));

		// ストリーミングを行わない場合は、バッファをソースにアタッチ
		if(!Buffer->GetStreaming())
		{
			alSourcei(Source, AL_BUFFER, Buffer->GetBuffer());
			tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("alSourcei(AL_BUFFER)"));
		}
	}
	catch(...)
	{
		Clear();
		throw;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		バッファに関するオブジェクトの解放などのクリーンアップ処理
//---------------------------------------------------------------------------
void tRisaALSource::Clear()
{
	if(DecodeThread) tRisaWaveDecodeThreadPool::Free(DecodeThread), DecodeThread = NULL;

	volatile tRisaOpenAL::tCriticalSectionHolder cs_holder;

	if(SourceAllocated) alDeleteSources(1, &Source);
	tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("alDeleteSources"));
	SourceAllocated = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		バッファのデータを埋める
//---------------------------------------------------------------------------
void tRisaALSource::FillBuffer()
{
	if(Buffer->GetStreaming())
	{
		Buffer->QueueStream(Source);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		再生の開始
//---------------------------------------------------------------------------
void tRisaALSource::Play()
{
	if(Buffer->GetStreaming())
	{
		// 初期サンプルをいくつか queue する
		Buffer->PrepareStream(Source);

		// デコードスレッドを作成する
		if(!DecodeThread) DecodeThread = tRisaWaveDecodeThreadPool::Get(this);
		// デコードスレッドを作成するとその時点から
		// FillBuffer が呼ばれるようになる
	}

	// 再生を開始する
	{
		volatile tRisaOpenAL::tCriticalSectionHolder cs_holder;

		alSourcePlay(Source);
		tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("alSourcePlay"));
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		再生の停止
//---------------------------------------------------------------------------
void tRisaALSource::Stop()
{
	// デコードスレッドを削除する
	if(Buffer->GetStreaming())
	{
		if(DecodeThread) tRisaWaveDecodeThreadPool::Free(DecodeThread), DecodeThread = NULL;
		// デコードスレッドを削除した時点で
		// FillBuffer は呼ばれなくなる
	}

	// 再生を停止する
	{
		volatile tRisaOpenAL::tCriticalSectionHolder cs_holder;

		alSourceStop(Source);
		tRisaOpenAL::instance()->ThrowIfError(RISSE_WS("alSourcePlay"));
	}

	if(Buffer->GetStreaming())
	{
		// 全てのバッファを unqueueする
		Buffer->UnqueueAllBuffers(Source);
	}
}
//---------------------------------------------------------------------------
