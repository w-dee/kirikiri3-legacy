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
tRissePropertyClass::tRissePropertyClass() :
	tRisseClassBase(tRisseObjectClass::GetPointer())
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRissePropertyClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// なにもしない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す
		tRissePropertyClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);

		// 引数 = {getter, setter}
		tRissePropertyInstance * obj = This.CheckAndGetObjectInterafce<tRissePropertyInstance, tRissePropertyClass>();
		if(args.HasArgument(0)) obj->SetGetter(args[0]);
		if(args.HasArgument(1)) obj->SetSetter(args[1]);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_PROPERTY(ss_getter)
	{
		RISSE_BEGINE_NATIVE_PROPERTY_GETTER
		{
			tRissePropertyInstance * obj = This.CheckAndGetObjectInterafce<tRissePropertyInstance, tRissePropertyClass>();
			if(result) *result = obj->GetGetter();
		}
		RISSE_END_NATIVE_PROPERTY_GETTER

		/*
		このプロパティへの書き込みのサポートはない
		(いったん定義したプロパティの内容を個別に変えられると困る場合があるため.特にプリミティブクラスのプロパティ.
		また、これが再定義できてもあまりうれしくないと思うのだが)
		RISSE_BEGINE_NATIVE_PROPERTY_SETTER
		{
			tRissePropertyInstance * obj = This.CheckAndGetObjectInterafce<tRissePropertyInstance, tRissePropertyClass>();
			obj->SetGetter(value);
		}
		RISSE_END_NATIVE_PROPERTY_SETTER
		*/
	}
	RISSE_END_NATIVE_PROPERTY

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_PROPERTY(ss_setter)
	{
		RISSE_BEGINE_NATIVE_PROPERTY_GETTER
		{
			tRissePropertyInstance * obj = This.CheckAndGetObjectInterafce<tRissePropertyInstance, tRissePropertyClass>();
			if(result) *result = obj->GetSetter();
		}
		RISSE_END_NATIVE_PROPERTY_GETTER

		/*
		このプロパティへの書き込みのサポートはない
		(いったん定義したプロパティの内容を個別に変えられると困る場合があるため.特にプリミティブクラスのプロパティ.
		また、これが再定義できてもあまりうれしくないと思うのだが)
		RISSE_BEGINE_NATIVE_PROPERTY_SETTER
		{
			tRissePropertyInstance * obj = This.CheckAndGetObjectInterafce<tRissePropertyInstance, tRissePropertyClass>();
			obj->SetSetter(value);
		}
		RISSE_END_NATIVE_PROPERTY_SETTER
		*/
	}
	RISSE_END_NATIVE_PROPERTY

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRissePropertyClass::CreateNewObjectBase()
{
	return tRisseVariant(new tRissePropertyInstance());
}
//---------------------------------------------------------------------------

} /* namespace Risse */

