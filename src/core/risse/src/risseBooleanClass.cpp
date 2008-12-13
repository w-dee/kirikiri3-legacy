/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Boolean" クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "risseBooleanClass.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "risseScriptEngine.h"

/*
	Risseスクリプトから見える"Boolean" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(60941,39465,30047,17348,4512,31235,57590,9843);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tBooleanClass, ss_Boolean, engine->PrimitiveClass)
	BindFunction(this, ss_ovulate, &tBooleanClass::ovulate,
		tMemberAttribute().Set(tMemberAttribute::mcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, ss_initialize, &tBooleanClass::initialize,
		tMemberAttribute().Set(tMemberAttribute::mcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, mnString, &tBooleanClass::toString,
		tMemberAttribute().Set(tMemberAttribute::mcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, ss_dump, &tBooleanClass::dump,
		tMemberAttribute().Set(tMemberAttribute::mcConst).Set(tMemberAttribute::ocFinal));
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tBooleanClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドは「呼び出されない」
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tBooleanClass::toString(const tNativeCallInfo & info)
{
	return info.This.operator bool() ? ss_true : ss_false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tBooleanClass::dump(const tNativeCallInfo & info)
{
	return info.This.operator bool() ? ss_true : ss_false;
}
//---------------------------------------------------------------------------


} /* namespace Risse */

