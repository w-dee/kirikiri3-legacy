%{
/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
/*---------------------------------------------------------------------------*/
/*! @brief   Risse 日付parser bison 入力ファイル */
/* rissDateParsere.y */
/* Risse date parser bison input file */

#include "prec.h"

#include <stdio.h>
#include <string.h>
#include "risseTypes.h"
#include "risseGC.h"
#include "risseDateParser.h"

/* 名前空間を Risse に */
namespace Risse
{
RISSE_DEFINE_SOURCE_ID(61520,32259,31684,19117,29118,20416,49765,257);

/* token マッパーを include */
#include "risseDateLexerMap.def"

/* メモリ確保は Risse のインターフェースを使うように */
#define YYMALLOC	MallocCollectee
#define YYREALLOC	ReallocCollectee
#define YYFREE		FreeCollectee

/* 最大深さ */
#define YYMAXDEPTH 20000

/*! パーサへのアクセス */
#define PR (pr)

/* YYSTYPE の前方参照用定義 */
union YYSTYPE;

/* yylex のプロトタイプ */
int yylex(YYSTYPE * value, tDateParser *pr);

/* yyerror のプロトタイプ */
int raise_dperror(const char * msg, tDateParser *pr);

/* yyerror のリダイレクト */
#define rissedperror(pr, X) raise_dperror(X, pr);



%}

/*###########################################################################*/

/* デバッグ */
/* TODO: リリース前にはずしておこう！ */
%debug

/* 再入可能なパーサを出力 */
%pure-parser

/* 型 */
%parse-param {tDateParser * pr}
%lex-param   {tDateParser * pr}

/* union 定義 */
%union{
	long			val;
}

/* トークン定義 */
%token
					DP_NONE
					DP_NUMBER
					DP_AM			"am"
					DP_PM			"pm"
					DP_PLUS			"+"
					DP_MINUS		"-"
					DP_COLON		":"
					DP_LPARENTHESIS	"("
					DP_RPARENTHESIS	")"
					DP_COMMA		","
					DP_DOT			"."
					DP_SLASH		"/"
					DP_T			"T"
%token <val>		DP_MONTH
%token <val>		DP_WDAY
%token <val>		DP_TZ

%token <val>		DP_NUMBER1
%token <val>		DP_NUMBER2
%token <val>		DP_NUMBER3
%token <val>		DP_NUMBER4
%token <val>		DP_NUMBER5
%token <val>		DP_NUMBER6
%token <val>		DP_NUMBER7
%token <val>		DP_NUMBER8
%token <val>		DP_NUMBER9


%type <val>			number_1to2 number_3to4 number_8to9



%%
/*###########################################################################*/


/* the program */
input
	: day_of_the_week day_and_month
	  date_sep year_time_tz /* RFC822 or strftime style */
	| day_and_month
	  date_sep year_time_tz /* RFC822 or strftime style */
	| year4
	| year4 date_sep n_month
	| n_month_and_year
	| year4 date_sep n_month_and_day
	| year4 date_sep day_and_named_month
	| compact_date
	| day_and_month
	| day_and_month date_sep year4
	| year4 date_sep n_month_and_day t_opt time tz_omittable
	| year4 date_sep day_and_named_month t_opt time tz_omittable
	| compact_date t_opt time tz_omittable
			/* above are RFC 3339 5.6. Internet Date/Time Format style */
;

year_time_tz
	: year2to5 t_opt time tz_omittable
	| time tz_omittable year2to5
;


/* comma, ommitable */
comma_opt
	: "," | /*empty*/
;

/* comma or dot */
/* 小数点表記が場合によってはカンマ */
comma_or_dot : "," | "." ;


/* separators for date */
date_sep : "-" | "/" | "." | "," | /*empty*/ ;

/* separator for date and time */
/* RFC3339 では 日付と時刻を区切る "T" の代わりに空白が認められている?? */
t_opt
	: "T" | "," | /*empty*/
;

/* year, 4 or 5 digits  */
year4
	: DP_NUMBER4							{ PR->Year = $1, PR->YearSet = true; }
	| DP_NUMBER5							{ PR->Year = $1, PR->YearSet = true; }
;

/* year, 2 to 5 digits  */
year2to5
	: DP_NUMBER2							{ PR->Year = ($1<70)?($1+2000):($1+1900), PR->YearSet = true; }
	| DP_NUMBER3							{ PR->Year = $1+1900, PR->YearSet = true; }
	| DP_NUMBER4							{ PR->Year = $1, PR->YearSet = true; }
	| DP_NUMBER5							{ PR->Year = $1, PR->YearSet = true; }
;

/* day of the week */
day_of_the_week
	: DP_WDAY comma_opt						{ PR->Day = $1, PR->DaySet = true; }
;

/* numeric month or named month */
n_month
	: number_1to2							{ PR->Month = $1 - 1, PR->MonthSet = true; }
	| DP_MONTH								{ PR->Month = $1, PR->MonthSet = true; }
;

/* numeric month or named month, and day */
n_month_and_day
	: number_1to2 date_sep number_1to2		{ PR->Month = $1 - 1, PR->MonthSet = true;
											  PR->Date = $3, PR->DateSet = true; }
	| DP_MONTH date_sep number_1to2			{ PR->Month = $1, PR->MonthSet = true;
											  PR->Date = $3, PR->DateSet = true; }
;

/* day and named month */
day_and_named_month
	: number_1to2 date_sep DP_MONTH			{ PR->Date = $1, PR->DateSet = true;
											  PR->Month = $3, PR->MonthSet = true; }
;

/* numeric month or named month and year */
n_month_and_year
	: number_1to2 date_sep year4			{ PR->Month = $1 - 1, PR->MonthSet = true; }
	| DP_MONTH date_sep year4				{ PR->Month = $1, PR->MonthSet = true; }
;

/* day and month spec */
day_and_month
	: number_1to2 date_sep DP_MONTH			{ PR->Date = $1, PR->DateSet = true;
											  PR->Month = $3, PR->MonthSet = true; }
	| DP_MONTH date_sep number_1to2			{ PR->Date = $3, PR->DateSet = true;
											  PR->Month = $1, PR->MonthSet = true; }
;

/* compact date */
compact_date
	: number_8to9							{ PR->Year = $1 / 10000, PR->YearSet = true;
											  PR->Month = $1 / 100 % 100 - 1, PR->MonthSet = true;
											  PR->Date = $1 % 100, PR->DateSet = true; }
;

/* time */
time
	: time_hms am_pm
	| time_hms
	| am_pm time_hms
	| compact_time
	| compact_time comma_or_dot subsecond
;

time_hms
	: number_1to2 ":" number_1to2			{ PR->Hours = $1, PR->HoursSet = true;
											  PR->Minutes = $3, PR->MinutesSet = true; }
	| number_1to2 ":" number_1to2 ":"
	  number_1to2							{ PR->Hours = $1, PR->HoursSet = true;
											  PR->Minutes = $3, PR->MinutesSet = true;
											  PR->Seconds = $5, PR->SecondsSet = true; }
	| number_1to2 ":" number_1to2 ":"
	  number_1to2 comma_or_dot subsecond	{ PR->Hours = $1, PR->HoursSet = true;
											  PR->Minutes = $3, PR->MinutesSet = true;
											  PR->Seconds = $5, PR->SecondsSet = true; }
;

subsecond
	: DP_NUMBER1							{ PR->Milliseconds = $1*100     , PR->MillisecondsSet = true; }
	| DP_NUMBER2							{ PR->Milliseconds = $1*10      , PR->MillisecondsSet = true; }
	| DP_NUMBER3							{ PR->Milliseconds = $1         , PR->MillisecondsSet = true; }
	| DP_NUMBER4							{ PR->Milliseconds = $1/10      , PR->MillisecondsSet = true; }
	| DP_NUMBER5							{ PR->Milliseconds = $1/100     , PR->MillisecondsSet = true; }
	| DP_NUMBER6							{ PR->Milliseconds = $1/1000    , PR->MillisecondsSet = true; }
	| DP_NUMBER7							{ PR->Milliseconds = $1/10000   , PR->MillisecondsSet = true; }
	| DP_NUMBER8							{ PR->Milliseconds = $1/100000L , PR->MillisecondsSet = true; }
	| DP_NUMBER9							{ PR->Milliseconds = $1/1000000L, PR->MillisecondsSet = true; }
;

am_pm
	: "am"									{ PR->AMPM = false, PR->AMPMSet = true; }
	| "pm"									{ PR->AMPM = true , PR->AMPMSet = true; }
;

compact_time
	: DP_NUMBER6							{ PR->Hours = $1 / 10000, PR->HoursSet = true;
											  PR->Minutes = $1 / 100 % 100, PR->MinutesSet = true;
											  PR->Seconds = $1 % 100, PR->SecondsSet = true; }
;

/* timezones */
tz_name_omittable
	:	DP_TZ								{ PR->Timezone = $1, PR->TimezoneSet = true; }
	|	/* empty */
;

tz_offset_omittable
	:	"+" number_3to4						{ PR->TimezoneOffset = $2, PR->TimezoneOffsetSet = true; }
	|	"-" number_3to4						{ PR->TimezoneOffset =-$2, PR->TimezoneOffsetSet = true; }
	|	/* empty */
;

tz_omittable
	:	tz_name_omittable tz_offset_omittable
;



/* numbers */
number_1to2 : DP_NUMBER1 | DP_NUMBER2 ;
number_3to4 : DP_NUMBER3 | DP_NUMBER4 ;
number_8to9 : DP_NUMBER8 | DP_NUMBER9 ;


/*###########################################################################*/

%%

//---------------------------------------------------------------------------
int yylex(YYSTYPE * value, tDateParser *pr)
{
	int token = PR->GetToken(value->val);
//fprintf(stderr, "%d\n", token);
//fflush(stderr);
	return token;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int raise_dperror(char const * p, tDateParser *pr)
{
	(void)p;
	PR->HasError = true;
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tDateParser::tDateParser(const tString & str) : Input(str)
{
	Ptr = Input.c_str();

	Year = 1970; // 年
	Month = 0; // 月
	Date = 1; // 日
	Day = 4; // 曜日
	Hours = 0; // 時
	Minutes = 0; // 分
	Seconds = 0; // 秒
	Milliseconds = 0; // ミリ秒
	Timezone = 0; // タイムゾーン(普通は) -1159 ～ 1159 
	TimezoneOffset = 0; // タイムゾーンに対するオフセット
	AMPM = false; // AM(false), PM(true)

	YearSet = false; // 年が記述されていたか？
	MonthSet = false; // 月が記述されていたか？
	DateSet = false; // 日が記述されていたか？
	DaySet = false; // 曜日が記述されていたか？
	HoursSet = false; // 時が記述されていたか？
	MinutesSet = false; // 分が記述されていたか？
	SecondsSet = false; // 秒が記述されていたか？
	MillisecondsSet = false; // ミリ秒が記述されていたか？
	TimezoneSet = false; // タイムゾーンが記述されていたか？
	TimezoneOffsetSet = false; // タイムゾーンに対するオフセットが記述されていたか？
	AMPMSet = false; // AM/PM が記述されていたか？

	HasError = false; // エラーが発生したか？

	yyparse(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tDateParser::GetToken(long & val)
{
restart:
	if(!SkipSpace(Ptr)) return -1; // EOF
	const risse_char *ptr_save = Ptr;
	int id = MapToken(Ptr, val);
	if(id == 0) return -1;
	if(id == DP_NUMBER)
	{
		// 数値の始まり
		Ptr = ptr_save;
		long n = 0;
		long v = 0;
		while(iswdigit_nc(*Ptr))
		{
			v *= 10; v += *Ptr - '0';
			n ++;
			Ptr ++;
			if(n == 9) break; // 8 桁まで読む
		}
		while(iswdigit_nc(*Ptr)) Ptr ++; // それ以降は読み飛ばす
			/* 7桁より大きな数値は結局のところsubsecondsでしか
			   用いられない。W3C の日付フォーマットではsubsecondsの
			   桁数に制限がないがここでは7桁までの対応となる
			   (実際はparser内部で3桁にさらに丸めている) */
		val = v;

		// 桁数に応じて返すIDが違う。
		switch(n)
		{
		case 1: id = DP_NUMBER1; break;
		case 2: id = DP_NUMBER2; break;
		case 3: id = DP_NUMBER3; break;
		case 4: id = DP_NUMBER4; break;
		case 5: id = DP_NUMBER5; break;
		case 6: id = DP_NUMBER6; break;
		case 7: id = DP_NUMBER7; break;
		case 8: id = DP_NUMBER8; break;
		case 9: id = DP_NUMBER9; break;
		default: return -1;
		}
	}
	else if(id == DP_LPARENTHESIS)
	{
		// DP_RPARENTHESIS ………ではなくて次の ')' までスキップ
		while(*Ptr != ')' && *Ptr != 0) Ptr++;
		if(*Ptr != 0) Ptr ++;
		goto restart;
	}
	return id;
}
//---------------------------------------------------------------------------



} // namespace Risse




