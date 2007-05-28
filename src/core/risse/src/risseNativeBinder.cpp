//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ネイティブ関数/プロパティを簡単に記述するためのC++テンプレートサポート
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseNativeBinder.h"
#include "risseScriptEngine.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(50153,12161,23237,20278,22942,17690,37012,37765);


//---------------------------------------------------------------------------
tRisseNativeBindFunction::tRetValue tRisseNativeBindFunction::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	// このオブジェクトに対する関数呼び出しか？
	if(code == ocFuncCall && name.IsEmpty())
	{
		// このオブジェクトに対する関数呼び出しなので Callee を呼ぶ
		tRisseNativeBindFunctionCallingInfo info(GetRTTI()->GetScriptEngine(), result, flags, args, This);
		Callee(Class, TargetFunction, info);
		return rvNoError;
	}

	// それ以外の機能はこのインターフェースにはない
	return rvMemberNotFound;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectInterface * tRisseNativeBindFunction::New(tRisseScriptEngine * engine,
	tRisseClassBase * class_, void (tRisseObjectBase::*target)(), tCallee callee)
{
	// tRisseFunctionClass がまだ登録されていない場合は仮のメソッドを
	// 作成して登録する (のちに正式なメソッドオブジェクトに置き換えられる)
	if(engine->FunctionClass)
	{
		tRisseVariant v = tRisseVariant(engine->FunctionClass).New(
				0, tRisseMethodArgument::New(new tRisseNativeBindFunction(engine, class_, target, callee)));

		RISSE_ASSERT(v.GetType() == tRisseVariant::vtObject);
		RISSE_ASSERT(dynamic_cast<tRisseFunctionInstance*>(v.GetObjectInterface()) != NULL);

		return v.GetObjectInterface();
	}
	else
	{
		// 仮実装
		return (tRisseFunctionInstance *)(new tRisseNativeBindFunction(engine, class_, target, callee));
	}
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
tRisseNativeBindStaticFunction::tRetValue tRisseNativeBindStaticFunction::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	// このオブジェクトに対する関数呼び出しか？
	if(code == ocFuncCall && name.IsEmpty())
	{
		// このオブジェクトに対する関数呼び出しなので Callee を呼ぶ
		tRisseNativeBindFunctionCallingInfo info(GetRTTI()->GetScriptEngine(), result, flags, args, This);
		Callee(Class, TargetFunction, info);
		return rvNoError;
	}

	// それ以外の機能はこのインターフェースにはない
	return rvMemberNotFound;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectInterface * tRisseNativeBindStaticFunction::New(tRisseScriptEngine * engine,
	tRisseClassBase * class_, void (*target)(), tCallee callee)
{
	// tRisseFunctionClass がまだ登録されていない場合は仮のメソッドを
	// 作成して登録する (のちに正式なメソッドオブジェクトに置き換えられる)
	if(engine->FunctionClass)
	{
		tRisseVariant v = tRisseVariant(engine->FunctionClass).New(
				0, tRisseMethodArgument::New(new tRisseNativeBindStaticFunction(engine, class_, target, callee)));

		RISSE_ASSERT(v.GetType() == tRisseVariant::vtObject);
		RISSE_ASSERT(dynamic_cast<tRisseFunctionInstance*>(v.GetObjectInterface()) != NULL);

		return v.GetObjectInterface();
	}
	else
	{
		// 仮実装
		return (tRisseFunctionInstance *)(new tRisseNativeBindStaticFunction(engine, class_, target, callee));
	}
}
//---------------------------------------------------------------------------


} // namespace Risse
