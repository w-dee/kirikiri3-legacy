//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Waveフォーマットコンバータ
//---------------------------------------------------------------------------

#ifndef WaveFormatConverterH
#define WaveFormatConverterH
//---------------------------------------------------------------------------

#include "Wave.h"

//---------------------------------------------------------------------------
class tRisaWaveFormatConverter
{
	static void Convert(
		const tRisaWaveFormat &outformat, void * outdata,
		const tRisaWaveFormat &informat, const void * indata,
		size_t numsamples);
};
//---------------------------------------------------------------------------
#endif


