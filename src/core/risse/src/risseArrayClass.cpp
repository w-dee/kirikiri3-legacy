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
//! @brief		NativeProperty: Array.length
//---------------------------------------------------------------------------
static void Array_length_getter(RISSE_NATIVEPROPERTY_GETTER_ARGS)
{
	tRisseArrayObject * obj = This.CheckAndGetObjectInterafce<tRisseArrayObject, tRisseArrayClass>();
	tRisseArrayObject::tArray & array = obj->GetArray();

	if(result) *result = (risse_int64)array.size();
}
static void Array_length_setter(RISSE_NATIVEPROPERTY_SETTER_ARGS)
{
	tRisseArrayObject * obj = This.CheckAndGetObjectInterafce<tRisseArrayObject, tRisseArrayClass>();
	tRisseArrayObject::tArray & array = obj->GetArray();

	// TODO: デフォルトの値の扱い
	array.resize((risse_size)(risse_int64)(value));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseArrayClass::tRisseArrayClass() :
	tRisseClassBase(tRisseObjectClass::GetPointer())
{
	// クラスに必要なメソッドを登録する

	// construct, initialize などは新しいオブジェクトのコンテキスト上で実行されるので
	// コンテキストとしては null を指定する

	// construct
	RegisterNormalMember(ss_construct, tRisseVariant(new tRisseNativeFunctionBase(Array_construct)));
	// initialize
	RegisterNormalMember(ss_initialize, tRisseVariant(new tRisseNativeFunctionBase(Array_initialize)));

	// push
	RegisterNormalMember(ss_push, tRisseVariant(new tRisseNativeFunctionBase(Array_push)));
	// pop
	RegisterNormalMember(ss_pop, tRisseVariant(new tRisseNativeFunctionBase(Array_pop)));
	// length
	RegisterNormalMember(ss_length,
		tRisseVariant(new tRisseNativePropertyBase(Array_length_getter, Array_length_setter)),
			tRisseMemberAttribute(tRisseMemberAttribute::pcProperty));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseArrayClass::CreateNewObjectBase()
{
	return tRisseVariant(new tRisseArrayObject());
}
//---------------------------------------------------------------------------

} /* namespace Risse */

