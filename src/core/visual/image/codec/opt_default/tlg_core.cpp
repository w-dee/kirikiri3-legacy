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
extern 	risse_uint32 TLG6GolombCodeTable[256][8];
//---------------------------------------------------------------------------
}

//---------------------------------------------------------------------------
//! @brief		ゴロム・ライス符号展開ルーチン
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


#define FILL_BITS do { while(RISSE_UNLIKELY(bit_pos <= 24)) { bits += (*(bit_pool++) << bit_pos); bit_pos += 8; } } while(0)
#define STEP_BITS(n) (bits >>= (n), bit_pos -= (n))

	risse_int bit_pos = 0;
	risse_uint32 bits = 0;

	FILL_BITS;
	bool first_is_nonzero = (bits & 1);
	STEP_BITS(1);

	risse_int8 * limit = pixelbuf + pixel_count*4;

	if(first_is_nonzero) goto nonzero;

	while(true)
	{
		int count;

		/* get running count */
		RISSE_PREFETCH_FOR_WRITE(pixelbuf + 64);

		{
			risse_int b = RISSE_BSF(bits);
			STEP_BITS(b+1);
			FILL_BITS;

			count = 1 << b;
			count += (bits) & (count-1);

			STEP_BITS(b);
			FILL_BITS;
		}

		/* zero values */
		if(sizeof(ACCECSS_TYPE) == sizeof(risse_uint32))
		{
			/* fill distination with zero */
			do { *(ACCECSS_TYPE*)pixelbuf = 0; pixelbuf+=4; } while(--count);
		}
		else
		{
			/* do nothing but skip */
			pixelbuf += count * sizeof(risse_uint32);
		}

		if(RISSE_UNLIKELY(pixelbuf >= limit)) break;

nonzero:
		/* get running count */
		{
			risse_int b = RISSE_BSF(bits);
			STEP_BITS(b+1);
			FILL_BITS;

			count = 1 << b;
			count += (bits) & (count-1);

			STEP_BITS(b);
			FILL_BITS;
		}

		/* non-zero values */

		/* fill distination with glomb code */
		do
		{
			RISSE_PREFETCH_FOR_READ(bit_pool + 64);
			RISSE_PREFETCH_FOR_WRITE(pixelbuf + 48);

			int k = TLG6GolombBitLengthTable[a][n], v, sign;

			risse_int bit_count;
			risse_int b;

//fprintf(stderr, "* bits : %08x\n", bits);
			v = TLG6GolombCodeTable[bits&0xff][k];
			if(RISSE_LIKELY(v))
			{
				b = (v >> 8) & 0xff;
				a += v >> 16;
				STEP_BITS(b);
				FILL_BITS;
				*(ACCECSS_TYPE*)pixelbuf = (unsigned char) v;
			}
			else
			{
				if(RISSE_LIKELY(bits))
				{
					{
						bit_count = RISSE_BSF(bits);
						STEP_BITS(bit_count);
						STEP_BITS(1);
							// 注意: ここを単に STEP_BITS(bit_count+1) に
							// すると bit_count が 31 だった場合に
							// シフト演算が機能しなくなる。c >>= 32 で
							// c が 0 になることを期待するかもしれないが
							// x86 の場合は シフト回数の下位 5 ビットだ
							// けがマスクされて使用されるため c >>= 32
							// だと c の変化はない。
						FILL_BITS;
					}
//fprintf(stderr, "+ bit_count : %d\n", bit_count);
//fprintf(stderr, "+ bits : %08x, k : %d\n", bits, k);
				}
				else
				{
					bits = 0; // ご破算
					bit_pos =0;
					bit_count = *(bit_pool++);
//fprintf(stderr, "- bit_count : %d\n", bit_count);
					FILL_BITS;
//fprintf(stderr, "- bits : %08x, k : %d\n", bits, k);
				}
				v = (bit_count << k) + (bits & ((1<<k)-1));
				STEP_BITS(k);
				FILL_BITS;
				sign = (v & 1) - 1;
				v >>= 1;
				a += v;
				*(ACCECSS_TYPE*)pixelbuf = (unsigned char) ((v ^ sign) + sign + 1);
			}


			pixelbuf += 4;

			if(RISSE_UNLIKELY(--n < 0)) {
				a >>= 1;  n = TLG6_GOLOMB_N_COUNT - 1;
			}
		} while(--count);

		if(RISSE_UNLIKELY(pixelbuf >= limit)) break;
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
