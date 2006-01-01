//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
// @brief Win32 GDI 経由でのFreeType ドライバ
//---------------------------------------------------------------------------
#ifndef _NATIVEFREETYPEDRIVER_H_
#define _NATIVEFREETYPEDRIVER_H_

#include "tvpfontstruc.h"
#include "FreeTypeFace.h"


//---------------------------------------------------------------------------
//! @brief		FreeType フォントドライバ基底クラス
//---------------------------------------------------------------------------
class tTVPBaseFreeTypeFontDriver
{
public:
	tTVPBaseFreeTypeFontDriver();
	~tTVPBaseFreeTypeFontDriver();

	static void EnumerateFonts(wxArrayString & dest,
		tjs_uint32 flags,
		wxFontEncoding encoding = wxFONTENCODING_SYSTEM);
};
//---------------------------------------------------------------------------



#endif /*_NATIVEFREETYPEDRIVER_H_*/
