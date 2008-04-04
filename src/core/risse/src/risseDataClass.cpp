/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RisseのDataクラス関連の処理の実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "risseDataClass.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "rissePrimitiveClass.h"
#include "risseExceptionClass.h"
#include "risseScriptEngine.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(34010,6722,37400,20210,25254,51796,49710,12325);
//---------------------------------------------------------------------------
tDataClass::tDataClass(tScriptEngine * engine) :
	tClassBase(ss_Data, engine->PrimitiveClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tDataClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate,
		&tDataClass::ovulate,
		tMemberAttribute(	tMemberAttribute(tMemberAttribute::mcConst)|
								tMemberAttribute(tMemberAttribute::ocFinal)) );
	BindFunction(this, ss_construct,
		&tDataClass::construct,
		tMemberAttribute(	tMemberAttribute(tMemberAttribute::mcConst)|
								tMemberAttribute(tMemberAttribute::ocFinal)) );
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tDataClass::ovulate()
{
	// このクラスのインスタンスは作成できないので例外を投げる
	tInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tDataClass::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


} /* namespace Risse */

