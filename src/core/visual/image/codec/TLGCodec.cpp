//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief TLG5/TLG6形式コーデック
//---------------------------------------------------------------------------
#include "prec.h"
#include "visual/image/codec/TLGCodec.h"
#include "base/exception/RisaException.h"


#if defined(RISA_USE_MMX) || defined(RISA_USE_SSE)
	#include "base/cpu/opt_sse/xmmlib.h"
#endif

namespace Risa {
RISSE_DEFINE_SOURCE_ID(56439,41578,12253,18753,50573,34933,38961,51441);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
static void TLG5ComposeColors3To4(
	RISSE_RESTRICT risse_uint8 *outp,
	RISSE_RESTRICT const risse_uint8 *upper,
	RISSE_RESTRICT risse_uint8 * RISSE_RESTRICT const * buf,
	risse_int width)
{
	risse_int x = 0;
	risse_uint8 pc0, pc1, pc2;
	risse_uint8 c0, c1, c2;
	pc0 = pc1 = pc2 = 0;
	const risse_uint8 * buf0 = buf[0];
	const risse_uint8 * buf1 = buf[1];
	const risse_uint8 * buf2 = buf[2];

	for(; x < width; x++)
	{
		c0 = buf0[x];
		c1 = buf1[x];
		c2 = buf2[x];
		c0 += c1; c2 += c1;
		*(risse_uint32 *)outp =
								((((pc0 += c0) + upper[0]) & 0xff)      ) +
								((((pc1 += c1) + upper[1]) & 0xff) <<  8) +
								((((pc2 += c2) + upper[2]) & 0xff) << 16) +
								0xff000000;
		outp += 4;
		upper += 4;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static void TLG5ComposeColors4To4(
	RISSE_RESTRICT risse_uint8 *outp,
	RISSE_RESTRICT const risse_uint8 *upper,
	RISSE_RESTRICT risse_uint8 * RISSE_RESTRICT const * buf,
	risse_int width)
{
	risse_int x;
	risse_uint8 pc0, pc1, pc2, pc3;
	risse_uint8 c0, c1, c2, c3;
	pc0 = pc1 = pc2 = pc3 = 0;
	const risse_uint8 * buf0 = buf[0];
	const risse_uint8 * buf1 = buf[1];
	const risse_uint8 * buf2 = buf[2];
	const risse_uint8 * buf3 = buf[3];
	for(x = 0; x < width; x++)
	{
		c0 = buf0[x];
		c1 = buf1[x];
		c2 = buf2[x];
		c3 = buf3[x];
		c0 += c1; c2 += c1;
		*(risse_uint32 *)outp =
								((((pc0 += c0) + upper[0]) & 0xff)      ) +
								((((pc1 += c1) + upper[1]) & 0xff) <<  8) +
								((((pc2 += c2) + upper[2]) & 0xff) << 16) +
								((((pc3 += c3) + upper[3]) & 0xff) << 24);
		outp += 4;
		upper += 4;
	}
}
//---------------------------------------------------------------------------


//--------------------------------------------------------------------------
extern "C" {
// 実体は opt_x86 あるいは opt_default のディレクトリの中
risse_int TLG5DecompressSlide(
	RISSE_RESTRICT risse_uint8 *out,
	RISSE_RESTRICT const risse_uint8 *in,
	risse_int insize, risse_uint8 *text, risse_int initialr);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void tTLGImageDecoder::ProcessTLG5(tStreamAdapter & src,
					tPixel::tFormat pixel_format, tProgressCallback * callback,
					tDictionaryInstance * dict)
{

	// load TLG v5.0 lossless compressed graphic
	unsigned char mark[12];
	int width, height, colors, blockheight;
	src.ReadBuffer(mark, 1);
	colors = mark[0];
	width = src.ReadI32LE();
	height = src.ReadI32LE();
	blockheight = src.ReadI32LE();

	if(colors != 3 && colors != 4)
		tIOExceptionClass::Throw(RISSE_WS_TR("error on reading TLG5: unsupported color type"));

	int blockcount = (int)((height - 1) / blockheight) + 1;

	// skip block size section
	src.SetPosition(src.GetPosition() + blockcount * sizeof(risse_uint32));


	// decomperss
DWORD dim = GetTickCount();
	SetDimensions(width, height, tPixel::pfARGB32);
fprintf(stderr, "dim %dms\n", GetTickCount() - dim);

	risse_uint8 *inbuf = NULL;
	risse_uint8 *outbuf[4];
	risse_uint8 *text = NULL;
	risse_int r = 0;
	for(int i = 0; i < colors; i++) outbuf[i] = NULL;

	text = static_cast<risse_uint8*>(AlignedMallocAtomicCollectee(4096, 4));
	memset(text, 0, 4096);

	inbuf = static_cast<risse_uint8*>(AlignedMallocAtomicCollectee(blockheight * width + 10, 4));
	for(risse_int i = 0; i < colors; i++)
		outbuf[i] = static_cast<risse_uint8*>(AlignedMallocAtomicCollectee(blockheight * width + 10, 4));

static DWORD total =0;
static DWORD io = 0;
static DWORD lzss = 0;
static DWORD filter = 0;

DWORD total_start =  GetTickCount();

	risse_uint8 *prevline = NULL;
	for(risse_int y_blk = 0; y_blk < height; y_blk += blockheight)
	{
		// read file and decompress
		for(risse_int c = 0; c < colors; c++)
		{
DWORD io_start = GetTickCount();
			src.ReadBuffer(mark, 1);
			risse_uint32 size;
			size = src.ReadI32LE();
io += GetTickCount() - io_start;
			if(mark[0] == 0)
			{
				// modified LZSS compressed data
DWORD io_start = GetTickCount();
				src.ReadBuffer(inbuf, size);
io += GetTickCount() - io_start;
DWORD lzss_start = GetTickCount();
				r = TLG5DecompressSlide(outbuf[c], inbuf, size, text, r);
lzss += GetTickCount() - lzss_start;
			}
			else
			{
				// raw data
DWORD io_start = GetTickCount();
				src.ReadBuffer(outbuf[c], size);
io += GetTickCount() - io_start;
			}
		}

		// compose colors and store
		risse_int y_lim = y_blk + blockheight;
		if(y_lim > height) y_lim = height;
		risse_uint8 * outbufp[4];
		for(risse_int c = 0; c < colors; c++) outbufp[c] = outbuf[c];
		for(risse_int y = y_blk; y < y_lim; y++)
		{
			risse_uint8 *current =
				static_cast<risse_uint8*>(StartLines(y, 1, NULL));
			risse_uint8 *current_org = current;
			if(prevline)
			{
				// not first line
DWORD filter_start = GetTickCount();
				switch(colors)
				{
				case 3:
					TLG5ComposeColors3To4(current, prevline, outbufp, width);
					outbufp[0] += width; outbufp[1] += width;
					outbufp[2] += width;
					break;
				case 4:
					TLG5ComposeColors4To4(current, prevline, outbufp, width);
					outbufp[0] += width; outbufp[1] += width;
					outbufp[2] += width; outbufp[3] += width;
					break;
				}
filter += GetTickCount() - filter_start;
			}
			else
			{
				// first line
				switch(colors)
				{
				case 3:
					for(risse_int pr = 0, pg = 0, pb = 0, x = 0;
						x < width; x++)
					{
						risse_int b = outbufp[0][x];
						risse_int g = outbufp[1][x];
						risse_int r = outbufp[2][x];
						b += g; r += g;
						*(risse_uint32*)current =
							((risse_uint8)(pb += b)      )+
							((risse_uint8)(pg += g) << 8 )+
							((risse_uint8)(pr += r) << 16)+
							0xff000000;
						current += 4;
					}
					outbufp[0] += width;
					outbufp[1] += width;
					outbufp[2] += width;
					break;
				case 4:
					for(risse_int pr = 0, pg = 0, pb = 0, pa = 0, x = 0;
						x < width; x++)
					{
						risse_int b = outbufp[0][x];
						risse_int g = outbufp[1][x];
						risse_int r = outbufp[2][x];
						risse_int a = outbufp[3][x];
						b += g; r += g;
						*(risse_uint32*)current =
							((risse_uint8)(pb += b)      )+
							((risse_uint8)(pg += g) << 8 )+
							((risse_uint8)(pr += r) << 16)+
							((risse_uint8)(pa += a) << 24);
						current += 4;
					}
					outbufp[0] += width;
					outbufp[1] += width;
					outbufp[2] += width;
					outbufp[3] += width;
					break;
				}
			}
			DoneLines();

			prevline = current_org;
		}

	}
total += GetTickCount()- total_start;

fprintf(stderr, "total %dms\n", total);
fprintf(stderr, "io %dms\n", io);
fprintf(stderr, "lzss %dms\n", lzss);
fprintf(stderr, "filter %dms\n", filter);

}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
#define TLG6_W_BLOCK_SIZE 8
#define TLG6_H_BLOCK_SIZE 8
#define TLG6_GOLOMB_HALF_THRESHOLD 8
#define TLG6_GOLOMB_N_COUNT  4
#define TLG6_LeadingZeroTable_BITS 12
#define TLG6_LeadingZeroTable_SIZE  (1<<TLG6_LeadingZeroTable_BITS)
static risse_uint8 TLG6LeadingZeroTable[TLG6_LeadingZeroTable_SIZE];
static short const int TLG6GolombCompressed[TLG6_GOLOMB_N_COUNT][9] = {
		{3,7,15,27,63,108,223,448,130,},
		{3,5,13,24,51,95,192,384,257,},
		{2,5,12,21,39,86,155,320,384,},
		{2,3,9,18,33,61,129,258,511,},
	/* Tuned by W.Dee, 2004/03/25 */
};
static char TLG6GolombBitLengthTable
	[TLG6_GOLOMB_N_COUNT*2*128][TLG6_GOLOMB_N_COUNT] =
	{ { 0 } };
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static void TLG6MakeTable()
{

	/* table which indicates first set bit position + 1. */
	/* this may be replaced by BSF (IA32 instrcution). */
	for(int i = 0; i < TLG6_LeadingZeroTable_SIZE; i++)
	{
		int cnt = 0;
		int j;
		for(j = 1; j != TLG6_LeadingZeroTable_SIZE && !(i & j);
			j <<= 1, cnt++);
		cnt ++;
		if(j == TLG6_LeadingZeroTable_SIZE) cnt = 0;
		TLG6LeadingZeroTable[i] = cnt;
	}

	for(int n = 0; n < TLG6_GOLOMB_N_COUNT; n++)
	{
		int a = 0;
		for(int i = 0; i < 9; i++)
		{
			for(int j = 0; j < TLG6GolombCompressed[n][i]; j++)
				TLG6GolombBitLengthTable[a++][n] = (char)i;
		}
		if(a != TLG6_GOLOMB_N_COUNT*2*128)
			*(char*)0 = 0;   /* THIS MUST NOT BE EXECUETED! */
				/* (this is for compressed table data check) */
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#if RISSE_HOST_IS_BIG_ENDIAN
	#define TLG6_BYTEOF(a, x) (((risse_uint8*)(a))[(x)])

	#define TLG6_FETCH_32BITS(addr) ((risse_uint32)TLG6_BYTEOF((addr), 0) +  \
									((risse_uint32)TLG6_BYTEOF((addr), 1) << 8) + \
									((risse_uint32)TLG6_BYTEOF((addr), 2) << 16) + \
									((risse_uint32)TLG6_BYTEOF((addr), 3) << 24) )
#else
	#define TLG6_FETCH_32BITS(addr) (*(risse_uint32*)addr)
#endif
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static void TLG6DecodeGolombValuesForFirst(
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
			do { *(risse_uint32*)pixelbuf = 0; pixelbuf+=4; } while(--count);

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
				*(risse_uint32*)pixelbuf = (unsigned char) ((v ^ sign) + sign + 1);
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


//---------------------------------------------------------------------------
void TLG6DecodeGolombValues(
	RISSE_RESTRICT risse_int8 *pixelbuf,
	risse_int pixel_count,
	RISSE_RESTRICT risse_uint8 *bit_pool)
{
	/*
		decode values packed in "bit_pool".
		values are coded using golomb code.
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
			do { *pixelbuf = 0; pixelbuf+=4; } while(--count);

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
				*pixelbuf = (char) ((v ^ sign) + sign + 1);
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


//---------------------------------------------------------------------------
static inline risse_uint32 make_gt_mask(risse_uint32 a, risse_uint32 b){
	risse_uint32 tmp2 = ~b;
	risse_uint32 tmp = ((a & tmp2) + (((a ^ tmp2) >> 1) & 0x7f7f7f7f) ) & 0x80808080;
	tmp = ((tmp >> 7) + 0x7f7f7f7f) ^ 0x7f7f7f7f;
	return tmp;
}
static inline risse_uint32 packed_bytes_add(risse_uint32 a, risse_uint32 b)
{
	risse_uint32 tmp = (((a & b)<<1) + ((a ^ b) & 0xfefefefe) ) & 0x01010100;
	return a+b-tmp;
}
static inline risse_uint32 med2(risse_uint32 a, risse_uint32 b, risse_uint32 c){
	/* do Median Edge Detector   thx, Mr. sugi  at    kirikiri.info */
	risse_uint32 aa_gt_bb = make_gt_mask(a, b);
	risse_uint32 a_xor_b_and_aa_gt_bb = ((a ^ b) & aa_gt_bb);
	risse_uint32 aa = a_xor_b_and_aa_gt_bb ^ a;
	risse_uint32 bb = a_xor_b_and_aa_gt_bb ^ b;
	risse_uint32 n = make_gt_mask(c, bb);
	risse_uint32 nn = make_gt_mask(aa, c);
	risse_uint32 m = ~(n | nn);
	return (n & aa) | (nn & bb) | ((bb & m) - (c & m) + (aa & m));
}
static inline risse_uint32 med(risse_uint32 a, risse_uint32 b, risse_uint32 c, risse_uint32 v){
	return packed_bytes_add(med2(a, b, c), v);
}

#define TLG6_AVG_PACKED(x, y) ((((x) & (y)) + ((((x) ^ (y)) & 0xfefefefe) >> 1)) +\
			(((x)^(y))&0x01010101))

static inline risse_uint32 avg(risse_uint32 a, risse_uint32 b, risse_uint32 c, risse_uint32 v){
	return packed_bytes_add(TLG6_AVG_PACKED(a, b), v);
}

#define TLG6_DO_CHROMA_DECODE_PROTO(B, G, R, A, POST_INCREMENT) do \
			{ \
				risse_uint32 u = *prevline; \
				p = med(p, u, up, \
					(0xff0000 & ((R)<<16)) + (0xff00 & ((G)<<8)) + (0xff & (B)) + ((A) << 24) ); \
				up = u; \
				*curline = p; \
				curline ++; \
				prevline ++; \
				POST_INCREMENT \
			} while(--w);
#define TLG6_DO_CHROMA_DECODE_PROTO2(B, G, R, A, POST_INCREMENT) do \
			{ \
				risse_uint32 u = *prevline; \
				p = avg(p, u, up, \
					(0xff0000 & ((R)<<16)) + (0xff00 & ((G)<<8)) + (0xff & (B)) + ((A) << 24) ); \
				up = u; \
				*curline = p; \
				curline ++; \
				prevline ++; \
				POST_INCREMENT \
			} while(--w);
#define TLG6_DO_CHROMA_DECODE(N, R, G, B) case (N<<1): \
	TLG6_DO_CHROMA_DECODE_PROTO(R, G, B, IA, {in+=step;}) break; \
	case (N<<1)+1: \
	TLG6_DO_CHROMA_DECODE_PROTO2(R, G, B, IA, {in+=step;}) break;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static void TLG6DecodeLineGeneric(
	RISSE_RESTRICT risse_uint32 *prevline,
	RISSE_RESTRICT risse_uint32 *curline,
	risse_int width,
	risse_int start_block,
	risse_int block_limit,
	RISSE_RESTRICT risse_uint8 *filtertypes,
	risse_int skipblockbytes,
	RISSE_RESTRICT risse_uint32 *in,
	risse_uint32 initialp,
	risse_int oddskip,
	risse_int dir)
{
	/*
		chroma/luminosity decoding
		(this does reordering, color correlation filter, MED/AVG  at a time)
	*/
	risse_uint32 p, up;
	int step, i;

	if(start_block)
	{
		prevline += start_block * TLG6_W_BLOCK_SIZE;
		curline  += start_block * TLG6_W_BLOCK_SIZE;
		p  = curline[-1];
		up = prevline[-1];
	}
	else
	{
		p = up = initialp;
	}

	in += skipblockbytes * start_block;
	step = (dir&1)?1:-1;

	for(i = start_block; i < block_limit; i ++)
	{
		int w = width - i*TLG6_W_BLOCK_SIZE, ww;
		if(w > TLG6_W_BLOCK_SIZE) w = TLG6_W_BLOCK_SIZE;
		ww = w;
		if(step==-1) in += ww-1;
		if(i&1) in += oddskip * ww;
		switch(filtertypes[i])
		{
#define IA	(char)((*in>>24)&0xff)
#define IR	(char)((*in>>16)&0xff)
#define IG  (char)((*in>>8 )&0xff)
#define IB  (char)((*in    )&0xff)
		TLG6_DO_CHROMA_DECODE( 0, IB, IG, IR); 
		TLG6_DO_CHROMA_DECODE( 1, IB+IG, IG, IR+IG); 
		TLG6_DO_CHROMA_DECODE( 2, IB, IG+IB, IR+IB+IG); 
		TLG6_DO_CHROMA_DECODE( 3, IB+IR+IG, IG+IR, IR); 
		TLG6_DO_CHROMA_DECODE( 4, IB+IR, IG+IB+IR, IR+IB+IR+IG); 
		TLG6_DO_CHROMA_DECODE( 5, IB+IR, IG+IB+IR, IR); 
		TLG6_DO_CHROMA_DECODE( 6, IB+IG, IG, IR); 
		TLG6_DO_CHROMA_DECODE( 7, IB, IG+IB, IR); 
		TLG6_DO_CHROMA_DECODE( 8, IB, IG, IR+IG); 
		TLG6_DO_CHROMA_DECODE( 9, IB+IG+IR+IB, IG+IR+IB, IR+IB); 
		TLG6_DO_CHROMA_DECODE(10, IB+IR, IG+IR, IR); 
		TLG6_DO_CHROMA_DECODE(11, IB, IG+IB, IR+IB); 
		TLG6_DO_CHROMA_DECODE(12, IB, IG+IR+IB, IR+IB); 
		TLG6_DO_CHROMA_DECODE(13, IB+IG, IG+IR+IB+IG, IR+IB+IG); 
		TLG6_DO_CHROMA_DECODE(14, IB+IG+IR, IG+IR, IR+IB+IG+IR); 
		TLG6_DO_CHROMA_DECODE(15, IB, IG+(IB<<1), IR+(IB<<1));

		default: return;
		}
		if(step == 1)
			in += skipblockbytes - ww;
		else
			in += skipblockbytes + 1;
		if(i&1) in -= oddskip * ww;
#undef IR
#undef IG
#undef IB
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static void TLG6DecodeLine(
	RISSE_RESTRICT risse_uint32 *prevline,
	RISSE_RESTRICT risse_uint32 *curline,
	risse_int width,
	risse_int block_count,
	RISSE_RESTRICT risse_uint8 *filtertypes,
	risse_int skipblockbytes,
	RISSE_RESTRICT risse_uint32 *in,
	risse_uint32 initialp,
	risse_int oddskip,
	risse_int dir)
{
	TLG6DecodeLineGeneric(prevline, curline, width, 0, block_count,
		filtertypes, skipblockbytes, in, initialp, oddskip, dir);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void tTLGImageDecoder::ProcessTLG6(tStreamAdapter & src,
					tPixel::tFormat pixel_format, tProgressCallback * callback,
					tDictionaryInstance * dict)
{
	// load TLG v6.0 lossless/near-lossless compressed graphic
static DWORD total = 0;
DWORD total_start = GetTickCount();
	unsigned char buf[12];

	src.ReadBuffer(buf, 4);

	risse_int colors = buf[0]; // color component count

	if(colors != 1 && colors != 4 && colors != 3)
		tIOExceptionClass::Throw(
			tString(RISSE_WS_TR("error on reading TLG6: unsupported color count: %1"),
					tString::AsString((risse_int64)colors)));

	if(buf[1] != 0) // data flag
		tIOExceptionClass::Throw(
			tString(RISSE_WS_TR("error on reading TLG6: data flag must be 0 (any flags are not yet supported)")));

	if(buf[2] != 0) // color type  (currently always zero)
		tIOExceptionClass::Throw(
			tString(RISSE_WS_TR("error on reading TLG6: unsupported color type: %1"),
				tString::AsString((risse_int64)buf[1])));

	if(buf[3] != 0) // external golomb table (currently always zero)
		tIOExceptionClass::Throw(
			tString(RISSE_WS_TR("error on reading TLG6: external golomb bit length table is not yet supported")));

	risse_int width, height;

	width = src.ReadI32LE();
	height = src.ReadI32LE();

	risse_int max_bit_length;

	max_bit_length = src.ReadI32LE();

static DWORD dim = 0;
static DWORD io = 0;
static DWORD golomb = 0;
static DWORD filter = 0;

DWORD dim_start = GetTickCount();
	// set destination size
	SetDimensions(width, height, tPixel::pfARGB32);
dim += GetTickCount() - dim_start;

	// compute some values
	risse_int x_block_count = (risse_int)((width - 1)/ TLG6_W_BLOCK_SIZE) + 1;
	risse_int y_block_count = (risse_int)((height - 1)/ TLG6_H_BLOCK_SIZE) + 1;
	risse_int main_count = width / TLG6_W_BLOCK_SIZE;
	risse_int fraction = width -  main_count * TLG6_W_BLOCK_SIZE;

	// allocate memories
	risse_uint8 *bit_pool = (risse_uint8 *)AlignedMallocAtomicCollectee(max_bit_length / 8 + 5, 4);
	risse_uint32 *pixelbuf = (risse_uint32 *)AlignedMallocAtomicCollectee(
		sizeof(risse_uint32) * width * TLG6_H_BLOCK_SIZE + 1, 4);
	risse_uint8 *filter_types = (risse_uint8 *)AlignedMallocAtomicCollectee(
		x_block_count * y_block_count, 4);
	risse_uint32 *zeroline = (risse_uint32 *)AlignedMallocAtomicCollectee(width * sizeof(risse_uint32), 4);
	risse_uint8 *LZSS_text = (risse_uint8*)AlignedMallocAtomicCollectee(4096, 4);

	// initialize zero line (virtual y=-1 line)
	for(risse_int x = 0; x < width; x++) zeroline[x]=(colors==3?0xff000000:0x00000000);
		// 0xff000000 for colors=3 makes alpha value opaque

	// initialize LZSS text (used by chroma filter type codes)
	{
		risse_uint32 *p = (risse_uint32*)LZSS_text;
		for(risse_uint32 i = 0; i < 32*0x01010101; i+=0x01010101)
		{
			for(risse_uint32 j = 0; j < 16*0x01010101; j+=0x01010101)
				p[0] = i, p[1] = j, p += 2;
		}
	}

	// read chroma filter types.
	// chroma filter types are compressed via LZSS as used by TLG5.
	{
		risse_int inbuf_size = src.ReadI32LE();
		risse_uint8* inbuf = (risse_uint8*)AlignedMallocAtomicCollectee(inbuf_size, 4);
		src.ReadBuffer(inbuf, inbuf_size);

		TLG5DecompressSlide(filter_types, inbuf, inbuf_size,
			LZSS_text, 0);
	}

	// for each horizontal block group ...
	risse_uint32 *prevline = zeroline;
	for(risse_int y = 0; y < height; y += TLG6_H_BLOCK_SIZE)
	{
		risse_int ylim = y + TLG6_H_BLOCK_SIZE;
		if(ylim >= height) ylim = height;

		risse_int pixel_count = (ylim - y) * width;

		// decode values
		for(risse_int c = 0; c < colors; c++)
		{
			// read bit length
DWORD io_start = GetTickCount();
			risse_int bit_length = src.ReadI32LE();
io += GetTickCount() - io_start;

			// get compress method
			int method = (bit_length >> 30)&3;
			bit_length &= 0x3fffffff;

			// compute byte length
			risse_int byte_length = bit_length / 8;
			if(bit_length % 8) byte_length++;

			// read source from input
io_start = GetTickCount();
			src.ReadBuffer(bit_pool, byte_length);
io += GetTickCount() - io_start;

			// decode values
			// two most significant bits of bitlength are
			// entropy coding method;
			// 00 means Golomb method,
			// 01 means Gamma method (not yet suppoted),
			// 10 means modified LZSS method (not yet supported),
			// 11 means raw (uncompressed) data (not yet supported).

DWORD golomb_start = GetTickCount();
			switch(method)
			{
			case 0:
				if(c == 0 && colors != 1)
					TLG6DecodeGolombValuesForFirst((risse_int8*)pixelbuf,
						pixel_count, bit_pool);
				else
					TLG6DecodeGolombValues((risse_int8*)pixelbuf + c,
						pixel_count, bit_pool);
				break;
			default:
				tIOExceptionClass::Throw(
					tString(RISSE_WS_TR("error on reading TLG6: unsupported entropy coding method")));
			}
golomb += GetTickCount() - golomb_start;
		}

		// for each line
		unsigned char * ft =
			filter_types + (y / TLG6_H_BLOCK_SIZE)*x_block_count;
		int skipbytes = (ylim-y)*TLG6_W_BLOCK_SIZE;

		for(int yy = y; yy < ylim; yy++)
		{
			risse_uint32* curline = (risse_uint32*)StartLines(yy, 1, NULL);

			int dir = (yy&1)^1;
			int oddskip = ((ylim - yy -1) - (yy-y));
DWORD filter_start = GetTickCount();
			if(main_count)
			{
				int start =
					((width < TLG6_W_BLOCK_SIZE) ? width : TLG6_W_BLOCK_SIZE) *
						(yy - y);
				TLG6DecodeLine(
					prevline,
					curline,
					width,
					main_count,
					ft,
					skipbytes,
					pixelbuf + start, colors==3?0xff000000:0, oddskip, dir);
			}

			if(main_count != x_block_count)
			{
				int ww = fraction;
				if(ww > TLG6_W_BLOCK_SIZE) ww = TLG6_W_BLOCK_SIZE;
				int start = ww * (yy - y);
				TLG6DecodeLineGeneric(
					prevline,
					curline,
					width,
					main_count,
					x_block_count,
					ft,
					skipbytes,
					pixelbuf + start, colors==3?0xff000000:0, oddskip, dir);
			}
filter += GetTickCount() - filter_start;

			DoneLines();
			prevline = curline;
		}

	}
total += GetTickCount() - total_start;

fprintf(stderr, "total %dms\n", total);
fprintf(stderr, "dim %dms\n", dim);
fprintf(stderr, "io %dms\n", io);
fprintf(stderr, "golomb %dms\n", golomb);
fprintf(stderr, "filter %dms\n", filter);

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTLGImageDecoder::ProcessTLG(tStreamAdapter & src,
					tPixel::tFormat pixel_format, tProgressCallback * callback,
					tDictionaryInstance * dict)
{
	// read header
	unsigned char mark[12];
	src.ReadBuffer(mark, 11);

	// check for TLG raw data
	if(!memcmp("TLG5.0\x00raw\x1a\x00", mark, 11))
		ProcessTLG5(src, pixel_format, callback, dict);
	else if(!memcmp("TLG6.0\x00raw\x1a\x00", mark, 11))
		ProcessTLG6(src, pixel_format, callback, dict);
	else
		tIOExceptionClass::Throw(
			RISSE_WS_TR("error on reading TLG: invalid TLG header or unsupported TLG version"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTLGImageDecoder::Process(tStreamInstance * stream,
					tPixel::tFormat pixel_format, tProgressCallback * callback,
					tDictionaryInstance * dict)
{
	tStreamAdapter src(stream);

	// dict から読み取るべき内容は無し。dict をクリアする
	if(dict)
		dict->Invoke(tSS<'c','l','e','a','r'>());



	// read header
	risse_uint64 origin = src.GetPosition();
	unsigned char mark[12];
	src.ReadBuffer(mark, 11);

	// check for TLG0.0 sds
	if(!memcmp("TLG0.0\x00sds\x1a\x00", mark, 11))
	{
		// read TLG0.0 Structured Data Stream

		// TLG0.0 SDS tagged data is simple "NAME=VALUE," string;
		// Each NAME and VALUE have length:content expression.
		// eg: 4:LEFT=2:20,3:TOP=3:120,4:TYPE=1:3,
		// The last ',' cannot be ommited.
		// Each string (name and value) must be encoded in utf-8.

		// read raw data size
		risse_uint32 rawlen = src.ReadI32LE();

		// try to load TLG raw data
		ProcessTLG(src, pixel_format, callback, dict);

		// seek to meta info data point
		src.SetPosition(origin + rawlen + 11 + 4);

		// read tag data only if dict is given
		while(dict)
		{
			char chunkname[4];
			if(4 != src.Read(chunkname, 4)) break;
				// cannot read more
			risse_uint32 chunksize = src.ReadI32LE();
			if(!memcmp(chunkname, "tags", 4))
			{
				// tag information
				char *tag = NULL;
				char *name = NULL;
				char *value = NULL;
				try
				{
					tag = new char [chunksize + 1];
					src.ReadBuffer(tag, chunksize);
					tag[chunksize] = 0;

					const char *tagp = tag;
					const char *tagp_lim = tag + chunksize;
					while(tagp < tagp_lim)
					{
						risse_uint namelen = 0;
						while(*tagp >= '0' && *tagp <= '9')
							namelen = namelen * 10 + *tagp - '0', tagp++;
						if(*tagp != ':') tIOExceptionClass::Throw(
							RISSE_WS_TR("error on reading TLG: malformed TLG SDS tag structure, missing colon after name length"));
						tagp ++;
						name = new char [namelen + 1];
						memcpy(name, tagp, namelen);
						name[namelen] = '\0';
						tagp += namelen;
						if(*tagp != '=') tIOExceptionClass::Throw(
							RISSE_WS_TR("error on reading TLG: malformed TLG SDS tag structure, missing equals after name"));
						tagp++;
						risse_uint valuelen = 0;
						while(*tagp >= '0' && *tagp <= '9')
							valuelen = valuelen * 10 + *tagp - '0', tagp++;
						if(*tagp != ':') tIOExceptionClass::Throw(
							RISSE_WS_TR("error on reading TLG: malformed TLG SDS tag structure, missing colon after value length"));
						tagp++;
						value = new char [valuelen + 1];
						memcpy(value, tagp, valuelen);
						value[valuelen] = '\0';
						tagp += valuelen;
						if(*tagp != ',') tIOExceptionClass::Throw(
							RISSE_WS_TR("error on reading TLG: malformed TLG SDS tag structure, missing comma after a tag"));
						tagp++;

						// insert into name-value pairs ...
						dict->Invoke(tSS<'[',']','='>(), tVariant(tString(value)), tVariant(tString(name)));

						delete [] name, name = NULL;
						delete [] value, value = NULL;
					}
				}
				catch(...)
				{
					if(tag) delete [] tag;
					if(name) delete [] name;
					if(value) delete [] value;
					throw;
				}

				if(tag) delete [] tag;
				if(name) delete [] name;
				if(value) delete [] value;
			}
			else
			{
				// skip the chunk
				src.SetPosition(origin + src.GetPosition() + chunksize);
			}
		} // while

	}
	else
	{
		src.SetPosition(origin); // rewind

		// try to load TLG raw data
		ProcessTLG(src, pixel_format, callback, dict);
	}
}
//---------------------------------------------------------------------------

















//---------------------------------------------------------------------------
//! @brief		TLGイメージコーデックファクトリ
//---------------------------------------------------------------------------
class tTLGImageCodecFactory : public tImageDecoderFactory, public tImageEncoderFactory,
								public singleton_base<tTLGImageCodecFactory>
{
	virtual tImageDecoder * CreateDecoder() { return new tTLGImageDecoder; }
	virtual tImageEncoder * CreateEncoder() { return NULL /*new tTLGImageEncoder*/; }
public:
	//! @brief		コンストラクタ
	tTLGImageCodecFactory()
	{
		TLG6MakeTable(); // テーブルを作成

		tImageCodecFactoryManager::instance()->Register(tSS<'.','t','l','g'>(), (tImageDecoderFactory*)this);
		tImageCodecFactoryManager::instance()->Register(tSS<'.','t','l','g','5'>(), (tImageDecoderFactory*)this);
		tImageCodecFactoryManager::instance()->Register(tSS<'.','t','l','g','6'>(), (tImageDecoderFactory*)this);
		tImageCodecFactoryManager::instance()->Register(tSS<'.','t','l','g'>(), (tImageEncoderFactory*)this);
		tImageCodecFactoryManager::instance()->Register(tSS<'.','t','l','g','5'>(), (tImageEncoderFactory*)this);
		tImageCodecFactoryManager::instance()->Register(tSS<'.','t','l','g','6'>(), (tImageEncoderFactory*)this);
	}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa


