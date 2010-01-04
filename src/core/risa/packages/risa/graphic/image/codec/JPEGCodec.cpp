//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief JPEG形式コーデック
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/graphic/image/codec/JPEGCodec.h"
#include "risa/common/RisaException.h"


// boolean の定義が被るので
typedef int JPEGLIB_BOOLEAN;
#define boolean JPEGLIB_BOOLEAN
#define HAVE_BOOLEAN

extern "C"
{
    #if defined(__WXMSW__)
        #define XMD_H
    #endif
    #include <jpeglib.h>
    #include <jerror.h>
}


namespace Risa {
RISSE_DEFINE_SOURCE_ID(34856,19122,58311,17086,53382,40079,1228,36948);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
struct my_error_mgr
{
	struct jpeg_error_mgr pub;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
METHODDEF(void)
my_error_exit(j_common_ptr cinfo)
{
	tIOExceptionClass::Throw(tString(RISSE_WS_TR("error on reading JPEG: error code %1"),
						tString::AsString((risse_int64)cinfo->err->msg_code)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
METHODDEF(void)
my_output_message(j_common_ptr c)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#define BUFFER_SIZE 8192
struct my_source_mgr
{
	jpeg_source_mgr pub;
	JOCTET * buffer;
	tStreamAdapter * stream;
	boolean start_of_file;
} ;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
METHODDEF(void)
init_source(j_decompress_ptr cinfo)
{
	// ??
	my_source_mgr * src = (my_source_mgr*) cinfo->src;

	src->start_of_file = TRUE;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
METHODDEF(boolean)
fill_input_buffer(j_decompress_ptr cinfo)
{
	my_source_mgr * src = (my_source_mgr*) cinfo->src;

	int nbytes = src->stream->Read(src->buffer, BUFFER_SIZE);

	if(nbytes <= 0)
	{
		if(src->start_of_file)
			ERREXIT(cinfo, JERR_INPUT_EMPTY);
		WARNMS(cinfo, JWRN_JPEG_EOF);

	    src->buffer[0] = (JOCTET) 0xFF;
		src->buffer[1] = (JOCTET) JPEG_EOI;
		nbytes = 2;
	}

	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;

	src->start_of_file = FALSE;

	return TRUE;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
METHODDEF(void)
skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
	my_source_mgr * src = (my_source_mgr*) cinfo->src;

	if (num_bytes > 0) {
		while (num_bytes > (long) src->pub.bytes_in_buffer) {
			num_bytes -= (long) src->pub.bytes_in_buffer;
			fill_input_buffer(cinfo);
			/* note that we assume that fill_input_buffer will never return FALSE,
			 * so suspension need not be handled.
			 */
		}
		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
METHODDEF(void)
term_source (j_decompress_ptr cinfo)
{
	/* no work necessary here */
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
METHODDEF(void)
jpeg_StreamAdapter_src (j_decompress_ptr cinfo, tStreamAdapter * infile)
{
  my_source_mgr * src;

	if (cinfo->src == NULL) {	/* first time for this JPEG object? */
		cinfo->src = (struct jpeg_source_mgr *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				  sizeof(my_source_mgr));
	src = (my_source_mgr * ) cinfo->src;
	src->buffer = (JOCTET *)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				  BUFFER_SIZE * sizeof(JOCTET));
  }

  src = (my_source_mgr *) cinfo->src;
  src->pub.init_source = init_source;
  src->pub.fill_input_buffer = fill_input_buffer;
  src->pub.skip_input_data = skip_input_data;
  src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
  src->pub.term_source = term_source;
  src->stream = infile;
  src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
  src->pub.next_input_byte = NULL; /* until buffer loaded */
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void tJPEGImageDecoder::Process(tStreamInstance * stream,
					tPixel::tFormat pixel_format, tProgressCallback * callback,
					tDictionaryInstance * dict)
{
	tStreamAdapter src(stream);

	// dict から読み取るべき内容は無し。dict をクリアする
	if(dict)
		dict->Invoke(tSS<'c','l','e','a','r'>());

	// prepare variables
	jpeg_decompress_struct cinfo;
	my_error_mgr jerr;
	JSAMPARRAY buffer;

	// error handling
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	jerr.pub.output_message = my_output_message;

	// create decompress object
	jpeg_create_decompress(&cinfo);

	try
	{
		// set data source
		jpeg_StreamAdapter_src(&cinfo, &src);

		// read the header
		jpeg_read_header(&cinfo, TRUE);

		// decompress option
		cinfo.dct_method = JDCT_FLOAT;
		cinfo.do_fancy_upsampling = TRUE;

		// check output color space
		if ((cinfo.out_color_space == JCS_CMYK) || (cinfo.out_color_space == JCS_YCCK))
			tIOExceptionClass::Throw(
				tString(RISSE_WS_TR("error on reading JPEG: unsupported color space")));
		else
			cinfo.out_color_space = JCS_RGB;

		// start decompression
		jpeg_start_decompress(&cinfo);

		// let base class know the image dimensions
		SetDimensions(cinfo.output_width, cinfo.output_height, tPixel::pfARGB32);

		buffer = (*cinfo.mem->alloc_sarray)
			((j_common_ptr) &cinfo, JPOOL_IMAGE,
				cinfo.output_width * cinfo.output_components + 3,
				cinfo.rec_outbuf_height);

		while(cinfo.output_scanline < cinfo.output_height)
		{
			if(callback) callback->CallOnProgress(cinfo.output_height, cinfo.output_scanline);

			risse_size startline = cinfo.output_scanline;

			jpeg_read_scanlines(&cinfo, buffer, cinfo.rec_outbuf_height);

			risse_size endline = cinfo.output_scanline;
			risse_size bufline;
			risse_size line;

			for(line = startline, bufline = 0; line < endline; line++, bufline++)
			{
				// pixel format conversion
				// expand 24bits to 32bits
				RISSE_RESTRICT risse_uint32 * scanline =
					static_cast<risse_uint32*>(StartLines(line, 1, NULL));
				RISSE_RESTRICT const risse_uint8 * inbuf =
					static_cast<const risse_uint8*>(buffer[bufline]);

				risse_size w = cinfo.output_width;
				for(risse_size x = 0; x < w; x++)
				{
					scanline[x] = 0xff000000 +
						((risse_uint32)inbuf[0]      ) +
						((risse_uint32)inbuf[1] << 8 ) +
						((risse_uint32)inbuf[2] << 16);
					inbuf += 3;
				}

				DoneLines();
			}
		}
	}
	catch(...)
	{
		jpeg_destroy_decompress(&cinfo);
		throw;
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

}
//---------------------------------------------------------------------------













#define OUTPUT_BUF_SIZE  4096    /* choose an efficiently fwrite'able size */

//---------------------------------------------------------------------------
struct my_destination_mgr
{
	struct jpeg_destination_mgr pub;

	tStreamAdapter *stream;
	JOCTET * buffer;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
METHODDEF(void) my_init_destination (j_compress_ptr cinfo)
{
	my_destination_mgr * dest = (my_destination_mgr *) cinfo->dest;

	/* Allocate the output buffer --- it will be released when done with image */
	dest->buffer = (JOCTET *)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
		OUTPUT_BUF_SIZE * sizeof(JOCTET));
	dest->pub.next_output_byte = dest->buffer;
	dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
METHODDEF(boolean) my_empty_output_buffer (j_compress_ptr cinfo)
{
	my_destination_mgr * dest = (my_destination_mgr *) cinfo->dest;

	dest->stream->Write(dest->buffer, OUTPUT_BUF_SIZE);
	dest->pub.next_output_byte = dest->buffer;
	dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
	return TRUE;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
METHODDEF(void) my_term_destination (j_compress_ptr cinfo)
{
	my_destination_mgr * dest = (my_destination_mgr *) cinfo->dest;
	size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;
	/* Write any data remaining in the buffer */
	if (datacount > 0)
		dest->stream->Write(dest->buffer, datacount);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
GLOBAL(void) my_jpeg_io_dest (j_compress_ptr cinfo, tStreamAdapter * outfile)
{
	my_destination_mgr * dest;

	if (cinfo->dest == NULL) {	  /* first time for this JPEG object? */
		cinfo->dest = (struct jpeg_destination_mgr *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			sizeof(my_destination_mgr));
	}

	dest = (my_destination_mgr *) cinfo->dest;
	dest->pub.init_destination = my_init_destination;
	dest->pub.empty_output_buffer = my_empty_output_buffer;
	dest->pub.term_destination = my_term_destination;
	dest->stream = outfile;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tJPEGImageEncoder::Process(tStreamInstance * stream,
					tProgressCallback * callback,
					tDictionaryInstance * dict)
{
	tStreamAdapter dest(stream);

	struct jpeg_compress_struct cinfo;
	struct my_error_mgr jerr;
	JSAMPROW row_pointer[1];
	risse_uint8 * buf = NULL;

	int quality = 75;
	if(dict)
	{
		// _quality の取得
		tVariant val;
		val = dict->Invoke(tSS<'[',']'>(), tVariant(tSS<'_','q','u','a','l','i','t','y'>()));
		if(!val.IsVoid())
			quality = static_cast<int>((risse_int64)val);
	}

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	cinfo.err->output_message = my_output_message;

	jpeg_create_compress(&cinfo);

	try
	{
		my_jpeg_io_dest(&cinfo, &dest);

		risse_size width = 0;
		risse_size height = 0;
		GetDimensions(&width, &height);

		cinfo.image_width = width;
		cinfo.image_height = height;
		cinfo.input_components = 3;
		cinfo.in_color_space = JCS_RGB;
		jpeg_set_defaults(&cinfo);

		jpeg_set_quality(&cinfo, quality, TRUE);

		jpeg_start_compress(&cinfo, TRUE);

		// 画像のピクセルフォーマットを変換しながらエンコード
		buf = new risse_uint8[width * 3];

		while (cinfo.next_scanline < cinfo.image_height)
		{
			const risse_uint32 * inbuf =
				static_cast<const risse_uint32*>(
					GetLines(NULL, cinfo.next_scanline, 1, NULL, tPixel::pfARGB32));
			risse_uint8 * out = buf;
			for(risse_size x = 0; x < width; x++)
			{
				risse_uint32 v = *inbuf;
				out[0] = static_cast<risse_uint8>(v);
				out[1] = static_cast<risse_uint8>(v >> 8);
				out[2] = static_cast<risse_uint8>(v >> 16);
				out += 3;
				inbuf ++;
			}
			row_pointer[0] = buf;
			jpeg_write_scanlines(&cinfo, row_pointer, 1);
		}
		jpeg_finish_compress(&cinfo);
	}
	catch(...)
	{
		if(buf) delete [] buf;
		jpeg_destroy_compress(&cinfo);
		throw;
	}
	delete [] buf;
	jpeg_destroy_compress(&cinfo);
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
/**
 * JPEGイメージコーデックファクトリ
 */
class tJPEGImageCodecFactory : public tImageDecoderFactory, public tImageEncoderFactory,
								public singleton_base<tJPEGImageCodecFactory>
{
	virtual tImageDecoder * CreateDecoder() { return new tJPEGImageDecoder; }
	virtual tImageEncoder * CreateEncoder() { return new tJPEGImageEncoder; }
public:
	/**
	 * コンストラクタ
	 */
	tJPEGImageCodecFactory()
	{
		tImageCodecFactoryManager::instance()->Register(tSS<'.','j','p','g'>(), (tImageDecoderFactory*)this);
		tImageCodecFactoryManager::instance()->Register(tSS<'.','j','p','e','g'>(), (tImageDecoderFactory*)this);
		tImageCodecFactoryManager::instance()->Register(tSS<'.','j','f','i','f'>(), (tImageDecoderFactory*)this);
		tImageCodecFactoryManager::instance()->Register(tSS<'.','j','p','g'>(), (tImageEncoderFactory*)this);
		tImageCodecFactoryManager::instance()->Register(tSS<'.','j','p','e','g'>(), (tImageEncoderFactory*)this);
		tImageCodecFactoryManager::instance()->Register(tSS<'.','j','f','i','f'>(), (tImageEncoderFactory*)this);
	}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa


