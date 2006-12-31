//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief Exception クラス実装
//---------------------------------------------------------------------------
#ifndef risseExceptionH
#define risseExceptionH

#include "risseNative.h"

namespace Risse
{
//---------------------------------------------------------------------------
// tRisseNC_Exception
//---------------------------------------------------------------------------
class tRisseNC_Exception : public tRisseNativeClass
{
public:
	tRisseNC_Exception();
private:
	static risse_uint32 ClassID;
};
//---------------------------------------------------------------------------
} // namespace Risse

#endif
