//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief コンパイル済みヘッダ
//---------------------------------------------------------------------------
#ifndef __RISSE_PREC_H__
#define __RISSE_PREC_H__

#include <wx/defs.h>

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

#include <gc.h>
#include <gc_cpp.h>
#include <gc_allocator.h>

#endif
//---------------------------------------------------------------------------

