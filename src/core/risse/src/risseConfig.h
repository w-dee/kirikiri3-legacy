//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse 設定 (プラットフォーム依存実装など)
//---------------------------------------------------------------------------

#ifndef risseConfigH
#define risseConfigH

//---------------------------------------------------------------------------


/*
	many settings can be changed here.

	risseCommHead.h includes most common headers that will be needed to
	compile the entire Risse program.

	configuration about Critical Section for multithreading support is there in
	risseUtils.cpp/h.
*/

// TODO: autoconf integration

#include "risseTypes.h"
#include "stdarg.h"

namespace Risse
{

#if defined(DEBUG) || defined(_DEBUG) || defined(__WXDEBUG__)
	#define RISSE_DEBUG
#endif


// #define RISSE_NO_AVOID_ISWDIGIT
// #define RISSE_SUPPORT_VCL
// #define RISSE_MSG_EMPTY
// #define RISSE_DEBUG_TRACE
// #define RISSE_JP_LOCALIZED
#if _WIN32
 #define RISSE_TEXT_OUT_CRLF
#endif
// #define RISSE_WITH_IS_NOT_RESERVED_WORD


#define RISSE_malloc			malloc
#define RISSE_free			free
#define RISSE_realloc			realloc
#define RISSE_nsprintf		sprintf
#define RISSE_nstrcpy			strcpy
#define RISSE_nstrcat			strcat
#define RISSE_nstrlen			strlen
#define RISSE_octetcpy		memcpy
#define RISSE_octetcmp		memcmp


#if defined(__GNUC__)
	#define RISSE_cdecl
	#define RISSE_timezone timezone
	#define RISSE_rand rand
	#define RISSE_RAND_MAX RAND_MAX
#elif __WIN32__
	#define RISSE_cdecl __cdecl
	#define RISSE_timezone _timezone
	#define RISSE_rand _lrand
	#define RISSE_RAND_MAX LRAND_MAX
#elif _MSC_VER
	#define RISSE_cdecl __cdecl
	#define RISSE_vsnprintf		_vsnwprintf
	#define RISSE_sprintf			swprintf
	#define RISSE_timezone _timezone
	#define RISSE_rand rand
	#define RISSE_RAND_MAX RAND_MAX
#endif

void RISSE_debug_out(const risse_char *format, ...);

extern void NativeDebuggerBreak();

extern void SetFPUE();
extern void RestoreFPUE();




}


#endif

