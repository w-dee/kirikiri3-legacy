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
static void Expand1BitTo8BitPal(RISSE_RESTRICT risse_uint8 *dest, RISSE_RESTRICT const risse_uint8 *buf, risse_size len, const risse_uint32 *pal)
{
	risse_uint8 p[2];
	risse_uint8 *d=dest, *dlim;
	risse_uint8 b;

	p[0] = pal[0]&0xff, p[1] = pal[1]&0xff;
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


/*
//---------------------------------------------------------------------------
static void Expand1BitTo8Bit(RISSE_RESTRICT risse_uint8 *dest, RISSE_RESTRICT const risse_uint8 *buf, risse_size len)
{
	risse_uint8 *d=dest, *dlim;
	risse_uint8 b;

	dlim = dest + len-7;
	while(d < dlim)
	{
		b = *(buf++);
		d[0] = (risse_uint8)((b&(risse_uint)0x80)>>7);
		d[1] = (risse_uint8)((b&(risse_uint)0x40)>>6);
		d[2] = (risse_uint8)((b&(risse_uint)0x20)>>5);
		d[3] = (risse_uint8)((b&(risse_uint)0x10)>>4);
		d[4] = (risse_uint8)((b&(risse_uint)0x08)>>3);
		d[5] = (risse_uint8)((b&(risse_uint)0x04)>>2);
		d[6] = (risse_uint8)((b&(risse_uint)0x02)>>1);
		d[7] = (risse_uint8)((b&(risse_uint)0x01)   );
		d += 8;
	}
	dlim = dest + len;
	b = *buf;
	while(d<dlim)
	{
		*(d++) = (b&0x80) ? 1 : 0;
		b<<=1;
	}
}
//---------------------------------------------------------------------------
*/


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
static void Expand4BitTo8BitPal(RISSE_RESTRICT risse_uint8 *dest, RISSE_RESTRICT const risse_uint8 *buf, risse_size len, const risse_uint32 *pal)
{
	risse_uint8 *d=dest, *dlim;
	risse_uint8 b;

	dlim = dest + (len & ~1);
	while(d < dlim)
	{
		b = *(buf++);
		d[0] = (risse_uint8)pal[(b&0xf0)>>4];
		d[1] = (risse_uint8)pal[b&0x0f];
		d += 2;
	}
	if(len & 1)
	{
		b = *buf;
		if(d<dlim) *d = (risse_uint8)pal[(b&0xf0)>>4];
	}
}
//---------------------------------------------------------------------------


/*
//---------------------------------------------------------------------------
static void Expand4BitTo8Bit(RISSE_RESTRICT risse_uint8 *dest, RISSE_RESTRICT const risse_uint8 *buf, risse_size len)
{
	risse_uint8 *d=dest, *dlim;
	risse_uint8 b;

	dlim = dest + (len & ~1);
	while(d < dlim)
	{
		b = *(buf++);
		d[0] = (risse_uint8)((b&0xf0)>>4);
		d[1] = (risse_uint8)(b&0x0f);
		d += 2;
	}
	if(len & 1)
	{
		b = *buf;
		if(d<dlim) *d = (risse_uint8)((b&0xf0)>>4);
	}
}
//---------------------------------------------------------------------------
*/


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
static void Expand8BitTo8BitPal(RISSE_RESTRICT risse_uint8 *dest, RISSE_RESTRICT const risse_uint8 *buf, risse_size len, const risse_uint32 *pal)
{
	{
		risse_size ___index = 0;
		if(len > (4-1))
		{
			len -= (4-1);

			while(___index < len)
			{
				dest[(___index+0)] = pal[buf[(___index+0)]]&0xff;
				dest[(___index+1)] = pal[buf[(___index+1)]]&0xff;
				dest[(___index+2)] = pal[buf[(___index+2)]]&0xff;
				dest[(___index+3)] = pal[buf[(___index+3)]]&0xff;
				___index += 4;
			}

			len += (4-1);
		}

		while(___index < len)
		{
			dest[___index] = pal[buf[___index]]&0xff;
			___index ++;
		}
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


/*
//---------------------------------------------------------------------------
static void TVPExpand8BitTo32BitGray(RISSE_RESTRICT risse_uint32 *dest, RISSE_RESTRICT const risse_uint8 *buf, risse_size len)
{
	risse_uint8 a, b;
	{
		risse_size ___index = 0;
		if(len > (8-1))
		{
			len -= (8-1);

			while(___index < len)
			{
				a = buf[(___index+(0*2))];
				b = buf[(___index+(0*2+1))];
				dest[(___index+(0*2))] = 0xff000000 + (a * 0x10101);
				dest[(___index+(0*2+1))] = 0xff000000 + (b * 0x10101);
				a = buf[(___index+(1*2))];
				b = buf[(___index+(1*2+1))];
				dest[(___index+(1*2))] = 0xff000000 + (a * 0x10101);
				dest[(___index+(1*2+1))] = 0xff000000 + (b * 0x10101);
				a = buf[(___index+(2*2))];
				b = buf[(___index+(2*2+1))];
				dest[(___index+(2*2))] = 0xff000000 + (a * 0x10101);
				dest[(___index+(2*2+1))] = 0xff000000 + (b * 0x10101);
				a = buf[(___index+(3*2))];
				b = buf[(___index+(3*2+1))];
				dest[(___index+(3*2))] = 0xff000000 + (a * 0x10101);
				dest[(___index+(3*2+1))] = 0xff000000 + (b * 0x10101);
				___index += 8;
			}

			len += (8-1);
		}

		while(___index < len)
		{
			a = buf[___index];;
			dest[___index] = 0xff000000 + (a * 0x10101);;
			___index ++;
		}
	}
}
//---------------------------------------------------------------------------
*/


//---------------------------------------------------------------------------
static void Convert15BitTo8Bit(RISSE_RESTRICT risse_uint8 *dest, RISSE_RESTRICT const risse_uint16 *buf, risse_size len)
{
	{
		risse_size ___index = 0;
		if(len > (4-1))
		{
			len -= (4-1);

			while(___index < len)
			{
				{
				#if RISSE_HOST_IS_BIG_ENDIAN
					risse_uint16 s = *(risse_uint8*)(buf+(___index+0)) << 8 + *((risse_uint8*)(buf+(___index+0))+1);
				#else
					risse_uint16 s = buf[(___index+0)];
				#endif
					dest[(___index+0)] =
						((s&0x7c00)*56+ (s&0x03e0)*(187<<5)+ (s&0x001f)*(21<<10)) >> 15;
				}
				{
				#if RISSE_HOST_IS_BIG_ENDIAN
					risse_uint16 s = *(risse_uint8*)(buf+(___index+1)) << 8 + *((risse_uint8*)(buf+(___index+1))+1);
				#else
					risse_uint16 s = buf[(___index+1)];
				#endif
					dest[(___index+1)] =
						((s&0x7c00)*56+ (s&0x03e0)*(187<<5)+ (s&0x001f)*(21<<10)) >> 15;
				}
				{
				#if RISSE_HOST_IS_BIG_ENDIAN
					risse_uint16 s = *(risse_uint8*)(buf+(___index+2)) << 8 + *((risse_uint8*)(buf+(___index+2))+1);
				#else
					risse_uint16 s = buf[(___index+2)];
				#endif
					dest[(___index+2)] =
						((s&0x7c00)*56+ (s&0x03e0)*(187<<5)+ (s&0x001f)*(21<<10)) >> 15;
				}
				{
				#if RISSE_HOST_IS_BIG_ENDIAN
					risse_uint16 s = *(risse_uint8*)(buf+(___index+3)) << 8 + *((risse_uint8*)(buf+(___index+3))+1);
				#else
					risse_uint16 s = buf[(___index+3)];
				#endif
					dest[(___index+3)] =
						((s&0x7c00)*56+ (s&0x03e0)*(187<<5)+ (s&0x001f)*(21<<10)) >> 15;
				}
				___index += 4;
			}

			len += (4-1);
		}

		while(___index < len)
		{
		#if RISSE_HOST_IS_BIG_ENDIAN
			risse_uint16 s = *(risse_uint8*)(buf+___index) << 8 + *((risse_uint8*)(buf+___index)+1);
		#else
			risse_uint16 s = buf[___index];
		#endif
			dest[___index] =
				((s&0x7c00)*56+ (s&0x03e0)*(187<<5)+ (s&0x001f)*(21<<10)) >> 15;
			___index ++;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static void Convert15BitTo32Bit(RISSE_RESTRICT risse_uint32 *dest, RISSE_RESTRICT const risse_uint16 *buf, risse_size len)
{
	{
		risse_size ___index = 0;
		if(len > (4-1))
		{
			len -= (4-1);

			while(___index < len)
			{
				{
				#if RISSE_HOST_IS_BIG_ENDIAN
					risse_uint16 s = *(risse_uint8*)(buf+(___index+0)) << 8 + *((risse_uint8*)(buf+(___index+0))+1);
				#else
					risse_uint16 s = buf[(___index+0)];
				#endif
					risse_int r = s&0x7c00;
					risse_int g = s&0x03e0;
					risse_int b = s&0x001f;
					dest[(___index+0)] = 0xff000000 +
						(r <<  9) + ((r&0x7000)<<4) +
						(g <<  6) + ((g&0x0380)<<1) +
						(b <<  3) + (b>>2);
				}
				{
				#if RISSE_HOST_IS_BIG_ENDIAN
					risse_uint16 s = *(risse_uint8*)(buf+(___index+1)) << 8 + *((risse_uint8*)(buf+(___index+1))+1);
				#else
					risse_uint16 s = buf[(___index+1)];
				#endif
					risse_int r = s&0x7c00;
					risse_int g = s&0x03e0;
					risse_int b = s&0x001f;
					dest[(___index+1)] = 0xff000000 +
						(r <<  9) + ((r&0x7000)<<4) +
						(g <<  6) + ((g&0x0380)<<1) +
						(b <<  3) + (b>>2);
				}
				{
				#if RISSE_HOST_IS_BIG_ENDIAN
					risse_uint16 s = *(risse_uint8*)(buf+(___index+2)) << 8 + *((risse_uint8*)(buf+(___index+2))+1);
				#else
					risse_uint16 s = buf[(___index+2)];
				#endif
					risse_int r = s&0x7c00;
					risse_int g = s&0x03e0;
					risse_int b = s&0x001f;
					dest[(___index+2)] = 0xff000000 +
						(r <<  9) + ((r&0x7000)<<4) +
						(g <<  6) + ((g&0x0380)<<1) +
						(b <<  3) + (b>>2);
				}
				{
				#if RISSE_HOST_IS_BIG_ENDIAN
					risse_uint16 s = *(risse_uint8*)(buf+(___index+3)) << 8 + *((risse_uint8*)(buf+(___index+3))+1);
				#else
					risse_uint16 s = buf[(___index+3)];
				#endif
					risse_int r = s&0x7c00;
					risse_int g = s&0x03e0;
					risse_int b = s&0x001f;
					dest[(___index+3)] = 0xff000000 +
						(r <<  9) + ((r&0x7000)<<4) +
						(g <<  6) + ((g&0x0380)<<1) +
						(b <<  3) + (b>>2);
				}
				___index += 4;
			}

			len += (4-1);
		}

		while(___index < len)
		{
			{
			#if RISSE_HOST_IS_BIG_ENDIAN
				risse_uint16 s = *(risse_uint8*)(buf+___index) << 8 + *((risse_uint8*)(buf+___index)+1);
			#else
				risse_uint16 s = buf[___index];
			#endif
				risse_int r = s&0x7c00;
				risse_int g = s&0x03e0;
				risse_int b = s&0x001f;
				dest[___index] = 0xff000000 +
					(r <<  9) + ((r&0x7000)<<4) +
					(g <<  6) + ((g&0x0380)<<1) +
					(b <<  3) + (b>>2);
			}
			___index ++;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static inline unsigned char compose_grayscale(risse_int r, risse_int g, risse_int b)
{ return ((unsigned char)((((risse_int)(b)*19 + (risse_int)(g)*183 + (risse_int)(r)*54)>>8))); }
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static void Convert24BitTo8Bit(RISSE_RESTRICT risse_uint8 *dest, RISSE_RESTRICT const risse_uint8 *buf, risse_size len)
{
	risse_uint8 *slimglim = dest + len;
	risse_uint8 *slimglims = slimglim - 3;
	while(dest < slimglims)
	{
		dest[0] = compose_grayscale(buf[2], buf[1], buf[0]);
		dest[1] = compose_grayscale(buf[5], buf[4], buf[3]);
		dest[2] = compose_grayscale(buf[8], buf[7], buf[6]);
		dest[3] = compose_grayscale(buf[11], buf[10], buf[9]);
		dest += 4;
		buf += 12;
	}
	while(dest < slimglim)
	{
		dest[0] = compose_grayscale(buf[2], buf[1], buf[0]);
		dest ++;
		buf += 3;
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
static void Convert32BitTo8Bit(RISSE_RESTRICT risse_uint8 *dest, RISSE_RESTRICT const risse_uint32 *buf, risse_size len)
{
	{
		risse_size ___index = 0;
		if(len > (4-1))
		{

			len -= (4-1);

			while(___index < len)
			{
				{
				#if RISSE_HOST_IS_BIG_ENDIAN
					risse_uint32 d = buf[(___index+0)];
					dest[(___index+0)] = compose_grayscale(d&0xff, (d&0xff00)>>8, (d&0xff0000)>>16);
				#else
					risse_uint32 d = buf[(___index+0)];
					dest[(___index+0)] = compose_grayscale((d&0xff0000)>>16, (d&0xff00)>>8, d&0xff);
				#endif
				}
				{
				#if RISSE_HOST_IS_BIG_ENDIAN
					risse_uint32 d = buf[(___index+1)];
					dest[(___index+1)] = compose_grayscale(d&0xff, (d&0xff00)>>8, (d&0xff0000)>>16);
				#else
					risse_uint32 d = buf[(___index+1)];
					dest[(___index+1)] = compose_grayscale((d&0xff0000)>>16, (d&0xff00)>>8, d&0xff);
				#endif
				}
				{
				#if RISSE_HOST_IS_BIG_ENDIAN
					risse_uint32 d = buf[(___index+2)];
					dest[(___index+2)] = compose_grayscale(d&0xff, (d&0xff00)>>8, (d&0xff0000)>>16);
				#else
					risse_uint32 d = buf[(___index+2)];
					dest[(___index+2)] = compose_grayscale((d&0xff0000)>>16, (d&0xff00)>>8, d&0xff);
				#endif
				}
				{
				#if RISSE_HOST_IS_BIG_ENDIAN
					risse_uint32 d = buf[(___index+3)];
					dest[(___index+3)] = compose_grayscale(d&0xff, (d&0xff00)>>8, (d&0xff0000)>>16);
				#else
					risse_uint32 d = buf[(___index+3)];
					dest[(___index+3)] = compose_grayscale((d&0xff0000)>>16, (d&0xff00)>>8, d&0xff);
				#endif
				}
				___index += 4;
			}

			len += (4-1);
		}

		while(___index < len)
		{
			{
			#if RISSE_HOST_IS_BIG_ENDIAN
				risse_uint32 d = buf[___index];
				dest[___index] = compose_grayscale(d&0xff, (d&0xff00)>>8, (d&0xff0000)>>16);
			#else
				risse_uint32 d = buf[___index];
				dest[___index] = compose_grayscale((d&0xff0000)>>16, (d&0xff00)>>8, d&0xff);
			#endif
			}
			___index ++;
		}
	}
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
#ifndef BI_RGB // avoid re-define error on Win32
	#define BI_RGB			0
	#define BI_RLE8			1
	#define BI_RLE4			2
	#define BI_BITFIELDS	3
#endif

#ifdef __WIN32__
#pragma pack(push, 1)
#endif
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
};
#ifdef __WIN32__
#pragma pack(pop)
#endif
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
#define RISSE_BMP_READ_LINE_MAX 8
void tBMPImageDecoder::InternalLoadBMP(tStreamAdapter src, tImage * image,
					tPixel::tFormat pixel_format, tProgressCallback * callback,
					tDictionaryInstance * dict, RISSE_WIN_BITMAPINFOHEADER & bi, risse_uint8 * palsrc)
{
	// mostly taken from kirikiri2

	// TODO: only checked on Win32 platform
	bool grayscale = pixel_format == tPixel::pfGray8; // grayscale ?

	if(bi.biSize == 12)
	{
		// OS/2
		bi.biCompression = BI_RGB;
		bi.biClrUsed = 1 << bi.biBitCount;
	}

	risse_uint16 orgbitcount = bi.biBitCount;
	if(bi.biBitCount == 1 || bi.biBitCount == 4)
	{
		bi.biBitCount = 8;
	}

	switch(bi.biCompression)
	{
	case BI_RGB:
		// if there are no masks, use the defaults
		break; // use default

	case BI_BITFIELDS:
		tIOExceptionClass::Throw(RISSE_WS_TR("BITFIELDS not supported in BMP loading"));

	default:
		tIOExceptionClass::Throw(RISSE_WS_TR("compressed BMP not supported"));
	}

	// load palette
	risse_uint32 palette[256];   // (msb) argb (lsb)
	if(orgbitcount <= 8)
	{
		if(bi.biClrUsed == 0) bi.biClrUsed = 1 << orgbitcount ;
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

		if(grayscale)
		{
			// make grayscale palette
			for(int i = 0; i < 256; i++)
			{
				palette[i] =
					compose_grayscale((palette[i]>>16)&0xff, (palette[i]>>8)&0xff, palette[i]&0xff);
			}
		}
	}

	risse_int height;
	height = bi.biHeight<0?-bi.biHeight:bi.biHeight;
		// positive value of bi.biHeight indicates top-down DIB

	SetDimensions(bi.biWidth, height, grayscale ? tPixel::pfGray8 : tPixel::pfARGB32);

	risse_int pitch;
	pitch = (((bi.biWidth * orgbitcount) + 31) & ~31) /8;
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

		switch(orgbitcount)
		{
			// convert pixel format
		case 1:
			if(grayscale)
			{
				Expand1BitTo8BitPal(
					(risse_uint8*)scanline,
					(risse_uint8*)buf, bi.biWidth, palette);
			}
			else
			{
				Expand1BitTo32BitPal(
					(risse_uint32*)scanline,
					(risse_uint8*)buf, bi.biWidth, palette);
			}
			break;

		case 4:
			if(grayscale)
			{
				Expand4BitTo8BitPal(
					(risse_uint8*)scanline,
					(risse_uint8*)buf, bi.biWidth, palette);
			}
			else
			{
				Expand4BitTo32BitPal(
					(risse_uint32*)scanline,
					(risse_uint8*)buf, bi.biWidth, palette);
			}
			break;

		case 8:
			if(grayscale)
			{
				// convert to grayscale
				Expand8BitTo8BitPal(
					(risse_uint8*)scanline,
					(risse_uint8*)buf, bi.biWidth, palette);
			}
			else
			{
				Expand8BitTo32BitPal(
					(risse_uint32*)scanline,
					(risse_uint8*)buf, bi.biWidth, palette);
			}
			break;

		case 15:
		case 16:
			if(grayscale)
			{
				Convert15BitTo8Bit(
					(risse_uint8*)scanline,
					(risse_uint16*)buf, bi.biWidth);
			}
			else
			{
				Convert15BitTo32Bit(
					(risse_uint32*)scanline,
					(risse_uint16*)buf, bi.biWidth);
			}
			break;

		case 24:
			if(grayscale)
			{
				Convert24BitTo8Bit(
					(risse_uint8*)scanline,
					(risse_uint8*)buf, bi.biWidth);
			}
			else
			{
				Convert24BitTo32Bit(
					(risse_uint32*)scanline,
					(risse_uint8*)buf, bi.biWidth);
			}
			break;

		case 32:
			if(grayscale)
			{
				Convert32BitTo8Bit(
					(risse_uint8*)scanline,
					(risse_uint32*)buf, bi.biWidth);
			}
			else
			{
				// always copy from the buffer
				memcpy((risse_uint32*)scanline,
					(risse_uint32*)buf, bi.biWidth * sizeof(risse_uint32));
			}
			break;
		}

		DoneLines(); // image was written

		if(bi.biHeight>0) dest_y--; else dest_y++;
		buf += pitch;
		bufremain--;
	}
}
//---------------------------------------------------------------------------
void tBMPImageDecoder::Process(tStreamInstance * stream, tImage * image,
					tPixel::tFormat pixel_format, tProgressCallback * callback,
					tDictionaryInstance * dict)
{
	// Windows BMP Loader
	// mostly taken ( but totally re-written ) from SDL,
	// http://www.libsdl.org/
	tStreamAdapter src(stream);

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
	if(bi.biSize == 12)
	{
		// OS/2 Bitmap
		memset(&bi, 0, sizeof(bi));
		bi.biWidth = (risse_uint32)src.ReadI16LE();
		bi.biHeight = (risse_uint32)src.ReadI16LE();
		bi.biPlanes = src.ReadI16LE();
		bi.biBitCount = src.ReadI16LE();
		bi.biClrUsed = 1 << bi.biBitCount;
	}
	else if(bi.biSize == 40)
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
	}
	else
	{
		tIOExceptionClass::Throw(RISSE_WS_TR("non-supported version in BMP header"));
	}


	// load palette
	risse_int palsize = (bi.biBitCount <= 8) ?
		((bi.biClrUsed == 0 ? (1<<bi.biBitCount) : bi.biClrUsed) *
		((bi.biSize == 12) ? 3:4)) : 0;  // bi.biSize == 12 ( OS/2 palette )
	risse_uint8 *palette = NULL;

	if(palsize) palette = static_cast<risse_uint8*>(MallocAtomicCollectee(palsize));

	src.ReadBuffer(palette, palsize);
	src.SetPosition(firstpos + bf.bfOffBits);

	InternalLoadBMP(src, image, pixel_format, callback, dict, bi, palette);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa


