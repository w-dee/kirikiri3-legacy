//---------------------------------------------------------------------------
/*
	TJS3 Script Engine
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief 条件コンパイル式処理
//---------------------------------------------------------------------------

#ifndef tjsCompileControlH
#define tjsCompileControlH

#include "tjsString.h"

namespace TJS
{
//---------------------------------------------------------------------------
class tTJS;
class tTJSPPExprParser
{
public:
	tTJSPPExprParser(tTJS *tjs, const tjs_char *script);
	~tTJSPPExprParser();

	tjs_int32 Parse();

	tTJS * TJS;
	tjs_int GetNext (tjs_int32 &value);

	tTJS * GetTJS() { return TJS; }

	const tjs_char * GetString(tjs_int idx) const ;

	tjs_int32 Result;

private:
	std::vector<ttstr > IDs;

	const tjs_char *Script;
	const tjs_char *Current;

};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
}

#endif