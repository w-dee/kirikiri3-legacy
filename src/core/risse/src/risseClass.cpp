//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief C++ 用 Risse (クラス) の基底クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseClass.h"
#include "risseObjectBase.h"
#include "risseVariant.h"
#include "risseOpCodes.h"
#include "risseStaticStrings.h"
#include "risseArrayClass.h"
#include "risseClassClass.h"
#include "risseScriptEngine.h"
#include "risseNativeBinder.h"
#include "risseModuleClass.h"
#include "risseObjectClass.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(61181,65237,39210,16947,26767,23057,16328,36120);


/*
クラスからインスタンスが作られる手順:

クラスオブジェクトの new メソッドが呼ばれる
  newメソッド内
  | ovulate メソッドが呼ばれる(空のオブジェクトを作る)
  | そのオブジェクトにクラス情報を設定する
  | new メソッドは自分のクラスのfertilizeメソッドを呼ぶ(this=自分のクラス)
  |   fertilize メソッド内
  |   |  親クラスのfertilize メソッドを呼ぶ(this=親クラス,再帰)
  |   |  自分のクラスのmodules[]に登録されているモジュールのconstructメソッドを順番に呼ぶ(this=新しいオブジェクト)
  |   |  自分のクラスのconstructメソッドを呼ぶ(this=新しいオブジェクト)
  | new メソッドは新しいオブジェクトのinitializeメソッドを呼ぶ(this=新しいオブジェクト,再帰)
  | オブジェクトを返す

*/
//---------------------------------------------------------------------------
tRisseClassBase::tRisseClassBase(tRisseClassBase * super_class, bool extensible)
	 : tRisseObjectBase(ss_super, ss_members)
{
	RISSE_ASSERT(super_class != NULL);

	// このインスタンスの RTTI に Class クラスの RTTI を設定する
	SetClassClassRTTI(super_class->GetRTTI()->GetScriptEngine());

	// 親クラスのClassRTTIを引き継ぐ
	ClassRTTI = super_class->ClassRTTI;

	// ClassRTTIに情報を格納する
	RTTIMatcher = ClassRTTI.AddId(this);

	// members に members 用オブジェクトを作成して登録する
	tRisseVariant members = tRisseModuleClass::CreateMembersObject(
					super_class->GetRTTI()->GetScriptEngine(), super_class->ReadMember(ss_members));
	RegisterNormalMember(ss_members, members);

	// クラスに必要なメソッドを登録する
	RegisterMembers();

	// super を登録
	RegisterNormalMember(ss_super, tRisseVariant(super_class));

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseClassBase::tRisseClassBase(tRisseScriptEngine * engine)
	 : tRisseObjectBase(ss_prototype, ss_members)
{
	// このインスタンスの RTTI に Class クラスの RTTI を設定する
	SetClassClassRTTI(engine);

	// ClassRTTIに情報を格納する
	ClassRTTI.SetScriptEngine(engine);
	RTTIMatcher = ClassRTTI.AddId(this);

	// members に members 用オブジェクトを作成して登録する
	tRisseVariant members = tRisseModuleClass::CreateMembersObject(
					engine, tRisseVariant((tRisseClassBase*)NULL));
	RegisterNormalMember(ss_members, members);

	// クラスに必要なメソッドを登録する
	RegisterMembers();

	// super を登録
	RegisterNormalMember(ss_super, tRisseVariant((tRisseClassBase*)NULL));

	// this の prototype に members を設定
	RegisterNormalMember(ss_prototype, members);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseClassBase::SetClassClassRTTI(tRisseScriptEngine * engine)
{
	if(engine->ClassClass)
	{
		// Class クラスの RTTI を設定する
		SetRTTI(&engine->ClassClass->ClassRTTI);
	}
	else
	{
		// Class クラスはまだ構築されていないよう。
		// スクリプトエンジンの情報を持った RTTI を登録する
		SetRTTI(new tRisseRTTI(engine));
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseClassBase::RegisterMembers()
{
	// 各メソッドを登録

	// new や fertilize はクラス固有のメソッドなのでコンテキストとして
	// This (クラスそのもの)をあらかじめ設定する。
	// また、これらは members ではなく、クラスインスタンスそのものに対して指定する。
	tRisseVariant * pThis = new tRisseVariant(this);

	RisseBindFunction(this, mnNew, &tRisseClassBase::risse_new,
				tRisseMemberAttribute(tRisseMemberAttribute::vcConst), pThis, false);
	RisseBindFunction(this, ss_fertilize, &tRisseClassBase::fertilize,
				tRisseMemberAttribute(tRisseMemberAttribute::vcConst), pThis, false);

	// modules 配列を members に登録
	if(GetRTTI()->GetScriptEngine()->ArrayClass)
	{
		// Arrayクラスの構築中にArrayクラスのシングルトンインスタンスを参照できないため
		// Arrayクラスがすでに構築されている場合だけ、modules 配列を登録する
		// modules は members 配列に登録する (RegisterNormalMember の最後の引数に注目)
		RegisterNormalMember(ss_modules,
			tRisseVariant(tRisseVariant(GetRTTI()->GetScriptEngine()->ArrayClass).New()),
			tRisseMemberAttribute(), true);
	}

	// class を登録
	if(GetRTTI()->GetScriptEngine()->ClassClass)
	{
		// クラスのクラスはClass
		// これもわざわざ tRisseClassClass のインスタンスが有効かどうかをチェックしている。
		// 理由は modules 配列と同じ。
		pThis->SetPropertyDirect(GetRTTI()->GetScriptEngine(), ss_class,
			tRisseOperateFlags(tRisseMemberAttribute::GetDefault()) |
			tRisseOperateFlags::ofMemberEnsure|tRisseOperateFlags::ofInstanceMemberOnly,
			tRisseVariant(GetRTTI()->GetScriptEngine()->ClassClass), *pThis);
	}

	// members を Object クラスのインスタンスとしてマークする。
	// ここは Object クラスが初期化される前に呼ばれる可能性があるため、
	// 本当に Object クラスが初期化されているかどうかをチェックする。
	// すべてのクラスが初期化された後にこのメソッドはもう一度呼ばれるので、
	// その際は Object クラスが利用できる。
	if(GetRTTI()->GetScriptEngine()->ObjectClass != NULL)
	{
		tRisseObjectInterface * intf = ReadMember(ss_members).GetObjectInterface();
		GetRTTI()->GetScriptEngine()->ObjectClass->Bless(intf);
	}

	RisseBindFunction(this, ss_include, &tRisseClassBase::include);
	RisseBindFunction(this, ss_ovulate, &tRisseClassBase::ovulate);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseClassBase::tRetValue tRisseClassBase::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	return inherited::Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseClassBase::CallSuperClassMethod(tRisseVariantBlock * ret,
	const tRisseString & name,risse_uint32 flags, 
	const tRisseMethodArgument & args, const tRisseVariant & This)
{
	// class.super を取り出す
	tRisseVariant super;
	Do(ocDGet, &super, ss_super, tRisseOperateFlags::ofInstanceMemberOnly, tRisseMethodArgument::Empty());

	// super の中のメソッドを呼ぶ
	super.FuncCall_Object(ret, name, flags|tRisseOperateFlags::ofUseClassMembersRule, args, This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseClassBase::risse_new(const tRisseNativeCallInfo &info)
{
	// 空のオブジェクトを作る(ovulateメソッドを呼び出す)
	// (以降のメソッド呼び出しはこのオブジェクトをthisにして呼ぶ)
	// 「自分のクラス」はすなわち This のこと(のはず)
	// またクラスの members 内の ovulate を呼ぶために
	// tRisseOperateFlags::ofUseClassMembersRule を使用する。
	RISSE_ASSERT(info.This.GetType() == tRisseVariant::vtObject);
	tRisseVariant new_object;
	info.This.FuncCall_Object(&new_object, ss_ovulate,
								tRisseOperateFlags::ofUseClassMembersRule);

	if(new_object.GetType() == tRisseVariant::vtObject)
	{
		// プリミティブ型ではない場合
		RISSE_ASSERT(!new_object.IsNull());

		// This はクラスのはずだよなぁ
		tRisseClassBase * class_intf =
			info.This.CheckAndGetObjectInterafce<tRisseClassBase, tRisseClassBase>(
				info.engine->ClassClass);

		// プリミティブ型でなければ
		// RTTIとしてこのクラスの物を設定する
		new_object.GetObjectInterface()->SetRTTI(&class_intf->ClassRTTI);

		// そのオブジェクトにクラス情報を設定する
		// ここではclassメンバに「自分のクラス」を追加する
		// 「自分のクラス」はすなわち This のこと(のはず)
		new_object.SetPropertyDirect(info.engine, ss_class,
			tRisseOperateFlags(tRisseMemberAttribute::GetDefault()) |
			tRisseOperateFlags::ofMemberEnsure|tRisseOperateFlags::ofInstanceMemberOnly,
			info.This, new_object);
		// yet not

		// new メソッドは自分のクラスのfertilizeメソッドを呼ぶ。
		// 「自分のクラス」はすなわち This のこと(のはず)
		info.This.FuncCall_Object(NULL, ss_fertilize, 0,
			tRisseMethodArgument::New(new_object));
	}

	// new メソッドは新しいオブジェクトのinitializeメソッドを呼ぶ(再帰)
	// またクラスの members 内の construct を呼ぶために
	// tRisseOperateFlags::ofUseClassMembersRule を使用する。
	new_object.FuncCall(info.engine, NULL, ss_initialize,
		tRisseOperateFlags::ofUseClassMembersRule,
		info.args,
		new_object);

	// オブジェクトを返す
	if(info.result) *info.result = new_object;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseClassBase::fertilize(const tRisseVariant & instance,
			const tRisseNativeCallInfo &info)
{
	// 親クラスを得る
	tRisseVariant super_class = info.This.GetPropertyDirect(info.engine, ss_super);

	// 親クラスがあれば...
	if(!super_class.IsNull())
	{
		// 親クラスの fertilize メソッドを再帰して呼ぶ
		super_class.FuncCall_Object(NULL, ss_fertilize, 0,
			tRisseMethodArgument::New(instance));
	}

	// 自分のクラスのmodules[]に登録されているモジュールのconstructメソッドを順番に呼ぶ
	// not yet

	// 自分のクラスのconstructメソッドを呼ぶ
	// この際の呼び出し先の this は instance つまり新しいオブジェクトになる
	// 渡した instance を確実にコンテキストにして実行するために
	// またクラスの members 内の construct を呼ぶために
	// tRisseOperateFlags::ofUseClassMembersRule を用いる
	info.This.FuncCall(info.engine, NULL, ss_construct,
		tRisseOperateFlags::ofUseClassMembersRule,
		tRisseMethodArgument::Empty(), instance);

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseClassBase::include(const tRisseMethodArgument & args,
					const tRisseNativeCallInfo &info)
{
	// クラスの modules 配列にモジュールを追加する

	// modules を取り出す
	// modules 配列の場合は members の中に modules 配列がある場合があるので
	// ofUseClassMembersRule フラグをつける
	tRisseVariant modules =
		info.This.GetPropertyDirect(info.engine, ss_modules,
			tRisseOperateFlags::ofInstanceMemberOnly|tRisseOperateFlags::ofUseClassMembersRule);

	// Array.unshift を行う
	modules.Do(info.engine, ocFuncCall, NULL, ss_unshift, 0, args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseClassBase::ovulate()
{
	return tRisseVariant(new tRisseObjectBase());
}
//---------------------------------------------------------------------------


} // namespace Risse
