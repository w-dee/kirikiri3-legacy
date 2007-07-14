/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
tBooleanClass::tBooleanClass(tScriptEngine * engine) :
	tPrimitiveClassBase(ss_Boolean, engine->PrimitiveClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tBooleanClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	// construct は tPrimitiveClass 内ですでに登録されている

	BindFunction(this, ss_ovulate, &tBooleanClass::ovulate,
		tMemberAttribute().Set(tMemberAttribute::vcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, ss_initialize, &tBooleanClass::initialize,
		tMemberAttribute().Set(tMemberAttribute::vcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, mnString, &tBooleanClass::toString,
		tMemberAttribute().Set(tMemberAttribute::vcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, ss_dump, &tBooleanClass::dump,
		tMemberAttribute().Set(tMemberAttribute::vcConst).Set(tMemberAttribute::ocFinal));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tBooleanClass::ovulate()
{
	return tVariant();
}
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

