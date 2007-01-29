//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief "Class" (クラス) の実装
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseClass.h"
#include "risseClassClass.h"
#include "risseObjectClass.h"
#include "risseNativeFunction.h"
#include "risseNativeProperty.h"
#include "risseOpCodes.h"
#include "risseStaticStrings.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(28480,29035,20490,18954,3474,2858,57740,45280);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		NativeFunction: Class.construct
//---------------------------------------------------------------------------
static void Class_construct(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Class.initialize
//---------------------------------------------------------------------------
static void Class_initialize(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	// 親クラスの同名メソッドを呼び出す
	// TODO: コンストラクタの正しい実装

	if(args.HasArgument(0))
	{
		// スーパークラスが指定されている
		// super を登録
		tRisseOperateFlags access_flags =
			tRisseOperateFlags::ofMemberEnsure|tRisseOperateFlags::ofInstanceMemberOnly;
		This.SetPropertyDirect(ss_super,
			tRisseOperateFlags(tRisseMemberAttribute(tRisseMemberAttribute::pcVar))|access_flags,
			args[0], This);
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tRisseClassClass::tRisseClassClass() : tRisseClassBase(tRisseObjectClass::GetPointer())
{
	// クラスに必要なメソッドを登録する

	// construct, initialize などは新しいオブジェクトのコンテキスト上で実行されるので
	// コンテキストとしては null を指定する

	// construct
	RegisterNormalMember(ss_construct, tRisseVariant(new tRisseNativeFunctionBase(Class_construct)));
	// initialize
	RegisterNormalMember(ss_initialize, tRisseVariant(new tRisseNativeFunctionBase(Class_initialize)));

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseClassClass::CreateNewObjectBase()
{
	return tRisseVariant(new tRisseClassBase(tRisseObjectClass::GetPointer()));
}
//---------------------------------------------------------------------------

} // namespace Risse
