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
tRisseBooleanClass::tRisseBooleanClass(tRisseScriptEngine * engine) :
	tRissePrimitiveClassBase(engine->PrimitiveClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBooleanClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	// construct は tRissePrimitiveClass 内ですでに登録されている

	RisseBindFunction(this, ss_ovulate, &tRisseBooleanClass::ovulate,
		tRisseMemberAttribute().Set(tRisseMemberAttribute::vcConst).Set(tRisseMemberAttribute::ocFinal));
	RisseBindFunction(this, ss_initialize, &tRisseBooleanClass::initialize,
		tRisseMemberAttribute().Set(tRisseMemberAttribute::vcConst).Set(tRisseMemberAttribute::ocFinal));
	RisseBindFunction(this, mnString, &tRisseBooleanClass::toString,
		tRisseMemberAttribute().Set(tRisseMemberAttribute::vcConst).Set(tRisseMemberAttribute::ocFinal));
	RisseBindFunction(this, ss_dump, &tRisseBooleanClass::dump,
		tRisseMemberAttribute().Set(tRisseMemberAttribute::vcConst).Set(tRisseMemberAttribute::ocFinal));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseBooleanClass::ovulate()
{
	return tRisseVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBooleanClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドは「呼び出されない」
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseBooleanClass::toString(const tRisseNativeCallInfo & info)
{
	return info.This.operator bool() ? ss_true : ss_false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseBooleanClass::dump(const tRisseNativeCallInfo & info)
{
	return info.This.operator bool() ? ss_true : ss_false;
}
//---------------------------------------------------------------------------


} /* namespace Risse */

