//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

/* 正式な Parser ができるまでのダミーモジュール ! */
#ifndef RisseParserH
#define RisseParserH

namespace Risse
{
//---------------------------------------------------------------------------


#ifdef RISSE_DEFINE_TOKEN
	#undef RISSE_DEFINE_TOKEN
#endif

#ifdef RISSE_DEFINE_TOKEN_STRING
	#define RISSE_DEFINE_TOKEN(X) #X
#else
	#define RISSE_DEFINE_TOKEN(X) X
#endif

#ifdef RISSE_DEFINE_TOKEN_STRING
static const char * RisseLexerTokenString[] = {
#else
enum tRisseLexerToken {
#endif
RISSE_DEFINE_TOKEN(T_NONE                  ),
RISSE_DEFINE_TOKEN(T_GT                    ),
RISSE_DEFINE_TOKEN(T_EQUAL                 ),
RISSE_DEFINE_TOKEN(T_EXCRAMATION           ),
RISSE_DEFINE_TOKEN(T_AMPERSAND             ),
RISSE_DEFINE_TOKEN(T_VERTLINE              ),
RISSE_DEFINE_TOKEN(T_DOT                   ),
RISSE_DEFINE_TOKEN(T_PLUS                  ),
RISSE_DEFINE_TOKEN(T_MINUS                 ),
RISSE_DEFINE_TOKEN(T_ASTERISK              ),
RISSE_DEFINE_TOKEN(T_SLASH                 ),
RISSE_DEFINE_TOKEN(T_BACKSLASH             ),
RISSE_DEFINE_TOKEN(T_PERCENT               ),
RISSE_DEFINE_TOKEN(T_CHEVRON               ),
RISSE_DEFINE_TOKEN(T_LBRACKET              ),
RISSE_DEFINE_TOKEN(T_RBRACKET              ),
RISSE_DEFINE_TOKEN(T_LPARENTHESIS          ),
RISSE_DEFINE_TOKEN(T_RPARENTHESIS          ),
RISSE_DEFINE_TOKEN(T_TILDE                 ),
RISSE_DEFINE_TOKEN(T_QUESTION              ),
RISSE_DEFINE_TOKEN(T_COLON                 ),
RISSE_DEFINE_TOKEN(T_COMMA                 ),
RISSE_DEFINE_TOKEN(T_SEMICOLON             ),
RISSE_DEFINE_TOKEN(T_LBRACE                ),
RISSE_DEFINE_TOKEN(T_RBRACE                ),
RISSE_DEFINE_TOKEN(T_SHARP                 ),
RISSE_DEFINE_TOKEN(T_DOLLAR                ),
RISSE_DEFINE_TOKEN(T_BEGIN_STRING_LITERAL  ),
RISSE_DEFINE_TOKEN(T_BEGIN_MACRO           ),
RISSE_DEFINE_TOKEN(T_RBITSHIFTEQUAL        ),
RISSE_DEFINE_TOKEN(T_RBITSHIFT             ),
RISSE_DEFINE_TOKEN(T_RARITHSHIFTEQUAL      ),
RISSE_DEFINE_TOKEN(T_RARITHSHIFT           ),
RISSE_DEFINE_TOKEN(T_GTOREQUAL             ),
RISSE_DEFINE_TOKEN(T_LT                    ),
RISSE_DEFINE_TOKEN(T_LARITHSHIFTEQUAL      ),
RISSE_DEFINE_TOKEN(T_SWAP                  ),
RISSE_DEFINE_TOKEN(T_LTOREQUAL             ),
RISSE_DEFINE_TOKEN(T_LARITHSHIFT           ),
RISSE_DEFINE_TOKEN(T_BEGIN_OCTET_LITERAL   ),
RISSE_DEFINE_TOKEN(T_DISCEQUAL             ),
RISSE_DEFINE_TOKEN(T_EQUALEQUAL            ),
RISSE_DEFINE_TOKEN(T_DISCNOTEQUAL          ),
RISSE_DEFINE_TOKEN(T_NOTEQUAL              ),
RISSE_DEFINE_TOKEN(T_LOGICALANDEQUAL       ),
RISSE_DEFINE_TOKEN(T_LOGICALAND            ),
RISSE_DEFINE_TOKEN(T_AMPERSANDEQUAL        ),
RISSE_DEFINE_TOKEN(T_LOGICALOREQUAL        ),
RISSE_DEFINE_TOKEN(T_LOGICALOR             ),
RISSE_DEFINE_TOKEN(T_VERTLINEEQUAL         ),
RISSE_DEFINE_TOKEN(T_OMIT                  ),
RISSE_DEFINE_TOKEN(T_INCREMENT             ),
RISSE_DEFINE_TOKEN(T_PLUSEQUAL             ),
RISSE_DEFINE_TOKEN(T_MINUSEQUAL            ),
RISSE_DEFINE_TOKEN(T_DECREMENT             ),
RISSE_DEFINE_TOKEN(T_ASTERISKEQUAL         ),
RISSE_DEFINE_TOKEN(T_SLASHEQUAL            ),
RISSE_DEFINE_TOKEN(T_BEGIN_COMMENT         ),
RISSE_DEFINE_TOKEN(T_BACKSLASHEQUAL        ),
RISSE_DEFINE_TOKEN(T_PERCENTEQUAL          ),
RISSE_DEFINE_TOKEN(T_CHEVRONEQUAL          ),
RISSE_DEFINE_TOKEN(T_BREAK                 ),
RISSE_DEFINE_TOKEN(T_CONTINUE              ),
RISSE_DEFINE_TOKEN(T_CONST                 ),
RISSE_DEFINE_TOKEN(T_CATCH                 ),
RISSE_DEFINE_TOKEN(T_CLASS                 ),
RISSE_DEFINE_TOKEN(T_CASE                  ),
RISSE_DEFINE_TOKEN(T_DEBUGGER              ),
RISSE_DEFINE_TOKEN(T_DEFAULT               ),
RISSE_DEFINE_TOKEN(T_DELETE                ),
RISSE_DEFINE_TOKEN(T_DO                    ),
RISSE_DEFINE_TOKEN(T_EXTENDS               ),
RISSE_DEFINE_TOKEN(T_EXPORT                ),
RISSE_DEFINE_TOKEN(T_ENUM                  ),
RISSE_DEFINE_TOKEN(T_ELSE                  ),
RISSE_DEFINE_TOKEN(T_FUNCTION              ),
RISSE_DEFINE_TOKEN(T_FINALLY               ),
RISSE_DEFINE_TOKEN(T_FINAL                 ),
RISSE_DEFINE_TOKEN(T_FALSE                 ),
RISSE_DEFINE_TOKEN(T_FOR                   ),
RISSE_DEFINE_TOKEN(T_GLOBAL                ),
RISSE_DEFINE_TOKEN(T_GETTER                ),
RISSE_DEFINE_TOKEN(T_GOTO                  ),
RISSE_DEFINE_TOKEN(T_INCONTEXTOF           ),
RISSE_DEFINE_TOKEN(T_INVALIDATE            ),
RISSE_DEFINE_TOKEN(T_INSTANCEOF            ),
RISSE_DEFINE_TOKEN(T_ISVALID               ),
RISSE_DEFINE_TOKEN(T_IMPORT                ),
RISSE_DEFINE_TOKEN(T_INT                   ),
RISSE_DEFINE_TOKEN(T_IN                    ),
RISSE_DEFINE_TOKEN(T_IF                    ),
RISSE_DEFINE_TOKEN(T_NULL                  ),
RISSE_DEFINE_TOKEN(T_NEW                   ),
RISSE_DEFINE_TOKEN(T_OCTET                 ),
RISSE_DEFINE_TOKEN(T_PROTECTED             ),
RISSE_DEFINE_TOKEN(T_PROPERTY              ),
RISSE_DEFINE_TOKEN(T_PRIVATE               ),
RISSE_DEFINE_TOKEN(T_PUBLIC                ),
RISSE_DEFINE_TOKEN(T_RETURN                ),
RISSE_DEFINE_TOKEN(T_REAL                  ),
RISSE_DEFINE_TOKEN(T_SYNCHRONIZED          ),
RISSE_DEFINE_TOKEN(T_SWITCH                ),
RISSE_DEFINE_TOKEN(T_STATIC                ),
RISSE_DEFINE_TOKEN(T_SETTER                ),
RISSE_DEFINE_TOKEN(T_STRING                ),
RISSE_DEFINE_TOKEN(T_SUPER                 ),
RISSE_DEFINE_TOKEN(T_TYPEOF                ),
RISSE_DEFINE_TOKEN(T_THROW                 ),
RISSE_DEFINE_TOKEN(T_THIS                  ),
RISSE_DEFINE_TOKEN(T_TRUE                  ),
RISSE_DEFINE_TOKEN(T_TRY                   ),
RISSE_DEFINE_TOKEN(T_VOID                  ),
RISSE_DEFINE_TOKEN(T_VAR                   ),
RISSE_DEFINE_TOKEN(T_WHILE                 ),
RISSE_DEFINE_TOKEN(T_NAN                   ),
RISSE_DEFINE_TOKEN(T_INFINITY              ),
RISSE_DEFINE_TOKEN(T_WITH                  ),
RISSE_DEFINE_TOKEN(T_SYMBOL                ),
RISSE_DEFINE_TOKEN(T_CONSTVAL              ),
};

//---------------------------------------------------------------------------

} // namespace Risse

#endif

