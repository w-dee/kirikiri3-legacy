//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Waveフォーマットコンバータ
//---------------------------------------------------------------------------

#include "risa/prec.h"
#include <algorithm>
#include "risa/packages/risa/sound/Sound.h"
#include "risa/packages/risa/sound/WaveFormatConverter.h"

namespace Risa {
RISSE_DEFINE_SOURCE_ID(58054,38608,61255,17526,21894,38819,54476,53804);
//---------------------------------------------------------------------------


/*
	(opt_CPU)/WaveFormatConverter_CPU.cpp にある関数のプロトタイプ
*/
void PCMConvertLoopInt16ToFloat32(void * RISSE_RESTRICT dest, const void * RISSE_RESTRICT src, size_t numsamples);
void PCMConvertLoopFloat32ToInt16(void * RISSE_RESTRICT dest, const void * RISSE_RESTRICT src, size_t numsamples);



//---------------------------------------------------------------------------
/**
 * 汎用変換ループのテンプレート実装
 */
template <typename DESTTYPE, typename SRCTYPE>
static void PCMConvertLoop(void * RISSE_RESTRICT dest, const void * RISSE_RESTRICT src, size_t numsamples)
{
	if(DESTTYPE::id == SRCTYPE::id)
	{
		// 型が全く同じなので memcpy をつかう
		memcpy(dest, src, numsamples * SRCTYPE::size);
	}
	else
	{
		// 型が違うので変換を行う
		risse_uint8 * d = static_cast<risse_uint8*>(dest);
		const risse_uint8 * s = static_cast<const risse_uint8*>(src);
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
/**
 * int16→float32のテンプレート特化 (よくつかう)
 */
template <>
static void PCMConvertLoop<tPCMTypes::f32, tPCMTypes::i16>
				(void * dest, const void * src, size_t numsamples)
{
	PCMConvertLoopInt16ToFloat32(dest, src, numsamples);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * float32→int16のテンプレート特化 (よくつかう)
 */
template <>
static void PCMConvertLoop<tPCMTypes::i16, tPCMTypes::f32>
				(void * dest, const void * src, size_t numsamples)
{
	PCMConvertLoopFloat32ToInt16(dest, src, numsamples);
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void tWaveFormatConverter::Convert(
		tPCMTypes::tType outformat, void * RISSE_RESTRICT outdata,
		tPCMTypes::tType informat, const void * RISSE_RESTRICT indata,
		risse_int channels, size_t numsamples)
{
	// チャンネルはインターリーブされているので、numsamples にチャンネル数を掛ける
	numsamples *= channels;

	// 形式の特定を行う
	if(outformat == tPCMTypes::tunknown)
		tSoundExceptionClass::Throw(
			RISSE_WS_TR("tWaveFormatConverter::Convert: unsupported destination format"));

	if(informat == tPCMTypes::tunknown)
		tSoundExceptionClass::Throw(
			RISSE_WS_TR("tWaveFormatConverter::Convert: unsupported source format"));

	// 変換ループ用テーブル
	typedef void (*func_t)(void * dest, const void * src, size_t numsamples);
	#ifdef R
		#undef R
	#endif
	#define R(d, s) &PCMConvertLoop<tPCMTypes::d, tPCMTypes::s>
	static func_t table[tPCMTypes::tnum][tPCMTypes::tnum] = {
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


