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
#include "risseExceptionClass.h"


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(46147,47929,28573,19337,17834,3423,43968,61532);
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
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	// initialize は各C++サブクラスで実装すること。

	// construct, initialize などは新しいオブジェクトのコンテキスト上で実行されるので
	// コンテキストとしては null を指定する

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// デフォルトでは何もしない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------

















//---------------------------------------------------------------------------
tRissePrimitiveClass::tRissePrimitiveClass() :
	tRisseClassBase(tRisseObjectClass::GetPointer())
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRissePrimitiveClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// デフォルトでは何もしない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRissePrimitiveClass::CreateNewObjectBase()
{
	// このクラスのインスタンスは作成できないので例外を投げる
	tRisseInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
	return tRisseVariant();
}
//---------------------------------------------------------------------------

} /* namespace Risse */

