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
#ifndef ALSourceH
#define ALSourceH

#include <al.h>
#include <alc.h>
#include "sound/ALCommon.h"
#include "sound/ALBuffer.h"
#include "sound/WaveSegmentQueue.h"
#include "base/utils/RisaThread.h"
#include "base/utils/Singleton.h"
#include "base/event/Event.h"


class tRisaALSource;
//---------------------------------------------------------------------------
//! @brief		監視用スレッド
//! @note		監視用スレッドは、約50ms周期のコールバックをすべての
//!				ソースに発生させる。ソースではいくつかポーリングを
//!				行わなければならない場面でこのコールバックを利用する。
//---------------------------------------------------------------------------
class tRisaWaveWatchThread :
	public singleton_base<tRisaWaveWatchThread>,
	manual_start<tRisaWaveWatchThread>,
	public tRisaThread
{
	tRisaThreadEvent Event; //!< スレッドをたたき起こすため/スレッドを眠らせるためのイベント
	tRisaCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

	std::vector<tRisaALSource*> Sources; //!< Source の配列

public:
	tRisaWaveWatchThread();
	~tRisaWaveWatchThread();

	void RegisterSource(tRisaALSource * source);
	void UnregisterSource(tRisaALSource * source);

protected:
	void Execute(void);
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		tRisaALSourceのステータスを含むクラス
//---------------------------------------------------------------------------
class tRisaALSourceStatus
{
public:
	//! @brief	サウンドソースの状態
	enum tStatus
	{
		ssUnload, //!< data is not specified (tRisaALSourceではこの状態は存在しない)
		ssStop, //!< stopping
		ssPlay, //!< playing
		ssPause, //!< pausing
	};
};
//---------------------------------------------------------------------------



class tRisaWaveDecodeThread;
//---------------------------------------------------------------------------
//! @brief		OpenALソース
//---------------------------------------------------------------------------
class tRisaALSource :
				depends_on<tRisaOpenAL>,
				depends_on<tRisaWaveWatchThread>,
				depends_on<tRisaEventSystem>,
				protected tRisaEventDestination,
				public tRisaALSourceStatus
{
	friend class tRisaWaveDecodeThread;
	friend class tRisaWaveWatchThread;
public:
	// 定数など
	static const risse_uint STREAMING_NUM_BUFFERS = tRisaALBuffer::MAX_NUM_BUFFERS; //!< ストリーミング時のバッファの数(調整可)
	static const risse_uint STREAMING_PREPARE_BUFFERS = 4; //!< 再生開始前にソースにキューしておくバッファの数

private:
	//! @brief	イベントのID
	enum tEventId
	{
		eiStatusChanged, // ステータスが変更された
	};

	tRisaCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション
	risse_uint NumBuffersQueued; //!< キューに入っているバッファの数
	ALuint Source; //!< OpenAL ソース
	bool SourceAllocated; //!< Source がすでに割り当てられているかどうか
	boost::shared_ptr<tRisaALBuffer> Buffer; //!< バッファ
	tRisaWaveDecodeThread * DecodeThread; //!< デコードスレッド
	tStatus Status; //!< サウンドステータス
	tStatus PrevStatus; //!< 直前のサウンドステータス
	std::deque<tRisaWaveSegmentQueue> SegmentQueues; //!< セグメントキューの配列

public:
	tRisaALSource(boost::shared_ptr<tRisaALBuffer> buffer);
	tRisaALSource(const tRisaALSource * ref);
	virtual ~tRisaALSource();

private:
	void Init(boost::shared_ptr<tRisaALBuffer> buffer);

public:
	ALuint GetSource() const { return Source; } //!< Source を得る

private:
	void Clear();

private: //---- queue/buffer management
	void FillBuffer();
	void UnqueueAllBuffers();
	void QueueBuffer();

private:
	void WatchCallback();

	void CallStatusChanged();

protected:
	void OnEvent(tRisaEventInfo * info);

public:
	void Play();
	void Stop();
	void Pause();
	risse_uint64 GetPosition();

public:
	virtual void OnStatusChanged(tStatus status) {;}
};
//---------------------------------------------------------------------------

#endif
