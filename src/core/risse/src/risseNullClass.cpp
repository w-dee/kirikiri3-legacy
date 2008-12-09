/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Null" クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "risseNullClass.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "risseScriptEngine.h"

/*
	Risseスクリプトから見える"Null" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(28936,60521,6112,19205,55174,23971,13792,25618);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tNullClass, ss_Null, engine->PrimitiveClass, tVariant())
	// construct は tPrimitiveClass 内ですでに登録されている

	BindFunction(this, ss_ovulate, &tNullClass::ovulate,
		tMemberAttribute().Set(tMemberAttribute::mcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, ss_initialize, &tNullClass::initialize,
		tMemberAttribute().Set(tMemberAttribute::mcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, mnString, &tNullClass::toString,
		tMemberAttribute().Set(tMemberAttribute::mcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, ss_dump, &tNullClass::dump,
		tMemberAttribute().Set(tMemberAttribute::mcConst).Set(tMemberAttribute::ocFinal));
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNullClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドは「呼び出されない」
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tNullClass::toString()
{
	return tString(); // toString は必ず空の文字列を返す
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tNullClass::dump()
{
	return ss_null; // "null" を返す
}
//---------------------------------------------------------------------------


} /* namespace Risse */

