//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse 例外処理
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseVariant.h"
#include "risseException.h"
#include "risseScriptBlockBase.h"
#include "risseCharUtils.h"

#define RISSE_MAX_TRACE_TEXT_LEN 1500

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(31394,1220,40741,19179,55968,19738,1342,5195);


//---------------------------------------------------------------------------
const tRisseString & eRisse::GetMessageString() const
{
	static tRisseString nonamed_exception("No named exception");
	return nonamed_exception;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void eRisseError::Throw(const tRisseString & msg)
{
	// 例外メッセージを投げる
	throw eRisseError(msg);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString eRisseScriptError::BuildMessage(const tRisseString & msgbase,
		tRisseScriptBlockBase *block, risse_size pos)
{
	risse_size ln;
	if(block)
	{
		block->PositionToLineAndColumn(pos, &ln, NULL); // コードポイント位置->行
		risse_char tmp[40];
		Risse_int64_to_str((risse_int64)(ln+1), tmp); // 行は 0ベースなので +1 する
		return tRisseString(RISSE_WS_TR("%1 at %2 line %3"), msgbase, block->GetName(), tmp);
	}
	else
	{
		return msgbase;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void eRisseCompileError::Throw(const tRisseString & msg,
						tRisseScriptBlockBase *block, risse_size pos)
{
	// 例外メッセージを投げる
	throw eRisseCompileError(msg, block, pos);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
eRisseScriptException::eRisseScriptException(const tRisseString &  msg,
		tRisseScriptBlockBase *block, risse_size pos, const tRisseVariant & value) :
		eRisseScriptError(msg, block, pos), Value(new tRisseVariant(value))
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
eRisseScriptException::eRisseScriptException(const eRisseScriptException &ref) :
	eRisseScriptError(ref)
{
	Value = ref.Value;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void eRisseScriptException::Throw(const tRisseString & msg,
			tRisseScriptBlockBase *block, risse_size pos, const tRisseVariant & value)
{
	// 例外メッセージを投げる
	throw eRisseScriptException(msg, block, pos, value);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void RisseThrowCannotCallNonFunctionObjectException()
{
	eRisseError::Throw(RISSE_WS_TR("cannot call non-function object"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void RisseThrowCannotCreateInstanceFromNonClassObjectException()
{
	eRisseError::Throw(RISSE_WS_TR("cannot create instance from non-class object"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void RisseThrowBadArgumentCount(risse_int passed, risse_int expected)
{
	eRisseError::Throw(tRisseString(RISSE_WS_TR("invalid argument count (passed %1, expected %2)"),
		tRisseString::AsString(passed), tRisseString::AsString(expected)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void RisseThrowMemberNotFound(const tRisseString & name)
{
	if(!name.IsEmpty())
		eRisseError::Throw(tRisseString(RISSE_WS_TR("member \"%1\" not found"), name));
	else
		eRisseError::Throw(tRisseString(RISSE_WS_TR("member not found")));
}
//---------------------------------------------------------------------------











#if 0
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
eRisseScriptError::tScriptBlockHolder::tScriptBlockHolder(tRisseScriptBlockBase *block)
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
eRisseScriptError::eRisseScriptError(const tRisseString &  Msg,
	tRisseScriptBlockBase *block, risse_int pos) :
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
bool eRisseScriptError::AddTrace(tRisseScriptBlockBase *block, risse_int srcpos)
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
bool eRisseScriptError::AddTrace(const tRisseString & data)
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
static void RisseReportExceptionSource(const tRisseString &msg, tRisseScriptBlockBase *block,
	risse_int srcpos)
{
	tRisse *risse = block->GetRisse();
	risse->OutputExceptionToConsole((msg + RISSE_WS(" at ") +
		block->GetLineDescriptionString(srcpos)).c_str());
}
//---------------------------------------------------------------------------
static void RisseReportExceptionSource(const tRisseString &msg,
	tRisseInterCodeContext *context, risse_int codepos)
{
	tRisse *risse = context->GetBlock()->GetRisse();
	risse->OutputExceptionToConsole((msg + RISSE_WS(" at ") +
		context->GetPositionDescriptionString(codepos)).c_str());
}
//---------------------------------------------------------------------------
void Risse_eRisse() { throw eRisse(); }
//---------------------------------------------------------------------------
void Risse_eRisseError(const tRisseString & msg) { throw eRisseError(msg); }
void Risse_eRisseError(const risse_char* msg) { throw eRisseError(msg); }
//---------------------------------------------------------------------------
void Risse_eRisseVariantError(const tRisseString & msg) { throw eRisseVariantError(msg); }
void Risse_eRisseVariantError(const risse_char * msg) { throw eRisseVariantError(msg); }
//---------------------------------------------------------------------------
void Risse_eRisseScriptError(const tRisseString &msg, tRisseScriptBlockBase *block, risse_int srcpos)
{
	RisseReportExceptionSource(msg, block, srcpos);
	throw eRisseScriptError(msg, block, srcpos);
}
//---------------------------------------------------------------------------
void Risse_eRisseScriptError(const risse_char *msg, tRisseScriptBlockBase *block, risse_int srcpos)
{
	RisseReportExceptionSource(msg, block, srcpos);
	throw eRisseScriptError(msg, block, srcpos);
}
//---------------------------------------------------------------------------
void Risse_eRisseScriptError(const tRisseString &msg, tRisseInterCodeContext *context, risse_int codepos)
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
void Risse_eRisseScriptException(const tRisseString &msg, tRisseScriptBlockBase *block,
	risse_int srcpos, tRisseVariant &val)
{
	RisseReportExceptionSource(msg, block, srcpos);
	throw eRisseScriptException(msg, block, srcpos, val);
}
//---------------------------------------------------------------------------
void Risse_eRisseScriptException(const risse_char *msg, tRisseScriptBlockBase *block,
	risse_int srcpos, tRisseVariant &val)
{
	RisseReportExceptionSource(msg, block, srcpos);
	throw eRisseScriptException(msg, block, srcpos, val);
}
//---------------------------------------------------------------------------
void Risse_eRisseScriptException(const tRisseString &msg, tRisseInterCodeContext *context,
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
void Risse_eRisseCompileError(const tRisseString & msg, tRisseScriptBlockBase *block, risse_int srcpos)
{
	RisseReportExceptionSource(msg, block, srcpos);
	throw eRisseCompileError(msg, block, srcpos);
}
//---------------------------------------------------------------------------
void Risse_eRisseCompileError(const risse_char *msg, tRisseScriptBlockBase *block, risse_int srcpos)
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
			tRisseString str(RisseMemberNotFound);
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
			tRisseString p(RISSE_WS("Unknown failure : %08X"), RisseInt32ToHex(hr));
			Risse_eRisseError(p);
		}
	}
}
//---------------------------------------------------------------------------


#endif

} // namespace Risse
