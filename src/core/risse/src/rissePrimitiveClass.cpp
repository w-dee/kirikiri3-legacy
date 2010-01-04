/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

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
#include "risseOpCodes.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(46147,47929,28573,19337,17834,3423,43968,61532);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tPrimitiveClassBase::tPrimitiveClassBase(const tString & name, tClassBase * super_class) :
	tClassBase(name, super_class)
{
	// ゲートウェイインターフェースを構築する
	tObjectBase * gateway_obj = new tObjectBase();
	gateway_obj->SetRTTI(new tRTTI(super_class->GetRTTI()->GetScriptEngine()));
	Gateway = gateway_obj;

	// そのオブジェクトにクラス情報を設定する
	// ここではclassメンバに「自分のクラス」を追加する
	Gateway.SetPropertyDirect_Object(ss_class,
		tOperateFlags(tMemberAttribute::GetDefault()) |
		tOperateFlags::ofMemberEnsure|tOperateFlags::ofInstanceMemberOnly,
		tVariant(this), Gateway);

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	// initialize は各C++サブクラスで実装すること。

	// construct, initialize などは新しいオブジェクトのコンテキスト上で実行されるので
	// コンテキストとしては null を指定する

	BindFunction(this, ss_construct,
		&tPrimitiveClassBase::construct,
		tMemberAttribute(	tMemberAttribute(tMemberAttribute::mcConst)|
							tMemberAttribute(tMemberAttribute::ocFinal)) );
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPrimitiveClassBase::construct()
{
	// Primitive な各クラスは final クラスであり、サブクラスを作ることはできない。
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------















//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tPrimitiveClass, ss_Primitive, engine->ObjectClass)
	BindFunction(this, ss_ovulate,
		&tPrimitiveClass::ovulate,
		tMemberAttribute(	tMemberAttribute(tMemberAttribute::mcConst)|
							tMemberAttribute(tMemberAttribute::ocFinal)) );
	BindFunction(this, ss_construct,
		&tPrimitiveClass::construct,
		tMemberAttribute(	tMemberAttribute(tMemberAttribute::mcConst)|
							tMemberAttribute(tMemberAttribute::ocFinal)) );
	BindFunction(this, mnString,
		&tPrimitiveClass::toString,
		tMemberAttribute(	tMemberAttribute(tMemberAttribute::mcConst)|
							tMemberAttribute(tMemberAttribute::ocFinal)) );
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPrimitiveClass::construct()
{
	// Primitive な各クラスは final クラスであり、サブクラスを作ることはできない。
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tPrimitiveClass::toString(const tNativeCallInfo & info)
{
	return info.This.operator tString();
}
//---------------------------------------------------------------------------

} /* namespace Risse */

