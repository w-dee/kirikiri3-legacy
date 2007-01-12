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
#include "risseException.h"
#include "risseNativeFunction.h"
#include "risseObjectBase.h"
#include "risseVariant.h"
#include "risseException.h"
#include "risseOpCodes.h"

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
  | new メソッドは自分のクラスのinitializeメソッドを呼ぶ(this=新しいオブジェクト,再帰)
  | オブジェクトを返す

*/


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Class.new
//---------------------------------------------------------------------------
class tRisseNativeFunction_Class_new : public tRisseNativeFunctionBase
{
protected:
	//! @brief		Risseメソッド呼び出し時に呼ばれるメソッド
	void Call(
		tRisseVariant * result,
		tRisseOperateFlags flags,
		const tRisseMethodArgument & args,
		const tRisseMethodArgument & bargs,
		const tRisseVariant &This,
		const tRisseStackFrameContext &stack)
	{
		// 空のオブジェクトを作る
		// (以降のメソッド呼び出しはこのオブジェクトをthisにして呼ぶ)
		tRisseVariant new_object(new tObjectBase());

		// そのオブジェクトにクラス情報を設定する
		// yet not

		// new メソッドは自分のクラスのfertilizeメソッドを呼ぶ。
		// 「自分のクラス」はすなわち This のこと(のはず)
		// チェックはしないが。
		This.FuncCall(NULL, ss_fertilize, 0,
			tRisseMethodArgument::New(new_object),
			tRisseMethodArgument::Empty());

		// new メソッドは自分のクラスのinitializeメソッドを呼ぶ(再帰)
		This.FuncCall(NULL, ss_initialize, 0,
			args,
			bargs,
			new_object);

		// オブジェクトを返す
		if(ret) *ret = new_object;
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Class.fertilize
//---------------------------------------------------------------------------
class tRisseNativeFunction_Class_fertilize : public tRisseNativeFunctionBase
{
protected:
	//! @brief		Risseメソッド呼び出し時に呼ばれるメソッド
	void Call(
		tRisseVariant * result,
		tRisseOperateFlags flags,
		const tRisseMethodArgument & args,
		const tRisseMethodArgument & bargs,
		const tRisseVariant &This,
		const tRisseStackFrameContext &stack)
	{
		// 引数チェック
		if(args.GetCount() < 1) RisseThrowBadArgumentCount(args.GetCount(), 1);

		// 親クラスを得る
		tRisseVariant super_class = This.GetPropDirect(ss_super);

		// 親クラスがあれば...
		if(!super_class.IsNull())
		{
			// 親クラスの fertilize メソッドを再帰して呼ぶ
			super_class.FuncCall(NULL, ss_fertilize, 0,
				tRisseMethodArgument::New(args[0]),
				tRisseMethodArgument::Empty());
		}

		// 自分のクラスのmodules[]に登録されているモジュールのconstructメソッドを順番に呼ぶ
		// not yet

		// 自分のクラスのconstructメソッドを呼ぶ
		// この際の呼び出し先の this は args[0] つまり新しいオブジェクトになる
		This.FuncCall(NULL, ss_construct, 0,
			tRisseMethodArgument::Empty(),
			tRisseMethodArgument::Empty(),
			args[0]);
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRetValue tRisseClass::tRisseClass(const tRisseVariant & super_class)
{
	// クラスに必要なメソッドを登録する
	tRisseVariant This(this);

	// new や fertilize はクラス固有のメソッドなのでコンテキストとして
	// This (クラスそのもの)をあらかじめ設定する。

	// new
	This.SetPropDirect(mnNew, tRisseVariant(new tRisseNativeFunction_Class_new(), This));
	// fertilize
	This.SetPropDirect(ss_fertilize, tRisseVariant(new tRisseNativeFunction_Class_fertilize(), This));

	// super を登録
	This.SetPropDirect(ss_super, super_class);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRetValue tRisseClass::Operate(RISSE_OBJECTINTERFACE_OPERATE_DECL_ARG)
{
	if(code == ocNew && name.IsEmpty())
	{
		// このオブジェクトに対する new 指令
		// このオブジェクトの new メソッドを呼ぶ
		inherited::FuncCall(ret, mnNew, 0, args, bargs, This);
		return rvNoError;
	}
	return inherited::Operate(RISSE_OBJECTINTERFACE_PASS_ARG);
}
//---------------------------------------------------------------------------
} // namespace Risse
