//---------------------------------------------------------------------------
/*
	TJS3 Script Engine
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief tjs 共通ヘッダ (コンパイル済みヘッダ用)
//---------------------------------------------------------------------------


/*
	Add headers that would not be frequently changed.
*/
#ifndef tjsCommHeadH
#define tjsCommHeadH

#ifdef TJS_SUPPORT_WX
	#include <wx/wx.h>
#endif

#ifdef __WIN32__
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


