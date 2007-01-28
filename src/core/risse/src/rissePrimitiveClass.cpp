/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risseのプリミティブ型用クラス関連の処理の実装
//---------------------------------------------------------------------------

#include "risseTypes.h"
#include "rissePrimitiveClass.h"
#include "risseNativeFunction.h"
#include "risseNativeProperty.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(46147,47929,28573,19337,17834,3423,43968,61532);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: XXX.construct
//---------------------------------------------------------------------------
static void XXX_construct(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tRissePrimitiveClassBase::tRissePrimitiveClassBase(tRisseClassBase * super_class) :
	tRisseClassBase(super_class)
{
	// ゲートウェイインターフェースを構築する
	Gateway = new tRisseObjectBase();
	// そのオブジェクトにクラス情報を設定する
	// ここではclassメンバに「自分のクラス」を追加する
	Gateway.SetPropertyDirect(ss_class,
		tRisseOperateFlags(tRisseMemberAttribute(tRisseMemberAttribute::pcVar)) |
		tRisseOperateFlags::ofMemberEnsure|tRisseOperateFlags::ofInstanceMemberOnly,
		tRisseVariant(this), Gateway);

	// クラスに必要なメソッドを登録する

	// construct, initialize などは新しいオブジェクトのコンテキスト上で実行されるので
	// コンテキストとしては null を指定する

	// construct
	RegisterNormalMember(ss_construct, tRisseVariant(new tRisseNativeFunctionBase(XXX_construct)));
}
//---------------------------------------------------------------------------

















//---------------------------------------------------------------------------
//! @brief		NativeFunction: Primitive.construct
//---------------------------------------------------------------------------
static void Primitive_construct(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRissePrimitiveClass::tRissePrimitiveClass() :
	tRisseClassBase(tRisseObjectClass::GetPointer())
{
	// クラスに必要なメソッドを登録する

	// construct, initialize などは新しいオブジェクトのコンテキスト上で実行されるので
	// コンテキストとしては null を指定する

	// construct
	RegisterNormalMember(ss_construct, tRisseVariant(new tRisseNativeFunctionBase(Primitive_construct)));
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tRisseVariant tRissePrimitiveClass::CreateNewObjectBase()
{
	// このクラスのインスタンスは作成できないので例外を投げる
	RisseThrowCannotCreateInstanceFromThisClass();
	return tRisseVariant();
}
//---------------------------------------------------------------------------

} /* namespace Risse */

