//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief TLGコーデック用コア関数
//---------------------------------------------------------------------------

#include "prec.h"
#include "visual/image/codec/TLGDecoder.h"


extern "C" {



//---------------------------------------------------------------------------
//! @brief		スライド辞書法(変形)の展開ルーチン
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
extern risse_uint8 TLG6LeadingZeroTable[TLG6_LeadingZeroTable_SIZE];
extern char TLG6GolombBitLengthTable
	[TLG6_GOLOMB_N_COUNT*2*128][TLG6_GOLOMB_N_COUNT];
//---------------------------------------------------------------------------
}

//---------------------------------------------------------------------------
#if RISSE_HOST_IS_BIG_ENDIAN || !(defined(_X86_) || defined(_AMD64_))
	#define TLG6_BYTEOF(a, x) (((risse_uint8*)(a))[(x)])

	#define TLG6_FETCH_32BITS(addr) ((risse_uint32)TLG6_BYTEOF((addr), 0) +  \
									((risse_uint32)TLG6_BYTEOF((addr), 1) << 8) + \
									((risse_uint32)TLG6_BYTEOF((addr), 2) << 16) + \
									((risse_uint32)TLG6_BYTEOF((addr), 3) << 24) )
#else
	// intel architectures allows mis-alignment access of dwords ...
	// (of course there is a penalty)
	#define TLG6_FETCH_32BITS(addr) (*(risse_uint32*)addr)
#endif
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ゴロム・ライス符号展開ルーチン (risse_uint32 単位でアクセス)
//---------------------------------------------------------------------------
template <typename ACCECSS_TYPE>
static RISSE_FORCEINLINE void DecodeGolombValues(
	RISSE_RESTRICT risse_int8 *pixelbuf,
	risse_int pixel_count,
	RISSE_RESTRICT risse_uint8 *bit_pool)
{
	/*
		decode values packed in "bit_pool".
		values are coded using golomb code.

		"ForFirst" function do dword access to pixelbuf,
		clearing with zero except for blue (least siginificant byte).
	*/

	int n = TLG6_GOLOMB_N_COUNT - 1; /* output counter */
	int a = 0; /* summary of absolute values of errors */

	risse_int bit_pos = 1;
	risse_uint8 zero = (*bit_pool & 1)?0:1;

	risse_int8 * limit = pixelbuf + pixel_count*4;

	while(pixelbuf < limit)
	{
		/* get running count */
		int count;

		{
			risse_uint32 t = TLG6_FETCH_32BITS(bit_pool) >> bit_pos;
			risse_int b = TLG6LeadingZeroTable[t&(TLG6_LeadingZeroTable_SIZE-1)];
			int bit_count = b;
			while(!b)
			{
				bit_count += TLG6_LeadingZeroTable_BITS;
				bit_pos += TLG6_LeadingZeroTable_BITS;
				bit_pool += bit_pos >> 3;
				bit_pos &= 7;
				t = TLG6_FETCH_32BITS(bit_pool) >> bit_pos;
				b = TLG6LeadingZeroTable[t&(TLG6_LeadingZeroTable_SIZE-1)];
				bit_count += b;
			}


			bit_pos += b;
			bit_pool += bit_pos >> 3;
			bit_pos &= 7;

			bit_count --;
			count = 1 << bit_count;
			count += ((TLG6_FETCH_32BITS(bit_pool) >> (bit_pos)) & (count-1));

			bit_pos += bit_count;
			bit_pool += bit_pos >> 3;
			bit_pos &= 7;

		}

		if(zero)
		{
			/* zero values */

			/* fill distination with zero */
			do { *(ACCECSS_TYPE*)pixelbuf = 0; pixelbuf+=4; } while(--count);

			zero ^= 1;
		}
		else
		{
			/* non-zero values */

			/* fill distination with glomb code */

			do
			{
				int k = TLG6GolombBitLengthTable[a][n], v, sign;

				risse_uint32 t = TLG6_FETCH_32BITS(bit_pool) >> bit_pos;
				risse_int bit_count;
				risse_int b;
				if(t)
				{
					b = TLG6LeadingZeroTable[t&(TLG6_LeadingZeroTable_SIZE-1)];
					bit_count = b;
					while(!b)
					{
						bit_count += TLG6_LeadingZeroTable_BITS;
						bit_pos += TLG6_LeadingZeroTable_BITS;
						bit_pool += bit_pos >> 3;
						bit_pos &= 7;
						t = TLG6_FETCH_32BITS(bit_pool) >> bit_pos;
						b = TLG6LeadingZeroTable[t&(TLG6_LeadingZeroTable_SIZE-1)];
						bit_count += b;
					}
					bit_count --;
				}
				else
				{
					bit_pool += 5;
					bit_count = bit_pool[-1];
					bit_pos = 0;
					t = TLG6_FETCH_32BITS(bit_pool);
					b = 0;
				}


				v = (bit_count << k) + ((t >> b) & ((1<<k)-1));
				sign = (v & 1) - 1;
				v >>= 1;
				a += v;
				*(ACCECSS_TYPE*)pixelbuf = (unsigned char) ((v ^ sign) + sign + 1);
				pixelbuf += 4;

				bit_pos += b;
				bit_pos += k;
				bit_pool += bit_pos >> 3;
				bit_pos &= 7;

				if (--n < 0) {
					a >>= 1;  n = TLG6_GOLOMB_N_COUNT - 1;
				}
			} while(--count);
			zero ^= 1;
		}
	}
}
//---------------------------------------------------------------------------


extern "C" {
//---------------------------------------------------------------------------
void TLG6DecodeGolombValuesForFirst(
	RISSE_RESTRICT risse_int8 *pixelbuf,
	risse_int pixel_count,
	RISSE_RESTRICT risse_uint8 *bit_pool)
{
	DecodeGolombValues<risse_uint32>(pixelbuf, pixel_count, bit_pool);
}
//---------------------------------------------------------------------------
void TLG6DecodeGolombValues(
	RISSE_RESTRICT risse_int8 *pixelbuf,
	risse_int pixel_count,
	RISSE_RESTRICT risse_uint8 *bit_pool)
{
	DecodeGolombValues<risse_uint8>(pixelbuf, pixel_count, bit_pool);
}
//---------------------------------------------------------------------------


}
