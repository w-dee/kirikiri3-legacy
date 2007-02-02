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
//! @brief		NativeFunction: Class.new
//---------------------------------------------------------------------------
static void Class_new(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
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
		// デフォルトのコンテキストをnew_object自身に設定する
		new_object.Do(ocSetDefaultContext, NULL, tRisseString::GetEmptyString(), 0,
					tRisseMethodArgument::New(new_object));
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
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Class.fertilize
//---------------------------------------------------------------------------
static void Class_fertilize(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	// 引数チェック
	if(args.GetArgumentCount() < 1) RisseThrowBadArgumentCount(args.GetArgumentCount(), 1);

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
	This.FuncCall(NULL, ss_construct, 0,
		tRisseMethodArgument::Empty(), args[0]);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseClassBase::tRisseClassBase(tRisseClassBase * super_class) : tRisseObjectBase(ss_super)
{
	// 親クラスのRTTIを引き継ぐ
	if(super_class) RTTI = super_class->RTTI;

	// RTTIに情報を格納する
	RTTIMatcher = RTTI.AddId(this);

	// クラスに必要なメソッドを登録する
	tRisseVariant * pThis = new tRisseVariant(this);

	// new や fertilize はクラス固有のメソッドなのでコンテキストとして
	// This (クラスそのもの)をあらかじめ設定する。

	// new
	RegisterNormalMember(mnNew, tRisseVariant(new tRisseNativeFunctionBase(Class_new), pThis));
	// fertilize
	RegisterNormalMember(ss_fertilize, tRisseVariant(new tRisseNativeFunctionBase(Class_fertilize), pThis));

	// super を登録
	RegisterNormalMember(ss_super, tRisseVariant(super_class));

	// modules を登録
	RegisterModulesArray();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseClassBase::tRetValue tRisseClassBase::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(code == ocCreateNewObjectBase && name.IsEmpty())
	{
		// 空のオブジェクトを作成して返す
		RISSE_ASSERT(result != NULL);
		tRisseVariant new_object = CreateNewObjectBase();
		if(new_object.GetType() == tRisseVariant::vtObject)
		{
			// プリミティブ型でなければ
			new_object.GetObjectInterface()->SetRTTI(&RTTI);
			// RTTIとしてこのクラスの物を設定する
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
void tRisseClassBase::RegisterModulesArray()
{
	// modules 配列を登録
	if(tRisseArrayClass::GetInstanceAlive())
	{
		// Arrayクラスの構築中にArrayクラスのシングルトンインスタンスを参照できないため
		// Arrayクラスがすでに構築されている場合だけ、modules 配列を登録する
		RegisterNormalMember(ss_modules, tRisseVariant(tRisseVariant(tRisseArrayClass::GetPointer()).New()));
	}
}
//---------------------------------------------------------------------------

} // namespace Risse
