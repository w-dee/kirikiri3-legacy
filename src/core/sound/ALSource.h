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
#include "base/utils/RisaThread.h"
#include "base/utils/Singleton.h"


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





class tRisaWaveDecodeThread;
//---------------------------------------------------------------------------
//! @brief		OpenALソース
//---------------------------------------------------------------------------
class tRisaALSource : depends_on<tRisaOpenAL>, depends_on<tRisaWaveWatchThread>
{
	friend class tRisaWaveDecodeThread;
	friend class tRisaWaveWatchThread;

	tRisaCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション
	risse_uint NumBuffersQueued; //!< キューに入っているバッファの数
	ALuint Source; //!< OpenAL ソース
	bool SourceAllocated; //!< Source がすでに割り当てられているかどうか
	boost::shared_ptr<tRisaALBuffer> Buffer; //!< バッファ
	tRisaWaveDecodeThread * DecodeThread; //!< デコードスレッド
	volatile bool Playing; //!< 再生中に真

public:
	tRisaALSource(boost::shared_ptr<tRisaALBuffer> buffer);
	tRisaALSource(const tRisaALSource * ref);
	~tRisaALSource();

private:
	void Init(boost::shared_ptr<tRisaALBuffer> buffer);

public:
	ALuint GetSource() const { return Source; } //!< Source を得る

private:
	void Clear();

	void FillBuffer();
	void WatchCallback();

public:
	void Play();
	void Stop();
	bool GetPlaying() const { return Playing; }

};
//---------------------------------------------------------------------------

#endif
