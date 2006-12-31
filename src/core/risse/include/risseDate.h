//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief Date クラス実装
//---------------------------------------------------------------------------

#ifndef risseDateH
#define risseDateH

#include <time.h>
#include "risseNative.h"

namespace Risse
{
//---------------------------------------------------------------------------
class tRisseNI_Date : public tRisseNativeInstance
{
public:
	tRisseNI_Date();
	time_t DateTime;
private:
};

//---------------------------------------------------------------------------
class tRisseNC_Date : public tRisseNativeClass
{
public:
	tRisseNC_Date();

	static risse_uint32 ClassID;

private:
	tRisseNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------
} // namespace Risse

#endif
