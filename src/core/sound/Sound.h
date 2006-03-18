//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief サウンドクラス
//---------------------------------------------------------------------------
#ifndef SoundH
#define SoundH

#include "sound/ALCommon.h"
#include "sound/ALBuffer.h"
#include "sound/ALSource.h"
#include "sound/WaveDecoder.h"
#include "sound/WaveLoopManager.h"
#include "sound/WaveFilter.h"


class tRisaSound;
//---------------------------------------------------------------------------
//! @brief		OpenALイベントの発生先を tRisaSound に固定した OpenAL ソースクラス
//---------------------------------------------------------------------------
class tRisaSoundALSource : public tRisaALSource
{
	tRisaSound * Owner;

public:
	tRisaSoundALSource(tRisaSound * owner, boost::shared_ptr<tRisaALBuffer> buffer,
		boost::shared_ptr<tRisaWaveLoopManager> loopmanager);
	tRisaSoundALSource(tRisaSound * owner, const tRisaALSource * ref);

protected:
	void OnStatusChanged(tStatus status);
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief		サウンドクラス
//---------------------------------------------------------------------------
class tRisaSound :
	depends_on<tRisaOpenAL>,
	depends_on<tRisaWaveDecoderFactoryManager>,
	public tRisaALSourceStatus
{
	friend class tRisaSoundALSource;

	tStatus Status; //!< 直前のステータス

	boost::shared_ptr<tRisaSoundALSource> Source;
	boost::shared_ptr<tRisaALBuffer> Buffer;
	std::vector<boost::shared_ptr<tRisaWaveFilter> > Filters;
	boost::shared_ptr<tRisaWaveLoopManager> LoopManager;
	boost::shared_ptr<tRisaWaveDecoder> Decoder;

public:
	tRisaSound();
	tRisaSound(const ttstr & filename);
	virtual ~tRisaSound();

private:
	void Init();

protected:
	void Clear();

	void Rewind();

	void CallOnStatusChanged(tStatus status);

public:
	void Open(const ttstr & filename);
	void Close();
	void Play();
	void Stop();
	void Pause();
	risse_uint64 GetSamplePosition();
	double GetTimePosition();

	virtual void OnStatusChanged(tStatus status) {;}

	tStatus GetStatus() const { return Status; } //!< ステータスを返す
};
//---------------------------------------------------------------------------

#endif
