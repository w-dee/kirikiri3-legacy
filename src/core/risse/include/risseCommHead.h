//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief risse 共通ヘッダ (コンパイル済みヘッダ用)
//---------------------------------------------------------------------------


/*
	Add headers that would not be frequently changed.
*/
#ifndef risseCommHeadH
#define risseCommHeadH

#ifdef RISSE_SUPPORT_WX
	#include <wx/defs.h>
#endif

#ifdef __WIN32__
#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif


#include <string.h>
#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#include <vector>
#include <string>
#include <stdexcept>

//---------------------------------------------------------------------------
#endif


