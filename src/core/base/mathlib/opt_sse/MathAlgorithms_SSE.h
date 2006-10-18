//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 数学関数群
//---------------------------------------------------------------------------
#ifndef SIMDMATHH
#define SIMDMATHH

#include <math.h>

#include "base/cpu/opt_sse/xmmlib.h"

//---------------------------------------------------------------------------
// 定数など
//---------------------------------------------------------------------------
extern const float RISA_VFASTATAN2_C1[4] ;
extern const float RISA_VFASTATAN2_C2[4] ;
extern const float RISA_VFASTATAN2_E [4] ;
extern const float RISA_VFASTSINCOS_SS1[4] ;
extern const float RISA_VFASTSINCOS_SS2[4] ;
extern const float RISA_VFASTSINCOS_SS3[4] ;
extern const float RISA_VFASTSINCOS_SS4[4] ;
extern const float RISA_VFASTSINCOS_CC1[4] ;
extern const float RISA_VFASTSINCOS_CC2[4] ;
extern const float RISA_VFASTSINCOS_CC3[4] ;
extern const float RISA_V_R_2PI[4] ;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		atan2 の高速版 (4x float, SSE版)
//! @note		精度はあまり良くない。10bitぐらい。 @r
//!				原典: http://www.dspguru.com/comp.dsp/tricks/alg/fxdatan2.htm
//---------------------------------------------------------------------------
static inline __m128 RisaVFast_arctan2_F4_SSE(__m128 y, __m128 x)
{
   float angle;

	__m128 abs_y = _mm_add_ps(_mm_and_ps(y, PABSMASK), RISA_VFASTATAN2_E);
//   float abs_y = fabs(y)+1e-10;     // kludge to prevent 0/0 condition

	__m128 x_sign = _mm_and_ps(x, PCS_RRRR);// 0x80000000 if x < 0
	__m128 x_mask = _mm_cmpgt_ps(x, PFV_0); // 0xffffffff if x > 0
	__m128 abs_y2 = _mm_xor_ps(abs_y , x_sign);
	__m128 abs_y1 = _mm_xor_ps(abs_y2, PCS_RRRR);
	__m128 r      = _mm_div_ps(_mm_add_ps(x, abs_y1), _mm_add_ps(x, abs_y2));
	r             = _mm_xor_ps(r, x_sign);
	__m128 coeff_1_or_2 = _mm_or_ps(
		_mm_and_ps   (x_mask, RISA_VFASTATAN2_C1),
		_mm_andnot_ps(x_mask, RISA_VFASTATAN2_C2));
	__m128 angle  = _mm_sub_ps(coeff_1_or_2, _mm_mul_ps(RISA_VFASTATAN2_C1, r));
/*
   if (x>=0)
   {
      float r =    (x - abs_y) / (x + abs_y)  ;
      angle = coeff_1 - coeff_1 * r;
   }
   else
   {
      float r = -( (x + abs_y) / (x - abs_y)  );
      angle = coeff_2 - coeff_1 * r;
   }
*/
	__m128 y_sign = _mm_and_ps(y, PCS_RRRR);
	return _mm_xor_ps(angle, y_sign);
/*
   if (y < 0)
     return(-angle);     // negate if in quad III or IV
   else
     return(angle);
*/
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		SSEの丸めモードを最近値に設定する
//! @note		このメソッドは SSE で使用する丸めモード
//!				を設定する。これを必要とする各館数を呼び出す前に１回呼び出すこと。
//!				他のSSEを使用している関数が間に挟まると丸めモードが変わる可能性
//!				があるので注意すること。そのような場合は再びこれを呼び出して
//!				丸めモードを再設定すること。
//---------------------------------------------------------------------------
STIN void RisaSetRoundingModeToNearest_SSE()
{
	_MM_SET_ROUNDING_MODE(_MM_ROUND_NEAREST);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		sincos の高速版 (4x float, SSE版)
//! @note		原典: http://arxiv.org/PS_cache/cs/pdf/0406/0406049.pdf  @r
//!				呼び出しに先立って Risa_SetRoundingModeToNearest_SSE を呼ぶこと。
//---------------------------------------------------------------------------
STIN void RisaVFast_sincos_F4_SSE(__m128 v, __m128 &sin, __m128 &cos)
{
	__m128 s1, s2, c1, c2, fixmag1;

	__m128 x1 = _mm_mul_ps(v, RISA_V_R_2PI);
//	float x1=madd(v, (float)(1.0/(2.0*3.1415926536)), (float)(0.0));

	/* q1=x/2pi reduced onto (-0.5,0.5), q2=q1**2 */
	__m64 r0 = _mm_cvt_ps2pi(x1);
	__m64 r1 = _mm_cvt_ps2pi(_mm_movehl_ps(x1, x1));
	__m128 q1;
	q1 = _mm_movelh_ps(_mm_cvtpi32_ps(q1, r0), _mm_cvtpi32_ps(q1, r1));
	q1 = _mm_sub_ps(x1, q1);

	q2 = _mm_mul_ps(q1, q1);

//	float q1=nmsub(round(x1), (float)(1.0), x1); // q1 = x1 - round(x1)
//	float q2=madd(q1, q1, (float)(0.0));

	s1 = _mm_add_ps(_mm_mul_ps(q2, RISA_VFASTSINCOS_SS4), RISA_VFASTSINCOS_SS3);
		// s1 = (q2 * ss4 + ss3)
	s1 = _mm_add_ps(_mm_mul_ps(s1, q2), RISA_VFASTSINCOS_SS2);
		// s1 = (q2 * (q2 * ss4 + ss3) + ss2)
	s1 = _mm_add_ps(_mm_mul_ps(s1, q2), RISA_VFASTSINCOS_SS1);
		// s1 = (q2 * (q2 * (q2 * ss4 + ss3) + ss2) + ss1)
	s1 = _mm_mul_ps(s1, q1);

//	s1 = q1 * (q2 * (q2 * (q2 * ss4 + ss3) + ss2) + ss1);
//	s1= madd(q1,
//			madd(q2,
//				madd(q2,
//					madd(q2, (float)(ss4),
//								(float)(ss3)),
//									(float)( ss2)),
//							(float)(ss1)),
//						(float)(0.0));


	c1 = _mm_add_ps(_mm_mul_ps(q2, RISA_VFASTSINCOS_CC3), RISA_VFASTSINCOS_CC2);
		// c1 = (q2 * cc3 + cc2)
	c1 = _mm_add_ps(_mm_mul_ps(c1, q2), RISA_VFASTSINCOS_CC1);
		// c1 =  (q2 * (q2 * cc3 + cc2) + cc1 )
	c1 = _mm_add_ps(_mm_mul_ps(c1, q2), PFV_1);
//	c1= (q2 *  (q2 * (q2 * cc3 + cc2) + cc1 ) + 1.0);
//	c1= madd(q2,
//			madd(q2,
//				madd(q2, (float)(cc3),
//				(float)(cc2)),
//			(float)(cc1)),
//		(float)(1.0));

	/* now, do one out of two angle-doublings to get sin & cos theta/2 */
	c2 = _mm_sub_ps( _mm_mul_ps(c1, c1), _mm_mul_ps(s1, s1);
//	c2=nmsub(s1, s1, madd(c1, c1, (float)(0.0))); // c2 = (c1*c1) - (s1*s1)
	s2 = _mm_mul_ps(_mm_mul_ps(s1, c1), PFV_2);
//	s2=madd((float)(2.0), madd(s1, c1, (float)(0.0)), (float)(0.0)); // s2=2*s1*c1

	/* now, cheat on the correction for magnitude drift...
	if the pair has drifted to (1+e)*(cos, sin),
	the next iteration will be (1+e)**2*(cos, sin)
	which is, for small e, (1+2e)*(cos,sin).
	However, on the (1+e) error iteration,
	sin**2+cos**2=(1+e)**2=1+2e also,
	so the error in the square of this term
	will be exactly the error in the magnitude of the next term.
	Then, multiply final result by (1-e) to correct */

	/* this works with properly normalized sine-cosine functions, but un-normalized is more */
	__m128 c2_c2 = _mm_mul_ps(c2, c2);
	__m128 s2_s2 = _mm_mul_ps(s2, s2);
	fixmag1 = _mm_sub_ps(_mm_sub_ps(PFV_2, c2_c2), s2_s2);
//	fixmag1=nmsub(s2,s2, nmsub(c2, c2, (float)(2.0))); // fixmag1 = ( 2.0 - c2*c2 ) - s2*s2

	c1 = _mm_sub_ps(c2_c2, s2_s2);
//	c1=nmsub(s2, s2, madd(c2, c2, (float)(0.0))); // c1 = c2*c2 - s2*s2
	s1 = _mm_mul_ps(_mm_mul_ps(s2, c2), PFV_2);
//	s1=madd((float)(2.0), madd(s2, c2, (float)(0.0)), (float)(0.0));
	cos = _mm_mul_ps(c1, fixmag1);
//	cos=madd(c1, fixmag1, (float)(0.0));
	sin = _mm_mul_ps(s1, fixmag1);
//	sin=madd(s1, fixmag1, (float)(0.0));

	_mm_empty();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Phase Wrapping(radianを-PI～PIにラップする) (4x float, SSE版)
//---------------------------------------------------------------------------
STIN __m128 RisaWrap_Pi_F4_SSE(__m128 v)
{
	v = _mm_add_ps(v, RISA_V_PI); // v += M_PI
	__m128 v_quant = _mm_mul_ps(v, RISA_V_R_2PI); // v_quant = v / (2.0 * M_PI)

	// v_quantを小数点以下を切り捨てて整数に変換
	__m64 q0 = _mm_cvtt_ps2pi(v_quant); 
	__m64 q1 = _mm_cvtt_ps2pi(_mm_movehl_ps(v_quant, v_quant));

	// 負の場合はさらに1をひく (小数点以下切り捨てなので)
	q0 = _mm_sub_pi32(q0, _mm_srli_pi32(q0, 31)); // q0 = q0 - 1 if q0 < 0
	q1 = _mm_sub_pi32(q1, _mm_srli_pi32(q1, 31)); // q1 = q1 - 1 if q1 < 0

	// それらを実数に戻し、2 * M_PI をかける
	v_quant = _mm_movelh_ps(_mm_cvtpi32_ps(v, q0), _mm_cvtpi32_ps(v, q1));
	v_quant = _mm_mul_ps(v_quant, RISA_V_2PI);

	// それを v から引く
	v = _mm_sub_ps(v, v_quant);

	// MMX使い終わり
	_mm_empty();

	// 最初に足した M_PI を引いて戻る
	return _mm_sub_ps(v, RISA_V_PI); // v -= M_PI
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		窓関数を適用しながらのインターリーブ解除
//! @param		dest	格納先(複数)
//! @param		src		ソース
//! @param		win		窓関数
//! @param		numch	チャンネル数
//! @param		destofs	destの処理開始位置
//! @param		len		処理するサンプル数
//!						(各チャンネルごとの数; 実際に処理されるサンプル
//!						数の総計はlen*numchになる)
//---------------------------------------------------------------------------
void RisaDeinterleaveApplyingWindow(float * dest[], const float * src,
					float * win, int numch, size_t destofs, size_t len);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		窓関数を適用しながらのインターリーブ+オーバーラッピング
//! @param		dest	格納先
//! @param		src		ソース(複数)
//! @param		win		窓関数
//! @param		numch	チャンネル数
//! @param		srcofs	srcの処理開始位置
//! @param		len		処理するサンプル数
//!						(各チャンネルごとの数; 実際に処理されるサンプル
//!						数の総計はlen*numchになる)
//---------------------------------------------------------------------------
void  RisaInterleaveOverlappingWindow(float * dest, const float * const * src,
					float * win, int numch, size_t srcofs, size_t len)
{
	risse_size n;
	switch(numch)
	{
	case 1: // mono
		{
			const float * src0 = src[0] + srcofs;
			for(n = 0; n < len; n++)
			{
				dest[n] += src0[n] * win[n];
			}
		}
		break;

	case 2: // stereo
		{
			const float * src0 = src[0] + srcofs;
			const float * src1 = src[1] + srcofs;
			for(n = 0; n < len; n++)
			{
				dest[n*2 + 0] += src0[n] * win[n];
				dest[n*2 + 1] += src1[n] * win[n];
			}
		}
		break;

	default: // generic
		for(n = 0; n < len; n++)
		{
			for(int ch = 0; ch < numch; ch++)
			{
				*dest += src[ch][n + srcofs] * win[n];
				dest ++;
			}
		}
		break;
	}
}
//---------------------------------------------------------------------------


#endif
