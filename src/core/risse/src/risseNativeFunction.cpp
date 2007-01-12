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

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(47510,49015,53768,19018,63934,19864,53779,8975);
//---------------------------------------------------------------------------
tRetValue tRisseNativeFunctionBase::Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG)
{
	// このオブジェクトに対する関数呼び出しか？
	if(code == ocFuncCall && name.IsEmpty())
	{
		// このオブジェクトに対する関数呼び出しなので Call を呼ぶ
		Call(result, flags, args, bargs, This, stack);
		return rvNoError;
	}

	// そのほかの場合は親クラスのメソッドを呼ぶ
	return inherited::Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
}
//---------------------------------------------------------------------------
} // namespace Risse
