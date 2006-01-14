%{
/*---------------------------------------------------------------------------*/
/*
	Risse [�肹]
	alias RISE [りせ], acronym of "Rise Is a Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
/*---------------------------------------------------------------------------*/
/*! @brief   プリプロセッサ式評価 */
/* rissepp.y */
/* Risse conditional compiling control's conditional expression parser */

#include <malloc.h>

#include "risse.h"
#include "risseCompileControl.h"

/* param */
#define YYPARSE_PARAM pm
#define YYLEX_PARAM pm

NS_RISSE_START
RISSE_DEFINE_SOURCE_ID(1004);

/* yylex/yyerror prototype decl */
#define YYLEX_PROTO_DECL int pplex(YYSTYPE *yylex, void *pm);

int _pperror(char * msg, void *pm)
{
    return 0;
}

#define pperror(msg) _pperror(msg, pm);


#define risse (((tRissePPExprParser*)pm)->GetRisse())
#define ep ((tRissePPExprParser*)pm)
%}

%pure_parser

%union
{
	risse_int32		val;
	risse_int			nv;
}


%token
	PT_LPARENTHESIS				"("
	PT_RPARENTHESIS				")"
	PT_ERROR

	PT_COMMA					","
	PT_EQUAL					"="
	PT_NOTEQUAL					"!="
	PT_EQUALEQUAL				"=="
	PT_LOGICALOR				"||"
	PT_LOGICALAND				"&&"
	PT_VERTLINE					"|"
	PT_CHEVRON					"^"
	PT_AMPERSAND				"&"
	PT_LT						"<"
	PT_GT						">"
	PT_LTOREQUAL				"<="
	PT_GTOREQUAL				">="
	PT_PLUS						"+"
	PT_MINUS					"-"
	PT_ASTERISK					"*"
	PT_SLASH					"/"
	PT_PERCENT					"%"
	PT_EXCLAMATION				"!"

	PT_UN

%token <nv>		PT_SYMBOL
%token <val>	PT_NUM

%type <val>		expr


%left	","
%left	"||"
%left	"&&"
%left	"|"
%left	"^"
%left	"&"
%left   "="
%left	"!=" "=="
%left	"<" ">" "<=" ">="
%left	"+" "-"
%left	"%" "/" "*"
%right	"!" PT_UN

%%

input
	: expr						{ ep->Result = $1; }
;

expr
	: expr "," expr				{ $$ = $3; }
	| PT_SYMBOL "=" expr		{ risse->SetPPValue(ep->GetString($1), $3); $$ = $3; }
	| expr "!="	expr			{ $$ = $1 != $3; }
	| expr "==" expr			{ $$ = $1 == $3; }
	| expr "||" expr			{ $$ = $1 || $3; }
	| expr "&&" expr			{ $$ = $1 && $3; }
	| expr "|" expr				{ $$ = $1 | $3; }
	| expr "^" expr				{ $$ = $1 ^ $3; }
	| expr "&" expr				{ $$ = $1 & $3; }
	| expr "<" expr				{ $$ = $1 < $3; }
	| expr ">" expr				{ $$ = $1 > $3; }
	| expr ">=" expr			{ $$ = $1 >= $3; }
	| expr "<=" expr			{ $$ = $1 <= $3; }
	| expr "+" expr				{ $$ = $1 + $3; }
	| expr "-" expr				{ $$ = $1 - $3; }
	| expr "%" expr				{ $$ = $1 % $3; }
    | expr "*" expr				{ $$ = $1 * $3; }
	| expr "/" expr				{ if($3==0) { YYABORT; } else { $$ = $1 / $3; } }
	| "!" expr					{ $$ = ! $2; }
	| "+" expr %prec PT_UN		{ $$ = + $2; }
    | "-" expr %prec PT_UN		{ $$ = - $2; }
	| "(" expr ")"				{ $$ = $2; }
	| PT_NUM					{ $$ = $1; }
	| PT_SYMBOL					{ $$ = risse->GetPPValue(ep->GetString($1)); }
;

%%
