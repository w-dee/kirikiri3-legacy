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
#ifndef PixelTypeH
#define PixelTypeH

#include <risa_gl/pixel_store.hpp>
#include <risa_gl/pixel.hpp>
#include "base/script/RisseEngine.h"
#include "risse/include/risseModule.h"


namespace Risa {
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		ピクセル関連クラス
//---------------------------------------------------------------------------
class tPixel
{
public:

	//! @brief		ピクセル形式
	enum tFormat
	{
		pfGray8, //!< 8bpp グレースケール (risse_uint8)
		pfARGB32, //!< 32bpp Alpha, R, G, B (もっとも一般的) (risa_gl::pixel)
	};
	static const int NumFormats = 2; //!< 形式の最大数

	//! @brief		ピクセル形式デスクリプタ
	struct tDescriptor
	{
		int				Size; //!< 1ピクセルを格納するのに必要なサイズ
		const char *	Name; //!< 名前
		const char *	LongDesc; //!< 長い説明
	};

	//! @brief		ピクセル形式デスクリプタの配列(tFormatに対応)
	static tDescriptor Descriptors[NumFormats];

	//! @brief		ピクセル形式からピクセル形式デスクリプタを得る
	//! @param		format		ピクセル形式
	//! @return		対応するピクセル形式デスクリプタ
	static const tDescriptor & GetDescriptorFromFormat(tFormat format)
	{
		return Descriptors[static_cast<int>(format)];
	}

	//! @brief		ピクセル形式の変換を行う
	//! @param		dest			変換先
	//! @param		dest_format		変換先形式
	//! @param		src				変換元
	//! @param		src_format		変換元形式
	//! @param		length			変換する長さ(ピクセル単位)
	static void Convert(void * RISSE_RESTRICT dest, tFormat dest_format,
		const void * RISSE_RESTRICT src, tFormat src_format, risse_size length);

	//! @brief		ARGB32 形式の数値へ変換する
	//! @param		pixe			ピクセルへのポインタ
	//! @param		pixel_format	ピクセルのフォーマット
	//! @return		ARGB32 形式の値
	static risse_uint32 ConvertToARGB32(const void * pixel, tFormat pixel_format);

	//! @brief		ARGB32 形式の数値から変換する
	//! @param		pixe			ピクセルへのポインタ
	//! @param		pixel_format	ピクセルのフォーマット
	//! @param		v				ARGB32形式の値
	static void ConvertFromARGB32(void * pixel, tFormat pixel_format, risse_uint32 v);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa


#endif
