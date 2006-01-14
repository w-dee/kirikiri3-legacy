//---------------------------------------------------------------------------
/*
	Risse [�肹]
	alias RISE [りせ], acronym of "Rise Is a Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief C++ 例外処理とエラーメッセージ
//---------------------------------------------------------------------------
#include "risseCommHead.h"


#include "risseScriptBlock.h"
#include "risseError.h"
#include "risse.h"

#define RISSE_MAX_TRACE_TEXT_LEN 1500

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(1013);
//---------------------------------------------------------------------------
// RisseGetExceptionObject : retrieves risse 'Exception' object
//---------------------------------------------------------------------------
void RisseGetExceptionObject(tRisse *risse, tRisseVariant *res, tRisseVariant &msg,
	tRisseVariant *trace/* trace is optional */)
{
	if(!res) return; // not prcess

	// retrieve class "Exception" from global
	iRisseDispatch2 *global = risse->GetGlobalNoAddRef();
	tRisseVariant val;
	static tRisseString Exception_name(RISSE_WS("Exception"));
	risse_error hr = global->PropGet(0, Exception_name.c_str(),
		Exception_name.GetHint(), &val, global);
	if(RISSE_FAILED(hr)) Risse_eRisseError(RisseExceptionNotFound);
	// create an Exception object
	iRisseDispatch2 *excpobj;
	tRisseVariantClosure clo = val.AsObjectClosureNoAddRef();
	tRisseVariant *pmsg = &msg;
	hr = clo.CreateNew(0, NULL, NULL, &excpobj, 1, &pmsg, clo.ObjThis);
	if(RISSE_FAILED(hr)) Risse_eRisseError(RisseExceptionNotFound);
	if(trace)
	{
		static tRisseString trace_name(RISSE_WS("trace"));
		excpobj->PropSet(RISSE_MEMBERENSURE, trace_name.c_str(), trace_name.GetHint(),
			trace, excpobj);
	}
	*res = tRisseVariant(excpobj, excpobj);
	excpobj->Release();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// eRisseScriptException
//---------------------------------------------------------------------------
eRisseScriptError::tScriptBlockHolder::tScriptBlockHolder(tRisseScriptBlock *block)
{
	Block = block;
	Block->AddRef();
}
//---------------------------------------------------------------------------
eRisseScriptError::tScriptBlockHolder::~tScriptBlockHolder()
{
	Block->Release();
}
//---------------------------------------------------------------------------
eRisseScriptError::tScriptBlockHolder::tScriptBlockHolder(
		const tScriptBlockHolder &Holder)
{
	Block = Holder.Block;
	Block->AddRef();
}
//---------------------------------------------------------------------------
eRisseScriptError::eRisseScriptError(const ttstr &  Msg,
	tRisseScriptBlock *block, risse_int pos) :
		eRisseError(Msg), Block(block), Position(pos)
{
}
//---------------------------------------------------------------------------
risse_int eRisseScriptError::GetSourceLine() const
{
	return Block.Block->SrcPosToLine(Position) +1;
}
//---------------------------------------------------------------------------
const risse_char * eRisseScriptError::GetBlockName() const
{
	const risse_char * name = Block.Block->GetName() ;
	return name ? name : RISSE_WS("");
}
//---------------------------------------------------------------------------
bool eRisseScriptError::AddTrace(tRisseScriptBlock *block, risse_int srcpos)
{
	risse_int len = Trace.GetLen();
	if(len >= RISSE_MAX_TRACE_TEXT_LEN) return false;

	if(len != 0) Trace += RISSE_WS(" <-- ");
	Trace += block->GetLineDescriptionString(srcpos);

	return true;
}
//---------------------------------------------------------------------------
bool eRisseScriptError::AddTrace(tRisseInterCodeContext *context, risse_int codepos)
{
	risse_int len = Trace.GetLen();
	if(len >= RISSE_MAX_TRACE_TEXT_LEN) return false;

	if(len != 0) Trace += RISSE_WS(" <-- ");
	Trace += context->GetPositionDescriptionString(codepos);

	return true;
}
//---------------------------------------------------------------------------
bool eRisseScriptError::AddTrace(const ttstr & data)
{
	risse_int len = Trace.GetLen();
	if(len >= RISSE_MAX_TRACE_TEXT_LEN) return false;
	if(len != 0) Trace += RISSE_WS(" <-- ");
	Trace += data;
	return true;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// throw helper functions
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static void RisseReportExceptionSource(const ttstr &msg, tRisseScriptBlock *block,
	risse_int srcpos)
{
	tRisse *risse = block->GetRisse();
	risse->OutputExceptionToConsole((msg + RISSE_WS(" at ") +
		block->GetLineDescriptionString(srcpos)).c_str());
}
//---------------------------------------------------------------------------
static void RisseReportExceptionSource(const ttstr &msg,
	tRisseInterCodeContext *context, risse_int codepos)
{
	tRisse *risse = context->GetBlock()->GetRisse();
	risse->OutputExceptionToConsole((msg + RISSE_WS(" at ") +
		context->GetPositionDescriptionString(codepos)).c_str());
}
//---------------------------------------------------------------------------
void Risse_eRisse() { throw eRisse(); }
//---------------------------------------------------------------------------
void Risse_eRisseError(const ttstr & msg) { throw eRisseError(msg); }
void Risse_eRisseError(const risse_char* msg) { throw eRisseError(msg); }
//---------------------------------------------------------------------------
void Risse_eRisseVariantError(const ttstr & msg) { throw eRisseVariantError(msg); }
void Risse_eRisseVariantError(const risse_char * msg) { throw eRisseVariantError(msg); }
//---------------------------------------------------------------------------
void Risse_eRisseScriptError(const ttstr &msg, tRisseScriptBlock *block, risse_int srcpos)
{
	RisseReportExceptionSource(msg, block, srcpos);
	throw eRisseScriptError(msg, block, srcpos);
}
//---------------------------------------------------------------------------
void Risse_eRisseScriptError(const risse_char *msg, tRisseScriptBlock *block, risse_int srcpos)
{
	RisseReportExceptionSource(msg, block, srcpos);
	throw eRisseScriptError(msg, block, srcpos);
}
//---------------------------------------------------------------------------
void Risse_eRisseScriptError(const ttstr &msg, tRisseInterCodeContext *context, risse_int codepos)
{
	RisseReportExceptionSource(msg, context, codepos);
	throw eRisseScriptError(msg, context->GetBlock(), context->CodePosToSrcPos(codepos));
}
//---------------------------------------------------------------------------
void Risse_eRisseScriptError(const risse_char *msg, tRisseInterCodeContext *context, risse_int codepos)
{
	RisseReportExceptionSource(msg, context, codepos);
	throw eRisseScriptError(msg, context->GetBlock(), context->CodePosToSrcPos(codepos));
}
//---------------------------------------------------------------------------
void Risse_eRisseScriptException(const ttstr &msg, tRisseScriptBlock *block,
	risse_int srcpos, tRisseVariant &val)
{
	RisseReportExceptionSource(msg, block, srcpos);
	throw eRisseScriptException(msg, block, srcpos, val);
}
//---------------------------------------------------------------------------
void Risse_eRisseScriptException(const risse_char *msg, tRisseScriptBlock *block,
	risse_int srcpos, tRisseVariant &val)
{
	RisseReportExceptionSource(msg, block, srcpos);
	throw eRisseScriptException(msg, block, srcpos, val);
}
//---------------------------------------------------------------------------
void Risse_eRisseScriptException(const ttstr &msg, tRisseInterCodeContext *context,
	risse_int codepos, tRisseVariant &val)
{
	RisseReportExceptionSource(msg, context, codepos);
	throw eRisseScriptException(msg, context->GetBlock(), context->CodePosToSrcPos(codepos), val);
}
//---------------------------------------------------------------------------
void Risse_eRisseScriptException(const risse_char *msg, tRisseInterCodeContext *context,
	risse_int codepos, tRisseVariant &val)
{
	RisseReportExceptionSource(msg, context, codepos);
	throw eRisseScriptException(msg, context->GetBlock(), context->CodePosToSrcPos(codepos), val);
}
//---------------------------------------------------------------------------
void Risse_eRisseCompileError(const ttstr & msg, tRisseScriptBlock *block, risse_int srcpos)
{
	RisseReportExceptionSource(msg, block, srcpos);
	throw eRisseCompileError(msg, block, srcpos);
}
//---------------------------------------------------------------------------
void Risse_eRisseCompileError(const risse_char *msg, tRisseScriptBlock *block, risse_int srcpos)
{
	RisseReportExceptionSource(msg, block, srcpos);
	throw eRisseCompileError(msg, block, srcpos);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void RisseThrowFrom_risse_error(risse_error hr, const risse_char *name)
{
	// raise an exception descripted as risse_error
	// name = variable name ( otherwide it can be NULL )

	switch(hr)
	{
	case RISSE_E_MEMBERNOTFOUND:
	  {
		if(name)
		{
			ttstr str(RisseMemberNotFound);
			str.Replace(RISSE_WS("%1"), name);
			Risse_eRisseError(str);
		}
		else
		{
			Risse_eRisseError(RisseMemberNotFoundNoNameGiven);
		}
	  }
	case RISSE_E_NOTIMPL:
		Risse_eRisseError(RisseNotImplemented);
	case RISSE_E_INVALIDPARAM:
		Risse_eRisseError(RisseInvalidParam);
	case RISSE_E_BADPARAMCOUNT:
		Risse_eRisseError(RisseBadParamCount);
	case RISSE_E_INVALIDTYPE:
		Risse_eRisseError(RisseInvalidType);
	case RISSE_E_ACCESSDENYED:
		Risse_eRisseError(RisseAccessDenyed);
	case RISSE_E_INVALIDOBJECT:
		Risse_eRisseError(RisseInvalidObject);
	case RISSE_E_NATIVECLASSCRASH:
		Risse_eRisseError(RisseNativeClassCrash);
	default:
		if(RISSE_FAILED(hr))
		{
			ttstr p(RISSE_WS("Unknown failure : %08X"), RisseInt32ToHex(hr));
			Risse_eRisseError(p);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// error messages  ( can be localized )
//---------------------------------------------------------------------------
ttstr RisseNonamedException = RISSE_WS("No-named exception");
//---------------------------------------------------------------------------
} // namespace Risse


#define RISSE_DECL_MESSAGE_BODY
#undef risseErrorH
#include "risseError.h"
//---------------------------------------------------------------------------

