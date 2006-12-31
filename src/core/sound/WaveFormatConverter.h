//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
	//! @brief		PCM形式の変換を行う
	//! @param		outformat		出力フォーマット
	//! @param		outdata			出力フォーマットを書き出すバッファ
	//! @param		informat		入力フォーマット
	//! @param		indata			入力データ
	//! @param		channels		チャンネル数
	//! @param		numsamples		処理を行うサンプルグラニュール数
	static void Convert(
		tRisaPCMTypes::tType outformat, void * outdata,
		tRisaPCMTypes::tType informat, const void * indata,
		risse_int channels, size_t numsamples);
};
//---------------------------------------------------------------------------
#endif


