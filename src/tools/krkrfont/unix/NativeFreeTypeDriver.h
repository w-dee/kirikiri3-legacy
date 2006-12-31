//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
class tRisaBaseFreeTypeFontDriver
{
public:
	tRisaBaseFreeTypeFontDriver();
	~tRisaBaseFreeTypeFontDriver();

	static void EnumerateFonts(wxArrayString & dest,
		risse_uint32 flags,
		wxFontEncoding encoding = wxFONTENCODING_SYSTEM);
};
//---------------------------------------------------------------------------



#endif /*_NATIVEFREETYPEDRIVER_H_*/
