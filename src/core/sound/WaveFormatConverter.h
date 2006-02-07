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

#include "sound/Wave.h"

//---------------------------------------------------------------------------
class tRisaWaveFormatConverter
{
public:
	static void Convert(
		tRisaPCMTypes::tType outformat, void * outdata,
		tRisaPCMTypes::tType informat, const void * indata,
		risse_int channels, size_t numsamples);
};
//---------------------------------------------------------------------------
#endif


