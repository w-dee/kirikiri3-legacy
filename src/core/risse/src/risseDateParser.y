%{
/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
/*---------------------------------------------------------------------------*/
/*! @brief   Risse 日付parser bison 入力ファイル (文法定義) */
/* rissDateParsere.y */
/* Risse date parser bison input file */

#include "prec.h"

#include <stdio.h>
#include <string.h>
#include "risseTypes.h"
#include "risseGC.h"
#include "risseObject.h"
#include "risseExceptionClass.h"
#include "risseOpCodes.h"
#include "risseStaticStrings.h"

/* 名前空間を Risse に */
namespace Risse
{
RISSE_DEFINE_SOURCE_ID();

/* メモリ確保は Risse のインターフェースを使うように */
#define YYMALLOC	MallocCollectee
#define YYREALLOC	ReallocCollectee
#define YYFREE		FreeCollectee

/* 最大深さ */
#define YYMAXDEPTH 20000


%}

/*###########################################################################*/

/* デバッグ */
/* TODO: リリース前にはずしておこう！ */
%debug

/* 再入可能なパーサを出力 */
%pure-parser

/* 詳細なエラー出力 */
%error-verbose

/* GLR parser */
%glr-parser

/* union 定義 */
%union{
	int			val;
}

/* トークン定義 */
%token				DP_AM			"am"
					DP_PM			"pm"
					DP_PLUS			"+"
					DP_MINUS		"-"
					DP_COLON		":"
					DP_LPARENTHESIS	"("
					DP_RPARENTHESIS	")"
					DP_COMMA		","
					DP_DOT			"."
					DP_SLASH		"/"
%token <val>		DP_NUMBER
%token <val>		DP_MONTH
%token <val>		DP_WDAY
%token <val>		DP_TZ


/* expect */
%expect 1


%%
/*###########################################################################*/


/* the program */
input
	: day_of_the_week_opt day_and_month minus_opt year time tz_omittable
;

/* comma, ommitable */
comma_opt
	: "," | /*empty*/
;

/* minus or hyphen, ommitable */
minus_opt
	: "-" | /*empty*/
;

/* year */
year
	: DP_NUMBER								{ PR->SetYear($1); }
;

/* day of the week, ommitable */
day_of_the_week_opt
	: /*empty*/
	| DP_WDAY comma_opt						{ PR->SetDayOfTheWeek($1); }
;

/* day and month spec */
day_and_month
	: DP_NUMBER minus_opt DP_MONTH			{ PR->SetMDay($1); PR->SetMonth($3-1); }
	| DP_MONTH minus_opt DP_NUMBER			{ PR->SetMDay($3); PR->SetMonth($1-1); }
;

/* time */
time
	: time_hms am_pm_opt
	| am_pm_opt time_hms
;

time_hms
	: DP_NUMBER ":" DP_NUMBER								{ PR->SetHour($1); PR->SetMinute($3); }
	| DP_NUMBER ":" DP_NUMBER ":" DP_NUMBER					{ PR->SetHour($1); PR->SetMinute($3); PR->SetSecond($5); }
	| DP_NUMBER ":" DP_NUMBER ":" DP_NUMBER "." DP_NUMBER	{ PR->SetHour($1); PR->SetMinute($3); PR->SetSecond($5);
															  PR->SetSubSecond($7); }
;


am_pm_opt
	: "am"									{ PR->SetAM(); }
	| "pm"									{ PR->SetPM(); }
	| /* empty */
;

/* timezones */
tz_name_omittable
	:	DP_TZ											{ dp->SetTimeZone($1); }
	|	/* empty */
;

tz_offset_omittable
	:	'+' DP_NUMBER									{ dp->SetTimeZoneOffset($2); }
	|	'-' DP_NUMBER									{ dp->SetTimeZoneOffset(-$2); }
	|	/* empty */
;

tz_omittable
	:	tz_name_omittable
		tz_offset_omittable
;


/*###########################################################################*/

%%

//---------------------------------------------------------------------------
int yylex(YYSTYPE * value, void *pr)
{
	value->value = new tVariant();
	int token = PR->GetToken(*(value->value));
	llocp->first = LX->GetLastTokenStart();
	llocp->last = LX->GetPosition();
//	fprintf(stderr, "token : %d\n", token);
	return token;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int raise_yyerror(const char * msg, void *pr)
{
	tCompileExceptionClass::Throw(PR->GetScriptBlockInstance()->GetScriptEngine(),
		tString(msg), PR->GetScriptBlockInstance(), loc->first);
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tDateParser::tDateParser()
{
	ScriptBlockInstance = sb;
	Root = NULL;
	Lexer = lexer;

	yyparse(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tParser::GetToken(tVariant & value)
{
	// lexer 中に発生するかもしれない例外をキャッチする
	// これらには位置情報が含まれていない可能性があるため、トレース情報として
	// 追加する。
	try
	{
		try
		{
			return Lexer->GetToken(value);
		}
		catch(const tTemporaryException * e)
		{
			e->ThrowConverted(ScriptBlockInstance->GetScriptEngine());
		}
	}
	catch(const tTemporaryException * te)
	{
		const tVariant * e = te->Convert(ScriptBlockInstance->GetScriptEngine());
		e->AddTrace(ScriptBlockInstance, Lexer->GetPosition());
		throw e;
	}
	catch(const tVariant * e)
	{
		e->AddTrace(ScriptBlockInstance, Lexer->GetPosition());
		throw e;
	}
	catch(...)
	{
		throw;
	}
	return -1;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tParser::SetRootNode(tASTNode * root)
{
	Root = root;
}
//---------------------------------------------------------------------------


} // namespace Risse




