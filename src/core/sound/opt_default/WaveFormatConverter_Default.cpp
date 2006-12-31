//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Waveフォーマットコンバータのコア関数
//---------------------------------------------------------------------------

#include "prec.h"


//---------------------------------------------------------------------------
//! @brief		int16→float32変換
//---------------------------------------------------------------------------
void RisaPCMConvertLoopInt16ToFloat32(risse_restricted void * dest, risse_restricted const void * src, size_t numsamples)
{
	float * d = reinterpret_cast<float*>(dest);
	const risse_int16 * s = reinterpret_cast<const risse_int16*>(src);
	const risse_int16 * s_lim = s + numsamples;

	while(s < s_lim)
	{
		*d = *s * (1.0f/32768.0f);
		d += 1; s += 1;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		float32→int16変換
//---------------------------------------------------------------------------
void RisaPCMConvertLoopFloat32ToInt16(risse_restricted void * dest, risse_restricted const void * src, size_t numsamples)
{
	risse_uint16 * d = reinterpret_cast<risse_uint16*>(dest);
	const float * s = reinterpret_cast<const float*>(src);
	const float * s_lim = s + numsamples;

	while(s < s_lim)
	{
		float v = *s * 32767.0;
		*d = 
			 v > (float) 32767 ?  32767 :
			 v < (float)-32768 ? -32768 :
			 	v < 0 ? (risse_int16)(v - 0.5) : (risse_int16)(v + 0.5);
		d += 1; s += 1;
	}
}
//---------------------------------------------------------------------------


