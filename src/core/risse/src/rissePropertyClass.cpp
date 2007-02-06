/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Property" クラスの実装
//---------------------------------------------------------------------------

#include "risseTypes.h"
#include "rissePropertyClass.h"
#include "risseNativeFunction.h"
#include "risseNativeProperty.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"

/*
	Risseスクリプトから見える"Property" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(26307,13927,24599,16690,19605,20552,47176,34884);
static void Array_push(RISSE_NATIVEFUNCTION_CALLEE_ARGS);


//---------------------------------------------------------------------------
tRissePropertyInstance::tRissePropertyInstance()
{
	Getter.Nullize();
	Setter.Nullize();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRissePropertyInstance::tRetValue tRissePropertyInstance::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(name.IsEmpty())
	{
		if(code == ocFuncCall) // このオブジェクトに対するプロパティ読み込みか？
		{
			// このオブジェクトに対するプロパティ読み込みなので Getter を呼ぶ
			if(Getter.IsNull()) RisseThrowPropertyCannotBeRead();
			Getter.FuncCall(result, flags, tRisseMethodArgument::Empty(), This);
			return rvNoError;
		}
		else if(code == ocDSet) // このオブジェクトに対するプロパティ書き込みか？
		{
			// このオブジェクトに対するプロパティ書き込みなので Setter を呼ぶ
			if(Setter.IsNull()) RisseThrowPropertyCannotBeWritten();
			if(args.GetArgumentCount() < 1) RisseThrowBadArgumentCount(args.GetArgumentCount(), 1);
			Setter.FuncCall(NULL, flags, args, This);
			return rvNoError;
		}
	}

	// そのほかの場合は親クラスのメソッドを呼ぶ
	return inherited::Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		NativeFunction: Property.construct
//---------------------------------------------------------------------------
static void Property_construct(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Property.initialize
//---------------------------------------------------------------------------
static void Property_initialize(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	// 親クラスの同名メソッドを呼び出す
	tRissePropertyClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);

	// 引数 = {getter, setter}
	tRissePropertyInstance * obj = This.CheckAndGetObjectInterafce<tRissePropertyInstance, tRissePropertyClass>();
	if(args.HasArgument(0)) obj->SetGetter(args[0]);
	if(args.HasArgument(1)) obj->SetSetter(args[1]);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeProperty: Property.getter
//---------------------------------------------------------------------------
static void Property_getter_getter(RISSE_NATIVEPROPERTY_GETTER_ARGS)
{
	tRissePropertyInstance * obj = This.CheckAndGetObjectInterafce<tRissePropertyInstance, tRissePropertyClass>();
	if(result) *result = obj->GetGetter();
}
static void Property_getter_setter(RISSE_NATIVEPROPERTY_SETTER_ARGS)
{
	tRissePropertyInstance * obj = This.CheckAndGetObjectInterafce<tRissePropertyInstance, tRissePropertyClass>();
	obj->SetGetter(value);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeProperty: Property.setter
//---------------------------------------------------------------------------
static void Property_setter_getter(RISSE_NATIVEPROPERTY_GETTER_ARGS)
{
	tRissePropertyInstance * obj = This.CheckAndGetObjectInterafce<tRissePropertyInstance, tRissePropertyClass>();
	if(result) *result = obj->GetSetter();
}
static void Property_setter_setter(RISSE_NATIVEPROPERTY_SETTER_ARGS)
{
	tRissePropertyInstance * obj = This.CheckAndGetObjectInterafce<tRissePropertyInstance, tRissePropertyClass>();
	obj->SetSetter(value);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRissePropertyClass::tRissePropertyClass() :
	tRisseClassBase(tRisseObjectClass::GetPointer())
{
	// クラスに必要なメソッドを登録する

	// construct, initialize などは新しいオブジェクトのコンテキスト上で実行されるので
	// コンテキストとしては null を指定する

	// construct
	RegisterNormalMember(ss_construct, tRisseVariant(new tRisseNativeFunctionBase(Property_construct)));
	// initialize
	RegisterNormalMember(ss_initialize, tRisseVariant(new tRisseNativeFunctionBase(Property_initialize)));

	// getter
	RegisterNormalMember(ss_getter,
		tRisseVariant(new tRisseNativePropertyBase(Property_getter_getter, Property_getter_setter)),
			tRisseMemberAttribute(tRisseMemberAttribute::pcProperty));
	// setter
	RegisterNormalMember(ss_setter,
		tRisseVariant(new tRisseNativePropertyBase(Property_setter_getter, Property_setter_setter)),
			tRisseMemberAttribute(tRisseMemberAttribute::pcProperty));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRissePropertyClass::CreateNewObjectBase()
{
	return tRisseVariant(new tRissePropertyInstance());
}
//---------------------------------------------------------------------------

} /* namespace Risse */

