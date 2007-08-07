/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Date" クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "risseDateClass.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "risseScriptEngine.h"
#include "risseDateParser.h"
#include "risseExceptionClass.h"

#include <wx/longlong.h>

/*
	Risseスクリプトから見える"Date" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(49609,37074,39802,17175,13698,32989,907,37210);
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
tString tDateInstance::Format(wxDateTime::Tm & tm, int tzofs)
{
	// tm を format する
	// "Mon, 06 Aug 2007 17:38:43 GMT+0900" を帰す。
	tString buf;
	risse_char *p = buf.Allocate(50); // max 50 chars

	switch(tm.GetWeekDay())
	{
	case wxDateTime::Sun:  p[0]='S'; p[1]='u'; p[2]='n'; break;
	case wxDateTime::Mon:  p[0]='M'; p[1]='o'; p[2]='n'; break;
	case wxDateTime::Tue:  p[0]='T'; p[1]='u'; p[2]='e'; break;
	case wxDateTime::Wed:  p[0]='W'; p[1]='e'; p[2]='d'; break;
	case wxDateTime::Thu:  p[0]='T'; p[1]='h'; p[2]='u'; break;
	case wxDateTime::Fri:  p[0]='F'; p[1]='r'; p[2]='i'; break;
	case wxDateTime::Sat:  p[0]='S'; p[1]='a'; p[2]='t'; break;
	case wxDateTime::Inv_WeekDay:  p[0]='-'; p[1]='-'; p[2]='-'; break; // あり得ないと思うが
	}

	p[3] = ',';
	p[4] = ' ';
	p += 5;

	p[0] = '0' + tm.mday / 10;
	p[1] = '0' + tm.mday % 10;
	p[2] = ' ';
	p += 3;

	switch(tm.mon)
	{
	case wxDateTime::Jan:  p[0]='J'; p[1]='a'; p[2]='n'; break;
	case wxDateTime::Feb:  p[0]='F'; p[1]='e'; p[2]='b'; break;
	case wxDateTime::Mar:  p[0]='M'; p[1]='a'; p[2]='r'; break;
	case wxDateTime::Apr:  p[0]='A'; p[1]='p'; p[2]='r'; break;
	case wxDateTime::May:  p[0]='M'; p[1]='a'; p[2]='y'; break;
	case wxDateTime::Jun:  p[0]='J'; p[1]='u'; p[2]='n'; break;
	case wxDateTime::Jul:  p[0]='J'; p[1]='u'; p[2]='l'; break;
	case wxDateTime::Aug:  p[0]='A'; p[1]='u'; p[2]='g'; break;
	case wxDateTime::Sep:  p[0]='S'; p[1]='e'; p[2]='p'; break;
	case wxDateTime::Oct:  p[0]='O'; p[1]='c'; p[2]='t'; break;
	case wxDateTime::Nov:  p[0]='N'; p[1]='o'; p[2]='v'; break;
	case wxDateTime::Dec:  p[0]='D'; p[1]='e'; p[2]='c'; break;
	case wxDateTime::Inv_Month:  p[0]='-'; p[1]='-'; p[2]='-'; break; // あり得ないと思うが
	}
	p[3] = ' ';
	p+= 4;

	int_to_str(tm.year, p);
	p += ::Risse::strlen(p);
	p[0] = ' ';
	p++;

	p[0] = '0' + tm.hour / 10;
	p[1] = '0' + tm.hour % 10;
	p[2] = ':';
	p += 3;
	p[0] = '0' + tm.min / 10;
	p[1] = '0' + tm.min % 10;
	p[2] = ':';
	p += 3;
	p[0] = '0' + tm.sec / 10;
	p[1] = '0' + tm.sec % 10;
	p[2] = ' ';
	p += 3;

	p[0] = 'G'; p[1] = 'M'; p[2] = 'T'; // UTC のほうがいいですかね
	p[3] = '\0';
	p+=3;

	if(tzofs != 0)
	{
		int tz = tzofs;
		if(tz < 0)
			p[0] = '-', tz = -tz;
		else
			p[0] = '+';
		p[1] = '0' + (tz / 60) / 10;
		p[2] = '0' + (tz / 60) % 10;
		p[3] = '0' + (tz % 60) / 10;
		p[4] = '0' + (tz % 60) % 10;
		p[5] = '\0';
	}

	buf.FixLength();
	return buf;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tDateInstance::Parse(const tString & str)
{
	tDateParser parser(str);
	if(parser.HasError)
	{
		// parse に失敗
		tIllegalArgumentExceptionClass::ThrowInvalidDateString(GetRTTI()->GetScriptEngine());
	}

	// 現在時刻
	struct cached_now
	{
		wxDateTime::Tm tm;
		bool got;
		cached_now() { got = false; }
		const wxDateTime::Tm & get() {
			if(!got) { got = true; tm = wxDateTime::Now().GetTm();}
			 return tm;
		}
	} now;

	// パラメータの取得と調整
	// 上位の単位が指定されていない状態が連続している場合は現在時刻を代入するが、
	// そうでなければ 0 (あるいは時間原点の値) を代入する

	bool omit = true; // 値の省略が連続しているか？

	int year;
	if(parser.YearSet)
		year = parser.Year, omit = false;
	else
		year = now.get().year;

	wxDateTime::Month month;
	if(parser.MonthSet)
		month = (wxDateTime::Month)parser.Month, omit = false;
	else
		month = omit ? now.get().mon : wxDateTime::Jan;

	wxDateTime::wxDateTime_t date;
	if(parser.DateSet)
		date = (wxDateTime::wxDateTime_t)parser.Date, omit = false;
	else
		date = omit ? now.get().mday : 1;

	// - いまのところ曜日の指定は無視。

	wxDateTime::wxDateTime_t hours;
	if(parser.HoursSet)
		hours = (wxDateTime::wxDateTime_t)parser.Hours, omit = false;
	else
		hours = omit ? now.get().hour : 0;

	wxDateTime::wxDateTime_t minutes;
	if(parser.MinutesSet)
		minutes = (wxDateTime::wxDateTime_t)parser.Minutes, omit = false;
	else
		minutes = omit ? now.get().min : 0;

	wxDateTime::wxDateTime_t seconds;
	if(parser.SecondsSet)
		seconds = (wxDateTime::wxDateTime_t)parser.Seconds, omit = false;
	else
		seconds = omit ? now.get().sec : 0;

	wxDateTime::wxDateTime_t milliseconds;
	if(parser.MillisecondsSet)
		milliseconds = (wxDateTime::wxDateTime_t)parser.Milliseconds, omit = false;
	else
		milliseconds = omit ? now.get().msec : 0;

	// タイムゾーンと AM/PM
	bool adjust_timezone = false;
	int timezone;
	if(parser.TimezoneSet || parser.TimezoneOffsetSet)
	{
		timezone = (parser.TimezoneSet ? parser.Timezone : 0);
		if(timezone < 0)
			timezone = - (-timezone / 100 * 3600 + (-timezone) % 100 * 60);
		else
			timezone =   ( timezone / 100 * 3600 + ( timezone) % 100 * 60);
		int tzofs = (parser.TimezoneOffsetSet ? parser.TimezoneOffset : 0);
		if(tzofs < 0)
			tzofs = - (-tzofs / 100 * 3600 + (-tzofs) % 100 * 60);
		else
			tzofs =   ( tzofs / 100 * 3600 + ( tzofs) % 100 * 60);
		timezone += tzofs;
		adjust_timezone = true;
	}

	if(parser.AMPMSet)
	{
		if(!parser.AMPM)
		{
			// AM の場合
			hours = hours % 12;
		}
		else
		{
			// PM の場合
			hours = hours % 12 + 12;
		}
	}

	// 一応範囲チェック。これをやっておかないと wxWidgets が assert エラーを出すため。
	// しかしライブラリ側が例外機構によるエラー通知機能持ってないとつらいね
	if(hours >= 24 || seconds >= 62 || minutes >= 60 || milliseconds >= 1000)
		tIllegalArgumentExceptionClass::ThrowInvalidDateString(GetRTTI()->GetScriptEngine());

	if(date <= 0 || date > wxDateTime::GetNumberOfDays(month, year))
		tIllegalArgumentExceptionClass::ThrowInvalidDateString(GetRTTI()->GetScriptEngine());

	// DateTime に値を設定する
	DateTime.Set(date, (wxDateTime::Month)month, year, hours, minutes, seconds, milliseconds);
	if(adjust_timezone)
		DateTime = DateTime.ToTimezone(wxDateTime::TimeZone((wxDateTime::wxDateTime_t)(timezone)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tDateInstance::construct()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tDateInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();

	if(info.args.HasArgument(0))
	{
		// 引数が一つ
		if(info.args[0].GetType() == tVariant::vtInteger)
			DateTime = wxDateTime((wxLongLong)(risse_int64)info.args[0]);
		else if(info.args[0].GetType() == tVariant::vtObject)
		{
			tDateInstance * instance =
				info.args[0].AssertAndGetObjectInterafce<tDateInstance>(info.engine->DateClass);
			DateTime = instance->DateTime;
		}
		else
		{
			// parse する
			Parse(info.args[0]);
		}
	}
	else
	{
		// 現在時刻を代入する
		DateTime.SetToCurrent();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tDateInstance::getYear() const
{
	volatile tSynchronizer sync(this); // sync

	return DateTime.GetYear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tDateInstance::setYear(int y)
{
	volatile tSynchronizer sync(this); // sync

	DateTime.SetYear(y);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tDateInstance::getMonth() const
{
	volatile tSynchronizer sync(this); // sync

	return (int)DateTime.GetMonth();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tDateInstance::setMonth(int m)
{
	volatile tSynchronizer sync(this); // sync

	DateTime.SetMonth((wxDateTime::Month)m);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tDateInstance::getDate() const
{
	volatile tSynchronizer sync(this); // sync

	return DateTime.GetDay();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tDateInstance::setDate(int d)
{
	volatile tSynchronizer sync(this); // sync

	DateTime.SetDay(d);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tDateInstance::getDay() const
{
	volatile tSynchronizer sync(this); // sync

	return (int)DateTime.GetWeekDay();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tDateInstance::getHours() const
{
	volatile tSynchronizer sync(this); // sync

	return DateTime.GetHour();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tDateInstance::setHours(int h)
{
	volatile tSynchronizer sync(this); // sync

	DateTime.SetHour(h);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tDateInstance::getMinutes() const
{
	volatile tSynchronizer sync(this); // sync

	return DateTime.GetMinute();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tDateInstance::setMinutes(int m)
{
	volatile tSynchronizer sync(this); // sync

	DateTime.SetMinute(m);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tDateInstance::getSeconds() const
{
	volatile tSynchronizer sync(this); // sync

	return DateTime.GetSecond();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tDateInstance::setSeconds(int s)
{
	volatile tSynchronizer sync(this); // sync

	DateTime.SetSecond(s);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tDateInstance::getMilliseconds() const
{
	volatile tSynchronizer sync(this); // sync

	return DateTime.GetMillisecond();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tDateInstance::setMilliseconds(int ms)
{
	volatile tSynchronizer sync(this); // sync

	DateTime.SetMillisecond(ms);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_int64 tDateInstance::getTime() const
{
	volatile tSynchronizer sync(this); // sync

	return DateTime.GetValue().GetValue(); // undocumented in wxWidgets; use the source, Luke
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tDateInstance::setTime(risse_int64 s)
{
	volatile tSynchronizer sync(this); // sync

	wxLongLong ref_time(s);

	DateTime = wxDateTime(ref_time);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tDateInstance::getTimezoneOffset()
{
	wxDateTime::TimeZone tz(wxDateTime::Local);
	return tz.GetOffset() / 60;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tDateInstance::toGMTString() const
{
	volatile tSynchronizer sync(this); // sync

	wxDateTime::Tm tm(DateTime.GetTm(wxDateTime::UTC));

	return Format(tm, 0);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tDateInstance::toString() const
{
	volatile tSynchronizer sync(this); // sync

	wxDateTime::Tm tm(DateTime.GetTm());

	return Format(tm, wxDateTime::TimeZone(wxDateTime::Local).GetOffset() / 60);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tDateInstance::toLocaleString() const
{
	volatile tSynchronizer sync(this); // sync

	return tString(DateTime.Format().c_str());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tDateInstance::equal(const tVariant & rhs)
{
	tDateInstance *instance =
		rhs.CheckAndGetObjectInterafce<tDateInstance>(GetRTTI()->GetScriptEngine()->DateClass);

	volatile tSynchronizer sync1(this); // sync
	volatile tSynchronizer sync2(instance); // sync

	if(instance) return DateTime == instance->DateTime;
	return false;
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tDateClass::tDateClass(tScriptEngine * engine) :
	tClassBase(ss_Date, engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tDateClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tDateClass::ovulate);
	BindFunction(this, ss_construct, &tDateInstance::construct);
	BindFunction(this, ss_initialize, &tDateInstance::initialize);

	BindFunction(this, ss_getYear, &tDateInstance::getYear);
	BindFunction(this, ss_setYear, &tDateInstance::setYear);
	BindProperty(this, ss_year, &tDateInstance::getYear, &tDateInstance::setYear);
	BindFunction(this, ss_getMonth, &tDateInstance::getMonth);
	BindFunction(this, ss_setMonth, &tDateInstance::setMonth);
	BindProperty(this, ss_month, &tDateInstance::getMonth, &tDateInstance::setMonth);
	BindFunction(this, ss_getDate, &tDateInstance::getDate);
	BindFunction(this, ss_setDate, &tDateInstance::setDate);
	BindProperty(this, ss_date, &tDateInstance::getDate, &tDateInstance::setDate);
	BindFunction(this, ss_getDay, &tDateInstance::getDay);
	BindProperty(this, ss_day, &tDateInstance::getDay);
	BindFunction(this, ss_getHours, &tDateInstance::getHours);
	BindFunction(this, ss_setHours, &tDateInstance::setHours);
	BindProperty(this, ss_hours, &tDateInstance::getHours, &tDateInstance::setHours);
	BindFunction(this, ss_getMinutes, &tDateInstance::getMinutes);
	BindFunction(this, ss_setMinutes, &tDateInstance::setMinutes);
	BindProperty(this, ss_minutes, &tDateInstance::getMinutes, &tDateInstance::setMinutes);
	BindFunction(this, ss_getSeconds, &tDateInstance::getSeconds);
	BindFunction(this, ss_setSeconds, &tDateInstance::setSeconds);
	BindProperty(this, ss_seconds, &tDateInstance::getSeconds, &tDateInstance::setSeconds);
	BindFunction(this, ss_getMilliseconds, &tDateInstance::getMilliseconds);
	BindFunction(this, ss_setMilliseconds, &tDateInstance::setMilliseconds);
	BindProperty(this, ss_milliseconds, &tDateInstance::getMilliseconds, &tDateInstance::setMilliseconds);
	BindFunction(this, ss_getTime, &tDateInstance::getTime);
	BindFunction(this, ss_setTime, &tDateInstance::setTime);
	BindProperty(this, ss_time, &tDateInstance::getTime, &tDateInstance::setTime);
	BindFunction(this, ss_getTimezoneOffset, &tDateInstance::getTimezoneOffset);
	BindProperty(this, ss_timezoneOffset, &tDateInstance::getTimezoneOffset);

	BindFunction(this, ss_toGMTString, &tDateInstance::toGMTString);
	BindFunction(this, mnString, &tDateInstance::toString);
	BindFunction(this, ss_toLocaleString, &tDateInstance::toLocaleString);

	BindFunction(this, mnEqual, &tDateInstance::equal);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tDateClass::ovulate()
{
	return tVariant(new tDateInstance());
}
//---------------------------------------------------------------------------

} /* namespace Risse */

