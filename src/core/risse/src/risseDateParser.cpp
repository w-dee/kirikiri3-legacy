//---------------------------------------------------------------------------
/*
	Risse [�肹]
	alias RISE [りせ], acronym of "Rise Is a Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief 日付文字列パーサー
//---------------------------------------------------------------------------
#include "risseCommHead.h"

#include <time.h>

#include "risseDateParser.h"
#include "rissedate.tab.h"

#include "risseError.h"


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(1009);
//---------------------------------------------------------------------------
// interface to bison generated parser
//---------------------------------------------------------------------------
int dplex(YYSTYPE *yylex, void *pm)
{
	tRisseDateParser *parser = (tRisseDateParser*)pm;
	int tok =  parser->Lex(yylex);
	return tok;
}
int dpparse (void *YYPARSE_PARAM);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// Character component classifications
//---------------------------------------------------------------------------
static bool inline Risse_iswspace(risse_char ch)
{
	if(ch&0xff00) return false; else return isspace(ch);
}
//---------------------------------------------------------------------------
static bool inline Risse_iswdigit(risse_char ch)
{
	if(ch&0xff00) return false; else return isdigit(ch);
}
//---------------------------------------------------------------------------
static bool inline Risse_iswalpha(risse_char ch)
{
	if(ch&0xff00) return true; else return isalpha(ch);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tRisseDateParser : A date/time parser class
//---------------------------------------------------------------------------
tRisseDateParser::tRisseDateParser(const risse_char *in)
{
	Input = InputPointer = in;

	YearSet = MonthSet = MDaySet = HourSet = MinSet = SecSet = TimeZoneSet =
	TimeZoneOffsetSet = AMPMSet = false;

	if(dpparse(this)) Risse_eRisseError(ttstr(RISSE_WS_TR("can not parse date string '%1'"), ttstr(in)));

	// currently no omissions is allowed except for Secs
	if(!YearSet || !MonthSet || !MDaySet || !HourSet || !MinSet)
		Risse_eRisseError(ttstr(RISSE_WS_TR("can not parse date string '%1'"), ttstr(in)));
	if(!SecSet) SecSet = true, Sec = 0;

	// convert Timezone/TimezoneOffset to time_t representation
	if(TimeZoneSet)
	{
		// input timezone is [+/-]hhmm
		bool sign = TimeZone < 0 ? true : false;
		if(sign) TimeZone = -TimeZone;
		TimeZone = (int)(TimeZone / 100) * 60*60 + (TimeZone % 100) * 60;
		if(sign) TimeZone = -TimeZone;
	}
	if(TimeZoneOffsetSet)
	{
		// input timezone is [+/-]hhmm
		bool sign = TimeZoneOffset < 0 ? true : false;
		if(sign) TimeZoneOffset = -TimeZoneOffset;
		TimeZoneOffset = (int)(TimeZoneOffset / 100) * 60*60 + (TimeZoneOffset % 100) * 60;
		if(sign) TimeZoneOffset = -TimeZoneOffset;
	}

	// Timezone is default system timezone when timezone is omitted.
	if(!TimeZoneSet && !TimeZoneOffsetSet)
	{
		TimeZoneSet = true;
		TimeZone = -Risse_timezone;
	}

	// Adjust AM/PM
	if(AMPMSet && AMPM) Hour += 12;

	// build tm structure
	struct tm stm;
	memset(&stm, 0, sizeof(stm));
	stm.tm_year = Year - 1900;
	stm.tm_mon = Month;
	stm.tm_mday = MDay;
	stm.tm_hour = Hour;
	stm.tm_min = Min;
	stm.tm_sec = Sec;

	time_t tmv = mktime(&stm);
	if(tmv == -1) Risse_eRisseError(RisseInvalidValueForTimestamp);

	// adjust time zone
	tmv -= Risse_timezone;
	risse_int tz = 0;
	if(TimeZoneSet) tz += TimeZone;
	if(TimeZoneOffsetSet) tz += TimeZoneOffset;
	tmv -= tz;

	// store result
	Time = (risse_int64)tmv * 1000;
}
//---------------------------------------------------------------------------
tRisseDateParser::~tRisseDateParser()
{
}
//---------------------------------------------------------------------------
risse_int64 tRisseDateParser::GetTime()
{
	return Time;
}
//---------------------------------------------------------------------------
int tRisseDateParser::Lex(YYSTYPE *yylex)
{
	if(*InputPointer == 0) return -1;

	while( *InputPointer && Risse_iswspace(*InputPointer)) InputPointer++;

	if(*InputPointer == 0) return -1;

	if(Risse_iswdigit(*InputPointer))
	{
		risse_int32 val = *InputPointer - RISSE_WC('0');
		InputPointer ++;
		while(Risse_iswdigit(*InputPointer))
		{
			val *= 10;
			val += *InputPointer - RISSE_WC('0');
			InputPointer++;
		}
		yylex->val = val;
		return DP_NUMBER;
	}

	#include "risseDateWordMap.cc"

	int n =  (int)*(InputPointer++);
	if(n >= RISSE_WC('A') && n <= RISSE_WC('Z')) n += RISSE_WC('a') - RISSE_WC('A');
	return n;
}
//---------------------------------------------------------------------------
void tRisseDateParser::SkipToRightParenthesis()
{
	// skip InputPointer to ')'
	while(*InputPointer && *InputPointer != ')') InputPointer ++;
}
//---------------------------------------------------------------------------
void tRisseDateParser::SetMDay(int v)
{
	MDaySet = true;
	MDay = v;
}
//---------------------------------------------------------------------------
void tRisseDateParser::SetMonth(int v)
{
	MonthSet = true;
	Month = v;
}
//---------------------------------------------------------------------------
void tRisseDateParser::SetYear(int v)
{
	if(v < 100)
	{
		if(v <= 50)
			v = 2000 + v;
		else
			v = 1900 + v;
	}
	YearSet = true;
	Year = v;
}
//---------------------------------------------------------------------------
void tRisseDateParser::SetHours(int v)
{
	HourSet = true;
	Hour = v;
}
//---------------------------------------------------------------------------
void tRisseDateParser::SetMin(int v)
{
	MinSet = true;
	Min = v;
}
//---------------------------------------------------------------------------
void tRisseDateParser::SetSec(int v)
{
	SecSet = true;
	Sec = v;
}
//---------------------------------------------------------------------------
void tRisseDateParser::SetAMPM(bool is_pm)
{
	AMPMSet = true;
	AMPM = is_pm;
}
//---------------------------------------------------------------------------
void tRisseDateParser::SetTimeZone(int v)
{
	TimeZoneSet = true;
	TimeZone = v;
}
//---------------------------------------------------------------------------
void tRisseDateParser::SetTimeZoneOffset(int v)
{
	TimeZoneOffsetSet = true;
	TimeZoneOffset = v;
}
//---------------------------------------------------------------------------




}
