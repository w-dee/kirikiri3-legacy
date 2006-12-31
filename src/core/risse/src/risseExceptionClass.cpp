//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse 例外クラス
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseVariant.h"
#include "risseException.h"
#include "risseExceptionClass.h"


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(64113,30630,41963,17808,15295,58919,39993,4429);


//---------------------------------------------------------------------------
tRisseExitTryExceptionClass::tRisseExitTryExceptionClass(
	const void * id, risse_uint32 targ_idx, const tRisseVariant * value)
{
	Identifier = id;
	BranchTargetIndex = targ_idx;
	Value = value ? new tRisseVariant(*value) : NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseObjectInterface::tRetValue tRisseExitTryExceptionClass::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	// 暫定実装
	if(code == ocDGet && name == RISSE_WS("getExitTryRecord"))
	{
		// tRisseExitTryExceptionClass 型を返す
		if(result)
		{
			*result = reinterpret_cast<tRisseObjectInterface*>
				((tRisseExitTryExceptionClass*)this);
		}
	}
	return rvNoError;
}
//---------------------------------------------------------------------------


} // namespace Risse
