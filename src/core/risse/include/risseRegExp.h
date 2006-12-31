//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief RegExp クラス実装
//---------------------------------------------------------------------------
#ifndef risseRegExpH
#define risseRegExpH

#include <boost/regex.hpp>
#include "risseNative.h"

#ifdef BOOST_NO_WREGEX
#error "wregex is not usable! see details at risseRegExp.h"

gcc g++ has a lack of wstring.

you must modify:

include/g++-3/string: uncomment this:
typedef basic_string <wchar_t> wstring;


include/g++-3/std/bastring.h: 
    { if (length () == 0) return ""; terminate (); return data (); }

is not usable in wstring; fix like

    { static charT zero=(charT)0; if (length () == 0) return &zero; terminate (); return data (); }


boost/config/stdlib/libstdcpp3.hpp: insert this to the beginning of the file:
#define _GLIBCPP_USE_WCHAR_T

or uncomment in sgi.hpp

#     define BOOST_NO_STD_WSTRING

#endif

namespace Risse
{
typedef boost::reg_expression<risse_char> tRisseRegEx;
//---------------------------------------------------------------------------
// tRisseNI_RegExp
//---------------------------------------------------------------------------
class tRisseNI_RegExp : public tRisseNativeInstance
{
public:
	tRisseNI_RegExp();
	tRisseRegEx RegEx;
	risse_uint32 Flags;
	risse_uint Start;
	tRisseVariant Array;
	risse_uint Index;
	ttstr Input;
	risse_uint LastIndex;
	ttstr LastMatch;
	ttstr LastParen;
	ttstr LeftContext;
	ttstr RightContext;

private:

public:
	void Split(iRisseDispatch2 ** array, const ttstr &target, bool purgeempty);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// tRisseNC_RegExp
//---------------------------------------------------------------------------
class tRisseNC_RegExp : public tRisseNativeClass
{
public:
	tRisseNC_RegExp();

	static void Compile(risse_int numparam, tRisseVariant **param, tRisseNI_RegExp *_this);
	static bool Match(boost::match_results<const risse_char *>& what,
		ttstr target, tRisseNI_RegExp *_this);
	static iRisseDispatch2 * GetResultArray(bool matched, tRisseNI_RegExp *_this,
		const boost::match_results<const risse_char *>& what);

private:
	tRisseNativeInstance *CreateNativeInstance();

public:
	static risse_uint32 ClassID;

};
//---------------------------------------------------------------------------
extern iRisseDispatch2 * RisseCreateRegExpClass();
//---------------------------------------------------------------------------

} // namespace Risse

#endif
