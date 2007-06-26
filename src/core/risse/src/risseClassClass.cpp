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
#include "risseOpCodes.h"
#include "risseStaticStrings.h"
#include "risseExceptionClass.h"
#include "risseScriptEngine.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(28480,29035,20490,18954,3474,2858,57740,45280);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tRisseClassClass::tRisseClassClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->ModuleClass)
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

	RisseBindFunction(this, ss_ovulate, &tRisseClassClass::ovulate);
	RisseBindFunction(this, ss_construct, &tRisseClassClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseClassClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseClassClass::tRetValue tRisseClassClass::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	return inherited::Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseClassClass::ovulate(const tRisseNativeCallInfo &info)
{
	return tRisseVariant(new tRisseClassInstance(info.engine));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseClassClass::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseClassClass::initialize(const tRisseNativeCallInfo &info)
{
	// 親クラスの同名メソッドを呼び出す
	// 引数は  { 親クラス, 名前 }
	if(info.args.HasArgument(1))
	{
		// 名前を渡す
		info.InitializeSuperClass(tRisseMethodArgument::New(info.args[1]));
	}
	else
	{
		// 名前がないので引数無し
		info.InitializeSuperClass();
	}

	if(info.args.HasArgument(0) && !info.args[0].IsNull())
	{
		// スーパークラスが指定されている
		// スーパークラスはクラスのインスタンスかどうかをチェック
		tRisseVariant super_class = info.args[0];
		if(!super_class.InstanceOf(info.engine, tRisseVariant(info.engine->ClassClass)))
			tRisseClassDefinitionExceptionClass::ThrowSuperClassIsNotAClass();

		// super を登録
		tRisseOperateFlags access_flags =
			tRisseOperateFlags::ofMemberEnsure|tRisseOperateFlags::ofInstanceMemberOnly;
		info.This.SetPropertyDirect(info.engine, ss_super,
			tRisseOperateFlags(tRisseMemberAttribute::GetDefault())|
			access_flags,
			super_class, info.This);

		// 親クラスの ClassRTTI を引き継ぐ
		tRisseClassBase * this_class_intf =
			info.This.CheckAndGetObjectInterafce<tRisseClassBase, tRisseClassBase>(
				info.engine->ClassClass);
		tRisseClassBase * super_class_intf =
			super_class.CheckAndGetObjectInterafce<tRisseClassBase, tRisseClassBase>(
				info.engine->ClassClass);

		this_class_intf->GetClassRTTI() = super_class_intf->GetClassRTTI();
		this_class_intf->GetClassRTTI().AddId(this_class_intf);
	}

	// ここで登録した ovulate と construct と initialize を削除する。
	// これらは、This の初期化には必要だったが、このさきこのクラスに実装されるであろう
	// ユーザ定義の ovulate, construct と initialize にとってはじゃまである。
	// これらがここに残っていると、親クラス内の ovulate, construct や initialize を正常に
	// 参照できないという意味でもじゃまである。
	info.This.DeletePropertyDirect_Object(ss_ovulate, tRisseOperateFlags::ofInstanceMemberOnly);
	info.This.DeletePropertyDirect_Object(ss_construct, tRisseOperateFlags::ofInstanceMemberOnly);
	info.This.DeletePropertyDirect_Object(ss_initialize, tRisseOperateFlags::ofInstanceMemberOnly);
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
tRisseClassInstance::tRisseClassInstance(tRisseScriptEngine * engine) :
	tRisseClassClass(engine)
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

} // namespace Risse
