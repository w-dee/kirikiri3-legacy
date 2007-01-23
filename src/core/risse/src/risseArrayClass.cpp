/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Array" クラスの実装
//---------------------------------------------------------------------------

#include "risseTypes.h"
#include "risseArrayClass.h"
#include "risseNativeFunction.h"
#include "risseNativeProperty.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"

/*
	Risseスクリプトから見える"Array" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(65360,34010,1527,19914,27817,35057,17111,22724);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Array.construct
//---------------------------------------------------------------------------
static void Array_construct(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Array.initialize
//---------------------------------------------------------------------------
static void Array_initialize(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	// 親クラスの同名メソッドを呼び出す
	// TODO: コンストラクタの正しい実装
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Array.push
//---------------------------------------------------------------------------
static void Array_push(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	tRisseArrayObject * obj = This.CheckAndGetObjectInterafce<tRisseArrayObject, tRisseArrayClass>();
	tRisseArrayObject::tArray & array = obj->GetArray();

	for(risse_size i = 0; i < args.GetArgumentCount(); i++)
		array.push_back(args[i]);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Array.pop
//---------------------------------------------------------------------------
static void Array_pop(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	tRisseArrayObject * obj = This.CheckAndGetObjectInterafce<tRisseArrayObject, tRisseArrayClass>();
	tRisseArrayObject::tArray & array = obj->GetArray();

	tRisseVariant val;
	if(array.size() > 0)
	{
		val = array.back();
		array.pop_back();
	}
	if(result) *result = val;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseArrayClass::tRisseArrayClass() :
	tRisseClass(tRisseObjectClass::GetPointer())
{
	// クラスに必要なメソッドを登録する

	// construct, initialize などは新しいオブジェクトのコンテキスト上で実行されるので
	// コンテキストとしては null を指定する

	// construct
	RegisterNormalMember(ss_construct, tRisseVariant(new tRisseNativeFunctionBase(Array_construct)));
	// initialize
	RegisterNormalMember(ss_initialize, tRisseVariant(new tRisseNativeFunctionBase(Array_initialize)));

	// push
	RegisterNormalMember(RISSE_WS("push"), tRisseVariant(new tRisseNativeFunctionBase(Array_push)));
	// pop
	RegisterNormalMember(RISSE_WS("pop"), tRisseVariant(new tRisseNativeFunctionBase(Array_pop)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectBase * tRisseArrayClass::CreateNewObjectBase()
{
	return new tRisseArrayObject();
}
//---------------------------------------------------------------------------

} /* namespace Risse */

