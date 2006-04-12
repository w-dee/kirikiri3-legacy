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
	Script = script;
	Ptr = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tRisseLexer::GetToken(tRisseVariant & val)
{
	if(Ptr == NULL)
	{
		// 初回
		// Ptr にスクリプト文字列へのポインタを代入
		Ptr = Script.c_str();

		// Ptr の先頭は #! で始まっているか (UN*Xにおけるインタプリタに対応)
		if(Ptr[0] == RISSE_WC('#') && Ptr[1] == RISSE_WC('!'))
			SkipToLineEnd(Ptr); // 改行までスキップ
	}

	int token;
	do
	{
		// ホワイトスペースのスキップ
		if(!SkipSpace(Ptr)) { token = -1; break; } // EOF

		// 現在位置にあるトークンを解析
		const risse_char * ptr_start = Ptr;

		token = RisseMapToken(Ptr, val);

		// トークンによってはさらに処理をしなければならない場合があるので分岐
		switch(token)
		{
		case T_BEGIN_COMMENT: // コメントの開始
			// コメントをスキップする
			Ptr = ptr_start;
			switch(SkipComment(Ptr))
			{
			case scrEnded: // スクリプトが終わった
				token = -1; // EOF
			case scrContinue: // スクリプトはまだ続く
			case scrNotComment: // コメントではなかった(あり得ない)
				token = T_NONE;
			}
			break;

		default:
			break;
		}

	} while(token == T_NONE);

	return token;
}
//---------------------------------------------------------------------------


} // namespace Risse



