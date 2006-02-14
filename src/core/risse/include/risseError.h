//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief C++ 例外処理とエラーメッセージ
//---------------------------------------------------------------------------


#ifndef risseErrorH
#define risseErrorH

#ifndef RISSE_DECL_MESSAGE_BODY

#include <stdexcept>
#include <string>
#include "risse.h"
#include "risseVariant.h"
#include "risseString.h"
#include "risseMessage.h"

namespace Risse
{
//---------------------------------------------------------------------------
extern ttstr RisseNonamedException;

//---------------------------------------------------------------------------
// macro
//---------------------------------------------------------------------------
#ifdef RISSE_SUPPORT_VCL
	#define RISSE_CONVERT_TO_RISSE_EXCEPTION_ADDITIONAL \
		catch(const Exception &e) \
		{ \
			Risse_eRisseError(e.Message.c_str()); \
		}
#else
	#define RISSE_CONVERT_TO_RISSE_EXCEPTION_ADDITIONAL
#endif


#define RISSE_CONVERT_TO_RISSE_EXCEPTION \
	catch(const eRisseSilent &e) \
	{ \
		throw e; \
	} \
	catch(const eRisseScriptException &e) \
	{ \
		throw e; \
	} \
	catch(const eRisseScriptError &e) \
	{ \
		throw e; \
	} \
	catch(const eRisseError &e) \
	{ \
		throw e; \
	} \
	catch(const eRisse &e) \
	{ \
		Risse_eRisseError(e.GetMessageString()); \
	} \
	catch(const std::exception &e) \
	{ \
		Risse_eRisseError(e.what()); \
	} \
	catch(const wchar_t *text) \
	{ \
		Risse_eRisseError(text); \
	} \
	catch(const char *text) \
	{ \
		Risse_eRisseError(text); \
	} \
	RISSE_CONVERT_TO_RISSE_EXCEPTION_ADDITIONAL
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// RisseGetExceptionObject : retrieves risse 'Exception' object
//---------------------------------------------------------------------------
extern void RisseGetExceptionObject(tRisse *risse, tRisseVariant *res, tRisseVariant &msg,
	tRisseVariant *trace/* trace is optional */ = NULL);
//---------------------------------------------------------------------------

#ifdef RISSE_SUPPORT_VCL
	#define RISSE_CONVERT_TO_RISSE_EXCEPTION_OBJECT_ADDITIONAL(_risse, _result_condition, _result_addr, _before_catched, _when_catched) \
	catch(EAccessViolation &e) \
	{ \
		_before_catched; \
		if(_result_condition) \
		{ \
			tRisseVariant msg(e.Message.c_str()); \
			RisseGetExceptionObject((_risse), (_result_addr), msg, NULL); \
		} \
		_when_catched; \
	} \
	catch(Exception &e) \
	{ \
		_before_catched; \
		if(_result_condition) \
		{ \
			tRisseVariant msg(e.Message.c_str()); \
			RisseGetExceptionObject((_risse), (_result_addr), msg, NULL); \
		} \
		_when_catched; \
	}
#else
	#define RISSE_CONVERT_TO_RISSE_EXCEPTION_OBJECT_ADDITIONAL(_risse, _result_condition, _result_addr, _before_catched, _when_catched)
#endif


#define RISSE_CONVERT_TO_RISSE_EXCEPTION_OBJECT(risse, result_condition, result_addr, before_catched, when_catched) \
	catch(eRisseSilent &e) \
	{ \
		throw e; \
	} \
	catch(eRisseScriptException &e) \
	{ \
		before_catched \
		if(result_condition) *(result_addr) = e.GetValue(); \
		when_catched; \
	} \
	catch(eRisseScriptError &e) \
	{ \
		before_catched \
		if(result_condition) \
		{ \
			tRisseVariant msg(e.GetMessageString()); \
			tRisseVariant trace(e.GetTrace()); \
			RisseGetExceptionObject((risse), (result_addr), msg, &trace); \
		} \
		when_catched; \
	} \
	catch(eRisse &e)  \
	{  \
		before_catched \
		if(result_condition) \
		{ \
			tRisseVariant msg(e.GetMessageString()); \
			RisseGetExceptionObject((risse), (result_addr), msg, NULL); \
		} \
		when_catched; \
	} \
	catch(exception &e) \
	{ \
		before_catched \
		if(result_condition) \
		{ \
			tRisseVariant msg(e.what()); \
			RisseGetExceptionObject((risse), (result_addr), msg, NULL); \
		} \
		when_catched; \
	} \
	RISSE_CONVERT_TO_RISSE_EXCEPTION_OBJECT_ADDITIONAL(risse, result_condition, result_addr, before_catched, when_catched) \
	catch(...) \
	{ \
		before_catched \
		if(result_condition) (result_addr)->Clear(); \
		when_catched; \
	}




//---------------------------------------------------------------------------
// eRissexxxxx
//---------------------------------------------------------------------------
class eRisseSilent
{
	// silent exception
};
//---------------------------------------------------------------------------
class eRisse
{
public:
	eRisse() {;}
	eRisse(const eRisse&) {;}
	eRisse& operator= (const eRisse& e) { return *this; }
	virtual ~eRisse() {;}
	virtual const ttstr & GetMessageString() const 
	{ return RisseNonamedException; }
};
//---------------------------------------------------------------------------
void Risse_eRisse();
//---------------------------------------------------------------------------
class eRisseError : public eRisse
{
public:
	eRisseError(const ttstr & Msg) :
		Message(Msg) {;}
	const ttstr & GetMessageString() const { return Message; }

