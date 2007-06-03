//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse 設定 (プラットフォーム依存実装など)
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseCharUtils.h"

#ifdef RISSE_DEBUG_PROFILE_TIME
#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#ifdef __WIN32__
#include "float.h"
#endif

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(7674,14652,31932,17584,17587,60531,55828,58197);
//---------------------------------------------------------------------------
// debug support
//---------------------------------------------------------------------------
#ifdef RISSE_DEBUG_PROFILE_TIME
risse_uint RisseGetTickCount()
{
	return GetTickCount();
}
#endif
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// native debugger break point
//---------------------------------------------------------------------------
void RisseNativeDebuggerBreak()
{
	// This function is to be called mostly when the "debugger" risse statement is
	// executed.
	// Step you debbuger back to the the caller, and continue debugging.
	// Do not use "debugger" statement unless you run the program under the native
	// debugger, or the program may cause an unhandled debugger breakpoint
	// exception.

#if defined(__WIN32__) && defined(_M_IX86)
	#ifdef __BORLANDC__
			__emit__ (0xcc); // int 3 (Raise debugger breakpoint exception)
	#elif defined(__GNUG__)
			__asm__ __volatile__ (".byte 0xcc"); // int 3 (Raise debugger breakpoint exception)
	#else
			_asm _emit 0xcc; // int 3 (Raise debugger breakpoint exception)
	#endif
#endif
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// FPU control
//---------------------------------------------------------------------------
#if defined(__WIN32__) && !defined(__GNUC__)
static unsigned int RisseDefaultFPUCW = 0;
static unsigned int RisseNewFPUCW = 0;
static bool RisseFPUInit = false;
#endif
void RisseSetFPUE()
{
#if defined(__WIN32__) && !defined(__GNUC__)
	if(!RisseFPUInit)
	{
		RisseFPUInit = true;
		RisseDefaultFPUCW = _control87(0, 0);

		_default87 = RisseNewFPUCW = _control87(MCW_EM, MCW_EM);
#ifdef RISSE_SUPPORT_VCL
		Default8087CW = RisseNewFPUCW;
#endif
	}

//	_fpreset();
	_control87(RisseNewFPUCW, 0xffff);
#endif

}

void RisseRestoreFPUE()
{

#if defined(__WIN32__) && !defined(__GNUC__)
	if(!RisseFPUInit) return;
	_control87(RisseDefaultFPUCW, 0xffff);
#endif
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
} // namespace Risse



