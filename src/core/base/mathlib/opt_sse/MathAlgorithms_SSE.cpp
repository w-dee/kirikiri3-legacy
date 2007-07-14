//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 数学関数群
//---------------------------------------------------------------------------
#include "risse/include/risseTypes.h"
#include <math.h>
#include "base/cpu/opt_sse/xmmlib.h"
#include "base/mathlib/opt_sse/MathAlgorithms_SSE.h"

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

#define vat_c1 (M_PI /4)
#define vat_c2 (vat_c1*3)
#define vat_e  1e-10

_ALIGN16(const float) RISA_VFASTATAN2_C1[4] = { vat_c1, vat_c1, vat_c1, vat_c1 };
_ALIGN16(const float) RISA_VFASTATAN2_C2[4] = { vat_c2, vat_c2, vat_c2, vat_c2 };
_ALIGN16(const float) RISA_VFASTATAN2_E [4] = { vat_e,  vat_e,  vat_e,  vat_e  };


#define ss1 1.5707963235
#define ss2 -0.645963615
#define ss3 0.0796819754
#define ss4 -0.0046075748
#define cc1 -1.2336977925
#define cc2 0.2536086171
#define cc3 -0.0204391631

_ALIGN16(const float) RISA_VFASTSINCOS_SS1[4] = { ss1, ss1, ss1, ss1 };
_ALIGN16(const float) RISA_VFASTSINCOS_SS2[4] = { ss2, ss2, ss2, ss2 };
_ALIGN16(const float) RISA_VFASTSINCOS_SS3[4] = { ss3, ss3, ss3, ss3 };
_ALIGN16(const float) RISA_VFASTSINCOS_SS4[4] = { ss4, ss4, ss4, ss4 };
_ALIGN16(const float) RISA_VFASTSINCOS_CC1[4] = { cc1, cc1, cc1, cc1 };
_ALIGN16(const float) RISA_VFASTSINCOS_CC2[4] = { cc2, cc2, cc2, cc2 };
_ALIGN16(const float) RISA_VFASTSINCOS_CC3[4] = { cc3, cc3, cc3, cc3 };

#define recppi (1.0/M_PI)
#define recp2pi (1.0/(2.0 * M_PI))
#define pi (M_PI)
#define pi_2 (M_PI * 2.0)
_ALIGN16(const float) RISA_V_R_PI[4] = { recppi, recppi, recppi, recppi };
_ALIGN16(const float) RISA_V_R_2PI[4] = { recp2pi, recp2pi, recp2pi, recp2pi };
_ALIGN16(const float) RISA_V_PI[4] = { pi, pi, pi, pi };
_ALIGN16(const float) RISA_V_2PI[4] = { pi_2, pi_2, pi_2, pi_2 };

_ALIGN16(const risse_uint32) RISA_V_I32_1[4] = { 1, 1, 1, 1 };


