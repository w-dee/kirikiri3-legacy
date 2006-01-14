//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief 条件コンパイル式処理
//---------------------------------------------------------------------------

#ifndef risseCompileControlH
#define risseCompileControlH

#include "risseString.h"

namespace Risse
{
//---------------------------------------------------------------------------
class tRisse;
class tRissePPExprParser
{
public:
	tRissePPExprParser(tRisse *risse, const risse_char *script);
	~tRissePPExprParser();

	risse_int32 Parse();

	tRisse * Risse;
	risse_int GetNext (risse_int32 &value);

	tRisse * GetRisse() { return Risse; }

	const risse_char * GetString(risse_int idx) const ;

	risse_int32 Result;

private:
	std::vector<ttstr > IDs;

	const risse_char *Script;
	const risse_char *Current;

};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
}

#endif
