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

#include "prec.h"
#include <algorithm>
#include "WaveFormatConverter.h"
#include "RisaException.h"

RISSE_DEFINE_SOURCE_ID(2303);



//---------------------------------------------------------------------------
//! @brief		汎用変換ループのテンプレート実装
//---------------------------------------------------------------------------
template <typename DESTTYPE, typename SRCTYPE>
static void RisaPCMConvertLoop(void * dest, const void * src, size_t numsamples)
{
	if(DESTTYPE::id == SRCTYPE::id)
	{
		// 型が全く同じなので memcpy をつかう
		memcpy(dest, src, numsamples * SRCTYPE::size);
	}
	else
	{
		// 型が違うので変換を行う
		risse_uint8 * d = reinterpret_cast<risse_uint8*>(dest);
		const risse_uint8 * s = reinterpret_cast<risse_uint8*>(dest);
		const risse_uint8 * s_lim = s + SRCTYPE::size * numsamples;

		while(s < s_lim)
		{
			reinterpret_cast<DESTTYPE*>(d)->seti32(reinterpret_cast<const SRCTYPE*>(s)->geti32());
			d += DESTTYPE::size;
			s += SRCTYPE::size;
		}
	}
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		int16→float32のテンプレート特化 (よくつかう)
//---------------------------------------------------------------------------
template <>
static void RisaPCMConvertLoop<tRisaPCMTypes::f32, tRisaPCMTypes::i16>
				(void * dest, const void * src, size_t numsamples)
{
	// 型が違うので変換を行う
	risse_uint8 * d = reinterpret_cast<risse_uint8*>(dest);
	const risse_uint8 * s = reinterpret_cast<risse_uint8*>(dest);
	const risse_uint8 * s_lim = s + tRisaPCMTypes::i16::size * numsamples;

	while(s < s_lim)
	{
		reinterpret_cast<tRisaPCMTypes::f32*>(d)->value = 
			reinterpret_cast<const tRisaPCMTypes::i16*>(s)->value *
					(1.0f / (float)tRisaPCMTypes::i16::max_value);
		d += tRisaPCMTypes::f32::size;
		s += tRisaPCMTypes::i16::size;
	}
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		float32→int16のテンプレート特化 (よくつかう)
//---------------------------------------------------------------------------
template <>
static void RisaPCMConvertLoop<tRisaPCMTypes::i16, tRisaPCMTypes::f32>
				(void * dest, const void * src, size_t numsamples)
{
	// 型が違うので変換を行う
	risse_uint8 * d = reinterpret_cast<risse_uint8*>(dest);
	const risse_uint8 * s = reinterpret_cast<risse_uint8*>(dest);
	const risse_uint8 * s_lim = s + tRisaPCMTypes::f32::size * numsamples;

	while(s < s_lim)
	{
		float v = reinterpret_cast<const tRisaPCMTypes::f32*>(s)->value * 32767.0;
		reinterpret_cast<tRisaPCMTypes::i16*>(d)->value = 
			 v > (float)tRisaPCMTypes::i16::max_value ? tRisaPCMTypes::i16::max_value :
			 v < (float)tRisaPCMTypes::i16::min_value ? tRisaPCMTypes::i16::min_value :
			 	v < 0 ? (risse_int16)(v - 0.5) : (risse_int16)(v + 0.5);
		d += tRisaPCMTypes::i16::size;
		s += tRisaPCMTypes::f32::size;
	}
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		PCM形式の変換を行う
//! @param		outformat		出力フォーマット
//! @param		outdata			出力フォーマットを書き出すバッファ
//! @param		informat		入力フォーマット
//! @param		indata			入力データ
//! @param		numsamples		処理を行うサンプル数
//---------------------------------------------------------------------------
void tRisaWaveFormatConverter::Convert(
		const tRisaWaveFormat &outformat, void * outdata,
		const tRisaWaveFormat &informat, const void * indata,
		size_t numsamples)
{
	// 形式のチェック
	if(outformat.Channels != informat.Channels) // チャンネル数が違う?
		eRisaException::Throw(
			RISSE_WS_TR("tRisaWaveFormatConverter::Convert: can not convert PCM format: number of channels does not match"));

	// チャンネルはインターリーブされているので、numsamples にチャンネル数を掛ける
	numsamples *= outformat.Channels;

	// 形式の特定を行う
	tRisaPCMTypes::tType outtype, intype;

	outtype = outformat.GetRisaPCMType();
	if(outtype == tRisaPCMTypes::tunknown)
		eRisaException::Throw(
			RISSE_WS_TR("tRisaWaveFormatConverter::Convert: unsupported destination format"));

	intype = informat.GetRisaPCMType();
	if(intype == tRisaPCMTypes::tunknown)
		eRisaException::Throw(
			RISSE_WS_TR("tRisaWaveFormatConverter::Convert: unsupported source format"));

	// 変換ループ用テーブル
	typedef void (*func_t)(void * dest, const void * src, size_t numsamples);
	#define R_TF(d, s) &RisaPCMConvertLoop<tRisaPCMTypes::d, tRisaPCMTypes::s>
	static func_t table[tRisaPCMTypes::tnum][tRisaPCMTypes::tnum] = {
	{ R_TF( i8,i8),R_TF( i8,i16),R_TF( i8,i24),R_TF( i8,i32),R_TF( i8,f32), },
	{ R_TF(i16,i8),R_TF(i16,i16),R_TF(i16,i24),R_TF(i16,i32),R_TF(i16,f32), },
	{ R_TF(i24,i8),R_TF(i24,i16),R_TF(i24,i24),R_TF(i24,i32),R_TF(i24,f32), },
	{ R_TF(i32,i8),R_TF(i32,i16),R_TF(i32,i24),R_TF(i32,i32),R_TF(i32,f32), },
	{ R_TF(f32,i8),R_TF(f32,i16),R_TF(f32,i24),R_TF(f32,i32),R_TF(f32,f32), },
	};
	#undef R_TF


	// 変換ループ用に分岐
	(table[outtype][intype])(outdata, indata, numsamples);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		この形式に対応する tRisaPCMTypes::tType を返す
//---------------------------------------------------------------------------
tRisaPCMTypes::tType tRisaWaveFormat::GetRisaPCMType() const
{
	if(IsFloat)
	{
		if(BytesPerSample == 4) return tRisaPCMTypes::tf32;
	}
	else
	{
		switch(BytesPerSample)
		{
		case 1: return tRisaPCMTypes::ti8;
		case 2: return tRisaPCMTypes::ti16;
		case 3: return tRisaPCMTypes::ti24;
		case 4: return tRisaPCMTypes::ti32;
		}
	}
	return tRisaPCMTypes::tunknown;
}
//---------------------------------------------------------------------------


