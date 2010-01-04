//---------------------------------------------------------------------------
/*
	Risa [りさ]		 alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 実数離散フーリエ変換
//---------------------------------------------------------------------------
#ifndef REALFFT_H
#define REALFFT_H

#include "risseTypes.h"

/*
	Based on
		Real Discrete FFT package from
			http://momonga.t.u-tokyo.ac.jp/~ooura/fft-j.html
	and
		Ogg Vorbis Optimization Project
			http://homepage3.nifty.com/blacksword/
*/

namespace Risa {
//---------------------------------------------------------------------------

    void rdft(int, int, float * RISSE_RESTRICT, int * RISSE_RESTRICT, float * RISSE_RESTRICT);


//---------------------------------------------------------------------------
} // namespace Risa


#endif
