//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief tRisseVariantやtRisseObjectInterfaceのOperateメソッドの戻り値処理
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseObject.h"
#include "risseException.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(52663,54398,40985,18806,48826,65290,5179,62065);
//---------------------------------------------------------------------------
void tRisseOperateRetValue::RaiseError(tRetValue ret, const tRisseString & name)
{
	switch(ret)
	{
	case rvMemberNotFound:
		// "メンバが見つからない"エラー
		RisseThrowMemberNotFound(name);
		break;

	default:
	case rvNoError:
		break;

	}
}
//---------------------------------------------------------------------------
}

