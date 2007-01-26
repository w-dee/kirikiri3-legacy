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

#include "risseTypes.h"
#include "risseRealClass.h"
#include "risseNativeFunction.h"
#include "risseNativeProperty.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "risseNumberClass.h"

/*
	Risseスクリプトから見える"String" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(5442,9100,49866,17725,24713,23464,12701,40981);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		NativeFunction: Real.initialize
//---------------------------------------------------------------------------
static void Real_initialize(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	// 親クラスの同名メソッドは「呼び出されない」

	// 引数をすべて連結した物を初期値に使う
	// 注意: いったん CreateNewObjectBase で作成されたオブジェクトの中身
	//       を変更するため、const_cast を用いる
	if(args.GetArgumentCount() > 0)
		*const_cast<tRisseVariant*>(&This) = args[0].operator risse_int64();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseRealClass::tRisseRealClass() : tRissePrimitiveClassBase(tRisseNumberClass::GetPointer())
{
	// クラスに必要なメソッドを登録する

	// construct は tRissePrimitiveClass 内ですでに登録されている

	// initialize
	RegisterNormalMember(ss_initialize, tRisseVariant(new tRisseNativeFunctionBase(Real_initialize)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseRealClass::CreateNewObjectBase()
{
	return tRisseVariant((risse_real)0);
}
//---------------------------------------------------------------------------

} /* namespace Risse */

