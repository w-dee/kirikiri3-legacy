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
#include "prec.h"
#include "risseTypes.h"
#include "rissePrimitiveClass.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "risseExceptionClass.h"
#include "risseScriptEngine.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(46147,47929,28573,19337,17834,3423,43968,61532);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRissePrimitiveClassBase::tRissePrimitiveClassBase(const tRisseString & name, tRisseClassBase * super_class) :
	tRisseClassBase(name, super_class)
{
	// ゲートウェイインターフェースを構築する
	tRisseObjectBase * gateway_obj = new tRisseObjectBase();
	gateway_obj->SetRTTI(new tRisseRTTI(super_class->GetRTTI()->GetScriptEngine()));
	Gateway = gateway_obj;

	// そのオブジェクトにクラス情報を設定する
	// ここではclassメンバに「自分のクラス」を追加する
	Gateway.SetPropertyDirect_Object(ss_class,
		tRisseOperateFlags(tRisseMemberAttribute::GetDefault()) |
		tRisseOperateFlags::ofMemberEnsure|tRisseOperateFlags::ofInstanceMemberOnly,
		tRisseVariant(this), Gateway);

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	// initialize は各C++サブクラスで実装すること。

	// construct, initialize などは新しいオブジェクトのコンテキスト上で実行されるので
	// コンテキストとしては null を指定する

	RisseBindFunction(this, ss_construct,
		&tRissePrimitiveClassBase::construct,
		tRisseMemberAttribute(	tRisseMemberAttribute(tRisseMemberAttribute::vcConst)|
								tRisseMemberAttribute(tRisseMemberAttribute::ocFinal)) );
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRissePrimitiveClassBase::construct()
{
	// Primitive な各クラスは final クラスであり、サブクラスを作ることはできない。
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------















//---------------------------------------------------------------------------
tRissePrimitiveClass::tRissePrimitiveClass(tRisseScriptEngine * engine) :
	tRisseClassBase(ss_Primitive, engine->ObjectClass)
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

	RisseBindFunction(this, ss_ovulate,
		&tRissePrimitiveClass::ovulate,
		tRisseMemberAttribute(	tRisseMemberAttribute(tRisseMemberAttribute::vcConst)|
								tRisseMemberAttribute(tRisseMemberAttribute::ocFinal)) );
	RisseBindFunction(this, ss_construct,
		&tRissePrimitiveClass::construct,
		tRisseMemberAttribute(	tRisseMemberAttribute(tRisseMemberAttribute::vcConst)|
								tRisseMemberAttribute(tRisseMemberAttribute::ocFinal)) );
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRissePrimitiveClass::ovulate()
{
	// このクラスのインスタンスは作成できないので例外を投げる
	tRisseInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
	return tRisseVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRissePrimitiveClass::construct()
{
	// Primitive な各クラスは final クラスであり、サブクラスを作ることはできない。
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------

} /* namespace Risse */

