//---------------------------------------------------------------------------
/*
	TJS3 Script Engine
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief TJS3 設定 (プラットフォーム依存実装など)
//---------------------------------------------------------------------------

#ifndef tjsConfigH
#define tjsConfigH

namespace TJS
{
//---------------------------------------------------------------------------


/*
	many settings can be changed here.

	tjsCommHead.h includes most common headers that will be needed to
	compile the entire TJS program.

	configuration about Critical Section for multithreading support is there in
	tjsUtils.cpp/h.
*/

// TODO: autoconf integration

#include "tjsTypes.h"
#include "stdarg.h"


// #define TJS_NO_AVOID_ISWDIGIT
// #define TJS_SUPPORT_VCL
// #define TJS_MSG_EMPTY
// #define TJS_DEBUG_TRACE
// #define TJS_JP_LOCALIZED
// #define TJS_TEXT_OUT_CRLF
// #define TJS_WITH_IS_NOT_RESERVED_WORD


#define TJS_malloc			malloc
#define TJS_free			free
#define TJS_realloc			realloc
#define TJS_nsprintf		sprintf
#define TJS_nstrcpy			strcpy
#define TJS_nstrcat			strcat
#define TJS_nstrlen			strlen
#define TJS_octetcpy		memcpy
#define TJS_octetcmp		memcmp


#if defined(__GNUC__)
	#define TJS_cdecl
	#define TJS_timezone timezone
	#define TJS_rand rand
	#define TJS_RAND_MAX RAND_MAX
#elif __WIN32__
	#define TJS_cdecl __cdecl
	#define TJS_timezone _timezone
	#define TJS_rand _lrand
	#define TJS_RAND_MAX LRAND_MAX
#endif

void TJS_debug_out(const tjs_char *format, ...);

extern void TJSNativeDebuggerBreak();

extern void TJSSetFPUE();
extern void TJSRestoreFPUE();






}


#endif