//---------------------------------------------------------------------------
static void _RisaDeinterleaveApplyingWindow(float * dest[], const float * src,
					float * win, int numch, size_t destofs, size_t len)
{
	size_t n;
	switch(numch)
	{
	case 1: // mono
		{
			float * dest0 = dest[0] + destofs;
			int condition = 
				(RisaIsAlignedTo128bits(dest0) ? 0:4) + 
				(RisaIsAlignedTo128bits(src)   ? 0:2) +
				(RisaIsAlignedTo128bits(win)   ? 0:1 );

#define R(cond, destf, srcf, winf) \
			case cond: \
				for(n = 0; n < len - 7; n += 8) \
				{ \
					destf(dest0+n  , _mm_mul_ps(srcf(src+n  ), winf(win+n  ))); \
					destf(dest0+n+4, _mm_mul_ps(srcf(src+n+4), winf(win+n+4))); \
				} \
				break

			// それぞれのアラインメントに応じた処理を行う
			if(len >= 8)
				switch(condition)
				{
					R(0, _mm_store_ps , _mm_load_ps , _mm_load_ps );
					R(1, _mm_store_ps , _mm_load_ps , _mm_loadu_ps);
					R(2, _mm_store_ps , _mm_loadu_ps, _mm_load_ps );
					R(3, _mm_store_ps , _mm_loadu_ps, _mm_loadu_ps);
					R(4, _mm_storeu_ps, _mm_load_ps , _mm_load_ps );
					R(5, _mm_storeu_ps, _mm_load_ps , _mm_loadu_ps);
					R(6, _mm_storeu_ps, _mm_loadu_ps, _mm_load_ps );
					R(7, _mm_storeu_ps, _mm_loadu_ps, _mm_loadu_ps);
				}

#undef R

			for(     ; n < len; n++)
			{
				dest0[n] += src[n] * win[n];
			}
		}
		break;

	case 2: // stereo
		{
			float * dest0 = dest[0] + destofs;
			float * dest1 = dest[1] + destofs;

			int condition = 
				( (RisaIsAlignedTo128bits(dest0)&&
				   RisaIsAlignedTo128bits(dest1)) ? 0:4) + 
				(RisaIsAlignedTo128bits(src)      ? 0:2) +
				(RisaIsAlignedTo128bits(win)      ? 0:1);

#define R(cond, destf, srcf, winf) \
			case cond: \
				for(n = 0; n < len - 3; n += 4)                                                \
				{                                                                              \
					__m128 win3210 = winf(win + n);                                            \
					__m128 win1100 = _mm_shuffle_ps(win3210, win3210, _MM_SHUFFLE(1,1,0,0));   \
					__m128 win3322 = _mm_shuffle_ps(win3210, win3210, _MM_SHUFFLE(3,3,2,2));   \
					__m128 src3210 = srcf(src + n*2    );                                      \
					__m128 src7654 = srcf(src + n*2 + 4);                                      \
					__m128 m0 = _mm_mul_ps(src3210, win1100);                                  \
					__m128 m1 = _mm_mul_ps(src7654, win3322);                                  \
					__m128 dest0_3210 = _mm_shuffle_ps(m0, m1, _MM_SHUFFLE(2,0,2,0));          \
					__m128 dest1_3210 = _mm_shuffle_ps(m0, m1, _MM_SHUFFLE(3,1,3,1));          \
					destf(dest0 + n, dest0_3210);                                              \
					destf(dest1 + n, dest1_3210);                                              \
				} \
				break

			if(len >= 4)
				switch(condition)
				{
					R(0, _mm_store_ps , _mm_load_ps , _mm_load_ps );
					R(1, _mm_store_ps , _mm_load_ps , _mm_loadu_ps);
					R(2, _mm_store_ps , _mm_loadu_ps, _mm_load_ps );
					R(3, _mm_store_ps , _mm_loadu_ps, _mm_loadu_ps);
					R(4, _mm_storeu_ps, _mm_load_ps , _mm_load_ps );
					R(5, _mm_storeu_ps, _mm_load_ps , _mm_loadu_ps);
					R(6, _mm_storeu_ps, _mm_loadu_ps, _mm_load_ps );
					R(7, _mm_storeu_ps, _mm_loadu_ps, _mm_loadu_ps);
				}

#undef R

			for(     ; n < len; n++)
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
				dest[ch][n + destofs] = *src * win[n];
				src ++;
			}
		}
		break;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
RISA_DEFINE_STACK_ALIGN_128_TRAMPOLINE(
	void, RisaDeinterleaveApplyingWindow, (float * dest[], const float * src,
					float * win, int numch, size_t destofs, size_t len),
	_RisaDeinterleaveApplyingWindow, (dest, src, win, numch, destofs, len) )
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static void  _RisaInterleaveOverlappingWindow(float * dest, const float * const * src,
					float * win, int numch, size_t srcofs, size_t len)
{
	size_t n;
	switch(numch)
	{
	case 1: // mono
		{
			const float * src0 = src[0] + srcofs;

			int condition = 
				(RisaIsAlignedTo128bits(dest)  ? 0:4) + 
				(RisaIsAlignedTo128bits(src0)  ? 0:2) +
				(RisaIsAlignedTo128bits(win)   ? 0:1 );

#define R(cond, destf, destlf, srcf, winf) \
			case cond: \
				for(n = 0; n < len - 7; n += 8) \
				{ \
					destf(dest+n  , _mm_add_ps(_mm_mul_ps(srcf(src0+n  ), winf(win+n  )), destlf(dest+n  ))); \
					destf(dest+n+4, _mm_add_ps(_mm_mul_ps(srcf(src0+n+4), winf(win+n+4)), destlf(dest+n+4))); \
				} \
				break

			if(len >= 8)
				switch(condition)
				{
					R(0, _mm_store_ps , _mm_load_ps ,_mm_load_ps , _mm_load_ps );
					R(1, _mm_store_ps , _mm_load_ps ,_mm_load_ps , _mm_loadu_ps);
					R(2, _mm_store_ps , _mm_load_ps ,_mm_loadu_ps, _mm_load_ps );
					R(3, _mm_store_ps , _mm_load_ps ,_mm_loadu_ps, _mm_loadu_ps);
					R(4, _mm_storeu_ps, _mm_loadu_ps,_mm_load_ps , _mm_load_ps );
					R(5, _mm_storeu_ps, _mm_loadu_ps,_mm_load_ps , _mm_loadu_ps);
					R(6, _mm_storeu_ps, _mm_loadu_ps,_mm_loadu_ps, _mm_load_ps );
					R(7, _mm_storeu_ps, _mm_loadu_ps,_mm_loadu_ps, _mm_loadu_ps);
				}
#undef R
			for(     ; n < len; n++)
			{
				dest[n] += src0[n] * win[n];
			}
		}
		break;

	case 2: // stereo
		{
			const float * src0 = src[0] + srcofs;
			const float * src1 = src[1] + srcofs;

			int condition = 
				(RisaIsAlignedTo128bits(dest)  ? 0:4) + 
				((RisaIsAlignedTo128bits(src0) &&
				  RisaIsAlignedTo128bits(src1))? 0:2) +
				(RisaIsAlignedTo128bits(win)   ? 0:1 );

#define R(cond, destf, destlf, srcf, winf) \
			case cond: \
				for(n = 0; n < len - 3; n += 4) \
				{ \
					__m128 src0_3210  = srcf(src0 + n);        \
					__m128 src1_3210  = srcf(src1 + n);        \
					__m128 win3210    = winf(win  + n);        \
					__m128 dest6420_a = src0_3210 * win3210;   \
					__m128 dest7531_a = src1_3210 * win3210;   \
					__m128 dest3120_a = _mm_movelh_ps(dest6420_a, dest7531_a);  \
					__m128 dest6475_a = _mm_movehl_ps(dest6420_a, dest7531_a);  \
					__m128 dest3210_a = _mm_shuffle_ps(dest3120_a, dest3120_a, _MM_SHUFFLE(3,1,2,0)); \
					__m128 dest7654_a = _mm_shuffle_ps(dest6475_a, dest6475_a, _MM_SHUFFLE(1,3,0,2)); \
					destf(dest+n*2  , _mm_add_ps(destlf(dest+n*2  ), dest3210_a)); \
					destf(dest+n*2+4, _mm_add_ps(destlf(dest+n*2+4), dest7654_a)); \
				} \
				break

			if(len >= 4)
				switch(condition)
				{
					R(0, _mm_store_ps , _mm_load_ps ,_mm_load_ps , _mm_load_ps );
					R(1, _mm_store_ps , _mm_load_ps ,_mm_load_ps , _mm_loadu_ps);
					R(2, _mm_store_ps , _mm_load_ps ,_mm_loadu_ps, _mm_load_ps );
					R(3, _mm_store_ps , _mm_load_ps ,_mm_loadu_ps, _mm_loadu_ps);
					R(4, _mm_storeu_ps, _mm_loadu_ps,_mm_load_ps , _mm_load_ps );
					R(5, _mm_storeu_ps, _mm_loadu_ps,_mm_load_ps , _mm_loadu_ps);
					R(6, _mm_storeu_ps, _mm_loadu_ps,_mm_loadu_ps, _mm_load_ps );
					R(7, _mm_storeu_ps, _mm_loadu_ps,_mm_loadu_ps, _mm_loadu_ps);
				}

#undef R

			for(    ; n < len; n++)
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


//---------------------------------------------------------------------------
RISA_DEFINE_STACK_ALIGN_128_TRAMPOLINE(
	void, RisaInterleaveOverlappingWindow, (float * dest, const float * const * src,
					float * win, int numch, size_t srcofs, size_t len),
	_RisaInterleaveOverlappingWindow, (dest, src, win, numch, srcofs, len) )
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa



