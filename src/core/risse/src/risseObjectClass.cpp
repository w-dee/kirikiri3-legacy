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
static void Object_construct(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Object.initialize
//---------------------------------------------------------------------------
static void Object_initialize(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Object.getInstanceMember
//---------------------------------------------------------------------------
static void Object_getInstanceMember(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	// 引数チェック
	if(args.GetArgumentCount() < 1) RisseThrowBadArgumentCount(args.GetArgumentCount(), 1);

	// This のインスタンスメンバを取得する
	tRisseVariant ret = 
		This.GetPropertyDirect(args[0], tRisseOperateFlags::ofInstanceMemberOnly,
					This);
	if(result) *result = ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Object.setInstanceMember
//---------------------------------------------------------------------------
static void Object_setInstanceMember(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	// 引数チェック
	if(args.GetArgumentCount() < 2) RisseThrowBadArgumentCount(args.GetArgumentCount(), 2);

	// This のインスタンスメンバを設定する
	This.SetPropertyDirect(args[0],
		tRisseOperateFlags::ofInstanceMemberOnly|tRisseOperateFlags::ofMemberEnsure,
					args[1], This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Object.ptr  引数のオブジェクトインターフェースのアドレスを数値化する
//---------------------------------------------------------------------------
static void Object_ptr(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	for(risse_size i = 0; i < args.GetArgumentCount(); i++)
	{
		if(i != 0)
			RisseFPrint(stdout, RISSE_WS(", "));

		if(args[i].GetType() == tRisseVariant::vtObject)
		{
			risse_char buf[40];
			Risse_pointer_to_str(args[i].GetObjectInterface(), buf);
			RisseFPrint(stdout, (tRisseString(RISSE_WS("Object@")) + buf).c_str());
		}
		else
		{
			RisseFPrint(stdout, (args[i].operator tRisseString()).c_str());
		}
	}
	RisseFPrint(stdout, RISSE_WS("\n"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Object.puts  内容を標準出力に出力し、改行する
//---------------------------------------------------------------------------
static void Object_puts(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	// This を標準出力に出力する
	RisseFPrint(stdout, This.operator tRisseString().c_str());
	// 改行する
	RisseFPrint(stdout, RISSE_WS("\n"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectClass::tRisseObjectClass() : tRisseClassBase(NULL)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseObjectClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する

	// construct, initialize などは新しいオブジェクトのコンテキスト上で実行されるので
	// コンテキストとしては null を指定する
	tRisseVariant * pThis = new tRisseVariant(this);
	// construct
	RegisterNormalMember(ss_construct, tRisseVariant(tRisseNativeFunctionBase::New(Object_construct)));
	// initialize
	RegisterNormalMember(ss_initialize, tRisseVariant(tRisseNativeFunctionBase::New(Object_initialize)));
	// getInstanceMember
	RegisterNormalMember(ss_getInstanceMember, tRisseVariant(tRisseNativeFunctionBase::New(Object_getInstanceMember)));
	// setInstanceMener
	RegisterNormalMember(ss_setInstanceMember, tRisseVariant(tRisseNativeFunctionBase::New(Object_setInstanceMember)));
	// static show
	RegisterNormalMember(RISSE_WS("ptr"), tRisseVariant(tRisseNativeFunctionBase::New(Object_ptr), pThis));
	// puts
	RegisterNormalMember(RISSE_WS("puts"), tRisseVariant(tRisseNativeFunctionBase::New(Object_puts)));
}
//---------------------------------------------------------------------------

} /* namespace Risse */

