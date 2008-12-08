//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
#include "risseExceptionClass.h"

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
tClassBase::tClassBase(const tString & name, tClassBase * super_class, bool extensible)
	 : tObjectBase(ss_super, ss_members)
{
	RISSE_ASSERT(super_class != NULL);

	// このインスタンスの RTTI に Class クラスの RTTI を設定する
	SetClassClassRTTI(super_class->GetRTTI()->GetScriptEngine());

	// 親クラスのClassRTTIを引き継ぐ
	ClassRTTI = super_class->ClassRTTI;

	// ClassRTTIに情報を格納する
	RTTIMatcher = ClassRTTI.AddId(this);

	// members に members 用オブジェクトを作成して登録する
	tVariant members = tModuleClass::CreateMembersObject(
					super_class->GetRTTI()->GetScriptEngine(), super_class->ReadMember(ss_members));
	RegisterNormalMember(ss_members, members);

	// クラスに必要なメソッドを登録する
	RegisterMembers();

	// super を登録
	RegisterNormalMember(ss_super, tVariant(super_class));

	// name はクラス名
	// This に name という名前で値を登録し、書き込み禁止にする
	RegisterNormalMember(ss_name, name, tMemberAttribute(tMemberAttribute::mcConst));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tClassBase::tClassBase(tScriptEngine * engine)
	 : tObjectBase(ss_prototype, ss_members)
{
	// このインスタンスの RTTI に Class クラスの RTTI を設定する
	SetClassClassRTTI(engine);

	// ClassRTTIに情報を格納する
	ClassRTTI.SetScriptEngine(engine);
	RTTIMatcher = ClassRTTI.AddId(this);

	// members に members 用オブジェクトを作成して登録する
	tVariant members = tModuleClass::CreateMembersObject(
					engine, tVariant::GetNullObject());
	RegisterNormalMember(ss_members, members);

	// クラスに必要なメソッドを登録する
	RegisterMembers();

	// super を登録
	RegisterNormalMember(ss_super, tVariant::GetNullObject());

	// this の prototype に members を設定
	RegisterNormalMember(ss_prototype, members);

	// クラス名は Object
	// This に name という名前で値を登録し、書き込み禁止にする
	RegisterNormalMember(ss_name, ss_Object, tMemberAttribute(tMemberAttribute::mcConst));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tClassBase::SetClassClassRTTI(tScriptEngine * engine)
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
		SetRTTI(new tRTTI(engine));
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tClassBase::RegisterInstance(const tVariant & target)
{
	tString name = GetPropertyDirect(ss_name);
	target.SetPropertyDirect_Object(name,
		tOperateFlags(tMemberAttribute::GetDefault()) |
		tOperateFlags::ofMemberEnsure |
		tOperateFlags::ofInstanceMemberOnly |
		tOperateFlags::ofUseClassMembersRule,
				tVariant(this));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tClassBase::RegisterMembers()
{
	// 各メソッドを登録

	// new や fertilize はクラス固有のメソッドなのでコンテキストとして
	// This (クラスそのもの)をあらかじめ設定する。
	// また、これらは members ではなく、クラスインスタンスそのものに対して指定する。
	tVariant * pThis = new tVariant(this);

	BindFunction(this, mnNew, &tClassBase::risse_new,
				tMemberAttribute(tMemberAttribute::mcConst), pThis, false);
	BindFunction(this, ss_fertilize, &tClassBase::fertilize,
				tMemberAttribute(tMemberAttribute::mcConst), pThis, false);

	// modules 配列を members に登録
	if(GetRTTI()->GetScriptEngine()->ArrayClass)
	{
		// Arrayクラスの構築中にArrayクラスのシングルトンインスタンスを参照できないため
		// Arrayクラスがすでに構築されている場合だけ、modules 配列を登録する
		// modules は members 配列に登録する (RegisterNormalMember の最後の引数に注目)
		RegisterNormalMember(ss_modules,
			tVariant(tVariant(GetRTTI()->GetScriptEngine()->ArrayClass).New()),
			tMemberAttribute(), true);
	}

	// class を登録
	if(GetRTTI()->GetScriptEngine()->ClassClass)
	{
		// クラスのクラスはClass
		// これもわざわざ tClassClass のインスタンスが有効かどうかをチェックしている。
		// 理由は modules 配列と同じ。
		// class は internal として登録する
		pThis->SetPropertyDirect(GetRTTI()->GetScriptEngine(), ss_class,
			tOperateFlags(tMemberAttribute::GetDefault()) |
			tOperateFlags::ofMemberEnsure|tOperateFlags::ofInstanceMemberOnly,
			tVariant(GetRTTI()->GetScriptEngine()->ClassClass), *pThis);
		pThis->SetAttributeDirect(GetRTTI()->GetScriptEngine(),
				ss_class, tMemberAttribute(tMemberAttribute::acInternal));
	}

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

	BindFunction(this, ss_include, &tClassBase::include);
	BindFunction(this, ss_ovulate, &tClassBase::ovulate);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tClassBase::tRetValue tClassBase::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	return inherited::Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tClassBase::CallSuperClassMethod(tVariantBlock * ret,
	const tString & name,risse_uint32 flags,
	const tMethodArgument & args, const tVariant & This)
{
	// class.super を取り出す
	tVariant super;
	Do(ocDGet, &super, ss_super, tOperateFlags::ofInstanceMemberOnly, tMethodArgument::Empty());

	// super の中のメソッドを呼ぶ
	super.FuncCall_Object(ret, name, flags|tOperateFlags::ofUseClassMembersRule, args, This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tClassBase::ThrowCannotCreateInstanceFromThisClass()
{
	tInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tClassBase::risse_new(const tNativeCallInfo &info)
{
	// 空のオブジェクトを作る(ovulateメソッドを呼び出す)
	// (以降のメソッド呼び出しはこのオブジェクトをthisにして呼ぶ)
	// 「自分のクラス」はすなわち This のこと(のはず)
	// またクラスの members 内の ovulate を呼ぶために
	// tOperateFlags::ofUseClassMembersRule を使用する。
	RISSE_ASSERT(info.This.GetType() == tVariant::vtObject);
	tVariant new_object;
	info.This.FuncCall_Object(&new_object, ss_ovulate,
								tOperateFlags::ofUseClassMembersRule);

	if(new_object.GetType() == tVariant::vtObject)
	{
		// プリミティブ型ではない場合
		RISSE_ASSERT(!new_object.IsNull());

		// This はクラスのはずだよなぁ
		tClassBase * class_intf =
			info.This.AssertAndGetObjectInterafce<tClassBase>(
				info.engine->ClassClass);

		// プリミティブ型でなければ
		// RTTIとしてこのクラスの物を設定する
		new_object.GetObjectInterface()->SetRTTI(&class_intf->ClassRTTI);

		// そのオブジェクトにクラス情報を設定する
		// ここではclassメンバに「自分のクラス」を追加する
		// 「自分のクラス」はすなわち This のこと(のはず)
		new_object.SetPropertyDirect(info.engine, ss_class,
			tOperateFlags(tMemberAttribute::GetDefault()) |
			tOperateFlags::ofMemberEnsure|tOperateFlags::ofInstanceMemberOnly,
			info.This, new_object);
		new_object.SetAttributeDirect(info.engine,
			ss_class, tMemberAttribute(tMemberAttribute::acInternal));
		// yet not

		// new メソッドは自分のクラスのfertilizeメソッドを呼ぶ。
		// 「自分のクラス」はすなわち This のこと(のはず)
		info.This.FuncCall_Object(NULL, ss_fertilize, 0,
			tMethodArgument::New(new_object));
	}

	// new メソッドは新しいオブジェクトのinitializeメソッドを呼ぶ(再帰)
	// またクラスの members 内の construct を呼ぶために
	// tOperateFlags::ofUseClassMembersRule を使用する。
	new_object.FuncCall(info.engine, NULL, ss_initialize,
		tOperateFlags::ofUseClassMembersRule,
		info.args,
		new_object);

	// オブジェクトを返す
	if(info.result) *info.result = new_object;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tClassBase::fertilize(const tVariant & instance,
			const tNativeCallInfo &info)
{
	// 親クラスを得る
	tVariant super_class = info.This.GetPropertyDirect(info.engine, ss_super);

	// 親クラスがあれば...
	if(!super_class.IsNull())
	{
		// 親クラスの fertilize メソッドを再帰して呼ぶ
		super_class.FuncCall_Object(NULL, ss_fertilize, 0,
			tMethodArgument::New(instance));
	}

	// 自分のクラスのmodules[]に登録されているモジュールのconstructメソッドを順番に呼ぶ
	// not yet

	// 自分のクラスのconstructメソッドを呼ぶ
	// この際の呼び出し先の this は instance つまり新しいオブジェクトになる
	// 渡した instance を確実にコンテキストにして実行するために
	// またクラスの members 内の construct を呼ぶために
	// tOperateFlags::ofUseClassMembersRule を用いる
	info.This.FuncCall(info.engine, NULL, ss_construct,
		tOperateFlags::ofUseClassMembersRule,
		tMethodArgument::Empty(), instance);

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tClassBase::include(const tMethodArgument & args,
					const tNativeCallInfo &info)
{
	// クラスの modules 配列にモジュールを追加する

	// modules を取り出す
	// modules 配列の場合は members の中に modules 配列がある場合があるので
	// ofUseClassMembersRule フラグをつける
	tVariant modules =
		info.This.GetPropertyDirect(info.engine, ss_modules,
			tOperateFlags::ofInstanceMemberOnly|tOperateFlags::ofUseClassMembersRule);

	// Array.unshift を行う
	modules.Do(info.engine, ocFuncCall, NULL, ss_unshift, 0, args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tClassBase::ovulate()
{
	return tVariant(new tObjectBase());
}
//---------------------------------------------------------------------------


} // namespace Risse
