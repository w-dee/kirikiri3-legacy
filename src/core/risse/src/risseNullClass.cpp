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
tNullClass::tNullClass(tScriptEngine * engine) :
	tPrimitiveClassBase(ss_Null, engine->PrimitiveClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNullClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	// construct は tPrimitiveClass 内ですでに登録されている

	BindFunction(this, ss_ovulate, &tNullClass::ovulate,
		tMemberAttribute().Set(tMemberAttribute::mcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, ss_initialize, &tNullClass::initialize,
		tMemberAttribute().Set(tMemberAttribute::mcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, mnString, &tNullClass::toString,
		tMemberAttribute().Set(tMemberAttribute::mcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, ss_dump, &tNullClass::dump,
		tMemberAttribute().Set(tMemberAttribute::mcConst).Set(tMemberAttribute::ocFinal));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tNullClass::ovulate()
{
	return tVariant();
}
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

