//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse レキシカル・アナライザ(字句解析器)の実装
//---------------------------------------------------------------------------
#include "../prec.h"

#include "../risseLexerUtils.h"
#include "risseLexer.h"
#include "risseParser.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(26774,17704,8265,19906,55701,8958,30467,4610);

// マッパーを include
#include "risseLexerMap.inc"


//---------------------------------------------------------------------------
tRisseLexer::tRisseLexer(const tRisseString & script)
{
	// フィールドを初期化
	Script = script;
	Ptr = Script.c_str();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tRisseLexer::GetToken(tRisseVariant & val)
{
	if(!SkipSpace(Ptr)) return -1;
	return RisseMapToken(Ptr, val);
}
//---------------------------------------------------------------------------


} // namespace Risse



