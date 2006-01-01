//---------------------------------------------------------------------------
/*
	TJS3 Script Engine
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief VMの実装
//---------------------------------------------------------------------------

#ifndef tjsInterCodeExecH
#define tjsInterCodeExecH


namespace TJS
{
//---------------------------------------------------------------------------
extern void TJSVariantArrayStackAddRef();
extern void TJSVariantArrayStackRelease();
extern void TJSVariantArrayStackCompact();
extern void TJSVariantArrayStackCompactNow();
//---------------------------------------------------------------------------
}


#endif