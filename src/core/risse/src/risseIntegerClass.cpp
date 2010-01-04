/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Integer" クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "risseIntegerClass.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "risseNumberClass.h"
#include "risseScriptEngine.h"

/*
	Risseスクリプトから見える"String" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(39234,49682,57279,16499,28574,56016,64030,59385);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tIntegerClass, ss_Integer, engine->NumberClass)
	// construct は tPrimitiveClass 内ですでに登録されている

	BindFunction(this, ss_ovulate, &tIntegerClass::ovulate,
		tMemberAttribute(	tMemberAttribute(tMemberAttribute::mcConst)|
								tMemberAttribute(tMemberAttribute::ocFinal)) );
	BindFunction(this, ss_initialize,
		&tIntegerClass::initialize,
		tMemberAttribute(	tMemberAttribute(tMemberAttribute::mcConst)|
								tMemberAttribute(tMemberAttribute::ocFinal)) );
	BindFunction(this, ss_times, &tIntegerClass::times);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIntegerClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドは「呼び出されない」

	// 引数を初期値に使う
	// 注意: いったん ovulate で作成されたオブジェクトの中身
	//       を変更するため、const_cast を用いる
	if(info.args.HasArgument(0))
		*const_cast<tVariant*>(&info.This) = info.args[0].operator risse_int64();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIntegerClass::times(const tNativeCallInfo & info)
{
	info.args.ExpectBlockArgumentCount(1);

	risse_int64 count = info.This.operator risse_int64();
	while(count --)
		info.args.GetBlockArgument(0).FuncCall(info.engine);
}
//---------------------------------------------------------------------------

} /* namespace Risse */

