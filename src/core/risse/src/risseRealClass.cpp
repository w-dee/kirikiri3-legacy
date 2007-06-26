/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
tRisseRealClass::tRisseRealClass(tRisseScriptEngine * engine) :
	tRissePrimitiveClassBase(engine->NumberClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseRealClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	// construct は tRissePrimitiveClass 内ですでに登録されている

	RisseBindFunction(this, ss_ovulate, &tRisseRealClass::ovulate,
		tRisseMemberAttribute().Set(tRisseMemberAttribute::vcConst).Set(tRisseMemberAttribute::ocFinal));
	RisseBindFunction(this, ss_initialize, &tRisseRealClass::initialize,
		tRisseMemberAttribute().Set(tRisseMemberAttribute::vcConst).Set(tRisseMemberAttribute::ocFinal));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseRealClass::ovulate()
{
	return tRisseVariant((risse_real)0.0);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseRealClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドは「呼び出されない」

	// 引数を初期値に使う
	// 注意: いったん ovulate で作成されたオブジェクトの中身
	//       を変更するため、const_cast を用いる
	if(info.args.HasArgument(0))
		*const_cast<tRisseVariant*>(&info.This) = info.args[0].operator risse_real();
}
//---------------------------------------------------------------------------

} /* namespace Risse */

