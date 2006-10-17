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




