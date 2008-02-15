//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ピクセル形式関連
//---------------------------------------------------------------------------
#include "prec.h"
#include "visual/image/PixelType.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(46171,32073,43095,17838,16560,33312,37643,36234);
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
tPixel::tDescriptor tPixel::Descriptors[tPixel::NumFormats] = {
	{
		1 // size
	},
	{
		4 // size
	},
};
//---------------------------------------------------------------------------



} // namespace Risa
namespace risa_gl {
//---------------------------------------------------------------------------
//	(暫定) ピクセル形式の変換(risa_glのpixel_convertの特殊化)
//---------------------------------------------------------------------------
//! @brief		pixelからrisse_uint8へ
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
//! @brief		risse_uint8からpixelへ
template <>
pixel pixel_convert<risse_uint8, pixel>(const risse_uint8 & src)
{
	return pixel(src, src, src);
}
//---------------------------------------------------------------------------
} // namespace risa_gl
namespace Risa {





//---------------------------------------------------------------------------
//! @brief		変換ループ(汎用)
//---------------------------------------------------------------------------
template <typename src_type, typename dest_type>
static void ConvertLoop(RISSE_RESTRICT void * dest, risse_size dest_size,
	RISSE_RESTRICT const void * src, risse_size src_size, risse_size length)
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
void tPixel::Convert(RISSE_RESTRICT void * dest, tPixel::tFormat dest_format,
		RISSE_RESTRICT const void * src, tPixel::tFormat src_format, risse_size length)
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
	typedef void (*tConvertLoop)(RISSE_RESTRICT void * dest, risse_size dest_size,
		RISSE_RESTRICT const void * src, risse_size src_size, risse_size length);
	static tConvertLoop Loops[/*dest*/NumFormats][/*src*/NumFormats] = {
		{ NULL, ConvertLoop<risa_gl::pixel, risse_uint8> },
		{ ConvertLoop<risse_uint8, risa_gl::pixel>, NULL },
	};

	// 変換を行う
	Loops[dest_format][src_format](dest, dest_desc.Size, src, src_desc.Size, length);
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tPixelConstsModule::tPixelConstsModule(tScriptEngine * engine) :
	tModuleBase(tSS<'P','i','x','e','l','C','o','n','s','t','s'>(), engine)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPixelConstsModule::RegisterMembers()
{
	GetInstance()->RegisterFinalConstMember(
			tSS<'p','f','G','r','a','y','8'>(),
			tVariant((risse_int64)tPixel::pfGray8), true);
	GetInstance()->RegisterFinalConstMember(
			tSS<'p','f','A','R','G','B','3','2'>(),
			tVariant((risse_int64)tPixel::pfARGB32), true);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		PixelConsts モジュールレジストラ
template class tRisseModuleRegisterer<tPixelConstsModule>;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