	void AppendMessage(const ttstr & msg) { Message += msg; }

private:
	ttstr Message;
};
//---------------------------------------------------------------------------
void Risse_eRisseError(const ttstr & msg);
void Risse_eRisseError(const risse_char* msg);
//---------------------------------------------------------------------------
class eRisseVariantError : public eRisseError
{
public:
	eRisseVariantError(const ttstr & Msg) :
		eRisseError(Msg) {;}

	eRisseVariantError(const eRisseVariantError &ref) :
		eRisseError(ref) {;}
};
//---------------------------------------------------------------------------
void Risse_eRisseVariantError(const ttstr & msg);
void Risse_eRisseVariantError(const risse_char * msg);
//---------------------------------------------------------------------------
class tRisseScriptBlock;
class tRisseInterCodeContext;
class eRisseScriptError : public eRisseError
{
	class tScriptBlockHolder
	{
	public:
		tScriptBlockHolder(tRisseScriptBlock *block);
		~tScriptBlockHolder();
		tScriptBlockHolder(const tScriptBlockHolder &holder);
		tRisseScriptBlock *Block;
	} Block;

	risse_int Position;

	ttstr Trace;

public:
	tRisseScriptBlock * GetBlockNoAddRef() { return Block.Block; }

	risse_int GetPosition() const { return Position; }

	risse_int GetSourceLine() const;

	const risse_char * GetBlockName() const;

	const ttstr & GetTrace() const { return Trace; }

	bool AddTrace(tRisseScriptBlock *block, risse_int srcpos);
	bool AddTrace(tRisseInterCodeContext *context, risse_int codepos);
	bool AddTrace(const ttstr & data);

	eRisseScriptError(const ttstr &  Msg,
		tRisseScriptBlock *block, risse_int pos);

	eRisseScriptError(const eRisseScriptError &ref) :
		eRisseError(ref), Block(ref.Block), Position(ref.Position), Trace(ref.Trace) {;}
};
//---------------------------------------------------------------------------
void Risse_eRisseScriptError(const ttstr &msg, tRisseScriptBlock *block, risse_int srcpos);
void Risse_eRisseScriptError(const risse_char *msg, tRisseScriptBlock *block, risse_int srcpos);
void Risse_eRisseScriptError(const ttstr &msg, tRisseInterCodeContext *context, risse_int codepos);
void Risse_eRisseScriptError(const risse_char *msg, tRisseInterCodeContext *context, risse_int codepos);
//---------------------------------------------------------------------------
class eRisseScriptException : public eRisseScriptError
{
	tRisseVariant Value;
public:
	tRisseVariant & GetValue() { return Value; }

	eRisseScriptException(const ttstr & Msg,
		tRisseScriptBlock *block, risse_int pos, tRisseVariant &val)
			: eRisseScriptError(Msg, block, pos), Value(val) {}

	eRisseScriptException(const eRisseScriptException &ref) :
		eRisseScriptError(ref), Value(ref.Value) {;}
};
//---------------------------------------------------------------------------
void Risse_eRisseScriptException(const ttstr &msg, tRisseScriptBlock *block,
	risse_int srcpos, tRisseVariant &val);
void Risse_eRisseScriptException(const risse_char *msg, tRisseScriptBlock *block,
	risse_int srcpos, tRisseVariant &val);
void Risse_eRisseScriptException(const ttstr &msg, tRisseInterCodeContext *context,
	risse_int codepos, tRisseVariant &val);
void Risse_eRisseScriptException(const risse_char *msg, tRisseInterCodeContext *context,
	risse_int codepos, tRisseVariant &val);
//---------------------------------------------------------------------------
class eRisseCompileError : public eRisseScriptError
{
public:
	eRisseCompileError(const ttstr &  Msg, tRisseScriptBlock *block, risse_int pos) :
		eRisseScriptError(Msg, block, pos) {;}

	eRisseCompileError(const eRisseCompileError &ref) : eRisseScriptError(ref) {;}

};
//---------------------------------------------------------------------------
void Risse_eRisseCompileError(const ttstr & msg, tRisseScriptBlock *block, risse_int srcpos);
void Risse_eRisseCompileError(const risse_char * msg, tRisseScriptBlock *block, risse_int srcpos);
//---------------------------------------------------------------------------
void RisseThrowFrom_risse_error(risse_error hr, const risse_char *name = NULL);
#define RISSE_THROW_IF_ERROR(x) { \
	risse_error ____er; ____er = (x); if(RISSE_FAILED(____er)) RisseThrowFrom_risse_error(____er); }
//---------------------------------------------------------------------------
} // namespace Risse
//---------------------------------------------------------------------------
#endif // #ifndef RISSE_DECL_MESSAGE_BODY



//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// messages
//---------------------------------------------------------------------------
namespace Risse
{
#ifdef RISSE_DECL_MESSAGE_BODY
	#define RISSE_MSG_DECL(name, msg) tRisseMessageHolder name(RISSE_WS2(#name), msg);
#else
	#define RISSE_MSG_DECL(name, msg) extern tRisseMessageHolder name;
#endif
//---------------------------------------------------------------------------
#ifdef RISSE_JP_LOCALIZED
	#include "risseError_jp.h"
#else
RISSE_MSG_DECL(RisseInternalError, RISSE_WS("Internal error"))
RISSE_MSG_DECL(RisseWarning, RISSE_WS("Warning: "))
RISSE_MSG_DECL(RisseWarnEvalOperator, RISSE_WS("Non-global post-! operator is used (note that the post-! operator behavior is changed on Risse2 version 2.4.1)"))
RISSE_MSG_DECL(RisseNarrowToWideConversionError, RISSE_WS("Cannot convert given narrow string to wide string"))
RISSE_MSG_DECL(RisseVariantConvertError, RISSE_WS("Cannot convert the variable type (%1 to %2)"))
RISSE_MSG_DECL(RisseVariantConvertErrorToObject, RISSE_WS("Cannot convert the variable type (%1 to Object)"))
RISSE_MSG_DECL(RisseIDExpected, RISSE_WS("Specify an ID"))
RISSE_MSG_DECL(RisseSubstitutionInBooleanContext, RISSE_WS("Substitution in boolean context (use form of '(A=B)!=0' to compare to zero)"));
RISSE_MSG_DECL(RisseCannotModifyLHS, RISSE_WS("This expression cannot be used as a lvalue"))
RISSE_MSG_DECL(RisseInsufficientMem, RISSE_WS("Insufficient memory"))
RISSE_MSG_DECL(RisseCannotGetResult, RISSE_WS("Cannot get the value of this expression"))
RISSE_MSG_DECL(RisseNullAccess, RISSE_WS("Accessing to null object"))
RISSE_MSG_DECL(RisseMemberNotFound, RISSE_WS("Member \"%1\" does not exist"))
RISSE_MSG_DECL(RisseMemberNotFoundNoNameGiven, RISSE_WS("Member does not exist"))
RISSE_MSG_DECL(RisseNotImplemented, RISSE_WS("Called method is not implemented"))
RISSE_MSG_DECL(RisseInvalidParam, RISSE_WS("Invalid argument"))
RISSE_MSG_DECL(RisseBadParamCount, RISSE_WS("Invalid argument count"))
RISSE_MSG_DECL(RisseInvalidType, RISSE_WS("Not a function or invalid method/property type"))
RISSE_MSG_DECL(RisseSpecifyDicOrArray, RISSE_WS("Specify a Dictionary object or an Array object"));
RISSE_MSG_DECL(RisseSpecifyArray, RISSE_WS("Specify an Array object"));
RISSE_MSG_DECL(RisseStringDeallocError, RISSE_WS("Cannot free the string memory block"))
RISSE_MSG_DECL(RisseStringAllocError, RISSE_WS("Cannot allocate the string memory block"))
RISSE_MSG_DECL(RisseMisplacedBreakContinue, RISSE_WS("Cannot place \"break\" or \"continue\" here"))
RISSE_MSG_DECL(RisseMisplacedCase, RISSE_WS("Cannot place \"case\" here"))
RISSE_MSG_DECL(RisseMisplacedReturn, RISSE_WS("Cannot place \"return\" here"))
RISSE_MSG_DECL(RisseStringParseError, RISSE_WS("Un-terminated string/regexp/octet literal"))
RISSE_MSG_DECL(RisseNumberError, RISSE_WS("Cannot be parsed as a number"))
RISSE_MSG_DECL(RisseUnclosedComment, RISSE_WS("Un-terminated comment"))
RISSE_MSG_DECL(RisseInvalidChar, RISSE_WS("Invalid character \'%1\'"))
RISSE_MSG_DECL(RisseExpected, RISSE_WS("Expected %1"))
RISSE_MSG_DECL(RisseSyntaxError, RISSE_WS("Syntax error (%1)"))
RISSE_MSG_DECL(RissePPError, RISSE_WS("Error in conditional compiling expression"))
RISSE_MSG_DECL(RisseCannotGetSuper, RISSE_WS("Super class does not exist or cannot specify the super class"))
RISSE_MSG_DECL(RisseInvalidOpecode, RISSE_WS("Invalid VM code"))
RISSE_MSG_DECL(RisseRangeError, RISSE_WS("The value is out of the range"))
RISSE_MSG_DECL(RisseAccessDenyed, RISSE_WS("Invalid operation for Read-only or Write-only property"))
RISSE_MSG_DECL(RisseNativeClassCrash, RISSE_WS("Invalid object context"))
RISSE_MSG_DECL(RisseInvalidObject, RISSE_WS("The object is already invalidated"))
RISSE_MSG_DECL(RisseDuplicatedPropHandler, RISSE_WS("Duplicated \"setter\" or \"getter\""))
RISSE_MSG_DECL(RisseCannotOmit, RISSE_WS("\"...\" is used out of functions"))
RISSE_MSG_DECL(RisseCannotParseDate, RISSE_WS("Invalid date format"))
RISSE_MSG_DECL(RisseInvalidValueForTimestamp, RISSE_WS("Invalid value for date/time"))
RISSE_MSG_DECL(RisseExceptionNotFound, RISSE_WS("Cannot convert exception because \"Exception\" does not exist"))
RISSE_MSG_DECL(RisseInvalidFormatString, RISSE_WS("Invalid format string"))
RISSE_MSG_DECL(RisseDivideByZero, RISSE_WS("Division by zero"))
RISSE_MSG_DECL(RisseNotReconstructiveRandomizeData, RISSE_WS("Cannot reconstruct random seeds"))
RISSE_MSG_DECL(RisseSymbol, RISSE_WS("ID"))
RISSE_MSG_DECL(RisseCallHistoryIsFromOutOfRisseScript, RISSE_WS("[out of Risse script]"))
RISSE_MSG_DECL(RisseNObjectsWasNotFreed, RISSE_WS("Total %1 Object(s) was not freed"))
RISSE_MSG_DECL(RisseObjectCreationHistoryDelimiter, RISSE_WS(" <-- "));
RISSE_MSG_DECL(RisseObjectWasNotFreed, RISSE_WS("Object %1 [%2] wes not freed / The object was created at : %2"))
RISSE_MSG_DECL(RisseGroupByObjectTypeAndHistory, RISSE_WS("Group by object type and location where the object was created"))
RISSE_MSG_DECL(RisseGroupByObjectType, RISSE_WS("Group by object type"))
RISSE_MSG_DECL(RisseObjectCountingMessageGroupByObjectTypeAndHistory, RISSE_WS("%1 time(s) : [%2] %3"))
RISSE_MSG_DECL(RisseObjectCountingMessageRisseGroupByObjectType, RISSE_WS("%1 time(s) : [%2]"))
RISSE_MSG_DECL(RisseWarnRunningCodeOnDeletingObject, RISSE_WS("%4: Running code on deleting-in-progress object %1[%2] / The object was created at : %3"))
RISSE_MSG_DECL(RisseWriteError, RISSE_WS("Write error"))
RISSE_MSG_DECL(RisseReadError, RISSE_WS("Read error"))
RISSE_MSG_DECL(RisseSeekError, RISSE_WS("Seek error"))
#endif

#undef RISSE_MSG_DECL
//---------------------------------------------------------------------------

}
//---------------------------------------------------------------------------


#endif // #ifndef risseErrorH




