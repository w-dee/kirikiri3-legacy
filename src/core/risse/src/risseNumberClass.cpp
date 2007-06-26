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
tRisseNumberClass::tRisseNumberClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->PrimitiveClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseNumberClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_ovulate,
		&tRisseNumberClass::ovulate,
		tRisseMemberAttribute(	tRisseMemberAttribute(tRisseMemberAttribute::vcConst)|
								tRisseMemberAttribute(tRisseMemberAttribute::ocFinal)) );
	RisseBindFunction(this, ss_construct,
		&tRisseNumberClass::construct,
		tRisseMemberAttribute(	tRisseMemberAttribute(tRisseMemberAttribute::vcConst)|
								tRisseMemberAttribute(tRisseMemberAttribute::ocFinal)) );
	RisseBindProperty(this, ss_isNaN, &tRisseNumberClass::isNaN,
		tRisseMemberAttribute(	tRisseMemberAttribute(tRisseMemberAttribute::vcConst)|
								tRisseMemberAttribute(tRisseMemberAttribute::ocFinal)) );
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseNumberClass::ovulate()
{
	// このクラスのインスタンスは作成できないので例外を投げる
	tRisseInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
	return tRisseVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseNumberClass::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseNumberClass::isNaN(const tRisseNativePropGetInfo & info)
{
	tRisseVariant num = info.This.Plus();

	bool result;
	switch(num.GetType())
	{
	case tRisseVariant::vtReal:
		result = (bool)RISSE_FC_IS_NAN(RisseGetFPClass(num.operator risse_real()));
		break;

	case tRisseVariant::vtInteger:
	default:
		// 整数の場合やそのほかの場合は偽を返す
		result = false;
	}
	if(info.result) *info.result = result;
}
//---------------------------------------------------------------------------


} /* namespace Risse */

