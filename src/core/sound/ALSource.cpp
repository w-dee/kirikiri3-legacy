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
#include "base/utils/RisaThread.h"
#include "base/event/TickCount.h"
#include "base/event/IdleEvent.h"
#include "base/utils/PointerList.h"
#include "sound/Sound.h"
#include "base/log/Log.h"

namespace Risa {
RISSE_DEFINE_SOURCE_ID(51552,26074,48813,19041,30653,39645,11297,33602);
//---------------------------------------------------------------------------

/*
	TODO: デコードスレッドが比較的低いプライオリティで走ってるときは
	なかなかデコードスレッドがクリティカルセクションから抜けてくれないことがある。
	それへの対処。
	PhaseVocoder などのパラメータ設定も同様の問題があるので、デコードスレッドに
	ロック区間から速く抜けて欲しいときのための機構を用意するべき。
*/



//---------------------------------------------------------------------------
//! @brief		デコード用スレッド
//! @note		サウンドソースが再生中の間は、tWaveDecodeThreadPool -> tWaveDecodeThread -> Source
//!				の参照があるため、Source が GC により回収されることはない。
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
	tALSource * GetSource() { volatile tCriticalSection::tLocker cs_holder(CS);  return Source; }

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
		int sleep_ms  = 0;
		tALSource * source;

		{ // entering critical section
			volatile tCriticalSection::tLocker cs_holder(CS);
			source = Source;
		}

		{
			if(!source)
			{
				// まだソースが割り当てられていない
				sleep_ms = 60*1000; // 適当な時間待つ
			}
			else
			{
				// ソースが割り当てられている
				risse_uint64 start_tick = tTickCount::instance()->Get();

				bool buffer_repletion = source->Render();
					// デコードを行う(このメソッドは、本来呼び出されるべきで
					// ない場合に呼び出されても単に無視するはず)

				// 眠るべき時間を計算する
				// ここでは大まかに FillBuffer が tALBuffer::STREAMING_CHECK_SLEEP_MS
				// 周期で実行されるように調整する。バッファの残りが少ない場合は
				// すぐに次のバッファを埋める。
				// 楽観的なアルゴリズムなので Sleep 精度は問題にはならない。
				risse_uint64 end_tick = tTickCount::instance()->Get();
				sleep_ms = 
					buffer_repletion ? 
						(tALBuffer::STREAMING_CHECK_SLEEP_MS -
							static_cast<int>(end_tick - start_tick))
						: 1;
				if(sleep_ms < 0) sleep_ms = 1; // 0 を指定すると無限に待ってしまうので
				if(static_cast<unsigned int>(sleep_ms ) > tALBuffer::STREAMING_CHECK_SLEEP_MS)
					sleep_ms = tALBuffer::STREAMING_CHECK_SLEEP_MS;
			}
		} // end of critical section

		Event.Wait(sleep_ms); // 眠る
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
	tCriticalSection CS;

	gc_vector<tWaveDecodeThread *> FreeThreads; //!< 使用していないスレッドのリスト
	pointer_list<tWaveDecodeThread> UsingThreads; //!< 貸し出し中のスレッドのリスト

public:
	tWaveDecodeThreadPool();
	~tWaveDecodeThreadPool();

protected:
	void OnCompact(tCompactLevel level);

public:
	tWaveDecodeThread * Acquire(tALSource * source);
	void Unacquire(tWaveDecodeThread * buffer);
	bool CallWatchCallbacks();
	risse_int32 FireLabelEvents();
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
	volatile tCriticalSection::tLocker cs_holder(CS);

	// FreeThreads を解放する
	for(gc_vector<tWaveDecodeThread*>::iterator i = FreeThreads.begin();
		i != FreeThreads.end(); i++)
		delete (*i);

	// すべての UsingThreads の Source を stop する

	volatile pointer_list<tWaveDecodeThread>::scoped_lock lock(UsingThreads);
	size_t count = UsingThreads.get_locked_count();
	for(size_t i = 0; i < count; i++)
	{
		tWaveDecodeThread * th = UsingThreads.get_locked(i);
		if(th) th->GetSource()->Stop(false);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンパクトイベント
//---------------------------------------------------------------------------
void tWaveDecodeThreadPool::OnCompact(tCompactLevel level)
{
	volatile tCriticalSection::tLocker cs_holder(CS);

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
tWaveDecodeThread * tWaveDecodeThreadPool::Acquire(tALSource * source)
{
	volatile tCriticalSection::tLocker cs_holder(CS);

	// Free が空か？
	tWaveDecodeThread * newthread;
	if(FreeThreads.size() == 0)
	{
		// 新しくスレッドを作成して返す
		newthread = new tWaveDecodeThread();
	}
	else
	{
		// Free から一つスレッドをとってきて返す
		tWaveDecodeThread * th = FreeThreads.back();
		FreeThreads.pop_back();
		newthread = th;
	}

	// UsingThreads に追加
	UsingThreads.add(newthread);

	// tWaveWatchThread をたたき起こす
	tWaveWatchThread::instance()->Wakeup();

	// newthread を帰す
	newthread->SetSource(source);
	return newthread;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スレッドを一つ返す
//! @param		thread スレッド
//---------------------------------------------------------------------------
void tWaveDecodeThreadPool::Unacquire(tWaveDecodeThread * thread)
{
	volatile tCriticalSection::tLocker cs_holder(CS);

	thread->SetSource(NULL); // source を null に

	// UsingThreads からスレッドを削除
	UsingThreads.remove(thread);

	// FreeThreads にスレッドを入れる
	FreeThreads.push_back(thread);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		使用中の各スレッドの WatchCallback を呼び出す
//! @param		一つでも呼び出す物があった場合に true
//---------------------------------------------------------------------------
bool tWaveDecodeThreadPool::CallWatchCallbacks()
{
	// ここで使用している pointer_list は自分でスレッド保護を行うので
	// 明示的なロックはここでは必要ない。

	volatile pointer_list<tWaveDecodeThread>::scoped_lock lock(UsingThreads);
	size_t count = UsingThreads.get_locked_count();
	for(size_t i = 0; i < count; i++)
	{
		tWaveDecodeThread * th = UsingThreads.get_locked(i);
		if(th)
		{
			tALSource * source = th->GetSource();
			if(source) source->WatchCallback();
			// この中で Unacquire が呼ばれる可能性があるので注意
			// (pointer_list はそういう状況でもうまく扱うことができるので問題ない)
		}
	}
	return count != 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_int32 tWaveDecodeThreadPool::FireLabelEvents()
{
	// ここで使用している pointer_list は自分でスレッド保護を行うので
	// 明示的なロックはここでは必要ない。
	risse_int32 nearest_next = -1;
	volatile pointer_list<tWaveDecodeThread>::scoped_lock lock(UsingThreads);
	size_t count = UsingThreads.get_locked_count();
	for(size_t i = 0; i < count; i++)
	{
		tWaveDecodeThread * th = UsingThreads.get_locked(i);
		if(th)
		{
			tALSource * source = th->GetSource();
			if(source)
			{
				risse_int32 next = source->FireLabelEvents();
				// この中で Unacquire が呼ばれる可能性があるので注意
				// (pointer_list はそういう状況でもうまく扱うことができるので問題ない)
				if(next != -1)
				{
					if(nearest_next == -1 || nearest_next > next) nearest_next = next;
				}
			}
		}
	}

	return nearest_next;
}
//---------------------------------------------------------------------------









/*
	tWaveWatchThread の定義はこのヘッダファイルにある
*/


//---------------------------------------------------------------------------
tWaveWatchThread::tWaveWatchThread()
{
	tWaveDecodeThreadPool::ensure(); // tWaveDecodeThreadPool を作っておく
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
void tWaveWatchThread::Wakeup()
{
	Event.Signal(); // スレッドをたたき起こす
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWaveWatchThread::Execute(void)
{
	while(!ShouldTerminate())
	{
		long sleep_time;

		if(tWaveDecodeThreadPool::instance()->CallWatchCallbacks())
			sleep_time = 50; // 50ms 固定
		else
			sleep_time = 60 * 1000; // ソースが登録されていないので長めに眠る

		Event.Wait(sleep_time); // 適当な時間待つ
	}
}
//---------------------------------------------------------------------------




















//---------------------------------------------------------------------------
//! @brief		ラベルイベントタイミング発生用スレッド
//---------------------------------------------------------------------------
class tWaveLabelTimingThread : public tThread,
			public singleton_base<tWaveLabelTimingThread>,
			manual_start<tWaveLabelTimingThread>,
			protected depends_on<tTickCount>
{
	tThreadEvent Event; //!< スレッドをたたき起こすため/スレッドを眠らせるためのイベント
	tCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

public:
	tWaveLabelTimingThread();
	~tWaveLabelTimingThread();

	void Reschedule();

protected:
	void Execute(void);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tWaveLabelTimingThread::tWaveLabelTimingThread()
{
	Run(); // スレッドの実行を開始
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tWaveLabelTimingThread::~tWaveLabelTimingThread()
{
	Terminate(); // スレッドの終了を伝える
	Event.Signal(); // スレッドをたたき起こす
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		リスケジュールを行う
//---------------------------------------------------------------------------
void tWaveLabelTimingThread::Reschedule()
{
	Event.Signal(); // スレッドをたたき起こす
	// スレッドをたたき起こすとすぐにリスケジュールが行われる
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スレッドのエントリーポイント
//---------------------------------------------------------------------------
void tWaveLabelTimingThread::Execute(void)
{
	while(!ShouldTerminate())
	{
		risse_int32 sleep_time = tWaveDecodeThreadPool::instance()->FireLabelEvents();
		if(sleep_time < 0) sleep_time = 0x100000; // 次のイベントがみつからなかったので適当な時間を待つ
		else if(sleep_time == 0) sleep_time = 1;
		Event.Wait(sleep_time); // 次のラベルイベントまでまつ
	}
}
//---------------------------------------------------------------------------


































//---------------------------------------------------------------------------
tALSource::tInternalSource::tInternalSource()
{
	volatile tOpenAL::tCriticalSectionHolder cs_holder;

	// ソースの生成
	alGenSources(1, &Source);
	tOpenAL::instance()->ThrowIfError(RISSE_WS("alGenSources"));

	alSourcei(Source, AL_LOOPING, AL_FALSE);
	tOpenAL::instance()->ThrowIfError(RISSE_WS("alSourcei(AL_LOOPING)"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tALSource::tInternalSource::~tInternalSource()
{
	volatile tOpenAL::tCriticalSectionHolder al_cs_holder;
	alDeleteSources(1, &Source);
	tOpenAL::instance()->ThrowIfError(RISSE_WS("alDeleteSources"));
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tALSource::tALSource(tALBuffer * buffer,
	tWaveLoopManager * loopmanager) :
	CS(new tCriticalSection()), Buffer(buffer), LoopManager(loopmanager)
{
	Init(buffer);
	if(!Buffer->GetStreaming())
	{
		// バッファが非ストリーミングの場合は LoopManager は必要ないので解放する
		LoopManager = NULL; // TODO: real .Dispose();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tALSource::tALSource(const tALSource * ref) :
	CS(new tCriticalSection())
{
	// 他のソースと Buffer を共有したい場合に使う。
	// Buffer は 非Streaming バッファでなければならない。
	if(ref->Buffer->GetStreaming())
		tSoundExceptionClass::Throw(RISSE_WS_TR("Shared buffer must be a non-streaming buffer"));

	Init(ref->Buffer);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::Init(tALBuffer * buffer)
{
	// フィールドの初期化
	DecodeThread = NULL;
	Status = PrevStatus = ssStop;
	NeedRewind = false;
	DecodePosition = 0;
	Buffer = buffer; // 一応再設定

	// スレッドプールを作成
	tWaveDecodeThreadPool::ensure();

	// ラベルイベントタイミング発生用スレッドを作成
//	tWaveLabelTimingThread::ensure();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::EnsureSource()
{
	if(Source) return; // ソースがすでに存在する場合はなにもしない

	// ソースの生成
	int retry_count = 3;
	while(retry_count--)
	{
		try
		{
			Source = new tInternalSource();
		}
		catch(...)
		{
			if(retry_count == 0) throw; // リトライ失敗
			CollectGarbage(); // GC
		}
	}

	// ストリーミングを行わない場合は、バッファをソースにアタッチ
	if(!Buffer->GetStreaming())
	{
		volatile tOpenAL::tCriticalSectionHolder cs_holder;
		alSourcei(Source->Source, AL_BUFFER, Buffer->GetBuffer());
		tOpenAL::instance()->ThrowIfError(RISSE_WS("alSourcei(AL_BUFFER)"));
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::DeleteSource()
{
	if(Source) delete Source, Source = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tALSource::Render()
{
	// ロックは行わない
	// このメソッドは、比較的プライオリティの低いスレッドから呼ばれる可能性がある
	Buffer->FillRenderBuffer(); // 先行してデコードを進める

	if(Buffer->GetRenderBufferRemain() < (long)tALBuffer::STREAMING_BUFFER_HZ )
	{
		// 約一秒を切った場合は偽を返す
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::FillBuffer()
{
	// このメソッドは、比較的プライオリティの高いスレッドから呼ばれる可能性がある

	volatile tCriticalSection::tLocker cs_holder(*CS);

	RecheckStatus();

	if(!DecodeThread) return ; // デコードスレッドが使用不可なので呼び出さない
	if(!Source) return ; // デコードスレッドが使用不可なので呼び出さない

	if(Buffer->GetStreaming())
	{
		QueueBuffer();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::UnqueueAllBuffers()
{
	volatile tCriticalSection::tLocker cs_holder(*CS);

	RISSE_ASSERT(Buffer->GetStreaming());

	// ソースの削除と再生成を行う
	DeleteSource();

/*
	下記のようにバッファの内容をいったんすべて alSourceUnqueueBuffers を使って
	アンキューする方法は、なぜか alGetSourcei(AL_BUFFERS_QUEUED) が 1 を返し、
	alSourceUnqueueBuffers でそのバッファをアンキューしようとしてもエラーに
	なる事がある。なので、上記のようにいったんソースを削除し、作成し直す
	事にする。
*/

/*
	// ソースにキューされているバッファの数を得る
	ALint queued;
	alGetSourcei(Source->Source, AL_BUFFERS_QUEUED, &queued);
	tOpenAL::instance()->ThrowIfError(
		RISSE_WS("alGetSourcei(AL_BUFFERS_QUEUED) at tALSource::UnqueueAllBuffers"));

	if(queued > 0)
	{
		ALuint dummy_buffers[STREAMING_NUM_BUFFERS];

		// アンキューする
		alSourceUnqueueBuffers(Source->Source, queued, dummy_buffers);
		tOpenAL::instance()->ThrowIfError(
			RISSE_WS("alSourceUnqueueBuffers at tALSource::UnqueueAllBuffers"));
	}
*/
	// バッファもすべてアンキュー
	Buffer->FreeAllBuffers();

	// セグメントキューもクリア
	SegmentQueues.clear();
	SegmentEvents.clear();
	DecodePosition = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::QueueBuffer()
{
	// アンキューできるバッファがあるかを調べる
	if(!Source) return; // ソースが無い場合はなにもできない

	{
		ALint processed;

		{
			volatile tOpenAL::tCriticalSectionHolder cs_holder;
			alGetSourcei(Source->Source, AL_BUFFERS_PROCESSED, &processed);
			tOpenAL::instance()->ThrowIfError(
				RISSE_WS("alGetSourcei(AL_BUFFERS_PROCESSED)"));
		}

		if(processed >= 1)
		{
			// アンキューできるバッファがある
			// 一つアンキューする
			ALuint  buffer = 0;

			{
				volatile tOpenAL::tCriticalSectionHolder cs_holder;
				alSourceUnqueueBuffers(Source->Source, 1, &buffer);
				tOpenAL::instance()->ThrowIfError(
					RISSE_WS("alSourceUnqueueBuffers at tALSource::QueueStream"));
			}

			Buffer->PushFreeBuffer(buffer); // アンキューしたバッファを返す

			{
				if(SegmentQueues.size() > 0)
					SegmentQueues.pop_front(); // セグメントキューからも削除
				else
					fprintf(stderr, "!!! none to popup from SegmentQueues\n"); fflush(stderr);
			}
		}
	}

	// バッファに空きバッファがあるか
	if(Buffer->HasFreeBuffer())
	{
		// データが流し込まれたバッファを得る
		// TODO: segments と events のハンドリング
		tSegmentInfo info;
		risse_uint samples;
		ALuint  buffer = 0;
		bool filled = Buffer->PopFilledBuffer(buffer, info.SegmentQueue, samples);

		// バッファにデータを割り当て、キューする
		if(filled)
		{
			volatile tOpenAL::tCriticalSectionHolder cs_holder;
			alSourceQueueBuffers(Source->Source, 1, &buffer);
			tOpenAL::instance()->ThrowIfError(RISSE_WS("alSourceQueueBuffers"));
		}

		// セグメントキューに追加
		if(filled)
		{
			info.DecodePosition = DecodePosition;
			SegmentQueues.push_back(info);

			// ラベルが一つ以上入っていたら、ラベルイベントのタイミングの
			// リスケジュールを行う
			if(info.SegmentQueue.GetEvents().size() > 0)
			{
				// ラベルは SegmentEvents に移す
				gc_deque<tWaveEvent> & events = info.SegmentQueue.GetEvents();
				for(gc_deque<tWaveEvent>::iterator i = events.begin();
					i != events.end(); i++)
				{
					i->Offset += DecodePosition;
					SegmentEvents.push_back(*i);
				}

				// リスケジュールを行わせる
				tWaveLabelTimingThread::instance()->Reschedule();
			}


			DecodePosition += samples;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::RecheckStatus()
{
	if(Source && Status == ssPlay)
	{
		ALint state;

		{
			volatile tOpenAL::tCriticalSectionHolder al_cs_holder;
			alGetSourcei(Source->Source, AL_SOURCE_STATE, &state );
			tOpenAL::instance()->ThrowIfError(RISSE_WS("alGetSourcei(AL_SOURCE_STATE)"));
		}

		if(state != AL_PLAYING)
		{
			// Playing が真を表しているにもかかわらず、OpenAL のソースは再生を
			// 停止している
			InternalStop(2); // 再生を停止, イベントは非同期イベントとして通知
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::WatchCallback()
{
	volatile tCriticalSection::tLocker cs_holder(*CS);

	RecheckStatus();
	FillBuffer();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::CallStatusChanged(bool async)
{
	if(PrevStatus != Status)
	{
		if(async)
			OnStatusChangedAsync(Status);
		else
			OnStatusChanged(Status);
		PrevStatus = Status;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::Play()
{
	volatile tCriticalSection::tLocker cs_holder(*CS);

	RecheckStatus();

	if(Status == ssPlay) return; // すでに再生している場合は再生をしない

	EnsureSource();

	if(Buffer->GetStreaming())
	{
		// もし仮にデコードスレッドがあったとしても
		// ここで解放する
		if(DecodeThread)
			tWaveDecodeThreadPool::instance()->Unacquire(DecodeThread), DecodeThread = NULL;


		// 巻き戻しを行う
		if(NeedRewind)
		{
			// すべての状況で巻き戻しが必要なわけではないので
			// NeedRewind が真の時にしかここでは巻き戻しを行わない
			NeedRewind = false;
			LoopManager->ClearFlags(); // フラグをすべて 0 にする
			LoopManager->SetPosition(0); // 再生位置を最初に
		}

		if(Status != ssPause)
		{
			Buffer->GetFilter()->Reset(); // ポーズからのplayではない場合はフィルタをリセット

			// 初期サンプルをいくつか queue する
			for(risse_uint n = 0; n < STREAMING_PREPARE_BUFFERS; n++)
				QueueBuffer();
		}

		// デコードスレッドを作成する
		if(!DecodeThread) DecodeThread = tWaveDecodeThreadPool::instance()->Acquire(this);
		// デコードスレッドを作成するとその時点から
		// FillBuffer が呼ばれるようになる
	}

	// 再生を開始する
	{
		volatile tOpenAL::tCriticalSectionHolder al_cs_holder;

		alSourcePlay(Source->Source);
		tOpenAL::instance()->ThrowIfError(RISSE_WS("alSourcePlay"));
	}

	// 初期サンプルの queue 中にラベルイベントのリスケジュールが行われない
	// 可能性があるので(なぜならばその時点ではまだ再生が開始されていないから)
	// 念のためリスケジュールを行う
	tWaveLabelTimingThread::instance()->Reschedule();

	// ステータスの変更を通知
	Status = ssPlay;
	CallStatusChanged(false);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::InternalStop(int notify)
{
	// デコードスレッドを削除する
	if(Buffer->GetStreaming())
	{
		if(DecodeThread)
			tWaveDecodeThreadPool::instance()->Unacquire(DecodeThread), DecodeThread = NULL;
		// この時点で
		// FillBuffer は呼ばれなくなる
	}

	// 再生を停止する
	if(Source)
	{
		volatile tOpenAL::tCriticalSectionHolder al_cs_holder;

		alSourceStop(Source->Source);
		tOpenAL::instance()->ThrowIfError(RISSE_WS("alSourceStop"));
	}

	// ステータスの変更を通知
	Status = ssStop;
	if(notify != 0)
		CallStatusChanged(notify == 2);

	// 全てのバッファを unqueueする
	if(Source && Buffer->GetStreaming())
	{
		UnqueueAllBuffers();
	}

	// 次回再生開始前に巻き戻しが必要
	NeedRewind = true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::Stop(int notify)
{
	volatile tCriticalSection::tLocker cs_holder(*CS);

	RecheckStatus();

	InternalStop(notify);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::Pause()
{
	volatile tCriticalSection::tLocker cs_holder(*CS);

	RecheckStatus();

	EnsureSource();

	// 再生中の場合は
	if(Status == ssPlay)
	{
		// 再生を一時停止する
		{
			volatile tOpenAL::tCriticalSectionHolder al_cs_holder;

			alSourcePause(Source->Source);
			tOpenAL::instance()->ThrowIfError(RISSE_WS("alSourcePause"));
		}

		// ステータスの変更を通知
		Status = ssPause;
		CallStatusChanged(false);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tALSource::GetBufferPlayingPosition()
{
	RecheckStatus();

	EnsureSource();

	// 再生中や一時停止中でない場合は risse_size_max を返す
	if(Status != ssPlay && Status != ssPause) return risse_size_max;
	if(SegmentQueues.size() == 0) return risse_size_max;

	unsigned int unit = Buffer->GetOneBufferRenderUnit();

/*---- debug ----*/
{
	ALint queued;
	alGetSourcei(Source->Source, AL_BUFFERS_QUEUED, &queued);
	tOpenAL::instance()->ThrowIfError(
		RISSE_WS("alGetSourcei(AL_BUFFERS_QUEUED) at tALSource::UnqueueAllBuffers"));
	if(queued != (ALint) SegmentQueues.size())
		fprintf(stderr, "segment count does not match (al %d risa %d)\n", (int)queued, (int)SegmentQueues.size());
}
/*---- debug ----*/

	// 再生位置を取得する
	ALint pos = 0;
	{
		volatile tOpenAL::tCriticalSectionHolder al_cs_holder;

		alGetSourcei(Source->Source, AL_SAMPLE_OFFSET, &pos);
		tOpenAL::instance()->ThrowIfError(RISSE_WS("alGetSourcei(AL_SAMPLE_OFFSET)"));

//		tLogger::Log(RISSE_WS("al pos : ") + tString::AsString(pos));
	}

	// ときどき、 alGetSourcei(AL_SAMPLE_OFFSET) はキューされているバッファを
	// 超えるような変なオフセットを返す。
	// どうも折り返させれば OK なようなのでそうする。正常なオフセットを返さないのは
	// OpenAL の実装のバグかとおもわれるが、それを回避する。
	ALint size_max = (unsigned int)SegmentQueues.size() * unit;
	while(pos >= size_max) pos -= size_max;

	return (risse_size)pos;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tALSource::GetPosition()
{
	volatile tCriticalSection::tLocker cs_holder(*CS);

	risse_size pos = GetBufferPlayingPosition();

	// 再生中や一時停止中でない場合は 0 を返す
	if(pos == risse_size_max) return 0;

	// 返された値は キューの先頭からの再生オフセットなので、該当する
	// キューを探す
	unsigned int unit = Buffer->GetOneBufferRenderUnit();

	risse_size queue_index  = pos / unit;
	risse_size queue_offset = pos % unit;

//	fprintf(stderr, "get position queue in offset %d at queue index %d : ", queue_offset, queue_index);
//	SegmentQueues[queue_index].Dump();

	// 得られたのはフィルタ後の位置なのでフィルタ前のデコード位置に変換してから返す
	return SegmentQueues[queue_index].SegmentQueue.FilteredPositionToDecodePosition(queue_offset);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tALSource::SetPosition(risse_uint64 pos)
{
	volatile tCriticalSection::tLocker cs_holder(*CS);

	RecheckStatus();

	EnsureSource();

	if(Buffer->GetStreaming())
	{
		if(Status == ssPlay || Status == ssPause)
		{
			// デコードスレッドをいったん削除する
			if(DecodeThread)
				tWaveDecodeThreadPool::instance()->Unacquire(DecodeThread), DecodeThread = NULL;
			// この時点で
			// FillBuffer は呼ばれなくなる

			// 再生を停止する
			{
				volatile tOpenAL::tCriticalSectionHolder al_cs_holder;

				alSourceStop(Source->Source);
				tOpenAL::instance()->ThrowIfError(RISSE_WS("alSourceStop"));
			}
		}

		// 全てのバッファを unqueueする
		UnqueueAllBuffers();
		EnsureSource();

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
			if(!DecodeThread) DecodeThread = tWaveDecodeThreadPool::instance()->Acquire(this);
			// デコードスレッドを作成するとその時点から
			// FillBuffer が呼ばれるようになる
		}

		if(Status == ssPlay)
		{
			// 再生を開始する
			{
				volatile tOpenAL::tCriticalSectionHolder al_cs_holder;

				alSourcePlay(Source->Source);
				tOpenAL::instance()->ThrowIfError(RISSE_WS("alSourcePlay"));
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
risse_int32 tALSource::FireLabelEvents()
{
	// キュー中のイベントのうち、現在再生されているポイント以前の物に対して
	// イベントを発生させる。また、現在再生されているポイントより後の物に対しては
	// 最も近いイベントをさがして、それまでの時間を帰す。
	volatile tCriticalSection::tLocker cs_holder(*CS);

	// そもそもイベントがキュー中にあるか？
	gc_deque<tSegmentInfo>::iterator i;
	for(i = SegmentQueues.begin();
		i != SegmentQueues.end(); i++)
	{
		if(i->SegmentQueue.GetEvents().size() > 0) break;
	}

	if(i == SegmentQueues.end()) return -1; // イベントは何もない

	// 現在の再生位置を得る
	risse_size pos = GetBufferPlayingPosition();
	if(pos == risse_size_max) return -1; // 情報が得られなかった

	unsigned int unit = Buffer->GetOneBufferRenderUnit();

	risse_size queue_index  = pos / unit;
	risse_size queue_offset = pos % unit;

	// 得られたのはフィルタ後の位置なのでフィルタ前のデコード位置に変換してから返す
	risse_uint64 current_pos = SegmentQueues[queue_index].DecodePosition + queue_offset;

	// イベントキューを順にみていき、目的のイベントを探す
	// いちおうイベントはもっとも速い物から順にキューに入っているはずだが、
	// イベントがそんなに大量になることもないので、いまのところ念のため
	// すべてのイベントを見る
/*
tString nearest_label;
*/
	risse_int64 nearest_next = (risse_int64)-1;
	for(gc_deque<tWaveEvent>::iterator ei = SegmentEvents.begin(); ei != SegmentEvents.end(); /**/)
	{
		risse_uint64 event_pos = ei->Offset;
		if(event_pos <= current_pos)
		{
			// fire event
			wxFprintf(stderr, wxT("label event %s\n"), ei->Name.AsWxString().c_str());
			fflush(stderr);
			ei = SegmentEvents.erase(ei); // イベントは削除する
		}
		else
		{
			if(nearest_next == (risse_int64)-1 || (risse_uint64)nearest_next > event_pos - current_pos)
				nearest_next = event_pos - current_pos/*, nearest_label = ei->Name*/;
			ei ++;
		}
	}

	// 時間に変換
	if(nearest_next != (risse_int64)-1)
		nearest_next = nearest_next  * 1000 / LoopManager->GetFormat().Frequency + 1;
		// 1 を足しているのは誤差を丸めるため。
		// ぴったりだと、次回ここが呼ばれたときにはわずかにタイミングが早すぎる場合がある。

/*
	if(nearest_next != (risse_int64)-1)
	{
	wxFprintf(stderr, wxT("nearest event %s, after %d ms (at %d), current %d\n"),
		nearest_label.AsWxString().c_str(), (int)nearest_next, (int)(nearest_next + current_pos), (int)current_pos);
	}
*/
	// あまりに大きな数値は適当にまとめる(問題ない)
	if(nearest_next > 0x10000000) nearest_next = 0x10000000; // あまりに大きい


	return (risse_int32)nearest_next;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa

