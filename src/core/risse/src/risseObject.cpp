//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オブジェクトの C++ インターフェースの定義と実装
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseObject.h"
#include "risseException.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(53018,62403,2623,19559,39811,3052,55606,53445);
//---------------------------------------------------------------------------
tRisseIdentifyObject::tRetValue tRisseIdentifyObject::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	RisseThrowOperationIsNotImplemented();
	return rvNoError;
}
//---------------------------------------------------------------------------
} // namespace Risse
