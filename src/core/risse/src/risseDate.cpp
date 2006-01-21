//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief Date クラス実装
//---------------------------------------------------------------------------

#include "risseCommHead.h"

#include "risseError.h"
#include "risseDate.h"
#include "risseDateParser.h"

/*
note:
	To ensure portability, risse uses time_t as a date/time representation.
	if compiler's time_t holds only 32bits, it will cause the year 2038 problem.
	The author assumes that it is a compiler dependented problem, so any remedies
	are not given here.
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(12433,41422,21798,17538,50848,63522,48881,21554);
//---------------------------------------------------------------------------
static time_t RisseParseDateString(const risse_char *str)
{
	tRisseDateParser parser(str);
	return (time_t)(parser.GetTime() / 1000);
}
//---------------------------------------------------------------------------
// tRisseNI_Date : risse Native Instance : Date
//---------------------------------------------------------------------------
tRisseNI_Date::tRisseNI_Date()
{
	// C++ constructor
}
//---------------------------------------------------------------------------
// tRisseNC_Date : risse Native Class : Date
//---------------------------------------------------------------------------
risse_uint32 tRisseNC_Date::ClassID = (risse_uint32)-1;
tRisseNC_Date::tRisseNC_Date() :
	tRisseNativeClass(RISSE_WS("Date"))
{
	// class constructor

	RISSE_BEGIN_NATIVE_MEMBERS(/*Risse class name*/Date)
	RISSE_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var. name*/_this, /*var. type*/tRisseNI_Date,
	/*Risse class name*/ Date)
{
	if(numparams == 0)
	{
		time_t curtime;
		_this->DateTime = time(&curtime); // GMT current date/time
	}
	else if(numparams >= 1)
	{
		if(param[0]->Type() == tvtString)
		{
			// formatted string -> date/time
			_this->DateTime = RisseParseDateString(param[0]->GetString());
		}
		else
		{
			risse_int y, mon=0, day=1, h=0, m=0, s=0;
			y = (risse_int)param[0]->AsInteger();
			if(RISSE_PARAM_EXIST(1)) mon = (risse_int)param[1]->AsInteger();
			if(RISSE_PARAM_EXIST(2)) day = (risse_int)param[2]->AsInteger();
			if(RISSE_PARAM_EXIST(3)) h = (risse_int)param[3]->AsInteger();
			if(RISSE_PARAM_EXIST(4)) m = (risse_int)param[4]->AsInteger();
			if(RISSE_PARAM_EXIST(5)) s = (risse_int)param[5]->AsInteger();
			tm t;
			memset(&t, 0, sizeof(tm));
			t.tm_year = y - 1900;
			t.tm_mon = mon;
			t.tm_mday = day;
			t.tm_hour = h;
			t.tm_min = m;
			t.tm_sec = s;
			_this->DateTime = mktime(&t);
#ifdef _MSC_VER
			if(_this->DateTime == -1) Risse::Risse_eRisseError(RisseInvalidValueForTimestamp);
#else
			if(_this->DateTime == -1) Risse_eRisseError(RisseInvalidValueForTimestamp);
#endif
//			_this->DateTime -= Risse_timezone;
		}
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_CONSTRUCTOR_DECL(/*Risse class name*/Date)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/setYear)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Date);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	tm *te = localtime(&_this->DateTime);
	tm t;
	memcpy(&t, te, sizeof(tm));
	t.tm_year = (risse_int)param[0]->AsInteger() - 1900;
	_this->DateTime = mktime(&t);
#ifdef _MSC_VER
	if(_this->DateTime == -1) Risse::Risse_eRisseError(RisseInvalidValueForTimestamp);
#else
	if(_this->DateTime == -1) Risse_eRisseError(RisseInvalidValueForTimestamp);
#endif

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/setYear)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/setMonth)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Date);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	tm *te = localtime(&_this->DateTime);
	tm t;
	memcpy(&t, te, sizeof(tm));
	t.tm_mon = (risse_int)param[0]->AsInteger();
	_this->DateTime = mktime(&t);
#ifdef _MSC_VER
	if(_this->DateTime == -1) Risse::Risse_eRisseError(RisseInvalidValueForTimestamp);
