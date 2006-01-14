//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief 広域文字列マップの実装
//---------------------------------------------------------------------------
#ifndef risseGlobalStringMapH
#define risseGlobalStringMapH

#include "risseString.h"

namespace Risse
{
//---------------------------------------------------------------------------
// tRisseGlobalStringMap - hash map to keep constant strings shared
//---------------------------------------------------------------------------
extern void RisseAddRefGlobalStringMap();
extern void RisseReleaseGlobalStringMap();
ttstr RisseMapGlobalStringMap(const ttstr & string);
//---------------------------------------------------------------------------
} // namespace Risse

#endif
