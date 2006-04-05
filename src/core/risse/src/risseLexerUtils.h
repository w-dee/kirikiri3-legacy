//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse レキシカル・アナライザ(字句解析器)向けのユーティリティ関数群
//---------------------------------------------------------------------------
#include "prec.h"

namespace Risse
{
//---------------------------------------------------------------------------
class tRisseLexerUtility
{
public: // public static members
	static risse_int HexNum(risse_char ch);
	static risse_int OctNum(risse_char ch);
	static risse_int DecNum(risse_char ch);
	static risse_int BinNum(risse_char ch);
	static risse_int UnescapeBackSlash(risse_char ch);

	//! @brief	コメントをスキップしようとした際の状態
	enum tSkipCommentResult
	{
		scrContinue,	//!< スクリプトはまだ続く
		scrEnded,		//!< スクリプトは終わった
		scrNotComment	//!< 実はコメントではなかった
	};

	static tRisseSkipCommentResult SkipComment(const risse_char * & ptr);
	static bool SkipSpace(const risse_char * & ptr);


	static bool StringMatch(const risse_char * & ptr, const risse_char *wrd, bool isword);

	//! @brief InternalParseString の戻り値
	enum tParseStringResult
	{
		psrNone, //!< (エラー)
		psrDelimiter, //!< デリミタにぶつかった
		psrAmpersand, //!< & にぶつかった ( &式; 形式の埋め込み )
		psrDollar
	};

	static tParseStringResult
		ParseString(const risse_char * & ptr, tRisseString &val,
			risse_char delim, bool embexpmode);
	static bool ParseString(const risse_char * & ptr, tRisseString &val);


};

//---------------------------------------------------------------------------
} // namespace Risse


