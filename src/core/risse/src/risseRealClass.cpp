/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Real" クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "risseRealClass.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "risseNumberClass.h"
#include "risseScriptEngine.h"

/*
	Risseスクリプトから見える"String" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(5442,9100,49866,17725,24713,23464,12701,40981);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tRealClass::tRealClass(tScriptEngine * engine) :
	tPrimitiveClassBase(ss_Real, engine->NumberClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRealClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	// construct は tPrimitiveClass 内ですでに登録されている

	BindFunction(this, ss_ovulate, &tRealClass::ovulate,
		tMemberAttribute().Set(tMemberAttribute::vcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, ss_initialize, &tRealClass::initialize,
		tMemberAttribute().Set(tMemberAttribute::vcConst).Set(tMemberAttribute::ocFinal));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tRealClass::ovulate()
{
	return tVariant((risse_real)0.0);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRealClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドは「呼び出されない」

	// 引数を初期値に使う
	// 注意: いったん ovulate で作成されたオブジェクトの中身
	//       を変更するため、const_cast を用いる
	if(info.args.HasArgument(0))
		*const_cast<tVariant*>(&info.This) = info.args[0].operator risse_real();
}
//---------------------------------------------------------------------------

} /* namespace Risse */

