//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief RegExp クラス実装
//---------------------------------------------------------------------------
#include "risseCommHead.h"

#include "risseRegExp.h"
#include "risseArray.h"

#if !defined(_MSC_VER)
#include <function.h>
#endif

#if defined (_MSC_VER)
#include <boost/functional.hpp>
#endif

using namespace boost;


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(64835,7341,17430,20416,30122,9688,40678,52811);
//---------------------------------------------------------------------------
// Flags
//---------------------------------------------------------------------------
// some risse flags
const risse_uint32 globalsearch = (risse_uint32)(((risse_uint32)1)<<31);
const risse_uint32 risseflagsmask = (risse_uint32)0xff000000;

//---------------------------------------------------------------------------
static risse_uint32 RisseRegExpFlagToValue(risse_char ch, risse_uint32 prev)
{
	// converts flag letter to internal flag value.
	// this returns modified prev.
	// when ch is '\0', returns default flag value and prev is ignored.

	if(ch == 0)
	{
		return regbase::normal|regbase::use_except| // default behavior
			regbase::nocollate
				// for portability, local collation rules are disabled by the default
			;
	}

	switch(ch)
	{
	case RISSE_WC('g'): // global search
		prev|=globalsearch; return prev;
	case RISSE_WC('i'): // ignore case
		prev|=regbase::icase; return prev;
	case RISSE_WC('l'): // use localized collation
		prev &= ~regbase::nocollate; return prev;
	default:
		return prev;
	}
}
//---------------------------------------------------------------------------
static risse_uint32 RisseGetRegExpFlagsFromString(const risse_char *string)
{
	// returns a flag value represented by string

	risse_uint32 flag = RisseRegExpFlagToValue(0, 0);

	while(*string && *string != RISSE_WC('/'))
	{
		flag = RisseRegExpFlagToValue(*string, flag);
		string++;
	}

	return flag;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// predicating class for replace
//---------------------------------------------------------------------------
class tRisseReplacePredicator
{
	ttstr target;
	ttstr to;
	tRisseVariantClosure funcval;
	bool func;
	ttstr res;
	tRisseNI_RegExp * _this;
	risse_int lastpos;

public:
	tRisseReplacePredicator(tRisseVariant **param, risse_int numparams, tRisseNI_RegExp *__this,
		iRisseDispatch2 *objthis)
	{
		_this = __this;
		lastpos = 0;

		target = *param[0];
		if(param[1]->Type() != tvtObject)
		{
			to = (*param[1]);
			func = false;
		}
		else
		{
			funcval = param[1]->AsObjectClosureNoAddRef();
			if(funcval.ObjThis == NULL)
			{
				// replace with objthis when funcval's objthis is null
				funcval.ObjThis = objthis;
			}
			func = true;
		}

		// grep thru target string
		risse_int targlen = target.GetLen();
		unsigned int match_count = regex_grep
			(
			std::bind1st(std::mem_fun(&tRisseReplacePredicator::Callback), this),
			target.c_str(),
			_this->RegEx,
			match_default|match_not_dot_null);

		if(lastpos < targlen)
			res += ttstr(target.c_str() + lastpos, targlen - lastpos);
	}

	const ttstr & GetRes() const { return res; }

	bool Risse_cdecl Callback(const match_results<const risse_char *> what)
	{
		// callback on each match

		risse_int pos = what.position();
		risse_int len = what.length();

		if(pos > lastpos)
			res += ttstr(target.c_str() + lastpos, pos - lastpos);

		if(!func)
		{
			res += to;
		}
		else
		{
			// call the callback function descripted as param[1]
			tRisseVariant result;
			risse_error hr;
			iRisseDispatch2 *array =
				tRisseNC_RegExp::GetResultArray(true, _this, what);
			tRisseVariant arrayval(array, array);
			tRisseVariant *param = &arrayval;
			array->Release();
			hr = funcval.FuncCall(0, NULL, NULL, &result, 1, &param, NULL);
			if(RISSE_FAILED(hr)) return hr;
			result.ToString();
			res += result.GetString();
		}

		lastpos = pos + len;

		return _this->Flags & globalsearch;
	}
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// predicating class for split
//---------------------------------------------------------------------------
class tRisseSplitPredicator
{
	ttstr target;

	iRisseDispatch2 * array;

	risse_int lastpos;
	risse_int lastlen;

	risse_int storecount;

	bool purgeempty;

public:
	tRisseSplitPredicator(const ttstr &target, iRisseDispatch2 * array,
		const tRisseRegEx &regex, bool purgeempty)
	{
		this->array = array;
		this->purgeempty = purgeempty;
		lastpos = 0;
		storecount = 0;

		this->target = target;

		// grep thru target
		risse_int targlen = target.GetLen();
		unsigned int match_count = regex_grep
			(
			std::bind1st(std::mem_fun(&tRisseSplitPredicator::Callback), this),
			target.c_str(),
			regex,
			match_default|match_not_dot_null);


		// output last
		if(lastlen !=0 || lastpos != targlen)
		{
			// unless null match at last of target
			if(!purgeempty || targlen - lastpos)
			{
				tRisseVariant val(ttstr(target.c_str() + lastpos, targlen - lastpos));
				array->PropSetByNum(RISSE_MEMBERENSURE, storecount++, &val, array);
			}
		}
	}

	bool Risse_cdecl Callback(const match_results<const risse_char *> what)
	{
		risse_int pos = what.position();
		risse_int len = what.length();

		if(pos >= lastpos && (len || pos != 0))
		{
			if(!purgeempty || pos - lastpos)
			{
				tRisseVariant val(ttstr(target.c_str() + lastpos, pos - lastpos));
				array->PropSetByNum(RISSE_MEMBERENSURE, storecount++, &val, array);
			}
		}

		if(what.size() > 1)
		{
			// output sub-expression
			for(unsigned i = 1; i < what.size(); i++)
			{
				tRisseVariant val;
				if(!purgeempty || what[i].second - what[i].first)
				{
					val = ttstr(what[i].first, what[i].second - what[i].first);
					array->PropSetByNum(RISSE_MEMBERENSURE, storecount++, &val, array);
				}
			}
		}

		lastpos = pos + len;
		lastlen = len;

		return true;
	}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tRisseNI_RegExp : risse Native Instance : RegExp
//---------------------------------------------------------------------------
tRisseNI_RegExp::tRisseNI_RegExp()
{
	// C++constructor
	Flags = RisseRegExpFlagToValue(0, 0);
	Start = 0;
	Index =0;
	LastIndex = 0;
}
//---------------------------------------------------------------------------
void tRisseNI_RegExp::Split(iRisseDispatch2 ** array, const ttstr &target, bool purgeempty)
{
	bool arrayallocated = false;
	if(!*array) *array = RisseCreateArrayObject(), arrayallocated = true;

	try
	{
		tRisseSplitPredicator pred(target, *array, RegEx, purgeempty);
	}
	catch(...)
	{
		if(arrayallocated) (*array)->Release();
		throw;
	}
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tRisseNC_RegExp : risse Native Class : RegExp
//---------------------------------------------------------------------------
risse_uint32 tRisseNC_RegExp::ClassID = (risse_uint32)-1;
tRisseNC_RegExp::tRisseNC_RegExp() :
	tRisseNativeClass(RISSE_WS("RegExp"))
{
	// class constructor

	RISSE_BEGIN_NATIVE_MEMBERS(/*Risse class name*/RegExp)
	RISSE_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var. name*/_this, /*var. type*/tRisseNI_RegExp,
	/*Risse class name*/ RegExp)
{
	/*
		Risse constructor
	*/

	if(numparams >= 1)
	{
		tRisseNC_RegExp::Compile(numparams, param, _this);
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_CONSTRUCTOR_DECL(/*Risse class name*/RegExp)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/compile)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RegExp);

	/*
		compiles given regular expression and flags.
	*/

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	tRisseNC_RegExp::Compile(numparams, param, _this);


	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/compile)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/_compile)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RegExp);

	/*
		internal function; compiles given constant regular expression.
		input expression is following format:
		//flags/expression
		where flags is flag letters ( [gil] )
		and expression is a Regular Expression
	*/

	if(numparams != 1) return RISSE_E_BADPARAMCOUNT;

	ttstr expr = *param[0];

	const risse_char *p = expr.c_str();
	if(!p || !p[0]) return RISSE_E_FAIL;

	if(p[0] != RISSE_WC('/') || p[1] != RISSE_WC('/')) return RISSE_E_FAIL;

	p+=2;
	const risse_char *exprstart = Risse_strchr(p, RISSE_WC('/'));
	if(!exprstart) return RISSE_E_FAIL;
	exprstart ++;

	risse_uint32 flags = RisseGetRegExpFlagsFromString(p);

	try
	{
		_this->RegEx.assign(exprstart, (wregex::flag_type)(flags& ~risseflagsmask));
	}
	catch(std::exception &e)
	{
		Risse_eRisseError(e.what());
	}

	_this->Flags = flags;

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/_compile)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/test)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RegExp);

	/*
		do the text searching.
		return match found ( true ), or not found ( false ).
		this function may not change any internal status.
	*/

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;


	if(result)
	{
		ttstr target(*param[0]);
		match_results<const wchar_t *> what;
		*result = tRisseNC_RegExp::Match(what, target, _this);
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/test)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/match)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RegExp);

	/*
		do the text searching.
		this function is the same as test, except for its return value.
		match returns an array that contains each matching part.
		if match failed, returns empty array. eg.
		any internal status will not be changed.
	*/

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;


	if(result)
	{
		ttstr target(*param[0]);
		match_results<const risse_char *> what;
		bool matched = tRisseNC_RegExp::Match(what, target, _this);
		iRisseDispatch2 *array = tRisseNC_RegExp::GetResultArray(matched, _this, what);
		*result = tRisseVariant(array, array);
		array->Release();
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/match)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/exec)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RegExp);

	/*
		same as the match except for the internal status' change.
		var ar;
		var pat = /:(\d+):(\d+):/g;
		while((ar = pat.match(target).count)
		{
			// ...
		}
	*/

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	ttstr target(*param[0]);
	match_results<const risse_char *> what;
	bool matched = tRisseNC_RegExp::Match(what, target, _this);
	iRisseDispatch2 *array = tRisseNC_RegExp::GetResultArray(matched, _this, what);

	_this->Array = tRisseVariant(array, array);
	array->Release();

	if(matched)
	{
		_this->Input = *param[0];
		if(_this->RegEx.empty())
		{
			_this->Index = _this->Start;
			_this->LastIndex = _this->Start;
			_this->LastParen = ttstr();
			_this->LeftContext = ttstr(*param[0], _this->Start);
		}
		else
		{
			_this->Index = _this->Start + what.position();
			_this->LastIndex = _this->Start + what.position() + what.length();
			_this->LastMatch = ttstr(what[0].first,
				what[0].second - what[0].first);
			risse_uint last = what.size() -1;
			_this->LastParen = ttstr(what[last-1].first,
				what[last-1].second - what[last-1].first);
			_this->LeftContext = ttstr(*param[0], _this->Start + what.position());
			_this->RightContext = ttstr(target.c_str() + _this->LastIndex);
			if(_this->Flags & globalsearch)
			{
				// global search flag changes the next search starting position.
				risse_uint match_end = _this->LastIndex;
				_this->Start = match_end;
			}
		}
	}

	if(result)
	{
		*result = _this->Array;
	}

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/exec)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/replace)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RegExp);

	/*
		replaces the string

		newstring = /regexp/.replace(orgstring, newsubstring);
		newsubstring can be:
			1. normal string ( literal or expression that respresents string )
			2. a function
		function is called as in RegExp's context, returns new substring.

		or

		newstring = string.replace(/regexp/, newsubstring);
			( via String.replace method )

		replace method ignores start property, and does not change any
			internal status.
	*/

	if(numparams < 2) return RISSE_E_BADPARAMCOUNT;

	tRisseReplacePredicator predicate(param, numparams, _this, objthis);


	if(result) *result = predicate.GetRes();
