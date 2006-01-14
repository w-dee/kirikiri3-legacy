//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
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

// #define RISSE_NO_AVOID_ISWDIGIT
// #define RISSE_SUPPORT_VCL
// #define RISSE_MSG_EMPTY
// #define RISSE_DEBUG_TRACE
// #define RISSE_JP_LOCALIZED
// #define RISSE_TEXT_OUT_CRLF
// #define RISSE_WITH_IS_NOT_RESERVED_WORD


#define Risse_malloc			malloc
#define Risse_free			free
#define Risse_realloc			realloc
#define Risse_nsprintf		sprintf
#define Risse_nstrcpy			strcpy
#define Risse_nstrcat			strcat
#define Risse_nstrlen			strlen
#define Risse_octetcpy		memcpy
#define Risse_octetcmp		memcmp


#if defined(__GNUC__)
	#define Risse_cdecl
	#define Risse_timezone timezone
	#define Risse_rand rand
	#define RISSE_RAND_MAX RAND_MAX
#elif __WIN32__
	#define Risse_cdecl __cdecl
	#define Risse_timezone _timezone
	#define Risse_rand _lrand
	#define RISSE_RAND_MAX LRAND_MAX
#elif _MSC_VER
	#define Risse_cdecl __cdecl
	#define Risse_vsnprintf		_vsnwprintf
	#define Risse_sprintf			swprintf
	#define Risse_timezone _timezone
	#define Risse_rand rand
	#define RISSE_RAND_MAX RAND_MAX
#endif

void Risse_debug_out(const risse_char *format, ...);

extern void RisseNativeDebuggerBreak();

extern void RisseSetFPUE();
extern void RisseRestoreFPUE();






}


#endif

