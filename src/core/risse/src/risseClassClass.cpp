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
#include "risseExceptionClass.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(28480,29035,20490,18954,3474,2858,57740,45280);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tRisseClassClass::tRisseClassClass() : tRisseClassBase(tRisseModuleClass::GetPointer())
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseClassClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// デフォルトでは何もしない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
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
			// スーパークラスはクラスのインスタンスかどうかをチェック
			tRisseVariant super_class = args[0];
			if(!super_class.InstanceOf(tRisseVariant(tRisseClassClass::GetPointer())))
				tRisseClassDefinitionExceptionClass::ThrowSuperClassIsNotAClass();

			// super を登録
			tRisseOperateFlags access_flags =
				tRisseOperateFlags::ofMemberEnsure|tRisseOperateFlags::ofInstanceMemberOnly;
			This.SetPropertyDirect(ss_super,
				tRisseOperateFlags(tRisseMemberAttribute::GetDefault())|
				access_flags,
				super_class, This);
		}

		// ここで登録した construct と initialize を削除する。
		// これらは、This の初期化には必要だったが、このさきこのクラスに実装されるであろう
		// ユーザ定義の construct と initialize にとってはじゃまである。
		// これらがここに残っていると、親クラス内の construct や initialize を正常に
		// 参照できないという意味でもじゃまである。
		This.DeletePropertyDirect(ss_construct, tRisseOperateFlags::ofInstanceMemberOnly);
		This.DeletePropertyDirect(ss_initialize, tRisseOperateFlags::ofInstanceMemberOnly);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseClassClass::tRetValue tRisseClassClass::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	return inherited::Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseClassClass::CreateNewObjectBase()
{
	return tRisseVariant(new tRisseClassInstance());
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tRisseClassInstance::tRisseClassInstance()
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseClassInstance::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseClassInstance::tRetValue tRisseClassInstance::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	return inherited::Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseClassInstance::CreateNewObjectBase()
{
	return tRisseVariant(new tRisseObjectBase());
}
//---------------------------------------------------------------------------

} // namespace Risse
