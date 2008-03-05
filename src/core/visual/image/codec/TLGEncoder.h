//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief TLGエンコーダ
//---------------------------------------------------------------------------
#ifndef TLGEncoderH
#define TLGEncoderH

#include "visual/image/ImageCodec.h"



namespace Risa {
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		TLGイメージエンコーダ
//---------------------------------------------------------------------------
class tTLGImageEncoder : public tImageEncoder
{
private:
	void EncodeTLG5(tStreamInstance * stream,
					tProgressCallback * callback,
					int compos);
	void EncodeTLG6(tStreamInstance * stream,
					tProgressCallback * callback,
					int compos);
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
