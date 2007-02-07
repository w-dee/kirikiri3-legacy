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
#include "risseException.h"
#include "risseFunctionClass.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(47510,49015,53768,19018,63934,19864,53779,8975);
//---------------------------------------------------------------------------
tRisseNativeFunctionBase::tRetValue tRisseNativeFunctionBase::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	// このオブジェクトに対する関数呼び出しか？
	if(code == ocFuncCall && name.IsEmpty())
	{
		// このオブジェクトに対する関数呼び出しなので Callee を呼ぶ
		Callee(result, flags, args, This);
		return rvNoError;
	}

	// それ以外の機能はこのインターフェースにはない
	return rvMemberNotFound;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseFunctionInstance * tRisseNativeFunctionBase::New(tCallee callee)
{
	return (tRisseFunctionInstance *)(new tRisseNativeFunctionBase(callee));
/*
	tRisseVariant v = tRisseVariant(tRisseFunctionClass::GetPointer()).New(
			0, tRisseMethodArgument::New(new tRisseNativeFunctionBase(callee)));

	RISSE_ASSERT(v.GetType() == tRisseVariant::vtObject);
	RISSE_ASSERT(dynamic_cast<tRisseFunctionInstance*>(v.GetObjectInterface()) != NULL);

	tRisseFunctionInstance * intf = reinterpret_cast<tRisseFunctionInstance*>(v.GetObjectInterface());
	return intf;
*/
}
//---------------------------------------------------------------------------

} // namespace Risse