/*
	if(numparams >= 3)
	{
		if(param[3]) *param[3] = matchcount; // for internal usage
	}
*/
	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/replace)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/split)
{
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RegExp);

	/*
		replaces the string

		array = /regexp/.replace(targetstring, <reserved>, purgeempty);

		or

		array = targetstring.split(/regexp/, <reserved>, purgeempty);

		or

		array = [].split(/regexp/, targetstring, <reserved>, purgeempty);

		this method does not update properties
	*/

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	ttstr target(*param[0]);

	bool purgeempty = false;
	if(numparams >= 3) purgeempty = param[2]->operator bool();

	iRisseDispatch2 *array = NULL;

	_this->Split(&array, target, purgeempty);

	if(result) *result = tRisseVariant(array, array);

	array->Release();

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/split)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(matches)
{
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RegExp);
		*result = _this->Array;
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_DENY_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(matches)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(start)
{
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RegExp);
		*result = (tTVInteger)_this->Start;
		return RISSE_S_OK;
	}
    RISSE_END_NATIVE_PROP_GETTER

	RISSE_BEGIN_NATIVE_PROP_SETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/* var. name */_this, /* var. type */tRisseNI_RegExp);
		_this->Start = (risse_uint)(tTVInteger)*param;
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(start)
//---------------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(index)
{
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RegExp);
		*result = (tTVInteger)_this->Index;
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_DENY_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(index)
//---------------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(lastIndex)
{
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RegExp);
		*result = (tTVInteger)_this->LastIndex;
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_DENY_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(lastIndex)
//---------------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(input)
{
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RegExp);
		*result = _this->Input;
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_DENY_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(input)
//---------------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(lastMatch)
{
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RegExp);
		*result = _this->LastMatch;
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_DENY_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(lastMatch)
//---------------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(lastParen)
{
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RegExp);
		*result = _this->LastParen;
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_DENY_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(lastParen)
//---------------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(leftContext)
{
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RegExp);
		*result = _this->LeftContext;
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_DENY_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(leftContext)
//---------------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(rightContext)
{
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_RegExp);
		*result = _this->RightContext;
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_DENY_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(rightContext)
//---------------------------------------------------------------------------
	RISSE_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------
