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


//---------------------------------------------------------------------------
//! @brief		サウンドクラス
//---------------------------------------------------------------------------
class tRisaSound : depends_on<tRisaOpenAL>, depends_on<tRisaWaveDecoderFactoryManager>
{
	boost::shared_ptr<tRisaALSource> Source;
	boost::shared_ptr<tRisaALBuffer> Buffer;
	std::vector<boost::shared_ptr<tRisaWaveFilter> > Filters;
	boost::shared_ptr<tRisaWaveLoopManager> LoopManager;
	boost::shared_ptr<tRisaWaveDecoder> Decoder;

public:
	tRisaSound();
	tRisaSound(const ttstr & filename);
	~tRisaSound();

private:
	void Init();

protected:
	void Clear();

public:
	void Open(const ttstr & filename);
	void Close();
	void Play();
	void Stop();
};
//---------------------------------------------------------------------------

#endif
