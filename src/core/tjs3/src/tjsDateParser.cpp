//---------------------------------------------------------------------------
/*
	TJS3 Script Engine
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief 日付文字列パーサー
//---------------------------------------------------------------------------
#include "tjsCommHead.h"

#include <time.h>

#include "tjsDateParser.h"
#include "tjsdate.tab.h"

#include "tjsError.h"


namespace TJS
{
TJS_DEFINE_SOURCE_ID(1009);
//---------------------------------------------------------------------------
// interface to bison generated parser
//---------------------------------------------------------------------------
int dplex(YYSTYPE *yylex, void *pm)
{
	tTJSDateParser *parser = (tTJSDateParser*)pm;
	int tok =  parser->Lex(yylex);
	return tok;
}
int dpparse (void *YYPARSE_PARAM);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// Character component classifications
//---------------------------------------------------------------------------
static bool inline TJS_iswspace(tjs_char ch)
{
	if(ch&0xff00) return false; else return isspace(ch);
}
//---------------------------------------------------------------------------
static bool inline TJS_iswdigit(tjs_char ch)
{
	if(ch&0xff00) return false; else return isdigit(ch);
}
//---------------------------------------------------------------------------
static bool inline TJS_iswalpha(tjs_char ch)
{
	if(ch&0xff00) return true; else return isalpha(ch);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tTJSDateParser : A date/time parser class
//---------------------------------------------------------------------------
tTJSDateParser::tTJSDateParser(const tjs_char *in)
{
	Input = InputPointer = in;

	YearSet = MonthSet = MDaySet = HourSet = MinSet = SecSet = TimeZoneSet =
	TimeZoneOffsetSet = AMPMSet = false;

	if(dpparse(this)) TJS_eTJSError(ttstr(TJS_WS_TR("can not parse date string '%1'"), ttstr(in)));

	// currently no omissions is allowed except for Secs
	if(!YearSet || !MonthSet || !MDaySet || !HourSet || !MinSet)
		TJS_eTJSError(ttstr(TJS_WS_TR("can not parse date string '%1'"), ttstr(in)));
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
		TimeZone = -TJS_timezone;
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
	if(tmv == -1) TJS_eTJSError(TJSInvalidValueForTimestamp);

	// adjust time zone
	tmv -= TJS_timezone;
	tjs_int tz = 0;
	if(TimeZoneSet) tz += TimeZone;
	if(TimeZoneOffsetSet) tz += TimeZoneOffset;
	tmv -= tz;

	// store result
	Time = (tjs_int64)tmv * 1000;
}
//---------------------------------------------------------------------------
tTJSDateParser::~tTJSDateParser()
{
}
//---------------------------------------------------------------------------
tjs_int64 tTJSDateParser::GetTime()
{
	return Time;
}
//---------------------------------------------------------------------------
int tTJSDateParser::Lex(YYSTYPE *yylex)
{
	if(*InputPointer == 0) return -1;

	while( *InputPointer && TJS_iswspace(*InputPointer)) InputPointer++;

	if(*InputPointer == 0) return -1;

	if(TJS_iswdigit(*InputPointer))
	{
		tjs_int32 val = *InputPointer - TJS_WC('0');
		InputPointer ++;
		while(TJS_iswdigit(*InputPointer))
		{
			val *= 10;
			val += *InputPointer - TJS_WC('0');
			InputPointer++;
		}
		yylex->val = val;
		return DP_NUMBER;
	}

	#include "tjsDateWordMap.cc"

	int n =  (int)*(InputPointer++);
	if(n >= TJS_WC('A') && n <= TJS_WC('Z')) n += TJS_WC('a') - TJS_WC('A');
	return n;
}
//---------------------------------------------------------------------------
void tTJSDateParser::SkipToRightParenthesis()
{
	// skip InputPointer to ')'
	while(*InputPointer && *InputPointer != ')') InputPointer ++;
}
//---------------------------------------------------------------------------
void tTJSDateParser::SetMDay(int v)
{
	MDaySet = true;
	MDay = v;
}
//---------------------------------------------------------------------------
void tTJSDateParser::SetMonth(int v)
{
	MonthSet = true;
	Month = v;
}
//---------------------------------------------------------------------------
void tTJSDateParser::SetYear(int v)
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
void tTJSDateParser::SetHours(int v)
{
	HourSet = true;
	Hour = v;
}
//---------------------------------------------------------------------------
void tTJSDateParser::SetMin(int v)
{
	MinSet = true;
	Min = v;
}
//---------------------------------------------------------------------------
void tTJSDateParser::SetSec(int v)
{
	SecSet = true;
	Sec = v;
}
//---------------------------------------------------------------------------
void tTJSDateParser::SetAMPM(bool is_pm)
{
	AMPMSet = true;
	AMPM = is_pm;
}
//---------------------------------------------------------------------------
void tTJSDateParser::SetTimeZone(int v)
{
	TimeZoneSet = true;
	TimeZone = v;
}
//---------------------------------------------------------------------------
void tTJSDateParser::SetTimeZoneOffset(int v)
{
	TimeZoneOffsetSet = true;
	TimeZoneOffset = v;
}
//---------------------------------------------------------------------------




}
