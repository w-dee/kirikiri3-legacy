//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse レキシカル・アナライザ(字句解析器)の実装
//---------------------------------------------------------------------------
#ifndef RisseLexerH
#define RisseLexerH

#include "../risseLexerUtils.h"
#include "../risseCxxString.h"

namespace Risse
{

//---------------------------------------------------------------------------
class tRisseLexer : public tRisseLexerUtility, public tRisseCollectee
{
	tRisseString Script; //!< スクリプト
	const risse_char * Ptr; //!< 解析ポインタ

	//! @brief		トークンIDと値の組
	struct tTokenIdAndValue
	{
		int				Id;		//!< ID
		tRisseVariant	Value;	//!< 値
	};

	gc_deque<tTokenIdAndValue> TokenFIFO; //!< トークン用の FIFO バッファ (先読みを行う場合に使う)

	risse_char ContinueEmbeddableString;
		//!< 「埋め込み可能な」文字列の解析を次のトークン読み込み
		//!   で再開するかどうか(0=しない、'\'' または '"' =デリミタ)

public:
	//! @brief		コンストラクタ
	//! @param		script		入力スクリプト
	tRisseLexer(const tRisseString & script);

	//! @brief		トークンを得る
	//! @param		val		トークンの値の格納先
	//! @return		トークンID
	int GetToken(tRisseVariant & val);

	//! @brief		次のトークン読み込みで「埋め込み可能な」文字列の解析を再開する
	//! @param		delimiter		文字列の終了デリミタ
	void SetContinueEmbeddableString(risse_char delimiter) { ContinueEmbeddableString = delimiter; }

private:
	//! @brief		埋め込み可能な文字列リテラルの解析
	//! @param		val		トークンの値の格納先
	//! @param		delimiter		デリミタ ( '"' か '\'' )
	//! @return		トークンID
	int ParseEmbeddableString(tRisseVariant & val, risse_char delimiter);
};
//---------------------------------------------------------------------------

} // namespace Risse

#endif

