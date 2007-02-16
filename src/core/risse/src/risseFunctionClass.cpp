/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Function" クラスの実装
//---------------------------------------------------------------------------

#include "risseTypes.h"
#include "risseFunctionClass.h"
#include "risseNativeFunction.h"
#include "risseNativeProperty.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"

/*
	Risseスクリプトから見える"Function" クラスの実装

	Function クラスは引数を一つ取り、関数の内容(文字列)あるいは関数インスタンスを渡す。

	ネイティブ実装の関数あるいは tRisseCodeBlock クラスは実際に tRisseObjectInterface
	を実装しており、関数として動作できるのだが、そのままでは Function クラスのインスタンス
	としての振る舞いをしない。そこで、この tRisseFunctionClass および tRisseFunctionInstance
	でそれらの「裸の」関数インスタンスをラップすることとなる。
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(40031,39054,14544,17698,39309,64830,49090,60837);


//---------------------------------------------------------------------------
tRisseFunctionInstance::tRisseFunctionInstance()
{
	Body.Nullize();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseFunctionInstance::tRetValue tRisseFunctionInstance::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(name.IsEmpty())
	{
		if(code == ocFuncCall) // このオブジェクトに対するメソッド呼び出しか？
		{
			Body.FuncCall(result, flags, args, This);
			return rvNoError;
		}
	}

	// そのほかの場合は親クラスのメソッドを呼ぶ
	return inherited::Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
tRisseFunctionClass::tRisseFunctionClass() :
	tRisseClassBase(tRisseObjectClass::GetPointer())
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseFunctionClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// 何もしない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す
		tRisseFunctionClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);

		// 引数 = {body}
		// TODO: 文字列が渡された場合は内容をコンパイルして関数として動作するようにする
		tRisseFunctionInstance * obj = This.CheckAndGetObjectInterafce<tRisseFunctionInstance, tRisseFunctionClass>();
		if(args.HasArgument(0)) obj->SetBody(args[0]);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseFunctionClass::CreateNewObjectBase()
{
	return tRisseVariant(new tRisseFunctionInstance());
}
//---------------------------------------------------------------------------


} /* namespace Risse */

