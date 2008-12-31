/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

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
RISSE_IMPL_CLASS_BEGIN(tNumberClass, ss_Number, engine->PrimitiveClass)
	BindFunction(this, ss_ovulate,
		&tNumberClass::ovulate,
		tMemberAttribute(	tMemberAttribute(tMemberAttribute::mcConst)|
								tMemberAttribute(tMemberAttribute::ocFinal)) );
	BindFunction(this, ss_construct,
		&tNumberClass::construct,
		tMemberAttribute(	tMemberAttribute(tMemberAttribute::mcConst)|
								tMemberAttribute(tMemberAttribute::ocFinal)) );
	BindProperty(this, ss_isNaN, &tNumberClass::isNaN,
		tMemberAttribute(	tMemberAttribute(tMemberAttribute::mcConst)|
								tMemberAttribute(tMemberAttribute::ocFinal)) );
RISSE_IMPL_CLASS_END()
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

