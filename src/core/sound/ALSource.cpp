//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
#include "base/utils/Thread.h"
#include "base/event/TickCount.h"
#include "base/event/IdleEvent.h"
#include "base/exception/Exception.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(51552,26074,48813,19041,30653,39645,11297,33602);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		デコード用スレッド
//---------------------------------------------------------------------------
class tWaveDecodeThread : public tThread, protected depends_on<tTickCount>
{
	tALSource * Source; //!< このスレッドに関連づけられているソース
	tThreadEvent Event; //!< スレッドをたたき起こすため/スレッドを眠らせるためのイベント
	tCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

public:
	tWaveDecodeThread();
	~tWaveDecodeThread();

	void SetSource(tALSource * source);

protected:
	void Execute(void);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tWaveDecodeThread::tWaveDecodeThread()
{
	Source = NULL;
	Run(); // スレッドの実行を開始
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tWaveDecodeThread::~tWaveDecodeThread()
{
	Terminate(); // スレッドの終了を伝える
	Event.Signal(); // スレッドをたたき起こす
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ソースを設定する
//! @param		source ソース
//---------------------------------------------------------------------------
void tWaveDecodeThread::SetSource(tALSource * source)
{
	volatile tCriticalSection::tLocker cs_holder(CS);
	Source = source;
	Event.Signal(); // スレッドをたたき起こす
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スレッドのエントリーポイント
//---------------------------------------------------------------------------
void tWaveDecodeThread::Execute(void)
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
			risse_uint64 start_tick = depends_on<tTickCount>::locked_instance()->Get();
			// ソースが割り当てられている
			{
				volatile tCriticalSection::tLocker cs_holder(CS);
				Source->FillBuffer(); // デコードを行う
			}

			// 眠るべき時間を計算する
			// ここでは大まかに FillBuffer が tALBuffer::STREAMING_CHECK_SLEEP_MS
			// 周期で実行されるように調整する。
			// 楽観的なアルゴリズムなので Sleep 精度は問題にはならない。
			risse_uint64 end_tick = depends_on<tTickCount>::locked_instance()->Get();
			int sleep_ms = 
				tALBuffer::STREAMING_CHECK_SLEEP_MS -
					static_cast<int>(end_tick - start_tick);
			if(sleep_ms < 0) sleep_ms = 1; // 0 を指定すると無限に待ってしまうので
			if(static_cast<unsigned int>(sleep_ms ) > tALBuffer::STREAMING_CHECK_SLEEP_MS)
				sleep_ms = tALBuffer::STREAMING_CHECK_SLEEP_MS;
			Event.Wait(sleep_ms); // 眠る
		}
	}
}
//---------------------------------------------------------------------------


























//---------------------------------------------------------------------------
//! @brief		デコード用スレッドのプール
//---------------------------------------------------------------------------
class tWaveDecodeThreadPool :
								public singleton_base<tWaveDecodeThreadPool>,
								manual_start<tWaveDecodeThreadPool>,
								protected tCompactEventDestination
{
	gc_vector<tWaveDecodeThread *> FreeThreads; //!< 使用していないスレッドのリスト

public:
	tWaveDecodeThreadPool();
	~tWaveDecodeThreadPool();

protected:
	void OnCompact(tCompactLevel level);

private:
	tWaveDecodeThread * Acquire();
	void Unacquire(tWaveDecodeThread * buffer);

public:
	static tWaveDecodeThread * Get(tALSource * source);
	static void Free(tWaveDecodeThread * buffer);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tWaveDecodeThreadPool::tWaveDecodeThreadPool()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tWaveDecodeThreadPool::~tWaveDecodeThreadPool()
{
	// FreeThreads を解放する
	for(gc_vector<tWaveDecodeThread*>::iterator i = FreeThreads.begin();
		i != FreeThreads.end(); i++)
		delete (*i);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンパクトイベント
//---------------------------------------------------------------------------
void tWaveDecodeThreadPool::OnCompact(tCompactLevel level)
{
	if(level >= clSlowBeat)
	{
		// FreeThreads を解放する
		for(gc_vector<tWaveDecodeThread*>::iterator i = FreeThreads.begin();
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
tWaveDecodeThread * tWaveDecodeThreadPool::Acquire()
{
	// Free が空か？
	if(FreeThreads.size() == 0)
	{
		// 新しくスレッドを作成して返す
		return new tWaveDecodeThread();
	}
	else
	{
		// Free から一つスレッドをとってきて返す
		tWaveDecodeThread * th = FreeThreads.back();
		FreeThreads.pop_back();
		return th;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スレッドを一つ返す
//! @param		thread スレッド
//---------------------------------------------------------------------------
void tWaveDecodeThreadPool::Unacquire(tWaveDecodeThread * thread)
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
tWaveDecodeThread * tWaveDecodeThreadPool::Get(tALSource * source)
{
	tWaveDecodeThread * th;
	if(pointer r = instance())
		th = r->Acquire();
	else
		th = new tWaveDecodeThread();
	th->SetSource(source);
	return th;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スレッドを一つ返す
//! @param		thread スレッド
//---------------------------------------------------------------------------
void tWaveDecodeThreadPool::Free(tWaveDecodeThread * thread)
{
	thread->SetSource(NULL); // source を null に

	if(pointer r = instance())
		r->Unacquire(thread);
	else
		delete thread;
}
//---------------------------------------------------------------------------














/*
	tWaveWatchThread の定義はこのヘッダファイルにある
*/


//---------------------------------------------------------------------------
tWaveWatchThread::tWaveWatchThread()
{
	Run(); // スレッドの実行を開始
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tWaveWatchThread::~tWaveWatchThread()
{
	Terminate(); // スレッドの終了を伝える
	Event.Signal(); // スレッドをたたき起こす
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveWatchThread::RegisterSource(tALSource * source)
{
	volatile tCriticalSection::tLocker cs_holder(CS);
	if(Sources.size() == 0) Event.Signal(); // ながめに眠っていたソースをたたき起こす
	Sources.push_back(source);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveWatchThread::UnregisterSource(tALSource * source)
{
	volatile tCriticalSection::tLocker cs_holder(CS);
	gc_vector<tALSource*>::iterator i =
		std::find(Sources.begin(), Sources.end(), source);
	if(i != Sources.end()) Sources.erase(i);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveWatchThread::Execute(void)
{
	while(!ShouldTerminate())
	{
		long sleep_time;

		{
			volatile tCriticalSection::tLocker cs_holder(CS);
			if(Sources.size())
			{
				// すべてのソースの WatchCallback を呼び出す
				for(gc_vector<tALSource*>::iterator i = Sources.begin();
						i != Sources.end(); i++)
					(*i)->WatchCallback();
				sleep_time = 50; // 50ms 固定
			}
			else
			{
				sleep_time = 60 * 1000; // ソースが登録されていないので長めに眠る
			}
		}

		Event.Wait(sleep_time); // 適当な時間待つ
	}
}
//---------------------------------------------------------------------------





















//---------------------------------------------------------------------------
tALSource::tALSource(boost::shared_ptr<tALBuffer> buffer,
	boost::shared_ptr<tWaveLoopManager> loopmanager) :
	Buffer(buffer), LoopManager(loopmanager)
{
	Init(buffer);
	if(!Buffer->GetStreaming())
	{
		// バッファが非ストリーミングの場合は LoopManager は必要ないので解放する
		LoopManager.reset();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tALSource::tALSource(const tALSource * ref)
{
	// 他のソースと Buffer を共有したい場合に使う。
	// Buffer は 非Streaming バッファでなければならない。
	if(ref->Buffer->GetStreaming())
		eRisaException::Throw(RISSE_WS_TR("Shared buffer must be a non-streaming buffer"));

	Init(ref->Buffer);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tALSource::~tALSource()
{
	// TODO: ここでイベントをクリアする意味は？
//	depends_on<tEventSystem>::locked_instance()->CancelEvents(this); // pending のイベントを削除する
	Clear();
//	depends_on<tEventSystem>::locked_instance()->CancelEvents(this); // 念のため ...
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::Init(boost::shared_ptr<tALBuffer> buffer)
{
	// フィールドの初期化
	SourceAllocated = false;
	DecodeThread = NULL;
	Status = PrevStatus = ssStop;
	NeedRewind = false;

	// スレッドプールを作成
	tWaveDecodeThreadPool::ensure();

	try
	{
		volatile tCriticalSection::tLocker cs_holder(CS);

		{
			volatile tOpenAL::tCriticalSectionHolder cs_holder;

			// ソースの生成
			alGenSources(1, &Source);
			depends_on<tOpenAL>::locked_instance()->ThrowIfError(RISSE_WS("alGenSources"));
			SourceAllocated = true;

			alSourcei(Source, AL_LOOPING, AL_FALSE);
			depends_on<tOpenAL>::locked_instance()->ThrowIfError(RISSE_WS("alSourcei(AL_LOOPING)"));

			// ストリーミングを行わない場合は、バッファをソースにアタッチ
			if(!Buffer->GetStreaming())
			{
				alSourcei(Source, AL_BUFFER, Buffer->GetBuffer());
				depends_on<tOpenAL>::locked_instance()->ThrowIfError(RISSE_WS("alSourcei(AL_BUFFER)"));
			}
		}

		// Watch Thread に追加 (これ以降非同期にWatchCallbackが呼ばれるので注意)
		depends_on<tWaveWatchThread>::locked_instance()->RegisterSource(this);
	}
	catch(...)
	{
		Clear();
		throw;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::Clear()
{
	volatile tCriticalSection::tLocker cs_holder(CS);

	// 再生の停止
	Stop();

	// Watch Thread から登録解除
	depends_on<tWaveWatchThread>::locked_instance()->UnregisterSource(this);

	// デコードスレッドの削除
	if(DecodeThread) tWaveDecodeThreadPool::Free(DecodeThread), DecodeThread = NULL;

	// ソースの削除
	volatile tOpenAL::tCriticalSectionHolder al_cs_holder;
	if(SourceAllocated) alDeleteSources(1, &Source);
	depends_on<tOpenAL>::locked_instance()->ThrowIfError(RISSE_WS("alDeleteSources"));
	SourceAllocated = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::FillBuffer()
{
	// ここはクリティカルセクションでは保護しない！！！！！！！！！
	// (処理が長時間かかる場合があるので)
	// Buffer がアクセス不可の時にここが呼ばれないようにするのは
	// 他の箇所で保証すること

	if(Buffer->GetStreaming())
	{
		QueueBuffer();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::UnqueueAllBuffers()
{
	// ソースにキューされているバッファの数を得る
	ALint queued;
	alGetSourcei(Source, AL_BUFFERS_QUEUED, &queued);
	depends_on<tOpenAL>::locked_instance()->ThrowIfError(
		RISSE_WS("alGetSourcei(AL_BUFFERS_QUEUED) at tALSource::UnqueueAllBuffers"));

	if(queued > 0)
	{
		ALuint dummy_buffers[STREAMING_NUM_BUFFERS];

		// アンキューする
		alSourceUnqueueBuffers(Source, queued, dummy_buffers);
		depends_on<tOpenAL>::locked_instance()->ThrowIfError(
			RISSE_WS("alSourceUnqueueBuffers at tALSource::UnqueueAllBuffers"));
	}

	// バッファもすべてアンキュー
	Buffer->FreeAllBuffers();

	// セグメントキューもクリア
	SegmentQueues.clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::QueueBuffer()
{
	// アンキューできるバッファがあるかを調べる
	{
		ALint processed;

		{
			volatile tOpenAL::tCriticalSectionHolder cs_holder;
			alGetSourcei(Source, AL_BUFFERS_PROCESSED, &processed);
			depends_on<tOpenAL>::locked_instance()->ThrowIfError(
				RISSE_WS("alGetSourcei(AL_BUFFERS_PROCESSED)"));
		}

		if(processed >= 1)
		{
			// アンキューできるバッファがある
			// 一つアンキューする
			ALuint  buffer = 0;

			{
				volatile tOpenAL::tCriticalSectionHolder cs_holder;
				alSourceUnqueueBuffers(Source, 1, &buffer);
				depends_on<tOpenAL>::locked_instance()->ThrowIfError(
					RISSE_WS("alSourceUnqueueBuffers at tALSource::QueueStream"));
			}

			Buffer->PushFreeBuffer(buffer); // アンキューしたバッファを返す

			{
				volatile tCriticalSection::tLocker cs_holder(CS);
				SegmentQueues.pop_front(); // セグメントキューからも削除
			}
		}
	}

	// バッファに空きバッファがあるか
	if(Buffer->HasFreeBuffer())
	{
		// データが流し込まれたバッファを得る
		// TODO: segments と events のハンドリング
		tWaveSegmentQueue segmentqueue;
		ALuint  buffer = 0;
		bool filled = Buffer->PopFilledBuffer(buffer, segmentqueue);

		// バッファにデータを割り当て、キューする
		if(filled)
		{
			volatile tOpenAL::tCriticalSectionHolder cs_holder;
			alSourceQueueBuffers(Source, 1, &buffer);
			depends_on<tOpenAL>::locked_instance()->ThrowIfError(RISSE_WS("alSourceQueueBuffers"));
		}

		// セグメントキューに追加
		if(filled)
		{
			volatile tCriticalSection::tLocker cs_holder(CS);
			SegmentQueues.push_back(segmentqueue);
//			fprintf(stderr, "queue : ");
//			segmentqueue.Dump();
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::WatchCallback()
{
	volatile tCriticalSection::tLocker cs_holder(CS);

	if(Status == ssPlay)
	{
		ALint state;

		{
			volatile tOpenAL::tCriticalSectionHolder al_cs_holder;
			alGetSourcei(Source, AL_SOURCE_STATE, &state );
			depends_on<tOpenAL>::locked_instance()->ThrowIfError(RISSE_WS("alGetSourcei(AL_SOURCE_STATE)"));
		}

		if(state != AL_PLAYING)
		{
			// Playing が真を表しているにもかかわらず、OpenAL のソースは再生を
			// 停止している
			// 状態を修正
			Status = ssStop;
			// デコードスレッドも返す
			if(DecodeThread) tWaveDecodeThreadPool::Free(DecodeThread), DecodeThread = NULL;
			// イベントシステムにコールバックの発生を指示する
			depends_on<tEventSystem>::locked_instance()->PostEvent(
				new tEventInfo(eiStatusChanged, this, this));
			// 次回再生開始前に巻き戻しが必要
			NeedRewind = true;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::CallStatusChanged()
{
	if(PrevStatus != Status)
	{
		OnStatusChanged(Status);
		PrevStatus = Status;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::OnEvent(tEventInfo * info)
{
	volatile tCriticalSection::tLocker cs_holder(CS);

	switch(static_cast<tEventId>(info->GetId()))
	{
	case eiStatusChanged: // ステータスの変更を伝えるとき
		CallStatusChanged();
		break;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::Play()
{
	volatile tCriticalSection::tLocker cs_holder(CS);

	if(Status == ssPlay) return; // すでに再生している場合は再生をしない

	if(Buffer->GetStreaming())
	{
		// もし仮にデコードスレッドがあったとしても
		// ここで解放する
		if(DecodeThread) tWaveDecodeThreadPool::Free(DecodeThread), DecodeThread = NULL;

		// 巻き戻しを行う
		if(NeedRewind)
		{
			// すべての状況で巻き戻しが必要なわけではないので
			// NeedRewind が真の時にしかここでは巻き戻しを行わない
			NeedRewind = false;
			LoopManager->SetPosition(0); // 再生位置を最初に
			Buffer->GetFilter()->Reset(); // フィルタをリセット
		}

		// 初期サンプルをいくつか queue する
		for(risse_uint n = 0; n < STREAMING_PREPARE_BUFFERS; n++)
			QueueBuffer();

		// デコードスレッドを作成する
		if(!DecodeThread) DecodeThread = tWaveDecodeThreadPool::Get(this);
		// デコードスレッドを作成するとその時点から
		// FillBuffer が呼ばれるようになる
	}

	// 再生を開始する
	{
		volatile tOpenAL::tCriticalSectionHolder al_cs_holder;

		alSourcePlay(Source);
		depends_on<tOpenAL>::locked_instance()->ThrowIfError(RISSE_WS("alSourcePlay"));
	}

	// ステータスの変更を通知
	Status = ssPlay;
	CallStatusChanged();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::Stop()
{
	volatile tCriticalSection::tLocker cs_holder(CS);

	// デコードスレッドを削除する
	if(Buffer->GetStreaming())
	{
		if(DecodeThread) tWaveDecodeThreadPool::Free(DecodeThread), DecodeThread = NULL;
		// デコードスレッドを削除した時点で
		// FillBuffer は呼ばれなくなる
	}

	// 再生を停止する
	{
		volatile tOpenAL::tCriticalSectionHolder al_cs_holder;

		alSourceStop(Source);
		depends_on<tOpenAL>::locked_instance()->ThrowIfError(RISSE_WS("alSourceStop"));
	}

	// ステータスの変更を通知
	Status = ssStop;
	CallStatusChanged();

	// 全てのバッファを unqueueする
	if(Buffer->GetStreaming())
	{
		UnqueueAllBuffers();
	}

	// 次回再生開始前に巻き戻しが必要
	NeedRewind = true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::Pause()
{
	volatile tCriticalSection::tLocker cs_holder(CS);

	// 再生中の場合は
	if(Status == ssPlay)
	{
		// 再生を一時停止する
		{
			volatile tOpenAL::tCriticalSectionHolder al_cs_holder;

			alSourcePause(Source);
			depends_on<tOpenAL>::locked_instance()->ThrowIfError(RISSE_WS("alSourcePause"));
		}

		// ステータスの変更を通知
		Status = ssPause;
		CallStatusChanged();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tALSource::GetPosition()
{
	volatile tCriticalSection::tLocker cs_holder(CS);

	// 再生中や一時停止中でない場合は 0 を返す
	if(Status != ssPlay && Status != ssPause) return 0;
	if(SegmentQueues.size() == 0) return 0;

	// 再生位置を取得する
	ALint pos = 0;
	{
		volatile tOpenAL::tCriticalSectionHolder al_cs_holder;

		alGetSourcei(Source, AL_SAMPLE_OFFSET, &pos);
		depends_on<tOpenAL>::locked_instance()->ThrowIfError(RISSE_WS("alGetSourcei(AL_SAMPLE_OFFSET)"));
	}

	// 返された値は キューの先頭からの再生オフセットなので、該当する
	// キューを探す
	unsigned int unit = Buffer->GetOneBufferRenderUnit();
	unsigned int queue_index  = pos / unit;
	unsigned int queue_offset = pos % unit;

	// キューの範囲をはみ出していないか？
	if(queue_index >= SegmentQueues.size())
	{
		// はみ出している
		fprintf(stderr, "segment count ran out\n");
		return 0;
	}

//	fprintf(stderr, "get position queue in offset %d at queue index %d : ", queue_offset, queue_index);
//	SegmentQueues[queue_index].Dump();

	// 得られたのはフィルタ後の位置なのでフィルタ前のデコード位置に変換してから返す
	return SegmentQueues[queue_index].FilteredPositionToDecodePosition(queue_offset);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::SetPosition(risse_uint64 pos)
{
	volatile tCriticalSection::tLocker cs_holder(CS);

	if(Buffer->GetStreaming())
	{
		if(Status == ssPlay || Status == ssPause)
		{
			// デコードスレッドをいったん削除する
			if(DecodeThread) tWaveDecodeThreadPool::Free(DecodeThread), DecodeThread = NULL;
					// デコードスレッドを削除した時点で
					// FillBuffer は呼ばれなくなる

			// 再生を停止する
			{
				volatile tOpenAL::tCriticalSectionHolder al_cs_holder;

				alSourceStop(Source);
				depends_on<tOpenAL>::locked_instance()->ThrowIfError(RISSE_WS("alSourceStop"));
			}
		}

		// 全てのバッファを unqueueする
		UnqueueAllBuffers();

		// 再生位置の変更を行う
		NeedRewind = false;
		LoopManager->SetPosition(pos); // 再生位置を最初に
		Buffer->GetFilter()->Reset(); // フィルタをリセット

		if(Status == ssPlay || Status == ssPause)
		{
			// 初期サンプルをいくつか queue する
			for(risse_uint n = 0; n < STREAMING_PREPARE_BUFFERS; n++)
				QueueBuffer();

			// デコードスレッドを作成する
			if(!DecodeThread) DecodeThread = tWaveDecodeThreadPool::Get(this);
			// デコードスレッドを作成するとその時点から
			// FillBuffer が呼ばれるようになる
		}

		if(Status == ssPlay)
		{
			// 再生を開始する
			{
				volatile tOpenAL::tCriticalSectionHolder al_cs_holder;

				alSourcePlay(Source);
				depends_on<tOpenAL>::locked_instance()->ThrowIfError(RISSE_WS("alSourcePlay"));
			}
		}
	}
	else
	{
		// TODO: 非ストリーミング時の動作
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa

