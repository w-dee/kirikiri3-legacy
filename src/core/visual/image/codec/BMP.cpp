//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief BMP形式コーデック
//---------------------------------------------------------------------------
#include "prec.h"
#include "visual/image/codec/BMP.h"
#include "base/exception/RisaException.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(33256,33111,47197,17413,6573,47168,12670,30341);
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
#ifndef BI_RGB // avoid re-define error on Win32
	#define BI_RGB			0
	#define BI_RLE8			1
	#define BI_RLE4			2
	#define BI_BITFIELDS	3
#endif


struct tBitField
{
	risse_uint32	Mask; // mask value for
	int				Shift; // shift value
	int				Bits; // bit count
	risse_uint32	Scale; // scale

	tBitField() {
		Mask = 0;
		Shift = 0;
		Bits = 0;
		Scale = 1;
	}
	void MaskToShiftBits();
};
void tBitField::MaskToShiftBits()
{
	// Mask から Shift, Bits などへと変換する
	Shift = -1;
	Bits = 0;
	for(int i =0; i < 32; i++)
	{
		if((1L<<i)&Mask)
		{
			if(Shift == -1) Shift = i;
			Bits ++;
		}
	}
	if(Bits > 8)
	{
		// Bits が 8 より大きい場合は余分な分を捨てるようにする
		Shift += (8 - Bits);
		Bits = 8;
	}
	// scale を計算
	if(Bits != 0)
		Scale = 0xffff / ((1<<Bits) - 1);
	else
		Scale = 0;
}


struct tBitFields
{
	tBitField R;
	tBitField G;
	tBitField B;
	tBitField A;

	void MaskToShiftBits() {
		R.MaskToShiftBits();
		G.MaskToShiftBits();
		B.MaskToShiftBits();
		A.MaskToShiftBits();
	}
};

struct RISSE_WIN_BITMAPFILEHEADER
{
	risse_uint16	bfType;
	risse_uint32	bfSize;
	risse_uint16	bfReserved1;
	risse_uint16	bfReserved2;
	risse_uint32	bfOffBits;
};
struct RISSE_WIN_BITMAPINFOHEADER
{
	risse_uint32	biSize;
	risse_int32		biWidth;
	risse_int32		biHeight;
	risse_uint16	biPlanes;
	risse_uint16	biBitCount;
	risse_uint32	biCompression;
	risse_uint32	biSizeImage;
	risse_uint32	biXPelsPerMeter;
	risse_uint32	biYPelsPerMeter;
	risse_uint32	biClrUsed;
	risse_uint32	biClrImportant;

	tBitFields		BitFields;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static void Expand1BitTo32BitPal(RISSE_RESTRICT risse_uint32 *dest, RISSE_RESTRICT const risse_uint8 *buf, risse_size len, const risse_uint32 *pal)
{
	risse_uint32 p[2];
	risse_uint32 *d=dest, *dlim;
	risse_uint8 b;

	p[0] = pal[0], p[1] = pal[1];
	dlim = dest + len-7;
	while(d < dlim)
	{
		b = *(buf++);
		d[0] = p[(risse_uint)(b&(risse_uint)0x80)>>7];
		d[1] = p[(risse_uint)(b&(risse_uint)0x40)>>6];
		d[2] = p[(risse_uint)(b&(risse_uint)0x20)>>5];
		d[3] = p[(risse_uint)(b&(risse_uint)0x10)>>4];
		d[4] = p[(risse_uint)(b&(risse_uint)0x08)>>3];
		d[5] = p[(risse_uint)(b&(risse_uint)0x04)>>2];
		d[6] = p[(risse_uint)(b&(risse_uint)0x02)>>1];
		d[7] = p[(risse_uint)(b&(risse_uint)0x01)   ];
		d += 8;
	}
	dlim = dest + len;
	b = *buf;
	while(d<dlim)
	{
		*(d++) = (b&0x80) ? p[1] : p[0];
		b<<=1;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static void Expand4BitTo32BitPal(RISSE_RESTRICT risse_uint32 *dest, RISSE_RESTRICT const risse_uint8 *buf, risse_size len, const risse_uint32 *pal)
{
	risse_uint32 *d=dest, *dlim;
	risse_uint8 b;

	dlim = dest + (len & ~1);
	while(d < dlim)
	{
		b = *(buf++);
		d[0] = pal[(b&0xf0)>>4];
		d[1] = pal[b&0x0f];
		d += 2;
	}
	if(len & 1)
	{
		b = *buf;
		*d = pal[(b&0xf0)>>4];
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static void Expand8BitTo32BitPal(RISSE_RESTRICT risse_uint32 *dest, RISSE_RESTRICT const risse_uint8 *buf, risse_size len, const risse_uint32 *pal)
{
	{
		risse_size ___index = 0;
		if(len > (8-1))
		{
			len -= (8-1);

			while(___index < len)
			{
				dest[(___index+0)] = pal[buf[(___index+0)]];
				dest[(___index+1)] = pal[buf[(___index+1)]];
				dest[(___index+2)] = pal[buf[(___index+2)]];
				dest[(___index+3)] = pal[buf[(___index+3)]];
				dest[(___index+4)] = pal[buf[(___index+4)]];
				dest[(___index+5)] = pal[buf[(___index+5)]];
				dest[(___index+6)] = pal[buf[(___index+6)]];
				dest[(___index+7)] = pal[buf[(___index+7)]];
				___index += 8;
			}

			len += (8-1);
		}

		while(___index < len)
		{
			dest[___index] = pal[buf[___index]];
			___index ++;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static void Convert24BitTo32Bit(RISSE_RESTRICT risse_uint32 *dest, RISSE_RESTRICT const risse_uint8 *buf, risse_size len)
{
	risse_uint32 *slimglim = dest + len;
	risse_uint32 *slimglims = slimglim - 7;
	while(dest < slimglims)
	{
#if RISSE_HOST_IS_BIG_ENDIAN
		dest[0] = 0xff000000 + buf[0] + (buf[1]<<8) + (buf[2]<<16);
		dest[1] = 0xff000000 + buf[3] + (buf[4]<<8) + (buf[5]<<16);
		dest[2] = 0xff000000 + buf[6] + (buf[7]<<8) + (buf[8]<<16);
		dest[3] = 0xff000000 + buf[9] + (buf[10]<<8) + (buf[11]<<16);
		dest += 4;
		buf += 12;
		dest[0] = 0xff000000 + buf[0] + (buf[1]<<8) + (buf[2]<<16);
		dest[1] = 0xff000000 + buf[3] + (buf[4]<<8) + (buf[5]<<16);
		dest[2] = 0xff000000 + buf[6] + (buf[7]<<8) + (buf[8]<<16);
		dest[3] = 0xff000000 + buf[9] + (buf[10]<<8) + (buf[11]<<16);
		dest += 4;
		buf += 12;
#else
		risse_uint32 a = *(risse_uint32*)buf, b;
		risse_uint32 c = *(risse_uint32*)(buf+12), d;
		dest[0] = 0xff000000 + (a & 0x00ffffff);
		dest[4] = 0xff000000 + (c & 0x00ffffff);
		b = *(risse_uint32*)(buf+4);
		d = *(risse_uint32*)(buf+16);
		dest[1] = 0xff000000 + ((a >> 24) + ((b & 0xffff)<<8));
		dest[5] = 0xff000000 + ((c >> 24) + ((d & 0xffff)<<8));
		a = *(risse_uint32*)(buf+8);
		c = *(risse_uint32*)(buf+20);
		dest[2] = 0xff000000 + ((b >> 16) + ((a & 0xff)<<16));
		dest[6] = 0xff000000 + ((d >> 16) + ((c & 0xff)<<16));
		dest[3] = 0xff000000 + (a >> 8);
		dest[7] = 0xff000000 + (c >> 8);
		dest += 8;
		buf += 24;
#endif
	}
	while(dest < slimglim)
	{
#if RISSE_HOST_IS_BIG_ENDIAN
		*(dest++) = 0xff000000 + buf[0] + (buf[1]<<8) + (buf[2]<<16);
#else
		*(dest++) = 0xff000000 + buf[0] + (buf[1]<<8) + (buf[2]<<16);
#endif
		buf += 3;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static void Convert32BitTo32Bit(RISSE_RESTRICT risse_uint32 *dest, RISSE_RESTRICT const risse_uint32 *buf, risse_size len)
{
	{
		risse_size ___index = 0;
		if(len > (4-1))
		{
			len -= (4-1);

			while(___index < len)
			{
				{
				#if TJS_HOST_IS_BIG_ENDIAN
					risse_uint32 d = buf[(___index+0)];
					dest[(___index+0)] = ((d&0xff)<<24) + ((d&0xff00)<<8) +  ((d&0xff0000)>>8) + ((d&0xff000000)>>24);
				#else
					risse_uint32 d = buf[(___index+0)];
					dest[(___index+0)] = d;
				#endif
				}
				{
				#if TJS_HOST_IS_BIG_ENDIAN
					risse_uint32 d = buf[(___index+1)];
					dest[(___index+1)] = ((d&0xff)<<24) + ((d&0xff00)<<8) +  ((d&0xff0000)>>8) + ((d&0xff000000)>>24);
				#else
					risse_uint32 d = buf[(___index+1)];
					dest[(___index+1)] = d;
				#endif
				}
				{
				#if TJS_HOST_IS_BIG_ENDIAN
					risse_uint32 d = buf[(___index+2)];
					dest[(___index+2)] = ((d&0xff)<<24) + ((d&0xff00)<<8) +  ((d&0xff0000)>>8) + ((d&0xff000000)>>24);
				#else
					risse_uint32 d = buf[(___index+2)];
					dest[(___index+2)] = d;
				#endif
				}
				{
				#if TJS_HOST_IS_BIG_ENDIAN
					risse_uint32 d = buf[(___index+3)];
					dest[(___index+3)] = ((d&0xff)<<24) + ((d&0xff00)<<8) +  ((d&0xff0000)>>8) + ((d&0xff000000)>>24);
				#else
					risse_uint32 d = buf[(___index+3)];
					dest[(___index+3)] = d;
				#endif
				}
				___index += 4;
			}

			len += (4-1);
		}

		while(___index < len)
		{
			{
			#if TJS_HOST_IS_BIG_ENDIAN
				risse_uint32 d = buf[___index];
				dest[___index] = ((d&0xff)<<24) + ((d&0xff00)<<8) +  ((d&0xff0000)>>8) + ((d&0xff000000)>>24);
			#else
				risse_uint32 d = buf[___index];
				dest[___index] = d;
			#endif
			}
			___index ++;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static void Convert16BitFieldsTo32Bit(
				const tBitFields & fields,
				RISSE_RESTRICT risse_uint32* scanline,
				RISSE_RESTRICT const risse_uint16* buf, risse_size length)
{
	risse_uint16	r_mask	= static_cast<risse_uint16>(fields.R.Mask);
	int				r_shift	= fields.R.Shift;
	risse_uint32	r_scale	= fields.R.Scale;
	risse_uint16	g_mask	= static_cast<risse_uint16>(fields.G.Mask);
	int				g_shift	= fields.G.Shift;
	risse_uint32	g_scale	= fields.G.Scale;
	risse_uint16	b_mask	= static_cast<risse_uint16>(fields.B.Mask);
	int				b_shift	= fields.B.Shift;
	risse_uint32	b_scale	= fields.B.Scale;
	risse_uint16	a_mask	= static_cast<risse_uint16>(fields.A.Mask);
	int				a_shift	= fields.A.Shift;
	risse_uint32	a_scale	= fields.A.Scale;

	risse_uint32	a_opaque = a_mask ? 0 : 0xff000000L;
		// alpha を持たない場合は常に 0xff000000

	for(risse_size i = 0; i < length ; i++)
	{
		risse_uint16 v = buf[i];
#if TJS_HOST_IS_BIG_ENDIAN
		v = (v >> 8) + (v << 8);
#endif
		scanline[i] =
			((((v & r_mask) >> r_shift) * r_scale >> 8) << 16) +
			((((v & g_mask) >> g_shift) * g_scale >> 8) <<  8) +
			((((v & b_mask) >> b_shift) * b_scale >> 8)      ) +
			((((v & a_mask) >> a_shift) * a_scale >> 8) << 24) +
			a_opaque ;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static void Convert32BitFieldsTo32Bit(
				const tBitFields & fields,
				RISSE_RESTRICT risse_uint32* scanline,
				RISSE_RESTRICT const risse_uint32* buf, risse_size length)
{
	risse_uint32	r_mask	= fields.R.Mask;
	int				r_shift	= fields.R.Shift;
	risse_uint32	r_scale	= fields.R.Scale;
	risse_uint32	g_mask	= fields.G.Mask;
	int				g_shift	= fields.G.Shift;
	risse_uint32	g_scale	= fields.G.Scale;
	risse_uint32	b_mask	= fields.B.Mask;
	int				b_shift	= fields.B.Shift;
	risse_uint32	b_scale	= fields.B.Scale;
	risse_uint32	a_mask	= fields.A.Mask;
	int				a_shift	= fields.A.Shift;
	risse_uint32	a_scale	= fields.A.Scale;

	risse_uint32	a_opaque = a_mask ? 0 : 0xff000000L;
		// alpha を持たない場合は常に 0xff000000

	for(risse_size i = 0; i < length ; i++)
	{
		risse_uint32 v = buf[i];
#if TJS_HOST_IS_BIG_ENDIAN
		v = ((v&0xff)<<24) + ((v&0xff00)<<8) +  ((v&0xff0000)>>8) + ((v&0xff000000)>>24);
#endif
		scanline[i] =
			((((v & r_mask) >> r_shift) * r_scale >> 8) << 16) +
			((((v & g_mask) >> g_shift) * g_scale >> 8) <<  8) +
			((((v & b_mask) >> b_shift) * b_scale >> 8)      ) +
			((((v & a_mask) >> a_shift) * a_scale >> 8) << 24) +
			a_opaque ;
	}
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
#define RISSE_BMP_READ_LINE_MAX 8
void tBMPImageDecoder::InternalLoadBMP(tStreamAdapter src,
					tPixel::tFormat pixel_format, tProgressCallback * callback,
					tDictionaryInstance * dict, RISSE_WIN_BITMAPINFOHEADER & bi,
					risse_uint8 * palsrc)
{
	// mostly taken from kirikiri2

	// TODO: only checked on Win32 platform
	int bitcount = static_cast<int>(bi.biBitCount);

	if(bi.biSize == 12)
	{
		// OS/2
		bi.biCompression = BI_RGB;
		bi.biClrUsed = 1 << bi.biBitCount;
	}

	switch(bi.biCompression)
	{
	case BI_RGB:
	case BI_BITFIELDS:
		break; // use default

	default:
		tIOExceptionClass::Throw(RISSE_WS_TR("compressed BMP not supported"));
	}

	// load palette
	risse_uint32 palette[256];   // (msb) argb (lsb)
	if(bitcount <= 8)
	{
		if(bi.biClrUsed == 0) bi.biClrUsed = 1 << bitcount ;
		if(bi.biSize == 12)
		{
			// read OS/2 palette
			for(risse_uint i = 0; i < bi.biClrUsed; i++)
			{
				palette[i] = palsrc[0] + (palsrc[1]<<8) + (palsrc[2]<<16) +
					0xff000000;
				palsrc += 3;
			}
		}
		else
		{
			// read Windows palette
			for(risse_uint i = 0; i<bi.biClrUsed; i++)
			{
				palette[i] = palsrc[0] + (palsrc[1]<<8) + (palsrc[2]<<16) +
					0xff000000;
					// we assume here that the palette's unused segment is useless.
					// fill it with 0xff ( = completely opaque )
				palsrc += 4;
			}
		}
	}

	risse_int height;
	height = bi.biHeight<0?-bi.biHeight:bi.biHeight;
		// positive value of bi.biHeight indicates top-down DIB

	SetDimensions(bi.biWidth, height, tPixel::pfARGB32);

	risse_int pitch;
	pitch = (((bi.biWidth * bitcount) + 31) & ~31) /8;
	risse_uint8 *readbuf =
		static_cast<risse_uint8 *>(
			AlignedMallocAtomicCollectee(pitch * RISSE_BMP_READ_LINE_MAX, 4));
	risse_uint8 *buf;
	risse_int bufremain = 0;

	// process per a line
	risse_int src_y = 0;
	risse_int dest_y;
	if(bi.biHeight>0) dest_y = bi.biHeight-1; else dest_y = 0;

	for(; src_y < height; src_y++)
	{
		if(bufremain == 0)
		{
			if(callback) callback->CallOnProgress(height, src_y);

			risse_int remain = height - src_y;
			risse_int read_lines = remain > RISSE_BMP_READ_LINE_MAX ?
				RISSE_BMP_READ_LINE_MAX : remain;
			src.ReadBuffer(readbuf, pitch * read_lines);
			bufremain = read_lines;
			buf = readbuf;
		}

		risse_offset pitch = 0;
		void *scanline = StartLines(dest_y, 1, pitch);

		switch(bitcount)
		{
			// convert pixel format
		case 1:
			Expand1BitTo32BitPal(
				(risse_uint32*)scanline,
				(risse_uint8*)buf, bi.biWidth, palette);
			break;

		case 4:
			Expand4BitTo32BitPal(
				(risse_uint32*)scanline,
				(risse_uint8*)buf, bi.biWidth, palette);
			break;

		case 8:
			Expand8BitTo32BitPal(
				(risse_uint32*)scanline,
				(risse_uint8*)buf, bi.biWidth, palette);
			break;

		case 15:
		case 16:
			Convert16BitFieldsTo32Bit(
				bi.BitFields,
				(risse_uint32*)scanline,
				(risse_uint16*)buf, bi.biWidth);
			break;

		case 24:
			Convert24BitTo32Bit(
				(risse_uint32*)scanline,
				(risse_uint8*)buf, bi.biWidth);
			break;

		case 32:
			Convert32BitFieldsTo32Bit(
				bi.BitFields,
				(risse_uint32*)scanline,
				(risse_uint32*)buf, bi.biWidth);
			break;
		}

		DoneLines(); // image was written

		if(bi.biHeight>0) dest_y--; else dest_y++;
		buf += pitch;
		bufremain--;
	}
}
//---------------------------------------------------------------------------
void tBMPImageDecoder::Process(tStreamInstance * stream,
					tPixel::tFormat pixel_format, tProgressCallback * callback,
					tDictionaryInstance * dict)
{
	// Windows BMP Loader
	// mostly taken ( but totally re-written ) from SDL,
	// http://www.libsdl.org/
	tStreamAdapter src(stream);

	// dict から読み取るべき内容は無し。dict をクリアする
	if(dict)
		dict->Invoke(tSS<'c','l','e','a','r'>());

	// TODO: only checked in Win32 platform
	risse_uint64 firstpos = src.GetPosition();

	// read BITMAPFILEHEADER

	// check the magic
	risse_uint8 magic[2];
	src.ReadBuffer(magic, 2);
	if(magic[0] != 0x42 || magic[1] != 0x4d)
		tIOExceptionClass::Throw(RISSE_WS_TR("not a Windows BMP file nor an OS/2 BMP file"));

	// read the BITMAPFILEHEADER
	RISSE_WIN_BITMAPFILEHEADER bf;
	bf.bfSize = src.ReadI32LE();
	bf.bfReserved1 = src.ReadI16LE();
	bf.bfReserved2 = src.ReadI16LE();
	bf.bfOffBits = src.ReadI32LE();

	// read the BITMAPINFOHEADER
	RISSE_WIN_BITMAPINFOHEADER bi;
	bi.biSize = src.ReadI32LE();
	bool bitfields_read = false;
	if(bi.biSize == 12)
	{
		// OS/2 Bitmap
		memset(&bi, 0, sizeof(bi));
		bi.biWidth = (risse_uint32)src.ReadI16LE();
		bi.biHeight = (risse_uint32)src.ReadI16LE();
		bi.biPlanes = src.ReadI16LE();
		bi.biBitCount = src.ReadI16LE();
		bi.biCompression = BI_RGB;
		bi.biClrUsed = 1 << bi.biBitCount;
	}
	else if(bi.biSize == 40 || (bi.biSize >= 56 && bi.biSize <= 64))
	{
		// Windows Bitmap
		bi.biWidth = src.ReadI32LE();
		bi.biHeight = src.ReadI32LE();
		bi.biPlanes = src.ReadI16LE();
		bi.biBitCount = src.ReadI16LE();
		bi.biCompression = src.ReadI32LE();
		bi.biSizeImage = src.ReadI32LE();
		bi.biXPelsPerMeter = src.ReadI32LE();
		bi.biYPelsPerMeter = src.ReadI32LE();
		bi.biClrUsed = src.ReadI32LE();
		bi.biClrImportant = src.ReadI32LE();

		if(bi.biSize == 40 && bi.biCompression == BI_BITFIELDS)
		{
			bi.BitFields.R.Mask = src.ReadI32LE();
			bi.BitFields.G.Mask = src.ReadI32LE();
			bi.BitFields.B.Mask = src.ReadI32LE();
			bitfields_read = true;
		}

		if(bi.biSize >= 56 && bi.biSize <= 64)
		{
			// bit fields
			bi.BitFields.R.Mask = src.ReadI32LE();
			bi.BitFields.G.Mask = src.ReadI32LE();
			bi.BitFields.B.Mask = src.ReadI32LE();
			bi.BitFields.A.Mask = src.ReadI32LE();
			bitfields_read = true;
		}
	}
	else
	{
		tIOExceptionClass::Throw(RISSE_WS_TR("non-supported version in BMP header"));
	}

	if(!bitfields_read)
	{
		switch(bi.biBitCount)
		{
		case 32:
			bi.BitFields.R.Mask = 0x00ff0000;
			bi.BitFields.G.Mask = 0x0000ff00;
			bi.BitFields.B.Mask = 0x000000ff;
			bi.BitFields.A.Mask = 0x00000000;
			break;
		case 16:
			bi.BitFields.R.Mask = 0x7c00;
			bi.BitFields.G.Mask = 0x03e0;
			bi.BitFields.B.Mask = 0x001f;
			bi.BitFields.A.Mask = 0x00000000;
			break;
		default:;
		}
	}

	bi.BitFields.MaskToShiftBits();

	// load palette
	risse_int palsize = (bi.biBitCount <= 8) ?
		((bi.biClrUsed == 0 ? (1<<bi.biBitCount) : bi.biClrUsed) *
		((bi.biSize == 12) ? 3:4)) : 0;  // bi.biSize == 12 ( OS/2 palette )
	risse_uint8 *palette = NULL;

	if(palsize)
	{
		palette = static_cast<risse_uint8*>(MallocAtomicCollectee(palsize));
		src.ReadBuffer(palette, palsize);
	}

	// seek to the first bitmap bits
	src.SetPosition(firstpos + bf.bfOffBits);

	// load rest
	InternalLoadBMP(src, pixel_format, callback, dict, bi, palette);
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
void tBMPImageEncoder::Process(tStreamInstance * stream,
					tProgressCallback * callback,
					tDictionaryInstance * dict)
{
	// 今のところ dict の '_type' は '24' (別名'R8G8B8') と '32'
	// (別名 'A8R8G8B8') を受け付ける
	// (デフォルトは 24)
	int pixel_bytes = 3;
	if(dict)
	{
		tVariant val = dict->Invoke(tSS<'[',']'>(), tVariant(tSS<'_','t','y','p','e'>()));
		if(!val.IsVoid())
		{
			tString str = val.operator tString();
			if(str == RISSE_WS("24") ||
				str == RISSE_WS("R8G8B8"))
				pixel_bytes = 3;
			else if(str == RISSE_WS("32") ||
				str == RISSE_WS("A8R8G8B8"))
				pixel_bytes = 4;
			else
				tIllegalArgumentExceptionClass::Throw(
					RISSE_WS_TR("unknown bitmap sub-type in '_type' parameter"));
		}
	}

	tStreamAdapter dest(stream);
	risse_size width = 0;
	risse_size height = 0;
	GetDimensions(&width, &height);

	// prepare header
	int bitfields_size = pixel_bytes == 3 ? 0: 16; // BITFIELDS のサイズ
	risse_size bmppitch = width * pixel_bytes;
	bmppitch = (((bmppitch - 1) >> 2) + 1) << 2;

	dest.WriteI16LE(0x4d42);  /* bfType */
	dest.WriteI32LE(
			14 + // BITMAPFILEHEADER
			40 + bitfields_size +// BITMAPINFOHEADER
			bmppitch * height); /* bfSize */
	dest.WriteI16LE(0); /* bfReserved1 */
	dest.WriteI16LE(0); /* bfReserved2 */
	dest.WriteI32LE(
			14 + // BITMAPFILEHEADER
			40 + bitfields_size + // BITMAPINFOHEADER
			0); /* bfOffBits */

	dest.WriteI32LE(40 + bitfields_size); /* biSize = sizeof(BITMAPINFOHEADER) */
	dest.WriteI32LE(width); /* biWidth */
	dest.WriteI32LE(height); /* biHeight */
	dest.WriteI16LE(1); /* biPlanes */
	dest.WriteI16LE(pixel_bytes * 8); /* biBitCount */
	dest.WriteI32LE(bitfields_size ? BI_BITFIELDS : BI_RGB); /* biCompression */
	dest.WriteI32LE(0); /* biSizeImage */
	dest.WriteI32LE(0); /* biXPelsPerMeter */
	dest.WriteI32LE(0); /* biYPelsPerMeter */
	dest.WriteI32LE(0); /* biClrUsed */
	dest.WriteI32LE(0); /* biClrImportant */
	if(bitfields_size == 16)
	{
		dest.WriteI32LE(0x00ff0000); // R Mask
		dest.WriteI32LE(0x0000ff00); // G Mask
		dest.WriteI32LE(0x000000ff); // B Mask
		dest.WriteI32LE(0xff000000); // A Mask
	}

	// write bitmap body
	void * buf = NULL;
	for(risse_offset y = height - 1; y >= 0; y --)
	{
		if(callback) callback->CallOnProgress(height, height - y);
		if(pixel_bytes == 4)
		{
			risse_offset pitch = 0;
			if(!buf) buf = MallocAtomicCollectee(pixel_bytes * width);
			void * inbuf = GetLines(NULL, y, 1, pitch, tPixel::pfARGB32);
			Convert32BitTo32Bit(
				static_cast<risse_uint32*>(buf),
				static_cast<risse_uint32*>(inbuf),
				width); // これはendiannessの変換も行う
		}
		else if(pixel_bytes == 3)
		{
			risse_offset pitch = 0;
			if(!buf) buf = MallocAtomicCollectee(pixel_bytes * width);
			const risse_uint32 *inbuf = static_cast<const risse_uint32 *>(
				GetLines(NULL, y, 1, pitch, tPixel::pfARGB32));
			risse_uint8 *destbuf = static_cast<risse_uint8*>(buf);
			// 32bpp を 24bpp に詰め直す(alphaは無視)
			for(risse_size x = 0; x < width; x++)
			{
				risse_uint32 px = *inbuf;
				destbuf[0] =  px        & 0xff;
				destbuf[1] = (px >>  8) & 0xff;
				destbuf[2] = (px >> 16) & 0xff;
				destbuf += 3;
				inbuf ++;
			}
		}
		dest.WriteBuffer(buf, bmppitch);
	}


}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief		BMPイメージコーデックファクトリ
//---------------------------------------------------------------------------
class tBMPImageCodecFactory : public tImageCodecFactory, public singleton_base<tBMPImageCodecFactory>
{
	virtual tImageDecoder * CreateDecoder() { return new tBMPImageDecoder; }
	virtual tImageEncoder * CreateEncoder() { return new tBMPImageEncoder; }
public:
	//! @brief		コンストラクタ
	tBMPImageCodecFactory()
	{
		tImageCodecFactoryManager::instance()->Register(tSS<'.','b','m','p'>(), this);
		tImageCodecFactoryManager::instance()->Register(tSS<'.','d','i','b'>(), this);
	}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa


