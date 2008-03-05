//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief TLGデコーダ
//---------------------------------------------------------------------------
#ifndef TLGDecoderH
#define TLGDecoderH

#include "visual/image/ImageCodec.h"



namespace Risa {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#define TLG6_GOLOMB_N_COUNT  4
#define TLG6_LeadingZeroTable_BITS 12
#define TLG6_LeadingZeroTable_SIZE  (1<<TLG6_LeadingZeroTable_BITS)
extern "C" {
extern char TLG6GolombBitLengthTable
	[TLG6_GOLOMB_N_COUNT*2*128][TLG6_GOLOMB_N_COUNT];
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		TLGイメージデコーダ
//---------------------------------------------------------------------------
class tTLGImageDecoder : public tImageDecoder
{

private:
	//! @brief		(内部関数)TLG5 のデコードを行う
	void ProcessTLG5(tStreamAdapter & src,
						tPixel::tFormat pixel_format, tProgressCallback * callback,
						tDictionaryInstance * dict);
	//! @brief		(内部関数)TLG6 のデコードを行う
	void ProcessTLG6(tStreamAdapter & src,
						tPixel::tFormat pixel_format, tProgressCallback * callback,
						tDictionaryInstance * dict);
	//! @brief		(内部関数)TLG5/TLG6 のデコードを行う
	void ProcessTLG(tStreamAdapter & src,
						tPixel::tFormat pixel_format, tProgressCallback * callback,
						tDictionaryInstance * dict);

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
} // namespace Risa


#endif
