//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ネイティブ関数(ネイティブメソッド)を記述するためのサポート
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseNativeFunction.h"
#include "risseFunctionClass.h"
#include "risseScriptEngine.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(47510,49015,53768,19018,63934,19864,53779,8975);


//---------------------------------------------------------------------------
tRisseNativeFunction::tRetValue tRisseNativeFunction::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	// このオブジェクトに対する関数呼び出しか？
	if(code == ocFuncCall && name.IsEmpty())
	{
		// このオブジェクトに対する関数呼び出しなので Callee を呼ぶ
		Callee(GetRTTI()->GetScriptEngine(), result, flags, args, This);
		return rvNoError;
	}

	// それ以外の機能はこのインターフェースにはない
	return rvMemberNotFound;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectInterface * tRisseNativeFunction::New(tRisseScriptEngine * engine, tCallee callee)
{
	// tRisseFunctionClass がまだ登録されていない場合は仮のメソッドを
	// 作成して登録する (のちに正式なメソッドオブジェクトに置き換えられる)
	if(engine->FunctionClass)
	{
		tRisseVariant v = tRisseVariant(engine->FunctionClass).New(
				0, tRisseMethodArgument::New(new tRisseNativeFunction(engine, callee)));

		RISSE_ASSERT(v.GetType() == tRisseVariant::vtObject);
		RISSE_ASSERT(dynamic_cast<tRisseFunctionInstance*>(v.GetObjectInterface()) != NULL);

		return v.GetObjectInterface();
	}
	else
	{
		// 仮実装
		return (tRisseFunctionInstance *)(new tRisseNativeFunction(engine, callee));
	}
}
//---------------------------------------------------------------------------

} // namespace Risse
