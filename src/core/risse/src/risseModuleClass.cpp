//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief "Module" (モジュール) の実装
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseModuleClass.h"
#include "risseObjectClass.h"
#include "risseOpCodes.h"
#include "risseStaticStrings.h"
#include "risseScriptEngine.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(63047,20109,44050,17555,30336,10949,23175,16849);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tModuleClass::tModuleClass(tScriptEngine * engine) :
	tClassBase(ss_Module, engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tModuleClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tModuleClass::ovulate);
	BindFunction(this, ss_construct, &tModuleClass::construct);
	BindFunction(this, ss_initialize, &tModuleClass::initialize);

	// members を Object クラスのインスタンスとしてマークする。
	// ここは Object クラスが初期化される前に呼ばれる可能性があるため、
	// 本当に Object クラスが初期化されているかどうかをチェックする。
	// すべてのクラスが初期化された後にこのメソッドはもう一度呼ばれるので、
	// その際は Object クラスが利用できる。
	if(GetRTTI()->GetScriptEngine()->ObjectClass != NULL)
	{
		tObjectInterface * intf = ReadMember(ss_members).GetObjectInterface();
		GetRTTI()->GetScriptEngine()->ObjectClass->Bless(intf);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tModuleClass::ovulate(const tNativeCallInfo &info)
{
	tObjectBase * new_base_object = new tObjectBase();

	// new_base_object に members を登録する
	tVariant members = tModuleClass::CreateMembersObject(
					info.engine, tVariant::GetNullObject());
	new_base_object->RegisterNormalMember(ss_members, members);

	return tVariant((tObjectInterface*)new_base_object);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tModuleClass::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tModuleClass::initialize(const tNativeCallInfo &info)
{
	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();

	tVariant name;
	if(info.args.HasArgument(0)) name = info.args[0]; // もし引数が与えられていない場合はモジュールは匿名になる

	// name はクラス名
	// This に name という名前で値を登録し、書き込み禁止にする
	info.This.SetPropertyDirect(info.engine, ss_name,
		tMemberAttribute::GetDefault() |
		tOperateFlags::ofMemberEnsure|tOperateFlags::ofInstanceMemberOnly,
		name);
	(*const_cast<tVariant*>(&info.This)).
		Do(info.engine, ocDSetAttrib, NULL, ss_name,
			tMemberAttribute(tMemberAttribute::mcConst));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tModuleClass::CreateMembersObject(
			tScriptEngine * engine, const tVariant proto)
{
	tObjectBase * members = new tObjectBase(ss_prototype);
	members->SetRTTI(new tRTTI(engine));
	members->RegisterNormalMember(ss_prototype, proto);
	return tVariant((tObjectInterface*)members);
}
//---------------------------------------------------------------------------


} // namespace Risse
