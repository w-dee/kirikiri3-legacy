/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Object" クラスの実装
//---------------------------------------------------------------------------

#include "risseTypes.h"
#include "risseObjectClass.h"
#include "risseNativeFunction.h"
#include "risseStaticStrings.h"

/*
	Risseスクリプトから見える"Object" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(41134,45186,9497,17812,19604,2796,36426,21671);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Object.construct
//---------------------------------------------------------------------------
class tRisseNativeFunction_Object_construct : public tRisseNativeFunctionBase
{
protected:
	//! @brief		Risseメソッド呼び出し時に呼ばれるメソッド
	void Call(
		tRisseVariant * result,
		tRisseOperateFlags flags,
		const tRisseMethodArgument & args,
		const tRisseMethodArgument & bargs,
		const tRisseVariant &This,
		const tRisseStackFrameContext &stack)
	{
		// デフォルトでは何もしない
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Object.initialize
//---------------------------------------------------------------------------
class tRisseNativeFunction_Object_initialize : public tRisseNativeFunctionBase
{
protected:
	//! @brief		Risseメソッド呼び出し時に呼ばれるメソッド
	void Call(
		tRisseVariant * result,
		tRisseOperateFlags flags,
		const tRisseMethodArgument & args,
		const tRisseMethodArgument & bargs,
		const tRisseVariant &This,
		const tRisseStackFrameContext &stack)
	{
		// デフォルトでは何もしない
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Object.getInstanceMember
//---------------------------------------------------------------------------
class tRisseNativeFunction_Object_getInstanceMember : public tRisseNativeFunctionBase
{
protected:
	//! @brief		Risseメソッド呼び出し時に呼ばれるメソッド
	void Call(
		tRisseVariant * result,
		tRisseOperateFlags flags,
		const tRisseMethodArgument & args,
		const tRisseMethodArgument & bargs,
		const tRisseVariant &This,
		const tRisseStackFrameContext &stack)
	{
		// 引数チェック
		if(args.GetCount() < 1) RisseThrowBadArgumentCount(args.GetCount(), 1);

		// This のインスタンスメンバを取得する
		tRisseVariant ret = 
			This.GetPropertyDirect(args[0], tRisseOperateFlags::ofInstanceMemberOnly,
						This);
		if(result) *result = ret;
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Object.setInstanceMember
//---------------------------------------------------------------------------
class tRisseNativeFunction_Object_setInstanceMember : public tRisseNativeFunctionBase
{
protected:
	//! @brief		Risseメソッド呼び出し時に呼ばれるメソッド
	void Call(
		tRisseVariant * result,
		tRisseOperateFlags flags,
		const tRisseMethodArgument & args,
		const tRisseMethodArgument & bargs,
		const tRisseVariant &This,
		const tRisseStackFrameContext &stack)
	{
		// 引数チェック
		if(args.GetCount() < 2) RisseThrowBadArgumentCount(args.GetCount(), 2);

		// This のインスタンスメンバを設定する
		This.SetPropertyDirect(args[0], tRisseOperateFlags::ofInstanceMemberOnly,
						args[1], This);
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectClass::tRisseObjectClass() : tRisseClass(tRisseVariant::GetNullObject())
{
	// クラスに必要なメソッドを登録する

	// construct, init などは新しいオブジェクトのコンテキスト上で実行されるので
	// コンテキストとしては null を指定する

	// construct
	RegisterNormalMember(mnNew, tRisseVariant(new tRisseNativeFunction_Object_construct()));
	// object
	RegisterNormalMember(ss_fertilize, tRisseVariant(new tRisseNativeFunction_Object_initialize()));
	// getInstanceMember
	RegisterNormalMember(ss_getInstanceMember, tRisseVariant(new tRisseNativeFunction_Object_getInstanceMember()));
	// setInstanceMener
	RegisterNormalMember(ss_setInstanceMember, tRisseVariant(new tRisseNativeFunction_Object_setInstanceMember()));
}
//---------------------------------------------------------------------------

} /* namespace Risse */

