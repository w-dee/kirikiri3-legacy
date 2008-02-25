//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief JPEGコーデック
//---------------------------------------------------------------------------
#ifndef JPEGCodecH
#define JPEGCodecH

#include "visual/image/ImageCodec.h"



namespace Risa {
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		JPEGイメージデコーダ
//---------------------------------------------------------------------------
class tJPEGImageDecoder : public tImageDecoder
{
public:
	//! @brief		デコードを行う
	//! @param		stream		入力ストリーム
	//! @param		pixel_format	要求するピクセル形式
	//! @param		callback	進捗コールバック(NULL=イラナイ)
	//! @param		dict		メタデータ用の辞書配列(NULL=メタデータ要らない場合)
	virtual void Process(tStreamInstance * stream,
						tPixel::tFormat pixel_format, tProgressCallback * callback,
						tDictionaryInstance * dict);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		JPEGイメージエンコーダ
//---------------------------------------------------------------------------
class tJPEGImageEncoder : public tImageEncoder
{
public:
	//! @brief		エンコードを行う
	//! @param		stream		入力ストリーム
	//! @param		pixel_format	要求するピクセル形式
	//! @param		callback	進捗コールバック(NULL=イラナイ)
	//! @param		dict		メタデータ用の辞書配列(NULL=メタデータ要らない場合)
	virtual void Process(tStreamInstance * stream,
					tProgressCallback * callback,
					tDictionaryInstance * dict);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


#endif
