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
#ifndef MATHALGORITHMSH
#define MATHALGORITHMSH

#include <math.h>

//---------------------------------------------------------------------------
//! @brief		atan2 の高速版 (1x float, C言語版)
//! @note		精度はあまり良くない。10bitぐらい。 @r
//!				原典: http://www.dspguru.com/comp.dsp/tricks/alg/fxdatan2.htm
//---------------------------------------------------------------------------
static inline float RisaVFast_arctan2(float y, float x)
{
   static const float coeff_1 = M_PI/4;
   static const float coeff_2 = 3*coeff_1;
   float angle;
   float abs_y = fabs(y)+1e-10;     // kludge to prevent 0/0 condition
   if (x>=0)
   {
      float r = (x - abs_y) / (x + abs_y);
      angle = coeff_1 - coeff_1 * r;
   }
   else
   {
      float r = (x + abs_y) / (abs_y - x);
      angle = coeff_2 - coeff_1 * r;
   }
   if (y < 0)
     return(-angle);     // negate if in quad III or IV
   else
     return(angle);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		sincos の高速版 (1x float, C言語版)
//! @note		原典: http://arxiv.org/PS_cache/cs/pdf/0406/0406049.pdf 
//---------------------------------------------------------------------------
static inline float RisaVFast_atan2_madd(float a, float b, float c) { return a*b+c; }
static inline float RisaVFast_atan2_nmsub(float a, float b, float c) { return -(a*b-c); }

static inline void RisaVFast_atan2_F1(float v, float &sin, float &cos)
{
	const float  ss1 =  1.5707963235  ;
	const float  ss2 =  -0.645963615  ;
	const float  ss3 =  0.0796819754  ;
	const float  ss4 =  -0.0046075748 ;
	const float  cc1 =  -1.2336977925 ;
	const float  cc2 =  0.2536086171  ;
	const float  cc3 =  -0.0204391631 ;

	float s1, s2, c1, c2, fixmag1;
	float x1=RisaVFast_atan2_madd(v, (float)(1.0/(2.0*3.1415926536)), (float)(0.0));
	/* q1=x/2pi reduced onto (-0.5,0.5), q2=q1**2 */
	float q1=RisaVFast_atan2_nmsub(round(x1), (float)(1.0), x1);
	float q2=RisaVFast_atan2_madd(q1, q1, (float)(0.0));
	s1= RisaVFast_atan2_madd(q1,
			RisaVFast_atan2_madd(q2,
				RisaVFast_atan2_madd(q2,
					RisaVFast_atan2_madd(q2, (float)(ss4),
								(float)(ss3)),
									(float)( ss2)),
							(float)(ss1)),
						(float)(0.0));
	c1= RisaVFast_atan2_madd(q2,
			RisaVFast_atan2_madd(q2,
				RisaVFast_atan2_madd(q2, (float)(cc3),
				(float)(cc2)),
			(float)(cc1)),
		(float)(1.0));

	/* now, do one out of two angle-doublings to get sin & cos theta/2 */
	c2=Risa_VFast_atan2_nmsub(s1, s1, RisaVFast_atan2_madd(c1, c1, (float)(0.0)));
	s2=Risa_VFast_atan2_madd((float)(2.0), RisaVFast_atan2_madd(s1, c1, (float)(0.0)), (float)(0.0));

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
	fixmag1=Risa_VFast_atan2_nmsub(s2,s2, RisaVFast_atan2_nmsub(c2, c2, (float)(2.0)));

	c1=RisaVFast_atan2_nmsub(s2, s2, RisaVFast_atan2_madd(c2, c2, (float)(0.0)));
	s1=RisaVFast_atan2_madd((float)(2.0), RisaVFast_atan2_madd(s2, c2, (float)(0.0)), (float)(0.0));
	cos=RisaVFast_atan2_madd(c1, fixmag1, (float)(0.0));
	sin=RisaVFast_atan2_madd(s1, fixmag1, (float)(0.0));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Phase Wrapping(radianを-PI～PIにラップする) (1x float, C言語版)
//---------------------------------------------------------------------------
static inline float RisaWrapPi_F1(float v)
{
	v += M_PI;
	float v_quant = v / (2.0)*M_PI;
	int i_quant = (int)v_quant;
	if(i_quant < 0) i_quant --;
	v_quant -= i_quant * (2.0 * PI);
	return v_quant + M_PI;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		窓関数を適用しながらのインターリーブ解除
//! @param		dest	格納先(複数)
//! @param		src		ソース
//! @param		win		窓関数
//! @param		numch	チャンネル数
//! @param		len		処理するサンプル数
//!						(各チャンネルごとの数; 実際に処理されるサンプル
//!						数の総計はlen*numchになる)
//---------------------------------------------------------------------------
void RisaDeinterleaveApplyingWindow(float * dest[], const float * src,
					float * win, int numch, size_t len)
{
	risse_size n;
	switch(numch)
	{
	case 1: // mono
		{
			float * dest0 = dest[0];
			for(n = 0; n < len; n++)
			{
				dest0[n] = src[n] * win[n];
			}
		}
		break;

	case 2: // stereo
		{
			float * dest0 = dest[0];
			float * dest1 = dest[1];
			for(n = 0; n < len; n++)
			{
				dest0[n] = src[n*2 + 0] * win[n];
				dest1[n] = src[n*2 + 1] * win[n];
			}
		}
		break;

	default: // generic
		for(n = 0; n < len; n++)
		{
			for(int ch = 0; ch < numch; ch++)
			{
				dest[ch][n] = *src * win[n];
				src ++;
			}
		}
		break;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		窓関数を適用しながらのインターリーブ+オーバーラッピング
//! @param		dest	格納先
//! @param		src		ソース(複数)
//! @param		win		窓関数
//! @param		numch	チャンネル数
//! @param		len		処理するサンプル数
//!						(各チャンネルごとの数; 実際に処理されるサンプル
//!						数の総計はlen*numchになる)
//---------------------------------------------------------------------------
void  RisaInterleaveOverlappingWindow(float * dest, const float * src,
					float * win, int numch, size_t len)
{
	risse_size n;
	switch(numch)
	{
	case 1: // mono
		{
			float * src0 = src[0];
			for(n = 0; n < len; n++)
			{
				dest[n] += src0[n] * win[n];
			}
		}
		break;

	case 2: // stereo
		{
			float * src0 = src[0];
			float * src1 = src[1];
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
				dest += src[ch][n] * win[n];
				dest ++;
			}
		}
		break;
	}
}
//---------------------------------------------------------------------------


#endif

