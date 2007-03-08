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
#include "../prec.h"

#include "../risseLexerUtils.h"
#include "../risseExceptionClass.h"
#include "risseLexer.h"
#include "risseParser.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(26774,17704,8265,19906,55701,8958,30467,4610);

// マッパーを include
#include "risseLexerMap.inc"


//---------------------------------------------------------------------------
tRisseLexer::tRisseLexer(const tRisseString & script)
{
	// フィールドを初期化
	NextIsRegularExpression = false;
	ContinueEmbeddableString = 0;
	Script = script;
	Ptr = NULL;
	PtrOrigin = NULL;
	PtrLastTokenStart = NULL;
	LastTokenId = T_NL; // 最初の空行を読み飛ばすように
	NewLineRunningCount = 1;
	FuncCallReduced = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tRisseLexer::GetToken(tRisseVariant & val)
{
	int id;

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
		LastTokenId = id;
		return id;
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
		tRisseString pat, flags;
		if(!ParseRegExp(Ptr, pat, flags)) return 0;
		// 一回には一回のトークンしか返すことができないので
		// 最初にパターンを送り、次にフラグを送る
		TokenFIFO.push_back(tTokenIdAndValue(T_REGEXP_FLAGS, flags));
		val = pat;
		id = T_REGEXP;
		LastTokenId = id;
		return id;
	}

	// ContinueEmbeddableString ?
	if(ContinueEmbeddableString != 0)
	{
		// 「埋め込み可能な」文字列リテラルの開始
		risse_char delimiter = ContinueEmbeddableString;
		ContinueEmbeddableString = 0;
		id = ParseEmbeddableString(val, delimiter);
		LastTokenId = id;
		return id;
	}

	// トークンを読む
	id = T_NONE;
	do
	{
		// ホワイトスペースのスキップ
		if(!SkipSpaceExceptForNewLine(Ptr)) { id = -1; break; } // EOF

		// 改行のチェック
		if(*Ptr == '\r' || *Ptr == '\n')
		{
			StepNewLineChar(Ptr);
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


		// 現在位置にあるトークンを解析
		const risse_char * ptr_start = PtrLastTokenStart = Ptr;

		id = RisseMapToken(Ptr, val);

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
				tRisseCompileExceptionClass::Throw(RISSE_WS_TR("unrecognized token"));
				break;
			}

		case T_RBRACE:
			{
				// "}" の場合はセミコロンを返し、その直後に "}" を返す。
				// これは、 "}" の直前でセミコロンを省略できるようにするためである。
				// 逆に、"}" の直前には文脈にかかわらず ";" が来るため、余分な ";" を
				// 吸収する文法を定義しておかなければならない。
				TokenFIFO.push_back(tTokenIdAndValue(T_RBRACE, tRisseVariant::GetVoidObject()));
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

		case T_BEGIN_STRING_LITERAL: // 文字列リテラルの開始
			{
				tRisseString str;
				switch(ParseString(Ptr, str, *ptr_start, false))
				{
				case psrNone:
					break;
				case psrDelimiter:
					id = T_CONSTVAL;
					break;
				case psrAmpersand:
				case psrDollar:
					break;
				}
				val = str;
				break;
			}

		case T_BEGIN_EMSTRING_LITERAL: // 「埋め込み可能な」文字列リテラルの開始
			{
				// この時点で ptr_start は '@' 、Ptr[-1] は '"' または '\'' のはず
				// (つまりデリミタ)
				id = ParseEmbeddableString(val, Ptr[-1]);
				break;
			}

		case T_BEGIN_OCTET_LITERAL: // オクテット列リテラルの開始
			{
				tRisseOctet octet;
				Ptr = ptr_start;
				if(!ParseOctet(Ptr, octet))
					tRisseCompileExceptionClass::Throw(RISSE_WS_TR("invalid octet literal"));
				val = octet;
				id = T_CONSTVAL;
				break;
			}

		case T_BEGIN_NUMBER: // 数値リテラルの開始
			{
				Ptr = ptr_start;
				if(!ParseNumber(Ptr, val))
					tRisseCompileExceptionClass::Throw(RISSE_WS_TR("invalid number literal"));
				id = T_CONSTVAL;
				break;
			}

		case T_ID: // 識別子
			{
				// ptr_start から Ptr までの範囲が記号である
				val = tRisseString(ptr_start, Ptr - ptr_start);
				break;
			}

		default:
			break;
		}

	} while(id == T_NONE);

	LastTokenId = id;
	return id;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseLexer::SetFuncCallReduced()
{
	FuncCallReduced = true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseLexer::CheckBlockAfterFunctionCall()
{
	// f()
	// {
	// }
	// のうち、"{" が読み込まれた直後にメソッドが呼ばれる。
	// f() のあとの改行の数を数え、1つきりならばエラーにする。
	if(FuncCallReduced && NewLineRunningCount == 1)
		tRisseCompileExceptionClass::Throw(RISSE_WS_TR("ambiguous block after function call"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tRisseLexer::ParseEmbeddableString(tRisseVariant & val, risse_char delimiter)
{
	tRisseString str;
	int id;
	switch(ParseString(Ptr, str, delimiter, true))
	{
	case psrNone:
		id = 0;
		break;
	case psrDelimiter: // デリミタに遭遇した; 文字列リテラルの終わり
		id = T_CONSTVAL;
		break;
	case psrAmpersand:// & に遭遇したのでいったん解析が打ち切られた
		id = (delimiter==RISSE_WC('"'))?
			T_EMSTRING_AMPERSAND_D:T_EMSTRING_AMPERSAND_S;
		break;
	case psrDollar: // ${ に遭遇したのでいったん解析が打ち切られた
		id = (delimiter==RISSE_WC('"'))?
			T_EMSTRING_DOLLAR_D:T_EMSTRING_DOLLAR_S;
		break;
	}
	val = str;
	return id;
}
//---------------------------------------------------------------------------


} // namespace Risse



