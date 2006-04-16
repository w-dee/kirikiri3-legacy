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
#include "../prec.h"

#include "../risseLexerUtils.h"
#include "../risseException.h"
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
	ContinueEmbeddableString = 0;
	Script = script;
	Ptr = NULL;
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
		Ptr = Script.c_str();

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
		return id;
	}

	// ContinueEmbeddableString ?
	if(ContinueEmbeddableString != 0)
	{
		// 「埋め込み可能な」文字列リテラルの開始
		risse_char delimiter = ContinueEmbeddableString;
		ContinueEmbeddableString = 0;
		return ParseEmbeddableString(val, delimiter);
	}

	// トークンを読む
	do
	{
		// ホワイトスペースのスキップ
		if(!SkipSpace(Ptr)) { id = -1; break; } // EOF

		// 現在位置にあるトークンを解析
		const risse_char * ptr_start = Ptr;

		id = RisseMapToken(Ptr, val);

		// トークンによってはさらに処理をしなければならない場合があるので分岐
		switch(id)
		{
		case 0:
			{
				// 認識できないトークン
				eRisseError::Throw(RISSE_WS_TR("Unrecognized token"));
				break;
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
					eRisseError::Throw(RISSE_WS_TR("Invalid octet literal"));
				val = octet;
				id = T_CONSTVAL;
				break;
			}

		case T_BEGIN_NUMBER: // 数値リテラルの開始
			{
				Ptr = ptr_start;
				if(!ParseNumber(Ptr, val))
					eRisseError::Throw(RISSE_WS_TR("Invalid number literal"));
				id = T_CONSTVAL;
				break;
			}

		case T_SYMBOL: // 記号(識別子)
			{
				// ptr_start から Ptr までの範囲が記号である
				val = tRisseString(ptr_start, Ptr - ptr_start);
				break;
			}

		default:
			break;
		}

	} while(id == T_NONE);

	return id;
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


