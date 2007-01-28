/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Integer" クラスの実装
//---------------------------------------------------------------------------

#include "risseTypes.h"
#include "risseIntegerClass.h"
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
RISSE_DEFINE_SOURCE_ID(39234,49682,57279,16499,28574,56016,64030,59385);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		NativeFunction: Integer.initialize
//---------------------------------------------------------------------------
static void Integer_initialize(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
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
//! @brief		NativeFunction: Integer.times
//---------------------------------------------------------------------------
static void Integer_times(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	if(args.GetBlockArgumentCount() < 1) RisseThrowBadBlockArgumentCount(args.GetArgumentCount(), 1);

	risse_int64 count = This.operator risse_int64();
	while(count --)
		args.GetBlockArgument(0).FuncCall();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseIntegerClass::tRisseIntegerClass() : tRissePrimitiveClassBase(tRisseNumberClass::GetPointer())
{
	// クラスに必要なメソッドを登録する

	// construct は tRissePrimitiveClass 内ですでに登録されている

	// initialize
	RegisterNormalMember(ss_initialize, tRisseVariant(new tRisseNativeFunctionBase(Integer_initialize)));

	// times
	RegisterNormalMember(ss_times, tRisseVariant(new tRisseNativeFunctionBase(Integer_times)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseIntegerClass::CreateNewObjectBase()
{
	return tRisseVariant((risse_int64)0);
}
//---------------------------------------------------------------------------

} /* namespace Risse */
