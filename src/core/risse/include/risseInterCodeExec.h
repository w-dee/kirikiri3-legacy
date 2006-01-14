//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief VMの実装
//---------------------------------------------------------------------------

#ifndef risseInterCodeExecH
#define risseInterCodeExecH


namespace Risse
{
//---------------------------------------------------------------------------
extern void RisseVariantArrayStackAddRef();
extern void RisseVariantArrayStackRelease();
extern void RisseVariantArrayStackCompact();
extern void RisseVariantArrayStackCompactNow();
//---------------------------------------------------------------------------
}


#endif
