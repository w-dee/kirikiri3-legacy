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
#include "risseNativeFunction.h"
#include "risseObjectBase.h"
#include "risseVariant.h"
#include "risseException.h"
#include "risseOpCodes.h"
#include "risseStaticStrings.h"
#include "risseArrayClass.h"
#include "risseClassClass.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(61181,65237,39210,16947,26767,23057,16328,36120);


/*
クラスからインスタンスが作られる手順:

クラスオブジェクトの new メソッドが呼ばれる
  newメソッド内
  | 空のオブジェクトを作る
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
tRisseClassBase::tRisseClassBase(tRisseClassBase * super_class) : tRisseObjectBase(ss_super)
{
	// 親クラスのRTTIを引き継ぐ
	if(super_class) RTTI = super_class->RTTI;

	// RTTIに情報を格納する
	RTTIMatcher = RTTI.AddId(this);

	// クラスに必要なメソッドを登録する
	RegisterMembers();

	// super を登録
	RegisterNormalMember(ss_super, tRisseVariant(super_class));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseClassBase::RegisterMembers()
{
	// 各メソッドを登録

	// new や fertilize はクラス固有のメソッドなのでコンテキストとして
	// This (クラスそのもの)をあらかじめ設定する。
	tRisseVariant * pThis = new tRisseVariant(this);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD_CONTEXT(mnNew, pThis)
	{
		// 空のオブジェクトを作る
		// (以降のメソッド呼び出しはこのオブジェクトをthisにして呼ぶ)
		// 「自分のクラス」はすなわち This のこと(のはず)
		RISSE_ASSERT(This.GetType() == tRisseVariant::vtObject);
		tRisseVariant new_object;
		This.GetObjectInterface()->Do(ocCreateNewObjectBase, &new_object);
		if(new_object.GetType() == tRisseVariant::vtObject)
		{
			// プリミティブ型ではない場合
			RISSE_ASSERT(!new_object.IsNull());

			// そのオブジェクトにクラス情報を設定する
			// ここではclassメンバに「自分のクラス」を追加する
			// 「自分のクラス」はすなわち This のこと(のはず)
			new_object.SetPropertyDirect(ss_class,
				tRisseOperateFlags(tRisseMemberAttribute(tRisseMemberAttribute::pcVar)) |
				tRisseOperateFlags::ofMemberEnsure|tRisseOperateFlags::ofInstanceMemberOnly,
				This, new_object);
			// yet not

			// new メソッドは自分のクラスのfertilizeメソッドを呼ぶ。
			// 「自分のクラス」はすなわち This のこと(のはず)
			This.FuncCall(NULL, ss_fertilize, 0,
				tRisseMethodArgument::New(new_object));
		}

		// new メソッドは新しいオブジェクトのinitializeメソッドを呼ぶ(再帰)
		new_object.FuncCall(NULL, ss_initialize, 0,
			args,
			new_object);

		// オブジェクトを返す
		if(result) *result = new_object;
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD_CONTEXT(ss_fertilize, pThis)
	{
		// 引数チェック
		args.ExpectArgumentCount(1);

		// 親クラスを得る
		tRisseVariant super_class = This.GetPropertyDirect(ss_super);

		// 親クラスがあれば...
		if(!super_class.IsNull())
		{
			// 親クラスの fertilize メソッドを再帰して呼ぶ
			super_class.FuncCall(NULL, ss_fertilize, 0,
				tRisseMethodArgument::New(args[0]));
		}

		// 自分のクラスのmodules[]に登録されているモジュールのconstructメソッドを順番に呼ぶ
		// not yet

		// 自分のクラスのconstructメソッドを呼ぶ
		// この際の呼び出し先の this は args[0] つまり新しいオブジェクトになる
		// 渡した args[0] を確実にコンテキストにして実行するために
		// tRisseOperateFlags::ofUseThisAsContext を用いる
		This.FuncCall(NULL, ss_construct, tRisseOperateFlags::ofUseThisAsContext,
			tRisseMethodArgument::Empty(), args[0]);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	// modules 配列を登録
	if(tRisseArrayClass::GetInstanceAlive())
	{
		// Arrayクラスの構築中にArrayクラスのシングルトンインスタンスを参照できないため
		// Arrayクラスがすでに構築されている場合だけ、modules 配列を登録する
		RegisterNormalMember(ss_modules, tRisseVariant(tRisseVariant(tRisseArrayClass::GetPointer()).New()));
	}

	// class を登録
	if(tRisseClassClass::GetInstanceAlive())
	{
		// クラスのクラスはClass
		// これもわざわざ tRisseClassClass のインスタンスが有効かどうかをチェックしている。
		// 理由は modules 配列と同じ。
		pThis->SetPropertyDirect(ss_class,
			tRisseOperateFlags(tRisseMemberAttribute(tRisseMemberAttribute::pcVar)) |
			tRisseOperateFlags::ofMemberEnsure|tRisseOperateFlags::ofInstanceMemberOnly,
			tRisseVariant(tRisseClassClass::GetPointer()), *pThis);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseClassBase::tRetValue tRisseClassBase::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(code == ocCreateNewObjectBase && name.IsEmpty())
	{
		// 空のオブジェクトを作成して返す
		// 注意! tRisseClassClass ではこの動作がオーバーライドされているので注意すること
		RISSE_ASSERT(result != NULL);
		tRisseVariant new_object = CreateNewObjectBase();
		if(new_object.GetType() == tRisseVariant::vtObject)
		{
			// プリミティブ型でなければ
			// RTTIとしてこのクラスの物を設定する
			new_object.GetObjectInterface()->SetRTTI(&RTTI);
		}
		*result = new_object;
		return rvNoError;
	}
	return inherited::Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseClassBase::CreateNewObjectBase()
{
	return tRisseVariant(new tRisseObjectBase());
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
	super.FuncCall(ret, name, flags|tRisseOperateFlags::ofUseThisAsContext, args, This);
}
//---------------------------------------------------------------------------



} // namespace Risse
