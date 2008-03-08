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
#define TLG6_BYTEOF(a, x) (((risse_uint8*)(a))[(x)])
#if RISSE_HOST_IS_BIG_ENDIAN || !(defined(_X86_) || defined(_AMD64_))

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
// GNU C 3.4 or later has __builtin_ctz and prefetch function
#if (__GNUC__ == 3 && __GNUC_MINOR__ >= 4 ) || __GNUC__ >= 4
	#define TLG6_BSF(__x) __builtin_ctz(__x)
	#define TLG6_PREFETCH_FOR_READ(__x) __builtin_prefetch(__x);
	#define TLG6_PREFETCH_FOR_WRITE(__x) __builtin_prefetch(__x, 1);
#else
	inline int TLG6_BSF(risse_uint32 r)
	{
		risse_uint32 cnt=0, b=1;
		while(b) { if(r&b) return cnt; cnt ++; b <<= 1; }
		return 0;
	}
	#define TLG6_PREFETCH_FOR_READ(__x)
	#define TLG6_PREFETCH_FOR_WRITE(__x)
#endif

// likely and unlikely 
#ifndef likely
	#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96)
	#define likely(cond) __builtin_expect(!!(int)(cond), 1)
	#define unlikely(cond) __builtin_expect((int)(cond), 0)
#else
	#define likely(cond) (cond)
	#define unlikely(cond) (cond)
	#endif
#endif /* !likely */
//---------------------------------------------------------------------------


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


#define FILL_BITS do { while(unlikely(bit_pos <= 24)) { bits += (*(bit_pool++) << bit_pos); bit_pos += 8; } } while(0)
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
		TLG6_PREFETCH_FOR_WRITE(pixelbuf + 64);

		{
			risse_int b = TLG6_BSF(bits);
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

		if(unlikely(pixelbuf >= limit)) break;

nonzero:
		/* get running count */
		{
			risse_int b = TLG6_BSF(bits);
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
			TLG6_PREFETCH_FOR_READ(bit_pool + 64);
			TLG6_PREFETCH_FOR_WRITE(pixelbuf + 48);

			int k = TLG6GolombBitLengthTable[a][n], v, sign;

			risse_int bit_count;
			risse_int b;

//fprintf(stderr, "* bits : %08x\n", bits);
			v = TLG6GolombCodeTable[bits&0xff][k];
			if(likely(v))
			{
				b = (v >> 8) & 0xff;
				a += v >> 16;
				STEP_BITS(b);
				FILL_BITS;
				*(ACCECSS_TYPE*)pixelbuf = (unsigned char) v;
			}
			else
			{
				if(likely(bits))
				{
					{
						bit_count = TLG6_BSF(bits);
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

			if (unlikely(--n < 0)) {
				a >>= 1;  n = TLG6_GOLOMB_N_COUNT - 1;
			}
		} while(--count);

		if(unlikely(pixelbuf >= limit)) break;
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
