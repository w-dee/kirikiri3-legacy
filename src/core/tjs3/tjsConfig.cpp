//---------------------------------------------------------------------------
/*
	TJS3 Script Engine
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// configuration
//---------------------------------------------------------------------------
#include "tjsCommHead.h"
#include "tjsCharUtils.h"

#ifdef TJS_DEBUG_PROFILE_TIME
#include <windows.h>
#endif

#ifdef __WIN32__
#include "float.h"
#endif

namespace TJS
{
TJS_DEFINE_SOURCE_ID(1007);
//---------------------------------------------------------------------------
// debug support
//---------------------------------------------------------------------------
#ifdef TJS_DEBUG_PROFILE_TIME
tjs_uint TJSGetTickCount()
{
	return GetTickCount();
}
#endif
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// native debugger break point
//---------------------------------------------------------------------------
void TJSNativeDebuggerBreak()
{
	// This function is to be called mostly when the "debugger" TJS statement is
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
static unsigned int TJSDefaultFPUCW = 0;
static unsigned int TJSNewFPUCW = 0;
static bool TJSFPUInit = false;
#endif
void TJSSetFPUE()
{
#if defined(__WIN32__) && !defined(__GNUC__)
	if(!TJSFPUInit)
	{
		TJSFPUInit = true;
		TJSDefaultFPUCW = _control87(0, 0);

		_default87 = TJSNewFPUCW = _control87(MCW_EM, MCW_EM);
#ifdef TJS_SUPPORT_VCL
		Default8087CW = TJSNewFPUCW;
#endif
	}

//	_fpreset();
	_control87(TJSNewFPUCW, 0xffff);
#endif

}

void TJSRestoreFPUE()
{

#if defined(__WIN32__) && !defined(__GNUC__)
	if(!TJSFPUInit) return;
	_control87(TJSDefaultFPUCW, 0xffff);
#endif
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
} // namespace TJS



