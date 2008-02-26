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



namespace Risa {
RISSE_DEFINE_SOURCE_ID(56439,41578,12253,18753,50573,34933,38961,51441);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
static void TLG5ComposeColors3To4(risse_uint8 *outp, const risse_uint8 *upper, risse_uint8 * const * buf, risse_int width)
{
	risse_int x;
	risse_uint8 pc[3];
	risse_uint8 c[3];
	pc[0] = pc[1] = pc[2] = 0;
	for(x = 0; x < width; x++)
	{
		c[0] = buf[0][x];
		c[1] = buf[1][x];
		c[2] = buf[2][x];
		c[0] += c[1]; c[2] += c[1];
		*(risse_uint32 *)outp =
								((((pc[0] += c[0]) + upper[0]) & 0xff)      ) +
								((((pc[1] += c[1]) + upper[1]) & 0xff) <<  8) +
								((((pc[2] += c[2]) + upper[2]) & 0xff) << 16) +
								0xff000000;
		outp += 4;
		upper += 4;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static void TLG5ComposeColors4To4(risse_uint8 *outp, const risse_uint8 *upper, risse_uint8 * const* buf, risse_int width)
{
	risse_int x;
	risse_uint8 pc[4];
	risse_uint8 c[4];
	pc[0] = pc[1] = pc[2] = pc[3] = 0;
	for(x = 0; x < width; x++)
	{
		c[0] = buf[0][x];
		c[1] = buf[1][x];
		c[2] = buf[2][x];
		c[3] = buf[3][x];
		c[0] += c[1]; c[2] += c[1];
		*(risse_uint32 *)outp =
								((((pc[0] += c[0]) + upper[0]) & 0xff)      ) +
								((((pc[1] += c[1]) + upper[1]) & 0xff) <<  8) +
								((((pc[2] += c[2]) + upper[2]) & 0xff) << 16) +
								((((pc[3] += c[3]) + upper[3]) & 0xff) << 24);
		outp += 4;
		upper += 4;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
static risse_int TLG5DecompressSlide(risse_uint8 *out, const risse_uint8 *in, risse_int insize, risse_uint8 *text, risse_int initialr)
{
	risse_int r = initialr;
	risse_uint flags = 0;
	const risse_uint8 *inlim = in + insize;
	while(in < inlim)
	{
		if(((flags >>= 1) & 256) == 0)
		{
			flags = 0[in++] | 0xff00;
		}
		if(flags & 1)
		{
			risse_int mpos = in[0] | ((in[1] & 0xf) << 8);
			risse_int mlen = (in[1] & 0xf0) >> 4;
			in += 2;
			mlen += 3;
			if(mlen == 18) mlen += 0[in++];

			while(mlen--)
			{
				0[out++] = text[r++] = text[mpos++];
				mpos &= (4096 - 1);
				r &= (4096 - 1);
			}
		}
		else
		{
			unsigned char c = 0[in++];
			0[out++] = c;
			text[r++] = c;
/*			0[out++] = text[r++] = 0[in++];*/
			r &= (4096 - 1);
		}
	}
	return r;
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
	SetDimensions(width, height, tPixel::pfARGB32);

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

	risse_uint8 *prevline = NULL;
	for(risse_int y_blk = 0; y_blk < height; y_blk += blockheight)
	{
		// read file and decompress
		for(risse_int c = 0; c < colors; c++)
		{
			src.ReadBuffer(mark, 1);
			risse_uint32 size;
			size = src.ReadI32LE();
			if(mark[0] == 0)
			{
				// modified LZSS compressed data
				src.ReadBuffer(inbuf, size);
				r = TLG5DecompressSlide(outbuf[c], inbuf, size, text, r);
			}
			else
			{
				// raw data
				src.ReadBuffer(outbuf[c], size);
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
							((pb += b)      )+
							((pg += g) << 8 )+
							((pr += r) << 16)+
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
							((pb += b)      )+
							((pg += g) << 8 )+
							((pr += r) << 16)+
							((pa += a) << 24);
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

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTLGImageDecoder::ProcessTLG6(tStreamAdapter & src,
					tPixel::tFormat pixel_format, tProgressCallback * callback,
					tDictionaryInstance * dict)
{
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


