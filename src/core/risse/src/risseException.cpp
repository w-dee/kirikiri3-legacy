//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief Exception クラス実装
//---------------------------------------------------------------------------
#include "risseCommHead.h"

#include "risseException.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(64751,19385,23679,19326,53436,10541,48090,40149);
//---------------------------------------------------------------------------
// tRisseNC_Exception : risse Native Class : Exception
//---------------------------------------------------------------------------
risse_uint32 tRisseNC_Exception::ClassID = (risse_uint32)-1;
tRisseNC_Exception::tRisseNC_Exception() :
	tRisseNativeClass(RISSE_WS("Exception"))
{
	// class constructor

	RISSE_BEGIN_NATIVE_MEMBERS(/*Risse class name*/Exception)
	RISSE_DECL_EMPTY_FINALIZE_METHOD
//---------------------------------------------------------------------------
RISSE_BEGIN_NATIVE_CONSTRUCTOR_DECL_NO_INSTANCE(/*Risse class name*/Exception)
{
	tRisseVariant val = RISSE_WS("");
	if(RISSE_PARAM_EXIST(0)) val.CopyRef(*param[0]);

	static tRisseString message_name(RISSE_WS("message"));
	objthis->PropSet(RISSE_MEMBERENSURE, message_name.c_str(), message_name.GetHint(),
		&val, objthis);

	if(RISSE_PARAM_EXIST(1)) val.CopyRef(*param[1]); else val = RISSE_WS("");

	static tRisseString trace_name(RISSE_WS("trace"));
	objthis->PropSet(RISSE_MEMBERENSURE, trace_name.c_str(), trace_name.GetHint(),
		&val, objthis);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_CONSTRUCTOR_DECL(/*Risse class name*/Exception)
//---------------------------------------------------------------------------
	RISSE_END_NATIVE_MEMBERS
} // tRisseNC_Exception::tRisseNC_Exception()
//---------------------------------------------------------------------------
} // namespace Risse

