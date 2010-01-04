/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

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
RISSE_IMPL_CLASS_BEGIN(tDataClass, ss_Data, engine->PrimitiveClass)
	BindFunction(this, ss_ovulate,
		&tDataClass::ovulate,
		tMemberAttribute(	tMemberAttribute(tMemberAttribute::mcConst)|
								tMemberAttribute(tMemberAttribute::ocFinal)) );
	BindFunction(this, ss_construct,
		&tDataClass::construct,
		tMemberAttribute(	tMemberAttribute(tMemberAttribute::mcConst)|
								tMemberAttribute(tMemberAttribute::ocFinal)) );
RISSE_IMPL_CLASS_END()

//---------------------------------------------------------------------------
void tDataClass::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


} /* namespace Risse */

