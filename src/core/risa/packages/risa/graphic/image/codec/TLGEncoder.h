//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief TLGエンコーダ
//---------------------------------------------------------------------------
#ifndef TLGEncoderH
#define TLGEncoderH

#include "risa/packages/risa/graphic/image/ImageCodec.h"



namespace Risa {
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * TLGイメージエンコーダ
 */
class tTLGImageEncoder : public tImageEncoder
{
	gc_vector<char> MetaData; //!< メタデータ
private:
	void EncodeTLG5(tStreamInstance * stream,
					tProgressCallback * callback,
					int compos);
	void EncodeTLG6(tStreamInstance * stream,
					tProgressCallback * callback,
					int compos);

	/**
	 * 辞書配列からのコールバックを受け取るためのRisseメソッド
	 */
	class tCallback : public tObjectInterface
	{
		tTLGImageEncoder & Encoder; //!< TLGイメージエンコーダインスタンス
	public:
		tCallback(tTLGImageEncoder & encoder) : Encoder(encoder) {}
		virtual tRetValue Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG);
	};
	tCallback DictCallback; //!< 辞書配列からのコールバックを受け取るための Risse メソッド

public:
	/**
	 * コンストラクタ
	 */
	tTLGImageEncoder();

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
