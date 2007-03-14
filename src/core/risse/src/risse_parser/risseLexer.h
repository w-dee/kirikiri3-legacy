//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse レキシカル・アナライザ(字句解析器)の実装
//---------------------------------------------------------------------------
#ifndef RisseLexerH
#define RisseLexerH

#include "../risseLexerUtils.h"
#include "../risseString.h"

namespace Risse
{

//---------------------------------------------------------------------------
class tRisseLexer : public tRisseLexerUtility
{
	tRisseString Script; //!< スクリプト
	const risse_char * Ptr; //!< 解析ポインタの現在位置
	const risse_char * PtrOrigin; //!< 解析ポインタの先頭
	const risse_char * PtrLastTokenStart; //!< 最後に返したトークンの先頭位置
	int		LastTokenId; //!< 最後に返したトークンの ID
	risse_size NewLineRunningCount; //!< 改行が連続した回数

	//! @brief		トークンIDと値の組
	struct tTokenIdAndValue : public tRisseCollectee
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
	bool FuncCallReduced; //!< 関数の呼び出しが還元し終わったか

	gc_vector<bool> IgnoreNewLineStack; //!< 改行を無視するかどうかを表すスタック

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

	//! @brief		最後に返したトークンの先頭位置を得る
	//! @return		最後に返したトークンの先頭位置
	risse_size GetLastTokenStart() const { return PtrLastTokenStart - PtrOrigin; }

	//! @brief		次のトークン読み込みで「埋め込み可能な」文字列の解析を再開する
	//! @param		delimiter		文字列の終了デリミタ
	void SetContinueEmbeddableString(risse_char delimiter) { ContinueEmbeddableString = delimiter; }

	//! @brief		次のトークン読み込みで正規表現パターンを解析する
	void SetNextIsRegularExpression() { NextIsRegularExpression = true; }

	//! @brief		改行を無視するかどうかを返す
	//! @return		改行を無視するかどうか
	bool GetIgnoreNewLine() const
	{
		if(IgnoreNewLineStack.size() == 0) return false;
		return IgnoreNewLineStack.back();
	}

	//! @brief		改行を無視する区間を開始する
	void PushIgnoreNewLine()
	{
		IgnoreNewLineStack.push_back(true);
	}

	//! @brief		改行を無視する区間を終了する
	void PopIgnoreNewLine()
	{
		RISSE_ASSERT(IgnoreNewLineStack.size() != 0);
		RISSE_ASSERT(IgnoreNewLineStack.back() == true);
		IgnoreNewLineStack.pop_back();
	}

	//! @brief		改行を無視しない区間を開始する
	void PushRecognizeNewLine()
	{
		IgnoreNewLineStack.push_back(false);
	}

	//! @brief		改行を無視しない区間を終了する
	void PopRecognizeNewLine()
	{
		RISSE_ASSERT(IgnoreNewLineStack.size() != 0);
		RISSE_ASSERT(IgnoreNewLineStack.back() == false);
		IgnoreNewLineStack.pop_back();
	}

	//! @brief		関数呼び出しが還元されたことを通知する
	void SetFuncCallReduced();

	//! @brief		関数呼び出しの次の "{" の位置をチェックする
	//! @note		f()@n
	//!				{@n
	//!				}@n
	//!				のような呼び出しは、関数をブロック付きで呼び出しているのか
	//!				それとも関数呼び出し + ブロックなのかの区別がつきにくいため、
	//!				あえて文法エラーにする。
	void CheckBlockAfterFunctionCall();

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