tRisseNativeInstance *tRisseNC_RegExp::CreateNativeInstance()
{
	return new tRisseNI_RegExp();
}
//---------------------------------------------------------------------------
void tRisseNC_RegExp::Compile(risse_int numparams, tRisseVariant **param, tRisseNI_RegExp *_this)
{
	ttstr expr = *param[0];

	risse_uint32 flags;
	if(numparams >= 2)
	{
		ttstr fs = *param[1];
		flags = RisseGetRegExpFlagsFromString(fs.c_str());
	}
	else
	{
		flags = RisseRegExpFlagToValue(0, 0);
	}

	if(expr.IsEmpty()) expr = RISSE_WS("(?:)"); // generate empty regular expression

	try
	{
		_this->RegEx.assign(expr.c_str(), (wregex::flag_type)(flags& ~risseflagsmask));
	}
	catch(std::exception &e)
	{
		Risse_eRisseError(e.what());
	}

	_this->Flags = flags;

}
//---------------------------------------------------------------------------
bool tRisseNC_RegExp::Match(match_results<const risse_char *>& what,
		ttstr target, tRisseNI_RegExp *_this)
{
	risse_uint32 flags = match_default|match_not_dot_null;

	risse_uint searchstart;

	if(_this->Flags & globalsearch)
	{
		risse_uint targlen = target.GetLen();
		if(_this->Start == targlen)
		{
			// Start already reached at end
			return _this->RegEx.empty();  // returns true if empty
		}
		else if(_this->Start > targlen)
		{
			// Start exceeds target's length
			return false;
		}
		searchstart = _this->Start;
	}
	else
	{
		searchstart = 0;
	}

	return regex_search(target.c_str()+searchstart,
		what, _this->RegEx, flags);

}
//---------------------------------------------------------------------------
iRisseDispatch2 * tRisseNC_RegExp::GetResultArray(bool matched, tRisseNI_RegExp *_this,
		const match_results<const risse_char *>& what)
{
	iRisseDispatch2 *array = RisseCreateArrayObject();
	if(matched)
	{
		if(_this->RegEx.empty())
		{
			tRisseVariant val(RISSE_WS(""));
			array->PropSetByNum(RISSE_MEMBERENSURE|RISSE_IGNOREPROP,
					0, &val, array);
		}
		else
		{
			risse_uint size = what.size();
			try
			{
				for(risse_uint i=0; i<size; i++)
				{
					tRisseVariant val;
					val = ttstr(what[i].first, what[i].second - what[i].first);
					array->PropSetByNum(RISSE_MEMBERENSURE|RISSE_IGNOREPROP,
						i, &val, array);
				}
			}
			catch(...)
			{
				array->Release();
				throw;
			}
		}
	}
	return array;
}
//---------------------------------------------------------------------------
iRisseDispatch2 * RisseCreateRegExpClass()
{
	return new tRisseNC_RegExp();
}
//---------------------------------------------------------------------------

} // namespace Risse