#else
	if(_this->DateTime == -1) Risse_eRisseError(RisseInvalidValueForTimestamp);
#endif

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/setMonth)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/setDate)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Date);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	tm *te = localtime(&_this->DateTime);
	tm t;
	memcpy(&t, te, sizeof(tm));
	t.tm_mday = (risse_int)param[0]->AsInteger();
	_this->DateTime = mktime(&t);
#ifdef _MSC_VER
	if(_this->DateTime == -1) Risse::Risse_eRisseError(RisseInvalidValueForTimestamp);
#else
	if(_this->DateTime == -1) Risse_eRisseError(RisseInvalidValueForTimestamp);
#endif

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/setDate)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/setHours)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Date);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	tm *te = localtime(&_this->DateTime);
	tm t;
	memcpy(&t, te, sizeof(tm));
	t.tm_hour = (risse_int)param[0]->AsInteger();
	_this->DateTime = mktime(&t) ;
#ifdef _MSC_VER
	if(_this->DateTime == -1) Risse::Risse_eRisseError(RisseInvalidValueForTimestamp);
#else
	if(_this->DateTime == -1) Risse_eRisseError(RisseInvalidValueForTimestamp);
#endif

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/setHours)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/setMinutes)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Date);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	tm *te = localtime(&_this->DateTime);
	tm t;
	memcpy(&t, te, sizeof(tm));
	t.tm_min = (risse_int)param[0]->AsInteger();
	_this->DateTime = mktime(&t);
#ifdef _MSC_VER
	if(_this->DateTime == -1) Risse::Risse_eRisseError(RisseInvalidValueForTimestamp);
#else
	if(_this->DateTime == -1) Risse_eRisseError(RisseInvalidValueForTimestamp);
#endif

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/setMinutes)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/setSeconds)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Date);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	tm *te = localtime(&_this->DateTime);
	tm t;
	memcpy(&t, te, sizeof(tm));
	t.tm_sec = (risse_int)param[0]->AsInteger();
	_this->DateTime = mktime(&t);
#ifdef _MSC_VER
	if(_this->DateTime == -1)Risse::Risse_eRisseError(RisseInvalidValueForTimestamp);
#else
	if(_this->DateTime == -1)Risse_eRisseError(RisseInvalidValueForTimestamp);
#endif

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/setSeconds)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/setTime)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Date);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	_this->DateTime = (time_t)(param[0]->AsInteger()/1000L);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/setTime)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/getDate)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Date);

	tm *t = localtime(&_this->DateTime);

	if(result) *result = (tTVInteger)(t->tm_mday);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/getDate)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/getDay)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Date);

	tm *t = localtime(&_this->DateTime);

	if(result) *result = (tTVInteger)(t->tm_wday);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/getDay)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/getHours)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Date);

	tm *t = localtime(&_this->DateTime);

	if(result) *result = (tTVInteger)(t->tm_hour);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/getHours)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/getMinutes)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Date);

	tm *t = localtime(&_this->DateTime);

	if(result) *result = (tTVInteger)(t->tm_min);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/getMinutes)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/getMonth)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Date);

	tm *t = localtime(&_this->DateTime);

	if(result) *result = (tTVInteger)(t->tm_mon);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/getMonth)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/getSeconds)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Date);

	tm *t = localtime(&_this->DateTime);

	if(result) *result = (tTVInteger)(t->tm_sec);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/getSeconds)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/getTime)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Date);

	if(result) *result = (risse_int64)(_this->DateTime)*1000L;

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/getTime)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/getTimezoneOffset) // static
{
	if(result) *result = (tTVInteger)(Risse_timezone/60);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/getTimezoneOffset)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/getYear)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Date);

	tm *t = localtime(&_this->DateTime);

	if(result) *result = (tTVInteger)(t->tm_year+1900);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/getYear)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/parse)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Date);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	_this->DateTime = RisseParseDateString(param[0]->GetString());

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/parse)
//----------------------------------------------------------------------
	RISSE_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------
tRisseNativeInstance *tRisseNC_Date::CreateNativeInstance()
{
	return new tRisseNI_Date(); 
}
//---------------------------------------------------------------------------
} // namespace Risse

//---------------------------------------------------------------------------

