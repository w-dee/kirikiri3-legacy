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
tClassClass::tClassClass(tScriptEngine * engine) :
	tClassBase(ss_Class, engine->ModuleClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tClassClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tClassClass::ovulate);
	BindFunction(this, ss_construct, &tClassClass::construct);
	BindFunction(this, ss_initialize, &tClassClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tClassClass::tRetValue tClassClass::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	return inherited::Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tClassClass::ovulate(const tNativeCallInfo &info)
{
	return tVariant(new tClassInstance(info.engine));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tClassClass::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tClassClass::initialize(const tNativeCallInfo &info)
{
	// 親クラスの同名メソッドを呼び出す
	// 引数は  { 親クラス, 名前 }
	if(info.args.HasArgument(1))
	{
		// 名前を渡す
		info.InitializeSuperClass(tMethodArgument::New(info.args[1]));
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
		tVariant super_class = info.args[0];
		if(!super_class.InstanceOf(info.engine, tVariant(info.engine->ClassClass)))
			tClassDefinitionExceptionClass::ThrowSuperClassIsNotAClass();

		// super を登録
		tOperateFlags access_flags =
			tOperateFlags::ofMemberEnsure|tOperateFlags::ofInstanceMemberOnly;
		info.This.SetPropertyDirect(info.engine, ss_super,
			tOperateFlags(tMemberAttribute::GetDefault())|
			access_flags,
			super_class, info.This);

		// members の prototype に super.members を指定
		tVariant members = info.This.GetPropertyDirect(info.engine, ss_members,
											tOperateFlags::ofInstanceMemberOnly);
		tVariant super_members = super_class.GetPropertyDirect(info.engine, ss_members,
											tOperateFlags::ofInstanceMemberOnly);
		members.SetPropertyDirect(info.engine, ss_prototype, 
			tOperateFlags(tMemberAttribute::GetDefault())|
			access_flags,
			super_members, info.This);

		// 親クラスの ClassRTTI を引き継ぐ
		tClassBase * this_class_intf =
			info.This.CheckAndGetObjectInterafce<tClassBase, tClassBase>(
				info.engine->ClassClass);
		tClassBase * super_class_intf =
			super_class.CheckAndGetObjectInterafce<tClassBase, tClassBase>(
				info.engine->ClassClass);

		this_class_intf->GetClassRTTI() = super_class_intf->GetClassRTTI();
		this_class_intf->GetClassRTTI().AddId(this_class_intf);
	}

	// ここで登録した ovulate と construct と initialize を削除する。
	// これらは、This の初期化には必要だったが、このさきこのクラスに実装されるであろう
	// ユーザ定義の ovulate, construct と initialize にとってはじゃまである。
	// これらがここに残っていると、親クラス内の ovulate, construct や initialize を正常に
	// 参照できないという意味でもじゃまである。
	info.This.DeletePropertyDirect_Object(ss_ovulate,
				tOperateFlags::ofInstanceMemberOnly|tOperateFlags::ofUseClassMembersRule);
	info.This.DeletePropertyDirect_Object(ss_construct,
				tOperateFlags::ofInstanceMemberOnly|tOperateFlags::ofUseClassMembersRule);
	info.This.DeletePropertyDirect_Object(ss_initialize,
				tOperateFlags::ofInstanceMemberOnly|tOperateFlags::ofUseClassMembersRule);
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
tClassInstance::tClassInstance(tScriptEngine * engine) :
	tClassClass(engine)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tClassInstance::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tClassInstance::tRetValue tClassInstance::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	return inherited::Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
}
//---------------------------------------------------------------------------

} // namespace Risse
