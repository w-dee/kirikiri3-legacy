/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risseの数値型用クラス関連の処理の実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "risseNumberClass.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "rissePrimitiveClass.h"
#include "risseExceptionClass.h"
#include "risseScriptEngine.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(2098,51592,31991,16696,47274,13601,12452,21741);
//---------------------------------------------------------------------------
tNumberClass::tNumberClass(tScriptEngine * engine) :
	tClassBase(ss_Number, engine->PrimitiveClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNumberClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate,
		&tNumberClass::ovulate,
		tMemberAttribute(	tMemberAttribute(tMemberAttribute::vcConst)|
								tMemberAttribute(tMemberAttribute::ocFinal)) );
	BindFunction(this, ss_construct,
		&tNumberClass::construct,
		tMemberAttribute(	tMemberAttribute(tMemberAttribute::vcConst)|
								tMemberAttribute(tMemberAttribute::ocFinal)) );
	BindProperty(this, ss_isNaN, &tNumberClass::isNaN,
		tMemberAttribute(	tMemberAttribute(tMemberAttribute::vcConst)|
								tMemberAttribute(tMemberAttribute::ocFinal)) );
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tNumberClass::ovulate()
{
	// このクラスのインスタンスは作成できないので例外を投げる
	tInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNumberClass::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNumberClass::isNaN(const tNativePropGetInfo & info)
{
	tVariant num = info.This.Plus();

	bool result;
	switch(num.GetType())
	{
	case tVariant::vtReal:
		result = (bool)RISSE_FC_IS_NAN(GetFPClass(num.operator risse_real()));
		break;

	case tVariant::vtInteger:
	default:
		// 整数の場合やそのほかの場合は偽を返す
		result = false;
	}
	if(info.result) *info.result = result;
}
//---------------------------------------------------------------------------


} /* namespace Risse */

