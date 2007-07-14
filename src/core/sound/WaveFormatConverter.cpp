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

#include "prec.h"
#include <algorithm>
#include "sound/WaveFormatConverter.h"
#include "base/exception/RisaException.h"

namespace Risa {
RISSE_DEFINE_SOURCE_ID(58054,38608,61255,17526,21894,38819,54476,53804);
//---------------------------------------------------------------------------


/*
	(opt_CPU)/WaveFormatConverter_CPU.cpp にある関数のプロトタイプ
*/
void RisaPCMConvertLoopInt16ToFloat32(risse_restricted void * dest, risse_restricted const void * src, size_t numsamples);
void RisaPCMConvertLoopFloat32ToInt16(risse_restricted void * dest, risse_restricted const void * src, size_t numsamples);



//---------------------------------------------------------------------------
//! @brief		汎用変換ループのテンプレート実装
//---------------------------------------------------------------------------
template <typename DESTTYPE, typename SRCTYPE>
static void RisaPCMConvertLoop(risse_restricted void * dest, const risse_restricted void * src, size_t numsamples)
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
		const risse_uint8 * s = reinterpret_cast<const risse_uint8*>(src);
		const risse_uint8 * s_lim = s + SRCTYPE::size * numsamples;

		while(s < s_lim)
		{
			risse_int32 value = reinterpret_cast<const SRCTYPE*>(s)->geti32();
			reinterpret_cast<DESTTYPE*>(d)->seti32(value);
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
	RisaPCMConvertLoopInt16ToFloat32(dest, src, numsamples);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		float32→int16のテンプレート特化 (よくつかう)
//---------------------------------------------------------------------------
template <>
static void RisaPCMConvertLoop<tRisaPCMTypes::i16, tRisaPCMTypes::f32>
				(void * dest, const void * src, size_t numsamples)
{
	RisaPCMConvertLoopFloat32ToInt16(dest, src, numsamples);
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void tRisaWaveFormatConverter::Convert(
		tRisaPCMTypes::tType outformat, risse_restricted void * outdata,
		tRisaPCMTypes::tType informat, risse_restricted const void * indata,
		risse_int channels, size_t numsamples)
{
	// チャンネルはインターリーブされているので、numsamples にチャンネル数を掛ける
	numsamples *= channels;

	// 形式の特定を行う
	if(outformat == tRisaPCMTypes::tunknown)
		eRisaException::Throw(
			RISSE_WS_TR("tRisaWaveFormatConverter::Convert: unsupported destination format"));

	if(informat == tRisaPCMTypes::tunknown)
		eRisaException::Throw(
			RISSE_WS_TR("tRisaWaveFormatConverter::Convert: unsupported source format"));

	// 変換ループ用テーブル
	typedef void (*func_t)(void * dest, const void * src, size_t numsamples);
	#ifdef R
		#undef R
	#endif
	#define R(d, s) &RisaPCMConvertLoop<tRisaPCMTypes::d, tRisaPCMTypes::s>
	static func_t table[tRisaPCMTypes::tnum][tRisaPCMTypes::tnum] = {
	{ R( i8,i8),R( i8,i16),R( i8,i24),R( i8,i32),R( i8,f32), },
	{ R(i16,i8),R(i16,i16),R(i16,i24),R(i16,i32),R(i16,f32), },
	{ R(i24,i8),R(i24,i16),R(i24,i24),R(i24,i32),R(i24,f32), },
	{ R(i32,i8),R(i32,i16),R(i32,i24),R(i32,i32),R(i32,f32), },
	{ R(f32,i8),R(f32,i16),R(f32,i24),R(f32,i32),R(f32,f32), },
	};
	#undef R


	// 変換ループ用に分岐
	(table[outformat][informat])(outdata, indata, numsamples);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa


