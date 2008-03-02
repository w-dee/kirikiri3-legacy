//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief TLG5コーデック用コア関数
//---------------------------------------------------------------------------

#include "prec.h"


extern "C" {

risse_int TLG5DecompressSlide(
	RISSE_RESTRICT risse_uint8 *out,
	RISSE_RESTRICT const risse_uint8 *in,
	risse_int insize, risse_uint8 *text, risse_int initialr)
{
	risse_int r = initialr;
	const risse_uint8 *inlim = in + insize;

getmore:
	risse_uint flags = 0[in++] | 0x100;

loop:
	{
		bool b= flags & 1;
		flags >>= 1;
		if(!flags) goto getmore;

		if(b)
		{
			risse_uint16 word = *(const risse_uint16*)in;
			risse_int mpos = word & 0xfff;
			risse_int mlen = word >> 12;
			in += 2;
			if(mlen == 15) mlen += 0[in++];

				0[out++] = text[r++] = text[mpos++];
				mpos &= (4096 - 1);
				r &= (4096 - 1);
				0[out++] = text[r++] = text[mpos++];
				mpos &= (4096 - 1);
				r &= (4096 - 1);
				0[out++] = text[r++] = text[mpos++];
				mpos &= (4096 - 1);
				r &= (4096 - 1);
			while(mlen--)
			{
				0[out++] = text[r++] = text[mpos++];
				mpos &= (4096 - 1);
				r &= (4096 - 1);
			}
		}
		else
		{
			0[out++] = text[r++] = 0[in++];
			r &= (4096 - 1);
		}
	}
	if(in < inlim) goto loop;

	return r;
}

}
