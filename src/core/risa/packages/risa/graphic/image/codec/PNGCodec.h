//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief PNGコーデック
//---------------------------------------------------------------------------
#ifndef PNGCodecH
#define PNGCodecH

#include "risa/packages/risa/graphic/image/ImageCodec.h"



namespace Risa {
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * PNGイメージデコーダ
 */
class tPNGImageDecoder : public tImageDecoder
{
	tDictionaryInstance * Dictionary; //!< メタデータ用辞書配列

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

	/**
	 * メタデータ用辞書配列が存在するかどうかを得る
	 * @return	メタデータ用辞書配列
	 */
	bool HasDictionary() const { return Dictionary != NULL; }

	/**
	 * メタデータ用辞書配列に値をpushする
	 * @param key	キー(文字列)
	 * @param value	値
	 */
	void PushMetadata(const tString & key, const tVariant & value);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * PNGイメージエンコーダ
 */
class tPNGImageEncoder : public tImageEncoder
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
