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
#include "base/cpu/opt_sse/xmmlib.h"
#include <math.h>

#define vat_c1 (M_PI /4)
#define vat_c2 (vat_c1*3)
#define vat_e  1e-10

_ALIGN16(const float) VFASTATAN2_C1[4] = { vat_c1, vat_c1, vat_c1, vat_c1 };
_ALIGN16(const float) VFASTATAN2_C2[4] = { vat_c2, vat_c2, vat_c2, vat_c2 };
_ALIGN16(const float) VFASTATAN2_E [4] = { vat_e,  vat_e,  vat_e,  vat_e  };


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

#define recp2pi (1.0/(2.0 * M_PI))
#define pi (M_PI)
#define pi_2 (M_PI * 2.0)
_ALIGN16(const float) RISA_V_R_2PI[4] = { recp2pi, recp2pi, recp2pi, recp2pi };
_ALIGN16(const float) RISA_V_PI[4] = { pi, pi, pi, pi };
_ALIGN16(const float) RISA_V_2PI[4] = { pi_2, pi_2, pi_2, pi_2 };





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
			int condition = 
				(RisaIsAlignedTo128bits(dest0) ? 0:4) + 
				(RisaIsAlignedTo128bits(src)   ? 0:2) +
				(RisaIsAlignedTo128bits(win)   ? 0:1 );

#define R(cond, destf, srcf, winf) \
			case cond: \
				for(n = 0; n < len - 7; n += 8) \
				{ \
					destf(dest0+n  , srcf(_mm_load_ps(src+n  ), winf(win+n  ))); \
					destf(dest0+n+4, srcf(_mm_load_ps(src+n+4), winf(win+n+4))); \
				} \
				break

			// それぞれのアラインメントに応じた処理を行う
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
				dest[n] += src0[n] * win[n];
			}
		}
		break;

	case 2: // stereo
		{
			float * dest0 = dest[0];
			float * dest1 = dest[1];

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
/*
				dest0[n  ] = src[n*2+0] * win[n  ];
				dest1[n  ] = src[n*2+1] * win[n  ];
				dest0[n+1] = src[n*2+2] * win[n+1];
				dest1[n+1] = src[n*2+3] * win[n+1];
				dest0[n+2] = src[n*2+4] * win[n+2];
				dest1[n+2] = src[n*2+5] * win[n+2];
				dest0[n+3] = src[n*2+6] * win[n+3];
				dest1[n+3] = src[n*2+7] * win[n+3];
*/

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
				dest[ch][n] = *src * win[n];
				src ++;
			}
		}
		break;
	}
}
//---------------------------------------------------------------------------

