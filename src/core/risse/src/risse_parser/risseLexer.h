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
enum tRisseLexerToken
{
T_GT                    ,
T_EQUAL                 ,
T_EXCRAMATION           ,
T_AMPERSAND             ,
T_VERTLINE              ,
T_DOT                   ,
T_PLUS                  ,
T_MINUS                 ,
T_ASTERISK              ,
T_SLASH                 ,
T_BACKSLASH             ,
T_PERCENT               ,
T_CHEVRON               ,
T_LBRACKET              ,
T_RBRACKET              ,
T_LPARENTHESIS          ,
T_RPARENTHESIS          ,
T_TILDE                 ,
T_QUESTION              ,
T_COLON                 ,
T_COMMA                 ,
T_SEMICOLON             ,
T_LBRACE                ,
T_RBRACE                ,
T_SHARP                 ,
T_DOLLAR                ,
T_BEGIN_STRING_LITERAL  ,
T_BEGIN_MACRO           ,
T_RBITSHIFTEQUAL        ,
T_RBITSHIFT             ,
T_RARITHSHIFTEQUAL      ,
T_RARITHSHIFT           ,
T_GTOREQUAL             ,
T_LT                    ,
T_LARITHSHIFTEQUAL      ,
T_SWAP                  ,
T_LTOREQUAL             ,
T_LARITHSHIFT           ,
T_BEGIN_OCTET_LITERAL   ,
T_DISCEQUAL             ,
T_EQUALEQUAL            ,
T_DISCNOTEQUAL          ,
T_NOTEQUAL              ,
T_LOGICALANDEQUAL       ,
T_LOGICALAND            ,
T_AMPERSANDEQUAL        ,
T_LOGICALOREQUAL        ,
T_LOGICALOR             ,
T_VERTLINEEQUAL         ,
T_OMIT                  ,
T_INCREMENT             ,
T_PLUSEQUAL             ,
T_MINUSEQUAL            ,
T_DECREMENT             ,
T_ASTERISKEQUAL         ,
T_SLASHEQUAL            ,
T_BEGIN_COMMENT         ,
T_BACKSLASHEQUAL        ,
T_PERCENTEQUAL          ,
T_CHEVRONEQUAL          ,
T_BREAK                 ,
T_CONTINUE              ,
T_CONST                 ,
T_CATCH                 ,
T_CLASS                 ,
T_CASE                  ,
T_DEBUGGER              ,
T_DEFAULT               ,
T_DELETE                ,
T_DO                    ,
T_EXTENDS               ,
T_EXPORT                ,
T_ENUM                  ,
T_ELSE                  ,
T_FUNCTION              ,
T_FINALLY               ,
T_FINAL                 ,
T_FALSE                 ,
T_FOR                   ,
T_GLOBAL                ,
T_GETTER                ,
T_GOTO                  ,
T_INCONTEXTOF           ,
T_INVALIDATE            ,
T_INSTANCEOF            ,
T_ISVALID               ,
T_IMPORT                ,
T_INT                   ,
T_IN                    ,
T_IF                    ,
T_NULL                  ,
T_NEW                   ,
T_OCTET                 ,
T_PROTECTED             ,
T_PROPERTY              ,
T_PRIVATE               ,
T_PUBLIC                ,
T_RETURN                ,
T_REAL                  ,
T_SYNCHRONIZED          ,
T_SWITCH                ,
T_STATIC                ,
T_SETTER                ,
T_STRING                ,
T_SUPER                 ,
T_TYPEOF                ,
T_THROW                 ,
T_THIS                  ,
T_TRUE                  ,
T_TRY                   ,
T_VOID                  ,
T_VAR                   ,
T_WHILE                 ,
T_NAN                   ,
T_INFINITY              ,
T_WITH                  ,
T_SYMBOL                ,
};


//---------------------------------------------------------------------------
class tRisseLexer : public tRisseLexerUtility
{
	tRisseString Script; //!< スクリプト
	const risse_char * Ptr; //!< 解析ポインタ
public:
	//! @brief		トークンを得る
	//! @return		トークンID
	int GetToken();
};
//---------------------------------------------------------------------------

} // namespace Risse

#endif

