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
	const risse_char * Ptr; //!< 解析ポインタの現在位置
	const risse_char * PtrOrigin; //!< 解析ポインタの先頭
	const risse_char * PtrPrevious; //!< 前回返したトークンの先頭位置

	//! @brief		トークンIDと値の組
	struct tTokenIdAndValue
	{
		int				Id;		//!< ID
		tRisseVariant	Value;	//!< 値

		//! @brief		コンストラクタ
		//! @param		id		Id
		//! @param		value	値
		tTokenIdAndValue(int id, const tRisseVariant & value)
			{ Id = id; Value = value; }
	};

	gc_deque<tTokenIdAndValue> TokenFIFO; //!< トークン用の FIFO バッファ (先読みを行う場合に使う)

	risse_char ContinueEmbeddableString;
		//!< 「埋め込み可能な」文字列の解析を次のトークン読み込み
		//!   で再開するかどうか(0=しない、'\'' または '"' =デリミタ)

	bool NextIsRegularExpression; //!< 次の解析は正規表現パターン

public:
	//! @brief		コンストラクタ
	//! @param		script		入力スクリプト
	tRisseLexer(const tRisseString & script);

	//! @brief		トークンを得る
	//! @param		val		トークンの値の格納先
	//! @return		トークンID
	int GetToken(tRisseVariant & val);

	//! @brief		現在の解析位置を得る
	//! @return		現在の解析位置
	risse_size GetPosition() const { return Ptr - PtrOrigin; }

	//! @brief		次のトークン読み込みで「埋め込み可能な」文字列の解析を再開する
	//! @param		delimiter		文字列の終了デリミタ
	void SetContinueEmbeddableString(risse_char delimiter) { ContinueEmbeddableString = delimiter; }

	//! @brief		次のトークン読み込みで正規表現パターンを解析する
	void SetNextIsRegularExpression() { NextIsRegularExpression = true; }

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

