//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ピクセル形式関連
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risseTypes.h"
#include "risa/packages/risa/graphic/image/PixelType.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(46171,32073,43095,17838,16560,33312,37643,36234);
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
tPixel::tDescriptor tPixel::Descriptors[tPixel::NumFormats] = {
	{
		1, // size
		"pfGray8", // name
		"8bit grayscale" // long desc
	},
	{
		4, // size
		"pfARGB32", // name
		"32bit A8R8G8B8" // long desc
	},
};
//---------------------------------------------------------------------------



} // namespace Risa
namespace risa_gl {
using namespace ::Risse;
//---------------------------------------------------------------------------
//	(暫定) ピクセル形式の変換(risa_glのpixel_convertの特殊化)
//---------------------------------------------------------------------------
/**
 * pixelからrisse_uint8へ
 */
template <>
risse_uint8 pixel_convert<pixel, risse_uint8>(const pixel & src)
{
	// たしか libpng のグレースケール変換と一緒
	return static_cast<risse_uint8>(
		(
		src.get_red() * 54 +
		src.get_green() * 183 +
		src.get_blue() * 19
		) >> 8);
}
//---------------------------------------------------------------------------
/**
 * risse_uint8からpixelへ
 */
template <>
pixel pixel_convert<risse_uint8, pixel>(const risse_uint8 & src)
{
	return pixel(src, src, src);
}
//---------------------------------------------------------------------------
} // namespace risa_gl
namespace Risa {





//---------------------------------------------------------------------------
/**
 * 変換ループ(汎用)
 */
template <typename src_type, typename dest_type>
static void ConvertLoop(void * RISSE_RESTRICT dest, risse_size dest_size,
	const void * RISSE_RESTRICT src, risse_size src_size, risse_size length)
{
	// sizeof(src_type) とかを使った方がスマートかな………
	char * d = static_cast<char *>(dest);
	const char * s = static_cast<const char *>(src);
	for(risse_size i = 0 ; i < length ; i++)
	{
		*static_cast<dest_type*>(static_cast<void*>(d)) =
			risa_gl::pixel_convert<src_type, dest_type>(
				*static_cast<const src_type *>(static_cast<const void*>(s)) );
		d += dest_size;
		s += src_size;
	}
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void tPixel::Convert(void * RISSE_RESTRICT dest, tPixel::tFormat dest_format,
		const void * RISSE_RESTRICT src, tPixel::tFormat src_format, risse_size length)
{
	RISSE_ASSERT(dest != NULL);
	RISSE_ASSERT(src != NULL);
	RISSE_ASSERT(dest_format < NumFormats);
	RISSE_ASSERT(src_format < NumFormats);

	// ピクセルデスクリプタを取得
	const tDescriptor & dest_desc = GetDescriptorFromFormat(dest_format);
	const tDescriptor & src_desc  = GetDescriptorFromFormat(src_format);
	if(dest_format == src_format)
	{
		// 形式一緒じゃん
		memcpy(dest, src, length * dest_desc.Size);
		return;
	}

	// 変換が必要

	// 変換テーブル
	typedef void (*tConvertLoop)(void * RISSE_RESTRICT dest, risse_size dest_size,
		const void * RISSE_RESTRICT src, risse_size src_size, risse_size length);
	static tConvertLoop Loops[/*dest*/NumFormats][/*src*/NumFormats] = {
		{ NULL, ConvertLoop<risa_gl::pixel, risse_uint8> },
		{ ConvertLoop<risse_uint8, risa_gl::pixel>, NULL },
	};

	// 変換を行う
	Loops[dest_format][src_format](dest, dest_desc.Size, src, src_desc.Size, length);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint32 tPixel::ConvertToARGB32(const void * pixel, tFormat pixel_format)
{
	RISSE_ASSERT(pixel != NULL);

	switch(pixel_format)
	{
	case pfGray8:
		return *static_cast<const risse_uint8*>(pixel) * 0x00010101 + 0xff000000;

	case pfARGB32:
		return *static_cast<const risse_uint32*>(pixel);
	}

	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPixel::ConvertFromARGB32(void * pixel, tFormat pixel_format, risse_uint32 v)
{
	RISSE_ASSERT(pixel != NULL);

	switch(pixel_format)
	{
	case pfGray8:
		// いまのところ Alpha 成分は無視
		*static_cast<risse_uint8*>(pixel) =
			(
				(v & 0x00ff0000) *   54      + // R
				(v & 0x0000ff00) * (183<< 8) + // G
				(v & 0x000000ff) * ( 19<<24)   // B
			) >> 24;
		return;

	case pfARGB32:
		*static_cast<risse_uint32*>(pixel) = v;
		return;
	}
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risa


