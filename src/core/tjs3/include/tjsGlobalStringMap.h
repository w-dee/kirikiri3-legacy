//---------------------------------------------------------------------------
/*
	TJS3 Script Engine
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief 広域文字列マップの実装
//---------------------------------------------------------------------------
#ifndef tjsGlobalStringMapH
#define tjsGlobalStringMapH

#include "tjsString.h"

namespace TJS
{
//---------------------------------------------------------------------------
// tTJSGlobalStringMap - hash map to keep constant strings shared
//---------------------------------------------------------------------------
extern void TJSAddRefGlobalStringMap();
extern void TJSReleaseGlobalStringMap();
ttstr TJSMapGlobalStringMap(const ttstr & string);
//---------------------------------------------------------------------------
} // namespace TJS

#endif