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
#include "risseModuleClass.h"
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
	// 引数は  { 親クラス, 名前 }
	if(args.HasArgument(1))
	{
		// 名前を渡す
		tRisseClassClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, tRisseMethodArgument::New(args[1]), This);
	}
	else
	{
		// 名前がないので引数無し
		tRisseClassClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, tRisseMethodArgument::Empty(), This);
	}

	if(args.HasArgument(0) && !args[0].IsNull())
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
tRisseClassClass::tRisseClassClass() : tRisseClassBase(tRisseModuleClass::GetPointer())
{
	// クラスに必要なメソッドを登録する

	// construct, initialize などは新しいオブジェクトのコンテキスト上で実行されるので
	// コンテキストとしては null を指定する

	// construct
	RegisterNormalMember(ss_construct, tRisseVariant(tRisseNativeFunctionBase::New(Class_construct)));
	// initialize
	RegisterNormalMember(ss_initialize, tRisseVariant(tRisseNativeFunctionBase::New(Class_initialize)));

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseClassClass::CreateNewObjectBase()
{
	return tRisseVariant(new tRisseClassInstance());
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tRisseVariant tRisseClassInstance::CreateNewObjectBase()
{
	return tRisseVariant(new tRisseObjectBase());
}
//---------------------------------------------------------------------------

} // namespace Risse
