//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief 日付文字列パーサー
//---------------------------------------------------------------------------
#ifndef risseDateParserH
#define risseDateParserH

#include "risseTypes.h"
#include "rissedate.tab.h"


namespace Risse
{


//---------------------------------------------------------------------------
// tRisseDateParser : A date/time parser class
//---------------------------------------------------------------------------
class tRisseDateParser
{
	bool YearSet;
	int Year;

	bool MonthSet;
	int Month;

	bool MDaySet;
	int MDay;

	bool HourSet;
	int Hour;

	bool MinSet;
	int Min;

	bool SecSet;
	int Sec;

	bool AMPMSet;
	bool AMPM; // pm:true am:false

	bool TimeZoneSet;
	int TimeZone;

	bool TimeZoneOffsetSet;
	int TimeZoneOffset;

	const risse_char *Input;
	const risse_char *InputPointer;

	risse_int64 Time; // time from 1970-01-01 00:00:00.00 GMT


	friend int dplex(YYSTYPE *yylex, void *pm);
	friend int dpparse (void *YYPARSE_PARAM);

public:
	tRisseDateParser(const risse_char *in);
	~tRisseDateParser();

	risse_int64 GetTime();

private:
	int Lex(YYSTYPE *yylex);

	void SkipToRightParenthesis();

	void SetMDay(int v);
	void SetMonth(int v);
	void SetYear(int v);
	void SetHours(int v);
	void SetMin(int v);
	void SetSec(int v);
	void SetAMPM(bool is_pm);
	void SetTimeZone(int v);
	void SetTimeZoneOffset(int v);


};

//---------------------------------------------------------------------------
} // namespace Risse
#endif
