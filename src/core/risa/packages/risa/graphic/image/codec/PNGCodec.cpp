//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief PNG形式コーデック
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include <png.h>
#include "risa/packages/risa/graphic/image/codec/PNGCodec.h"
#include "risa/common/RisaException.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(27127,64132,43289,19704,411,20462,45806,11915);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// meta callback information structure used by  PNG_read_chunk_callback
struct PNG_read_chunk_callback_user_struct
{
	tPNGImageDecoder * Decoder;
};
//---------------------------------------------------------------------------
// user_malloc_fn
static png_voidp PNG_malloc(png_structp ps, png_size_t size)
{
	return malloc(size);
}
//---------------------------------------------------------------------------
// user_free_fn
static void PNG_free (png_structp ps,void*/* png_structp*/ mem)
{
	free(mem);
}
//---------------------------------------------------------------------------
// user_error_fn
static void PNG_error (png_structp ps, png_const_charp msg)
{
	tIOExceptionClass::Throw(tString(RISSE_WS_TR("error on reading PNG: %1"), tString(msg)));
}
//---------------------------------------------------------------------------
// user_warning_fn
static void PNG_warning (png_structp ps, png_const_charp msg)
{
	// do nothing
}
//---------------------------------------------------------------------------
// user_read_data
static void PNG_read_data(png_structp png_ptr,png_bytep data,png_size_t length)
{
	((tStreamAdapter *)png_get_io_ptr(png_ptr))->ReadBuffer((void*)data, length);
}
//---------------------------------------------------------------------------
// read_row_callback
static void PNG_read_row_callback(png_structp png_ptr,png_uint_32 row,int pass)
{

}
//---------------------------------------------------------------------------
// read_chunk_callback
static int PNG_read_chunk_callback(png_structp png_ptr,png_unknown_chunkp chunk)
{
	// handle vpAg chunk (this will contain the virtual page size of the image)
	// vpAg chunk can be embeded by ImageMagick -trim option etc.
	// we don't care about how the chunk bit properties are being provided.
	if(	(chunk->name[0] == 0x76/*'v'*/ || chunk->name[0] == 0x56/*'V'*/) &&
		(chunk->name[1] == 0x70/*'p'*/ || chunk->name[1] == 0x50/*'P'*/) &&
		(chunk->name[2] == 0x61/*'a'*/ || chunk->name[2] == 0x41/*'A'*/) &&
		(chunk->name[3] == 0x67/*'g'*/ || chunk->name[3] == 0x47/*'G'*/) && chunk->size >= 9)
	{
		PNG_read_chunk_callback_user_struct * user_struct =
			static_cast<PNG_read_chunk_callback_user_struct *>(png_get_user_chunk_ptr(png_ptr));
		// vpAg found
		/*
			uint32 width
			uint32 height
			uchar unit
		*/
		// be careful because the integers are stored in network byte order
		#define PNG_read_be32(a) (((risse_uint32)(a)[0]<<24)+\
			((risse_uint32)(a)[1]<<16)+((risse_uint32)(a)[2]<<8)+\
			((risse_uint32)(a)[3]))
		risse_uint32 width  = PNG_read_be32(chunk->data+0);
		risse_uint32 height = PNG_read_be32(chunk->data+4);
		risse_uint8  unit   = chunk->data[8];

		// push information into meta-info
		user_struct->Decoder->PushMetadata(tSS<'v','p','a','g','_','w'>(),
			tVariant(tString::AsString((risse_int64)width)));
		user_struct->Decoder->PushMetadata(tSS<'v','p','a','g','_','h'>(),
			tVariant(tString::AsString((risse_int64)height)));
		switch(unit)
		{
		case PNG_OFFSET_PIXEL:
			user_struct->Decoder->PushMetadata(tSS<'v','p','a','g','_','u','n','i','t'>(),
				tVariant(tSS<'p','i','x','e','l'>()));
			break;
		case PNG_OFFSET_MICROMETER:
			user_struct->Decoder->PushMetadata(tSS<'v','p','a','g','_','u','n','i','t'>(),
				tVariant(tSS<'m','i','c','r','o','m','e','t','e','r'>()));
			break;
		default:
			user_struct->Decoder->PushMetadata(tSS<'v','p','a','g','_','u','n','i','t'>(),
				tVariant(tSS<'u','n','k','n','o','w','n'>()));
			break;
		}
		return 1; // chunk read success
	}
	return 0; // did not recognize
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void tPNGImageDecoder::Process(tStreamInstance * stream,
					tPixel::tFormat pixel_format, tProgressCallback * callback,
					tDictionaryInstance * dict)
{
	Dictionary = dict;

	tStreamAdapter src(stream);

	// dict から読み取るべき内容は無し。dict をクリアする
	if(Dictionary)
		Dictionary->Invoke(tSS<'c','l','e','a','r'>());

	png_structp png_ptr=NULL;
	png_infop info_ptr=NULL;
	png_infop end_info=NULL;

	png_uint_32 i;

	png_bytep *row_pointers=NULL;
	risse_uint8 *image=NULL;

	try
	{
		// create png_struct
		png_ptr=png_create_read_struct_2(
			PNG_LIBPNG_VER_STRING,
			(png_voidp)NULL, PNG_error, PNG_warning,
			(png_voidp)NULL, PNG_malloc, PNG_free);

		// set read_chunk_callback
		PNG_read_chunk_callback_user_struct read_chunk_callback_user_struct;
		read_chunk_callback_user_struct.Decoder = this;
		png_set_read_user_chunk_fn(png_ptr,
			static_cast<void*>(&read_chunk_callback_user_struct),
			PNG_read_chunk_callback);
		png_set_keep_unknown_chunks(png_ptr, 2, NULL, 0);
			// keep only if safe-to-copy chunks, for all unknown chunks

		// create png_info
		info_ptr=png_create_info_struct(png_ptr);

		// create end_info
		end_info=png_create_info_struct(png_ptr);

		// set stream interface
		png_set_read_fn(png_ptr, (voidp)&src, PNG_read_data);

		// set read_row_callback
		png_set_read_status_fn(png_ptr, PNG_read_row_callback);

		// set png_read_info
		png_read_info(png_ptr, info_ptr);

		// retrieve IHDR
		png_uint_32 width,height;
		int bit_depth;
		int color_type;
		int interlace_type;
		int compression_type;
		int filter_type;
		png_get_IHDR(
			png_ptr,
			info_ptr,
			&width,
			&height,
			&bit_depth,
			&color_type,
			&interlace_type,
			&compression_type,
			&filter_type);

		// 16bit/チャンネルは受け取れない
		if(bit_depth==16) png_set_strip_16(png_ptr);

		// retrieve offset information
		png_int_32 offset_x, offset_y;
		int offset_unit_type;
		if(HasDictionary() &&
			png_get_oFFs(png_ptr, info_ptr, &offset_x, &offset_y, &offset_unit_type))
		{
			// push offset information into metainfo data
			PushMetadata(tSS<'o','f','f','s','_','x'>(),
				tVariant(tString::AsString((risse_int64)offset_x)));
			PushMetadata(tSS<'o','f','f','s','_','y'>(),
				tVariant(tString::AsString((risse_int64)offset_y)));
			switch(offset_unit_type)
			{
			case PNG_OFFSET_PIXEL:
				PushMetadata(tSS<'o','f','f','s','_','u','n','i','t'>(),
					tVariant(tSS<'p','i','x','e','l'>()));
				break;
			case PNG_OFFSET_MICROMETER:
				PushMetadata(tSS<'o','f','f','s','_','u','n','i','t'>(),
					tVariant(tSS<'m','i','c','r','o','m','e','t','e','r'>()));
				break;
			default:
				PushMetadata(tSS<'o','f','f','s','_','u','n','i','t'>(),
					tVariant(tSS<'u','n','k','n','o','w','n'>()));
				break;
			}
		}

		tPixel::tFormat decoder_format;

		if(pixel_format == tPixel::pfGray8)
		{
			// convert the image to grayscale
			decoder_format = tPixel::pfGray8;

			if(color_type == PNG_COLOR_TYPE_PALETTE)
			{
				png_set_palette_to_rgb(png_ptr);
				png_set_bgr(png_ptr);
				if (bit_depth < 8)
					png_set_packing(png_ptr);
				png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER); // 32bpp 出力
				decoder_format = tPixel::pfARGB32; // このときだけ pfARGB32 なので注意
			}
			if(color_type == PNG_COLOR_TYPE_GRAY &&
				bit_depth < 8) png_set_gray_1_2_4_to_8(png_ptr);
			if(color_type == PNG_COLOR_TYPE_RGB ||
				color_type == PNG_COLOR_TYPE_RGB_ALPHA)
				png_set_rgb_to_gray_fixed(png_ptr, 1, 0, 0);
			if(color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
				color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
				png_set_strip_alpha(png_ptr);
		}
		else
		{
			// convert the image to full color ( 32bits ) one if needed
			decoder_format = tPixel::pfARGB32;

			if(color_type == PNG_COLOR_TYPE_PALETTE)
			{
				if(png_get_valid(png_ptr, info_ptr,PNG_INFO_tRNS))
				{
					// set expansion with palettized picture
					png_set_palette_to_rgb(png_ptr);
					png_set_tRNS_to_alpha(png_ptr);
					color_type=PNG_COLOR_TYPE_RGB_ALPHA;
				}
				else
				{
					png_set_palette_to_rgb(png_ptr);
					color_type=PNG_COLOR_TYPE_RGB;
				}
			}

			switch(color_type)
			{
			case PNG_COLOR_TYPE_GRAY_ALPHA:
				png_set_gray_to_rgb(png_ptr);
				color_type=PNG_COLOR_TYPE_RGB_ALPHA;
				png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
				break;

			case PNG_COLOR_TYPE_GRAY:
				png_set_expand(png_ptr);
				png_set_gray_to_rgb(png_ptr);
				color_type=PNG_COLOR_TYPE_RGB;
				png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
				break;

			case PNG_COLOR_TYPE_RGB_ALPHA:
				png_set_bgr(png_ptr);
				break;

			case PNG_COLOR_TYPE_RGB:
				png_set_bgr(png_ptr);
				png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
				break;

			default:
				tIOExceptionClass::Throw(RISSE_WS_TR("error on reading PNG: unsupported color type"));
			}
		}

		// call png_read_update_info
		png_read_update_info(png_ptr,info_ptr);

		// set size
		SetDimensions(width, height, decoder_format);

		// load image
		if(info_ptr->interlace_type == PNG_INTERLACE_NONE)
		{
			for(i=0; i<height; i++)
			{
				if(callback) callback->CallOnProgress(height, i);
				void *scanline = StartLines(i, 1, NULL);
				png_read_row(png_ptr, (png_bytep)scanline, NULL);
				DoneLines();
			}

			// finish loading
			png_read_end(png_ptr,info_ptr);
		}
		else
		{
			// interlace handling
			// load the image at once

			// TODO: progress callback ハンドリング

			row_pointers = new png_bytep[height];
			png_uint_32 rowbytes = png_get_rowbytes(png_ptr, info_ptr);
			image = new risse_uint8[rowbytes * height];
			for(i=0; i<height; i++)
			{
				row_pointers[i] = image+ i*rowbytes;
			}

			// loads image
			png_read_image(png_ptr, row_pointers);

			// finish loading
			png_read_end(png_ptr, info_ptr);

			// set the pixel data
			for(i=0; i<height; i++)
			{
				void *scanline = StartLines(i, 1, NULL);
				memcpy(scanline, row_pointers[i], rowbytes);
				DoneLines();
			}
		}
	}
	catch(...)
	{
		png_destroy_read_struct(&png_ptr,&info_ptr,&end_info);
		if(row_pointers) delete [] row_pointers;
		if(image) delete [] image;
		throw;
	}

	png_destroy_read_struct(&png_ptr,&info_ptr,&end_info);
	if(row_pointers) delete [] row_pointers;
	if(image) delete [] image;

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPNGImageDecoder::PushMetadata(const tString & key, const tVariant & value)
{
	if(!Dictionary) return;
	Dictionary->Invoke(tSS<'[',']','='>(), value, tVariant(key));
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
// function - user_write_data
static void PNG_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	static_cast<tStreamAdapter *>(png_get_io_ptr(png_ptr))->WriteBuffer((void*)data, length);
}
//---------------------------------------------------------------------------
// function - user_flush
static void PNG_flush(png_structp png_ptr)
{
	static_cast<tStreamAdapter *>(png_get_io_ptr(png_ptr))->Flush();
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void tPNGImageEncoder::Process(tStreamInstance * stream,
					tProgressCallback * callback,
					tDictionaryInstance * dict)
{
	// 今のところ dict の '_type' は bmp と同じく '24' (別名'R8G8B8') と '32'
	// (別名 'A8R8G8B8') と 'grayscale' (別名 'GRAY8') を受け付ける。
	// 32 の場合はアルファチャンネルが含まれる。
	// (デフォルトは 24)。
	// そのほか、vpag_w, vpag_h, vpag_unit と offs_x, offs_y, offs_unit も
	// 受け付ける
	int color_type = PNG_COLOR_TYPE_RGB;
	bool store_vpAg = false;
	risse_uint32 vpAg_w, vpAg_h;
	int vpAg_unit = PNG_OFFSET_PIXEL;
	bool store_oFFs = false;
	risse_int32 oFFs_x, oFFs_y;
	int oFFs_unit = PNG_OFFSET_PIXEL;


	if(dict)
	{
		// _type の処理
		tVariant val, val1, val2;
		val = dict->Invoke(tSS<'[',']'>(), tVariant(tSS<'_','t','y','p','e'>()));
		if(!val.IsVoid())
		{
			tString str = val.operator tString();
			if(str == tSS<'g','r','a','y','s','c','a','l','e'>() ||
				str == tSS<'G','R','A','Y','8'>())
				color_type = PNG_COLOR_TYPE_GRAY;
			else if(str == tSS<'2','4'>() ||
				str == tSS<'R','8','G','8','B','8'>())
				color_type = PNG_COLOR_TYPE_RGB;
			else if(str == tSS<'3','2'>() ||
				str == tSS<'A','8','R','8','G','8','B','8'>())
				color_type = PNG_COLOR_TYPE_RGB_ALPHA;
			else
				tIllegalArgumentExceptionClass::Throw(
					RISSE_WS_TR("unknown png sub-type in '_type' parameter"));
		}

		// vpAg 関連
		val1 = dict->Invoke(tSS<'[',']'>(), tVariant(tSS<'v','p','a','g','_','w'>()));
		val2 = dict->Invoke(tSS<'[',']'>(), tVariant(tSS<'v','p','a','g','_','h'>()));
		if(!val1.IsVoid() && !val2.IsVoid())
		{
			store_vpAg = true;
			vpAg_w = static_cast<risse_uint32>((risse_int64)val1);
			vpAg_h = static_cast<risse_uint32>((risse_int64)val2);
			val = dict->Invoke(tSS<'[',']'>(), tVariant(tSS<'v','p','a','g','_','u','n','i','t'>()));
			if(!val.IsVoid())
			{
				tString str = val.operator tString();
				if(str == tSS<'p','i','x','e','l'>())
					vpAg_unit = PNG_OFFSET_PIXEL;
				else if(str == tSS<'m','i','c','r','o','m','e','t','e','r'>())
					vpAg_unit = PNG_OFFSET_MICROMETER;
				else
					tIllegalArgumentExceptionClass::Throw(
						RISSE_WS_TR("unknown unit for 'vpag_unit' parameter"));
			}
		}

		// oFFs 関連
		val1 = dict->Invoke(tSS<'[',']'>(), tVariant(tSS<'o','f','f','s','_','x'>()));
		val2 = dict->Invoke(tSS<'[',']'>(), tVariant(tSS<'o','f','f','s','_','y'>()));
		if(!val1.IsVoid() && !val2.IsVoid())
		{
			store_oFFs = true;
			oFFs_x = static_cast<risse_int32>((risse_int64)val1);
			oFFs_y = static_cast<risse_int32>((risse_int64)val2);
			val = dict->Invoke(tSS<'[',']'>(), tVariant(tSS<'o','f','f','s','_','u','n','i','t'>()));
			if(!val.IsVoid())
			{
				tString str = val.operator tString();
				if(str == tSS<'p','i','x','e','l'>())
					oFFs_unit = PNG_OFFSET_PIXEL;
				else if(str == tSS<'m','i','c','r','o','m','e','t','e','r'>())
					oFFs_unit = PNG_OFFSET_MICROMETER;
				else
					tIllegalArgumentExceptionClass::Throw(
						RISSE_WS_TR("unknown unit for 'offs_unit' parameter"));
			}
		}
	}

	tStreamAdapter dest(stream);
	risse_size width = 0;
	risse_size height = 0;
	GetDimensions(&width, &height);


	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep row_buffer = NULL;

	try
	{
		// create png_struct
		png_ptr = png_create_write_struct_2
			(PNG_LIBPNG_VER_STRING,
			(png_voidp)this, PNG_error, PNG_warning,
			(png_voidp)this, PNG_malloc, PNG_free);

		// create png_info
		info_ptr = png_create_info_struct(png_ptr);

		// set write function
		png_set_write_fn(png_ptr, (png_voidp)&dest,
			PNG_write_data, PNG_flush);


		// set IHDR
		png_set_IHDR(png_ptr, info_ptr, width, height,
			8,
			color_type, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT);

		// set oFFs
		if(store_oFFs)
			png_set_oFFs(png_ptr, info_ptr, oFFs_x, oFFs_y, oFFs_unit);

		// write info
		png_write_info(png_ptr, info_ptr);

		// write vpAg private chunk
		if(store_vpAg)
		{
			png_byte png_vpAg[5] = {118, 112,  65, 103, '\0'};
			unsigned char vpag_chunk_data[9];
		#define PNG_write_be32(p, a) (\
			((unsigned char *)(p))[0] = (unsigned char)(((a) >>24) & 0xff), \
			((unsigned char *)(p))[1] = (unsigned char)(((a) >>16) & 0xff), \
			((unsigned char *)(p))[2] = (unsigned char)(((a) >> 8) & 0xff), \
			((unsigned char *)(p))[3] = (unsigned char)(((a)     ) & 0xff)  )
			PNG_write_be32(vpag_chunk_data,     vpAg_w);
			PNG_write_be32(vpag_chunk_data + 4, vpAg_h);
			vpag_chunk_data[8] = (unsigned char)vpAg_unit;
			png_write_chunk(png_ptr, png_vpAg, vpag_chunk_data, 9);
		}

		// write image
		if(color_type == PNG_COLOR_TYPE_RGB)
		{
			row_buffer = (png_bytep)png_malloc(png_ptr, 3 * width + 6);
			try
			{
				png_bytep row_pointer = row_buffer;

				for(risse_size i = 0; i < height; i++)
				{
					const risse_uint32 *in = static_cast<const risse_uint32 *>(
						GetLines(NULL, i, 1, NULL, tPixel::pfARGB32));
					png_bytep out = row_buffer;
					for(risse_size x = 0; x < width; x++)
					{
						risse_uint32 v = *in;
						out[2] = static_cast<risse_uint8>(v);
						out[1] = static_cast<risse_uint8>(v >> 8);
						out[0] = static_cast<risse_uint8>(v >> 16);
						out += 3;
						in ++;
					}
					png_write_row(png_ptr, row_pointer);
				}
			}
			catch(...)
			{
				png_free(png_ptr, row_buffer);
				throw;
			}
			png_free(png_ptr, row_buffer);
		}
		else if(color_type == PNG_COLOR_TYPE_RGB_ALPHA)
		{
			row_buffer = (png_bytep)png_malloc(png_ptr, 4 * width + 6);
			try
			{
				png_bytep row_pointer = row_buffer;

				for(risse_size i = 0; i < height; i++)
				{
					const risse_uint32 *in = static_cast<const risse_uint32 *>(
						GetLines(NULL, i, 1, NULL, tPixel::pfARGB32));
					png_bytep out = row_buffer;
					for(risse_size x = 0; x < width; x++)
					{
						risse_uint32 v = *in;
						out[2] = static_cast<risse_uint8>(v);
						out[1] = static_cast<risse_uint8>(v >> 8);
						out[0] = static_cast<risse_uint8>(v >> 16);
						out[3] = static_cast<risse_uint8>(v >> 24);
						out += 4;
						in ++;
					}
					png_write_row(png_ptr, row_pointer);
				}
			}
			catch(...)
			{
				png_free(png_ptr, row_buffer);
				throw;
			}
			png_free(png_ptr, row_buffer);
		}
		else
		{
			for(risse_size i = 0; i < height; i++)
			{
				const risse_uint32 *in = static_cast<const risse_uint32 *>(
					GetLines(NULL, i, 1, NULL, tPixel::pfGray8));
				png_bytep row_pointer = (png_bytep)in;
				png_write_row(png_ptr, row_pointer);
			}
		}

		// finish writing
		png_write_end(png_ptr, info_ptr);

	}
	catch(...)
	{
		png_destroy_write_struct(&png_ptr, &info_ptr);
		throw;
	}

	png_destroy_write_struct(&png_ptr, &info_ptr);

}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
/**
 * PNGイメージコーデックファクトリ
 */
class tPNGImageCodecFactory : public tImageDecoderFactory, public tImageEncoderFactory,
								public singleton_base<tPNGImageCodecFactory>
{
	virtual tImageDecoder * CreateDecoder() { return new tPNGImageDecoder; }
	virtual tImageEncoder * CreateEncoder() { return new tPNGImageEncoder; }
public:
	/**
	 * コンストラクタ
	 */
	tPNGImageCodecFactory()
	{
		tImageCodecFactoryManager::instance()->Register(tSS<'.','p','n','g'>(), (tImageDecoderFactory*)this);
		tImageCodecFactoryManager::instance()->Register(tSS<'.','p','n','g'>(), (tImageEncoderFactory*)this);
	}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa


