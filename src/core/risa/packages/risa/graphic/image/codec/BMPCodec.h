//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief BMPコーデック
//---------------------------------------------------------------------------
#ifndef BMPCodecH
#define BMPCodecH

#include "risa/packages/risa/graphic/image/ImageCodec.h"



namespace Risa {
//---------------------------------------------------------------------------



struct RISSE_WIN_BITMAPINFOHEADER;
//---------------------------------------------------------------------------
/**
 * BMPイメージデコーダ
 */
class tBMPImageDecoder : public tImageDecoder
{
public:
	/**
	 * デコードを行う
	 * @param stream		入力ストリーム
	 * @param pixel_format	要求するピクセル形式
	 * @param callback		進捗コールバック(NULL=イラナイ)
	 * @param dict			メタデータ用の辞書配列(NULL=メタデータ要らない場合)
	 */
	virtual void Process(tStreamInstance * stream,
						tPixel::tFormat pixel_format, tProgressCallback * callback,
						tDictionaryInstance * dict);

private:
	/**
	 * 内部関数
	 */
	void InternalLoadBMP(tStreamAdapter src,
						tPixel::tFormat pixel_format, tProgressCallback * callback,
						tDictionaryInstance * dict, RISSE_WIN_BITMAPINFOHEADER & bi,
						risse_uint8 * palsrc);


};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * BMPイメージエンコーダ
 */
class tBMPImageEncoder : public tImageEncoder
{
public:
	/**
	 * エンコードを行う
	 * @param stream		入力ストリーム
	 * @param pixel_format	要求するピクセル形式
	 * @param callback		進捗コールバック(NULL=イラナイ)
	 * @param dict			メタデータ用の辞書配列(NULL=メタデータ要らない場合)
	 */
	virtual void Process(tStreamInstance * stream,
					tProgressCallback * callback,
					tDictionaryInstance * dict);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


#endif
