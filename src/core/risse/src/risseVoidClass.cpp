/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Void" クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "risseVoidClass.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "risseScriptEngine.h"

/*
	Risseスクリプトから見える"Void" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(32602,55496,58565,16678,10384,39976,12087,42806);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tVoidClass::tVoidClass(tScriptEngine * engine) :
	tPrimitiveClassBase(ss_Void, engine->PrimitiveClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVoidClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	// construct は tPrimitiveClass 内ですでに登録されている

	BindFunction(this, ss_ovulate, &tVoidClass::ovulate,
		tMemberAttribute().Set(tMemberAttribute::vcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, ss_initialize, &tVoidClass::initialize,
		tMemberAttribute().Set(tMemberAttribute::vcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, mnString, &tVoidClass::toString,
		tMemberAttribute().Set(tMemberAttribute::vcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, ss_dump, &tVoidClass::dump,
		tMemberAttribute().Set(tMemberAttribute::vcConst).Set(tMemberAttribute::ocFinal));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tVoidClass::ovulate()
{
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tVoidClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドは「呼び出されない」
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tVoidClass::toString()
{
	return tString(); // toString は必ず空の文字列を返す
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tVoidClass::dump()
{
	return ss_void; // "void" を返す
}
//---------------------------------------------------------------------------


} /* namespace Risse */

