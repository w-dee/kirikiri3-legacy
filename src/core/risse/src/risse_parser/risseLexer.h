//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse レキシカル・アナライザ(字句解析器)の実装
//---------------------------------------------------------------------------
#ifndef LexerH
#define LexerH

#include "../risseLexerUtils.h"
#include "../risseString.h"

namespace Risse
{
class tScriptBlockInstance;
//---------------------------------------------------------------------------
class tLexer : public tLexerUtility
{
	tScriptBlockInstance * ScriptBlockInstance; //!< スクリプトブロック
	tString Script; //!< スクリプト
	const risse_char * Ptr; //!< 解析ポインタの現在位置
	const risse_char * PtrOrigin; //!< 解析ポインタの先頭
	const risse_char * PtrLastTokenStart; //!< 最後に返したトークンの先頭位置
	int		LastTokenId; //!< 最後に返したトークンの ID
	risse_size NewLineRunningCount; //!< 改行が連続した回数

	/**
	 * トークンIDと値の組
	 */
	struct tTokenIdAndValue : public tCollectee
	{
		int				Id;		//!< ID
		tVariant	Value;	//!< 値

		/**
		 * コンストラクタ
		 * @param id	Id
		 * @param value	値
		 */
		tTokenIdAndValue(int id, const tVariant & value)
			{ Id = id; Value = value; }
	};

	gc_deque<tTokenIdAndValue> TokenFIFO; //!< トークン用の FIFO バッファ (先読みを行う場合に使う)

	risse_char ContinueEmbeddableString;
		//!< 「埋め込み可能な」文字列の解析を次のトークン読み込み

	bool NextIsRegularExpression; //!< 次の解析は正規表現パターン
	bool FuncCallReduced; //!< 関数の呼び出しが還元し終わったか

	/**
	 * 改行の取り扱いに関する情報
	 */
	struct tNewLineRecogInfo : public tCollectee
	{
		bool			Ignore;		//!< 改行を無視するかどうか
		/**
		 * セミコロンで文が終わったか、改行で文が終わったかの状態
		 * @note	書き方のスタイルが混在している場合に警告するためにある
		 */
		enum			tSemicolonState
		{
			ssUnknown,		//!< 不明
			ssSemicolon,	//!< セミコロンで文が終わってる
			ssNewLine,		//!< 改行で文が終わってる
		} SemicolonState;

		/**
		 * コンストラクタ
		 * @param ignore	改行を無視するかどうか
		 * @param semistate	セミコロンの状態
		 */
		tNewLineRecogInfo(bool ignoge, tSemicolonState semistate = ssUnknown)
			{ Ignore = ignoge; SemicolonState = semistate; }
	};
	gc_vector<tNewLineRecogInfo> NewLineRecogInfo; //!< 改行の取り扱いに関する情報のスタック

	bool IgnoreNextNewLineStyleCheck; //!< 次回、改行のスタイルのチェックを無視するかどうか

public:
	/**
	 * コンストラクタ
	 * @param sb	スクリプトブロック
	 */
	tLexer(tScriptBlockInstance * sb);

	/**
	 * スクリプトを得る
	 */
	const tString & GetScript() const { return Script; }

	/**
	 * トークンを得る
	 * @param val	トークンの値の格納先
	 * @return	トークンID
	 */
	int GetToken(tVariant & val);

	/**
	 * 現在の解析位置を得る
	 * @return	現在の解析位置
	 */
	risse_size GetPosition() const { return Ptr - PtrOrigin; }

	/**
	 * 最後に返したトークンの先頭位置を得る
	 * @return	最後に返したトークンの先頭位置
	 */
	risse_size GetLastTokenStart() const { return PtrLastTokenStart - PtrOrigin; }

	/**
	 * 次のトークン読み込みで「埋め込み可能な」文字列の解析を再開する
	 * @param delimiter	文字列の終了デリミタ
	 */
	void SetContinueEmbeddableString(risse_char delimiter) { ContinueEmbeddableString = delimiter; }

	/**
	 * 次のトークン読み込みで正規表現パターンを解析する
	 */
	void SetNextIsRegularExpression() { NextIsRegularExpression = true; }

	/**
	 * 改行を無視するかどうかを返す
	 * @return	改行を無視するかどうか
	 */
	bool GetIgnoreNewLine() const
	{
		if(NewLineRecogInfo.size() == 0) return false;
		return NewLineRecogInfo.back().Ignore;
	}

	/**
	 * 改行を無視する区間を開始する
	 */
	void PushIgnoreNewLine()
	{
		NewLineRecogInfo.push_back(tNewLineRecogInfo(true));
	}

	/**
	 * 改行を無視する区間を終了する
	 */
	void PopIgnoreNewLine()
	{
		RISSE_ASSERT(NewLineRecogInfo.size() != 0);
		RISSE_ASSERT(NewLineRecogInfo.back().Ignore == true);
		NewLineRecogInfo.pop_back();
	}

	/**
	 * 改行を無視しない区間を開始する
	 */
	void PushRecognizeNewLine()
	{
		NewLineRecogInfo.push_back(tNewLineRecogInfo(false));
	}

	/**
	 * 改行を無視しない区間を終了する
	 */
	void PopRecognizeNewLine()
	{
		RISSE_ASSERT(NewLineRecogInfo.size() != 0);
		RISSE_ASSERT(NewLineRecogInfo.back().Ignore == false);
		NewLineRecogInfo.pop_back();
	}

	/**
	 * 改行で一文が終わったかどうかを通知する
	 * @param semicolon	セミコロンを用いた場合に真
	 */
	void NotifyStatementEndStyle(bool semicolon);

	/**
	 * 関数呼び出しが還元されたことを通知する
	 */
	void SetFuncCallReduced();

	/**
	 * 関数呼び出しの次の "{" の位置をチェックする
	 * @note	f()@n
	 *			{@n
	 *			}@n
	 *			のような呼び出しは、関数をブロック付きで呼び出しているのか
	 *			それとも関数呼び出し + ブロックなのかの区別がつきにくいため、
	 *			警告を表示する。
	 */
	void CheckBlockAfterFunctionCall();

private:
	/**
	 * 埋め込み可能な文字列リテラルの解析
	 * @param val		トークンの値の格納先
	 * @param delimiter	デリミタ ( '"' か '\'' )
	 * @return	トークンID
	 */
	int ParseEmbeddableString(tVariant & val, risse_char delimiter);
};
//---------------------------------------------------------------------------

} // namespace Risse

#endif

