//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse レキシカル・アナライザ(字句解析器)の実装
//---------------------------------------------------------------------------
#include "../prec.h"

#include "../risseLexerUtils.h"
#include "../risseExceptionClass.h"
#include "../risseScriptBlockClass.h"
#include "risseLexer.h"
#include "risseParser.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(26774,17704,8265,19906,55701,8958,30467,4610);

// マッパーを include
#include "risseLexerMap.def"


//---------------------------------------------------------------------------
tLexer::tLexer(tScriptBlockInstance * sb)
{
	// フィールドを初期化
	ScriptBlockInstance = sb;
	Script = ScriptBlockInstance->GetScript();
	NextIsRegularExpression = false;
	ContinueEmbeddableString = 0;
	Ptr = NULL;
	PtrOrigin = NULL;
	PtrLastTokenStart = NULL;
	LastTokenId = T_NL; // 最初の空行を読み飛ばすように
	NewLineRunningCount = 1;
	FuncCallReduced = false;
	IgnoreNextNewLineStyleCheck = false;

	// デフォルトでは改行を無視しない
	PushRecognizeNewLine();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tLexer::GetToken(tVariant & val)
{
	int id = T_NONE;

	// 初回?
	if(Ptr == NULL)
	{
		// 初回
		// Ptr にスクリプト文字列へのポインタを代入
		Ptr = PtrOrigin = PtrLastTokenStart = Script.c_str();

		// Ptr の先頭は #! で始まっているか (UN*Xにおけるインタプリタに対応)
		if(Ptr[0] == RISSE_WC('#') && Ptr[1] == RISSE_WC('!'))
			SkipToLineEnd(Ptr); // 改行までスキップ
	}

	// TokenFIFO に値が入っている場合は先頭をpopして返す
	if(TokenFIFO.size() != 0)
	{
		tTokenIdAndValue & value = TokenFIFO.front();
		id = value.Id;
		val = value.Value;
		TokenFIFO.pop_front();
		if(id == -1)
			return id; // -1 (スクリプトの終わり) が入っていた場合はここで帰る。
						// そうしないとまた -1 が push されてしまい無限ループに陥る。
		goto ret;
	}

	// NextIsRegularExpression ?
	if(NextIsRegularExpression)
	{
		NextIsRegularExpression = false;
		// 次のトークン読み込みで正規表現パターンを解析する
		// パーサは、/= や / を見つけ、それが２項演算子で無い場合に
		// SetNextIsRegularExpression() を呼び、それが
		// 正規表現パターンであることをlexerに伝える。
		// lexer はすでに /= や / を返したあとなので
		// 前の位置にもどり、正規表現パターンの解析を行う。
		Ptr = PtrLastTokenStart; // 前の解析位置に戻す
		tString pat, flags;
		if(!ParseRegExp(Ptr, pat, flags)) { id = -1; goto ret; }
		// 一回には一回のトークンしか返すことができないので
		// 最初にパターンを送り、次にフラグを送る
		TokenFIFO.push_back(tTokenIdAndValue(T_REGEXP_FLAGS, flags));
		val = pat;
		id = T_REGEXP;
		goto ret;
	}

	// ContinueEmbeddableString ?
	if(ContinueEmbeddableString != 0)
	{
		// 「埋め込み可能な」文字列リテラルの開始
		risse_char delimiter = ContinueEmbeddableString;
		ContinueEmbeddableString = 0;
		id = ParseEmbeddableString(val, delimiter);
		goto ret;
	}

	// トークンを読む
	do
	{
		if(GetIgnoreNewLine())
		{
			// 改行を無視する場合
			if(!SkipSpace(Ptr)) { id = -1; break; } // EOF
		}
		else
		{
			// 改行を無視しない場合

			// ホワイトスペースのスキップ
			if(!SkipSpaceExceptForNewLine(Ptr)) { id = -1; break; } // EOF

			// 改行のチェック
			if(*Ptr == '\r' || *Ptr == '\n')
			{
				StepNewLineChar(Ptr);
				if(LastTokenId == T_RBRACE)
				{
					// 直前のトークンが "}" だった場合は、
					// 直後の改行スタイルのチェックを行わない。
					// "}" の直後にはセミコロンを書かないが、それ以外
					// ではセミコロンを書く、といった混在のスタイルが
					// 十分ありうるため、警告を抑制する。
					IgnoreNextNewLineStyleCheck = true;
				}

				if(LastTokenId == T_NL)
				{
					// 連続する T_NL は一つにまとめる
					NewLineRunningCount ++;
					continue;
				}
				else
				{
					NewLineRunningCount = 1;
				}
				id = T_NL;
				break;
			}
		}


		// 現在位置にあるトークンを解析
		const risse_char * ptr_start = PtrLastTokenStart = Ptr;

		id = MapToken(Ptr, val);

		// LBRACE ?
		if(id != T_LBRACE)
		{
			// 関数呼び出しの次の "{" をチェックするためのフラグを倒す
			FuncCallReduced = false;
		}

		// トークンによってはさらに処理をしなければならない場合があるので分岐
		switch(id)
		{
		case 0:
			{
				// 認識できないトークン
				tCompileExceptionClass::Throw(RISSE_WS_TR("unrecognized token"));
				break;
			}

		case T_RBRACE:
			{
				// "}" の場合はセミコロンを返し、その直後に "}" を返す。
				// これは、 "}" の直前でセミコロンを省略できるようにするためである。
				// 逆に、"}" の直前には文脈にかかわらず ";" が来るため、余分な ";" を
				// 吸収する文法を定義しておかなければならない。
				TokenFIFO.push_back(tTokenIdAndValue(T_RBRACE, tVariant::GetVoidObject()));
				return T_SEMICOLON;
			}

		case T_BEGIN_COMMENT: // コメントの開始
			{
				// コメントをスキップする
				Ptr = ptr_start;
				switch(SkipComment(Ptr))
				{
				case scrEnded: // スクリプトが終わった
					id = -1; // EOF
					break;
				case scrContinue: // スクリプトはまだ続く
				case scrNotComment: // コメントではなかった(あり得ない)
					id = T_NONE;
					break;
				}
				break;
			}

		case T_BEGIN_EMSTRING_LITERAL: // 「埋め込み可能な」文字列リテラルの開始
			{
				// この時点で Ptr[-1] は '"' または '\'' のはず
				// (つまりデリミタ)
				id = ParseEmbeddableString(val, Ptr[-1]);
				break;
			}

		case T_BEGIN_OCTET_LITERAL: // オクテット列リテラルの開始
			{
				tOctet octet;
				Ptr = ptr_start;
				if(!ParseOctet(Ptr, octet))
					tCompileExceptionClass::Throw(RISSE_WS_TR("invalid octet literal"));
				val = octet;
				id = T_CONSTVAL;
				break;
			}

		case T_BEGIN_NUMBER: // 数値リテラルの開始
			{
				Ptr = ptr_start;
				if(!ParseNumber(Ptr, val))
					tCompileExceptionClass::Throw(RISSE_WS_TR("invalid number literal"));
				id = T_CONSTVAL;
				break;
			}

		case T_ID: // 識別子
			{
				// ptr_start から Ptr までの範囲が記号である
				val = tString(ptr_start, Ptr - ptr_start);
				break;
			}

		default:
			break;
		}

	} while(id == T_NONE);

ret:
	if(id == -1)
	{
		// スクリプトの終了。
		if(LastTokenId != T_NL)
		{
			// いったん改行を返してからスクリプトの終了ということにする
			// (最終行に改行が入っていない場合への対処)
			id = T_NL;
			TokenFIFO.push_back(tTokenIdAndValue(-1, tVariant::GetVoidObject()));
		}
	}

	LastTokenId = id;
	return id;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tLexer::NotifyStatementEndStyle(bool semicolon)
{
	if(NewLineRecogInfo.size() == 0) return;

	if(!IgnoreNextNewLineStyleCheck)
	{

		// 前回、文の終了にセミコロンを使ったのか、改行を使ったのかを調べる。
		// スタイルが変わっていれば警告する。
		bool warn = false;
		switch(NewLineRecogInfo.back().SemicolonState)
		{
		case tNewLineRecogInfo::ssUnknown:
			break;

		case tNewLineRecogInfo::ssSemicolon:
			if(!semicolon)
				warn = true;
			break;

		case tNewLineRecogInfo::ssNewLine:
			if(semicolon)
				warn = true;
			break;
		}
		NewLineRecogInfo.back().SemicolonState =
			semicolon ? tNewLineRecogInfo::ssSemicolon : tNewLineRecogInfo::ssNewLine;

		if(warn)
			ScriptBlockInstance->OutputWarning(GetLastTokenStart(), 
				RISSE_WS_TR("mixing semicolon style and newline style") );
	}
	IgnoreNextNewLineStyleCheck = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tLexer::SetFuncCallReduced()
{
	FuncCallReduced = true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tLexer::CheckBlockAfterFunctionCall()
{
	// f()
	// {
	// }
	// のうち、"{" が読み込まれた直後にメソッドが呼ばれる。
	// f() のあとの改行の数を数え、1つきりならば警告にする。
	if(FuncCallReduced && NewLineRunningCount == 1)
		ScriptBlockInstance->OutputWarning(GetLastTokenStart(), 
			RISSE_WS_TR("ambiguous block after function call") );
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tLexer::ParseEmbeddableString(tVariant & val, risse_char delimiter)
{
	tString str;
	int id;
	switch(ParseString(Ptr, str, delimiter, true))
	{
	case psrNone:
		id = -1;
		break;
	case psrDelimiter: // デリミタに遭遇した; 文字列リテラルの終わり
		id = T_CONSTVAL;
		break;
	case psrEmExpr: // \{ に遭遇したのでいったん解析が打ち切られた
		id = (delimiter==RISSE_WC('"'))?
			T_EMSTRING_D:T_EMSTRING_S;
		break;
	}
	val = str;
	return id;
}
//---------------------------------------------------------------------------


} // namespace Risse



