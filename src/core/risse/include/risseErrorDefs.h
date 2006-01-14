//---------------------------------------------------------------------------
/*
	Risse [�肹]
	alias RISE [りせ], acronym of "Rise Is a Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief 各種定数など
//---------------------------------------------------------------------------

#ifndef risseErrorDefsH
#define risseErrorDefsH

namespace Risse
{

// #define RISSE_STRICT_ERROR_CODE_CHECK
	// defining this enables strict error code checking,
	// for debugging.

/*[*/
//---------------------------------------------------------------------------
// return values as risse_error
//---------------------------------------------------------------------------
#define RISSE_E_MEMBERNOTFOUND		(-1001)
#define RISSE_E_NOTIMPL				(-1002)
#define RISSE_E_INVALIDPARAM			(-1003)
#define RISSE_E_BADPARAMCOUNT			(-1004)
#define RISSE_E_INVALIDTYPE			(-1005)
#define RISSE_E_INVALIDOBJECT			(-1006)
#define RISSE_E_ACCESSDENYED			(-1007)
#define RISSE_E_NATIVECLASSCRASH		(-1008)

#define RISSE_S_TRUE					(1)
#define RISSE_S_FALSE					(2)

#define RISSE_S_OK                    (0)
#define RISSE_E_FAIL					(-1)

#define RISSE_S_MAX (2)
	// maximum possible number of success status.
	// numbers over this may be regarded as a failure in
	// strict-checking mode.

#ifdef RISSE_STRICT_ERROR_CODE_CHECK
	static inline bool RISSE_FAILED(risse_error hr)
	{
		if(hr < 0) return true;
		if(hr > RISSE_S_MAX) return true;
		return false;
	}
#else
	#define RISSE_FAILED(x)				((x)<0)
#endif
#define RISSE_SUCCEEDED(x)			(!RISSE_FAILED(x))

static inline bool RisseIsObjectValid(risse_error hr)
{
	// checks object validity by returning value of iRisseDispatch2::IsValid

	if(hr == RISSE_S_TRUE) return true;  // mostly expected value for valid object
	if(hr == RISSE_E_NOTIMPL) return true; // also valid for object which does not implement IsValid

	return false; // otherwise the object is not valid
}

/*]*/

} // namespace Risse

#endif
