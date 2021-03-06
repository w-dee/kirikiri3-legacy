//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Waveフォーマットコンバータのコア関数
//---------------------------------------------------------------------------

#include "risseTypes.h"
using namespace Risse;
#include "risa/common/opt_sse/xmmlib.h"
#include "risa/common/opt_sse/MathAlgorithms_SSE.h"

namespace Risa {
//---------------------------------------------------------------------------

_ALIGN16(const float) RISA_V_VEC_REDUCE[4] =
	{ 1.0f/32767.0f, 1.0f/32767.0f, 1.0f/32767.0f, 1.0f/32767.0f };
_ALIGN16(const float) RISA_V_VEC_MAGNIFY[4] =
	{ 32767.0f, 32767.0f, 32767.0f, 32767.0f };



//---------------------------------------------------------------------------
/**
 * int16→float32変換
 */
void _PCMConvertLoopInt16ToFloat32(void * RISSE_RESTRICT dest, const void * RISSE_RESTRICT src, size_t numsamples)
{
	float * d = static_cast<float*>(dest);
	const risse_int16 * s = static_cast<const risse_int16*>(src);
	size_t n;

	// d がアラインメントされるまで一つずつ処理をする
	for(n = 0  ; n < numsamples && !IsAlignedTo128bits(d+n); n ++)
	{
		d[n] = s[n] * (1.0f/32767.0f);
	}

	// メインの部分
	if(numsamples >= 8)
	{
		for(       ; n < numsamples - 7; n += 8)
		{
			*(__m128*)(d + n +  0) = _mm_cvtpi16_ps(*(__m64*)(s+n+ 0))* PM128(RISA_V_VEC_REDUCE);
			*(__m128*)(d + n +  4) = _mm_cvtpi16_ps(*(__m64*)(s+n+ 4))* PM128(RISA_V_VEC_REDUCE);
		}
		_mm_empty();
	}

	// のこり
	for(       ; n < numsamples; n ++)
	{
		d[n] = s[n] * (1.0f/32767.0f);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
RISA_DEFINE_STACK_ALIGN_128_TRAMPOLINE(
	void, PCMConvertLoopInt16ToFloat32, (void * RISSE_RESTRICT dest, const void * RISSE_RESTRICT src, size_t numsamples),
	_PCMConvertLoopInt16ToFloat32, (dest, src, numsamples) )
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * float32→int16変換
 */
void _PCMConvertLoopFloat32ToInt16(void * RISSE_RESTRICT dest, const void * RISSE_RESTRICT src, size_t numsamples)
{
	risse_uint16 * d = reinterpret_cast<risse_uint16*>(dest);
	const float * s = reinterpret_cast<const float*>(src);
	size_t n;

	// s がアラインメントされるまで一つずつ処理をする
	for(n = 0; n < numsamples && !IsAlignedTo128bits(s+n); n ++)
	{
		float v = s[n] * 32767.0;
		d[n] = 
			 v > (float) 32767 ?  32767 :
			 v < (float)-32768 ? -32768 :
			 	v < 0 ? (risse_int16)(v - 0.5) : (risse_int16)(v + 0.5);
	}

	// メインの部分
	if(numsamples >= 8)
	{
		SetRoundingModeToNearest_SSE();
		for(     ; n < numsamples - 7; n += 8)
		{
			*(__m64*)(d + n + 0) = _mm_cvtps_pi16(*(__m128*)(s + n + 0)*PM128(RISA_V_VEC_MAGNIFY));
			*(__m64*)(d + n + 4) = _mm_cvtps_pi16(*(__m128*)(s + n + 4)*PM128(RISA_V_VEC_MAGNIFY));
		}
		_mm_empty();
	}

	// のこり
	for(     ; n < numsamples; n ++)
	{
		float v = s[n] * 32767.0;
		d[n] = 
			 v > (float) 32767 ?  32767 :
			 v < (float)-32768 ? -32768 :
			 	v < 0 ? (risse_int16)(v - 0.5) : (risse_int16)(v + 0.5);
	}
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
RISA_DEFINE_STACK_ALIGN_128_TRAMPOLINE(
	void, PCMConvertLoopFloat32ToInt16, (void * RISSE_RESTRICT dest, const void * RISSE_RESTRICT src, size_t numsamples),
	_PCMConvertLoopFloat32ToInt16, (dest, src, numsamples) )
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa




