//---------------------------------------------------------------------------
/*
	TJS3 Script Engine
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// TJS3's C++ exception class and exception message
//---------------------------------------------------------------------------


#ifndef tjsErrorH
#define tjsErrorH

#ifndef TJS_DECL_MESSAGE_BODY

#include <stdexcept>
#include <string>
#include "tjs.h"
#include "tjsVariant.h"
#include "tjsString.h"
#include "tjsMessage.h"

namespace TJS
{
//---------------------------------------------------------------------------
extern ttstr TJSNonamedException;

//---------------------------------------------------------------------------
// macro
//---------------------------------------------------------------------------
#ifdef TJS_SUPPORT_VCL
	#define TJS_CONVERT_TO_TJS_EXCEPTION_ADDITIONAL \
		catch(const Exception &e) \
		{ \
			TJS_eTJSError(e.Message.c_str()); \
		}
#else
	#define TJS_CONVERT_TO_TJS_EXCEPTION_ADDITIONAL
#endif


#define TJS_CONVERT_TO_TJS_EXCEPTION \
	catch(const eTJSSilent &e) \
	{ \
		throw e; \
	} \
	catch(const eTJSScriptException &e) \
	{ \
		throw e; \
	} \
	catch(const eTJSScriptError &e) \
	{ \
		throw e; \
	} \
	catch(const eTJSError &e) \
	{ \
		throw e; \
	} \
	catch(const eTJS &e) \
	{ \
		TJS_eTJSError(e.GetMessage()); \
	} \
	catch(const std::exception &e) \
	{ \
		TJS_eTJSError(e.what()); \
	} \
	catch(const wchar_t *text) \
	{ \
		TJS_eTJSError(text); \
	} \
	catch(const char *text) \
	{ \
		TJS_eTJSError(text); \
	} \
	TJS_CONVERT_TO_TJS_EXCEPTION_ADDITIONAL
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// TJSGetExceptionObject : retrieves TJS 'Exception' object
//---------------------------------------------------------------------------
extern void TJSGetExceptionObject(tTJS *tjs, tTJSVariant *res, tTJSVariant &msg,
	tTJSVariant *trace/* trace is optional */ = NULL);
//---------------------------------------------------------------------------

#ifdef TJS_SUPPORT_VCL
	#define TJS_CONVERT_TO_TJS_EXCEPTION_OBJECT_ADDITIONAL(_tjs, _result_condition, _result_addr, _before_catched, _when_catched) \
	catch(EAccessViolation &e) \
	{ \
		_before_catched; \
		if(_result_condition) \
		{ \
			tTJSVariant msg(e.Message.c_str()); \
			TJSGetExceptionObject((_tjs), (_result_addr), msg, NULL); \
		} \
		_when_catched; \
	} \
	catch(Exception &e) \
	{ \
		_before_catched; \
		if(_result_condition) \
		{ \
			tTJSVariant msg(e.Message.c_str()); \
			TJSGetExceptionObject((_tjs), (_result_addr), msg, NULL); \
		} \
		_when_catched; \
	}
#else
	#define TJS_CONVERT_TO_TJS_EXCEPTION_OBJECT_ADDITIONAL(_tjs, _result_condition, _result_addr, _before_catched, _when_catched)
#endif


#define TJS_CONVERT_TO_TJS_EXCEPTION_OBJECT(tjs, result_condition, result_addr, before_catched, when_catched) \
	catch(eTJSSilent &e) \
	{ \
		throw e; \
	} \
	catch(eTJSScriptException &e) \
	{ \
		before_catched \
		if(result_condition) *(result_addr) = e.GetValue(); \
		when_catched; \
	} \
	catch(eTJSScriptError &e) \
	{ \
		before_catched \
		if(result_condition) \
		{ \
			tTJSVariant msg(e.GetMessage()); \
			tTJSVariant trace(e.GetTrace()); \
			TJSGetExceptionObject((tjs), (result_addr), msg, &trace); \
		} \
		when_catched; \
	} \
	catch(eTJS &e)  \
	{  \
		before_catched \
		if(result_condition) \
		{ \
			tTJSVariant msg(e.GetMessage()); \
			TJSGetExceptionObject((tjs), (result_addr), msg, NULL); \
		} \
		when_catched; \
	} \
	catch(exception &e) \
	{ \
		before_catched \
		if(result_condition) \
		{ \
			tTJSVariant msg(e.what()); \
			TJSGetExceptionObject((tjs), (result_addr), msg, NULL); \
		} \
		when_catched; \
	} \
	TJS_CONVERT_TO_TJS_EXCEPTION_OBJECT_ADDITIONAL(tjs, result_condition, result_addr, before_catched, when_catched) \
	catch(...) \
	{ \
		before_catched \
		if(result_condition) (result_addr)->Clear(); \
		when_catched; \
	}




//---------------------------------------------------------------------------
// eTJSxxxxx
//---------------------------------------------------------------------------
class eTJSSilent
{
	// silent exception
};
//---------------------------------------------------------------------------
class eTJS
{
public:
	eTJS() {;}
	eTJS(const eTJS&) {;}
	eTJS& operator= (const eTJS& e) { return *this; }
	virtual ~eTJS() {;}
	virtual const ttstr & GetMessage() const 
	{ return TJSNonamedException; }
};
//---------------------------------------------------------------------------
void TJS_eTJS();
//---------------------------------------------------------------------------
class eTJSError : public eTJS
{
public:
	eTJSError(const ttstr & Msg) :
		Message(Msg) {;}
	const ttstr & GetMessage() const { return Message; }

	void AppendMessage(const ttstr & msg) { Message += msg; }

private:
	ttstr Message;
};
//---------------------------------------------------------------------------
void TJS_eTJSError(const ttstr & msg);
void TJS_eTJSError(const tjs_char* msg);
//---------------------------------------------------------------------------
class eTJSVariantError : public eTJSError
{
public:
	eTJSVariantError(const ttstr & Msg) :
		eTJSError(Msg) {;}

	eTJSVariantError(const eTJSVariantError &ref) :
		eTJSError(ref) {;}
};
//---------------------------------------------------------------------------
void TJS_eTJSVariantError(const ttstr & msg);
void TJS_eTJSVariantError(const tjs_char * msg);
//---------------------------------------------------------------------------
class tTJSScriptBlock;
class tTJSInterCodeContext;
class eTJSScriptError : public eTJSError
{
	class tScriptBlockHolder
	{
	public:
		tScriptBlockHolder(tTJSScriptBlock *block);
		~tScriptBlockHolder();
		tScriptBlockHolder(const tScriptBlockHolder &holder);
		tTJSScriptBlock *Block;
	} Block;

	tjs_int Position;

	ttstr Trace;

public:
	tTJSScriptBlock * GetBlockNoAddRef() { return Block.Block; }

	tjs_int GetPosition() const { return Position; }

	tjs_int GetSourceLine() const;

	const tjs_char * GetBlockName() const;

	const ttstr & GetTrace() const { return Trace; }

	bool AddTrace(tTJSScriptBlock *block, tjs_int srcpos);
	bool AddTrace(tTJSInterCodeContext *context, tjs_int codepos);
	bool AddTrace(const ttstr & data);

	eTJSScriptError(const ttstr &  Msg,
		tTJSScriptBlock *block, tjs_int pos);

	eTJSScriptError(const eTJSScriptError &ref) :
		eTJSError(ref), Block(ref.Block), Position(ref.Position), Trace(ref.Trace) {;}
};
//---------------------------------------------------------------------------
void TJS_eTJSScriptError(const ttstr &msg, tTJSScriptBlock *block, tjs_int srcpos);
void TJS_eTJSScriptError(const tjs_char *msg, tTJSScriptBlock *block, tjs_int srcpos);
void TJS_eTJSScriptError(const ttstr &msg, tTJSInterCodeContext *context, tjs_int codepos);
void TJS_eTJSScriptError(const tjs_char *msg, tTJSInterCodeContext *context, tjs_int codepos);
//---------------------------------------------------------------------------
class eTJSScriptException : public eTJSScriptError
{
	tTJSVariant Value;
public:
	tTJSVariant & GetValue() { return Value; }

	eTJSScriptException(const ttstr & Msg,
		tTJSScriptBlock *block, tjs_int pos, tTJSVariant &val)
			: eTJSScriptError(Msg, block, pos), Value(val) {}

	eTJSScriptException(const eTJSScriptException &ref) :
		eTJSScriptError(ref), Value(ref.Value) {;}
};
//---------------------------------------------------------------------------
void TJS_eTJSScriptException(const ttstr &msg, tTJSScriptBlock *block,
	tjs_int srcpos, tTJSVariant &val);
void TJS_eTJSScriptException(const tjs_char *msg, tTJSScriptBlock *block,
	tjs_int srcpos, tTJSVariant &val);
void TJS_eTJSScriptException(const ttstr &msg, tTJSInterCodeContext *context,
	tjs_int codepos, tTJSVariant &val);
void TJS_eTJSScriptException(const tjs_char *msg, tTJSInterCodeContext *context,
	tjs_int codepos, tTJSVariant &val);
//---------------------------------------------------------------------------
class eTJSCompileError : public eTJSScriptError
{
public:
	eTJSCompileError(const ttstr &  Msg, tTJSScriptBlock *block, tjs_int pos) :
		eTJSScriptError(Msg, block, pos) {;}

	eTJSCompileError(const eTJSCompileError &ref) : eTJSScriptError(ref) {;}

};
//---------------------------------------------------------------------------
void TJS_eTJSCompileError(const ttstr & msg, tTJSScriptBlock *block, tjs_int srcpos);
void TJS_eTJSCompileError(const tjs_char * msg, tTJSScriptBlock *block, tjs_int srcpos);
//---------------------------------------------------------------------------
void TJSThrowFrom_tjs_error(tjs_error hr, const tjs_char *name = NULL);
#define TJS_THROW_IF_ERROR(x) { \
	tjs_error ____er; ____er = (x); if(TJS_FAILED(____er)) TJSThrowFrom_tjs_error(____er); }
//---------------------------------------------------------------------------
} // namespace TJS
//---------------------------------------------------------------------------
#endif // #ifndef TJS_DECL_MESSAGE_BODY



//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// messages
//---------------------------------------------------------------------------
namespace TJS
{
#ifdef TJS_DECL_MESSAGE_BODY
	#define TJS_MSG_DECL(name, msg) tTJSMessageHolder name(TJS_WS(#name), msg);
#else
	#define TJS_MSG_DECL(name, msg) extern tTJSMessageHolder name;
#endif
//---------------------------------------------------------------------------
#ifdef TJS_JP_LOCALIZED
	#include "tjsError_jp.h"
#else
TJS_MSG_DECL(TJSInternalError, TJS_WS("Internal error"))
TJS_MSG_DECL(TJSWarning, TJS_WS("Warning: "))
TJS_MSG_DECL(TJSWarnEvalOperator, TJS_WS("Non-global post-! operator is used (note that the post-! operator behavior is changed on TJS2 version 2.4.1)"))
TJS_MSG_DECL(TJSNarrowToWideConversionError, TJS_WS("Cannot convert given narrow string to wide string"))
TJS_MSG_DECL(TJSVariantConvertError, TJS_WS("Cannot convert the variable type (%1 to %2)"))
TJS_MSG_DECL(TJSVariantConvertErrorToObject, TJS_WS("Cannot convert the variable type (%1 to Object)"))
TJS_MSG_DECL(TJSIDExpected, TJS_WS("Specify an ID"))
TJS_MSG_DECL(TJSSubstitutionInBooleanContext, TJS_WS("Substitution in boolean context (use form of '(A=B)!=0' to compare to zero)"));
TJS_MSG_DECL(TJSCannotModifyLHS, TJS_WS("This expression cannot be used as a lvalue"))
TJS_MSG_DECL(TJSInsufficientMem, TJS_WS("Insufficient memory"))
TJS_MSG_DECL(TJSCannotGetResult, TJS_WS("Cannot get the value of this expression"))
TJS_MSG_DECL(TJSNullAccess, TJS_WS("Accessing to null object"))
TJS_MSG_DECL(TJSMemberNotFound, TJS_WS("Member \"%1\" does not exist"))
TJS_MSG_DECL(TJSMemberNotFoundNoNameGiven, TJS_WS("Member does not exist"))
TJS_MSG_DECL(TJSNotImplemented, TJS_WS("Called method is not implemented"))
TJS_MSG_DECL(TJSInvalidParam, TJS_WS("Invalid argument"))
TJS_MSG_DECL(TJSBadParamCount, TJS_WS("Invalid argument count"))
TJS_MSG_DECL(TJSInvalidType, TJS_WS("Not a function or invalid method/property type"))
TJS_MSG_DECL(TJSSpecifyDicOrArray, TJS_WS("Specify a Dictionary object or an Array object"));
TJS_MSG_DECL(TJSSpecifyArray, TJS_WS("Specify an Array object"));
TJS_MSG_DECL(TJSStringDeallocError, TJS_WS("Cannot free the string memory block"))
TJS_MSG_DECL(TJSStringAllocError, TJS_WS("Cannot allocate the string memory block"))
TJS_MSG_DECL(TJSMisplacedBreakContinue, TJS_WS("Cannot place \"break\" or \"continue\" here"))
TJS_MSG_DECL(TJSMisplacedCase, TJS_WS("Cannot place \"case\" here"))
TJS_MSG_DECL(TJSMisplacedReturn, TJS_WS("Cannot place \"return\" here"))
TJS_MSG_DECL(TJSStringParseError, TJS_WS("Un-terminated string/regexp/octet literal"))
TJS_MSG_DECL(TJSNumberError, TJS_WS("Cannot be parsed as a number"))
TJS_MSG_DECL(TJSUnclosedComment, TJS_WS("Un-terminated comment"))
TJS_MSG_DECL(TJSInvalidChar, TJS_WS("Invalid character \'%1\'"))
TJS_MSG_DECL(TJSExpected, TJS_WS("Expected %1"))
TJS_MSG_DECL(TJSSyntaxError, TJS_WS("Syntax error (%1)"))
TJS_MSG_DECL(TJSPPError, TJS_WS("Error in conditional compiling expression"))
TJS_MSG_DECL(TJSCannotGetSuper, TJS_WS("Super class does not exist or cannot specify the super class"))
TJS_MSG_DECL(TJSInvalidOpecode, TJS_WS("Invalid VM code"))
TJS_MSG_DECL(TJSRangeError, TJS_WS("The value is out of the range"))
TJS_MSG_DECL(TJSAccessDenyed, TJS_WS("Invalid operation for Read-only or Write-only property"))
TJS_MSG_DECL(TJSNativeClassCrash, TJS_WS("Invalid object context"))
TJS_MSG_DECL(TJSInvalidObject, TJS_WS("The object is already invalidated"))
TJS_MSG_DECL(TJSDuplicatedPropHandler, TJS_WS("Duplicated \"setter\" or \"getter\""))
TJS_MSG_DECL(TJSCannotOmit, TJS_WS("\"...\" is used out of functions"))
TJS_MSG_DECL(TJSCannotParseDate, TJS_WS("Invalid date format"))
TJS_MSG_DECL(TJSInvalidValueForTimestamp, TJS_WS("Invalid value for date/time"))
TJS_MSG_DECL(TJSExceptionNotFound, TJS_WS("Cannot convert exception because \"Exception\" does not exist"))
TJS_MSG_DECL(TJSInvalidFormatString, TJS_WS("Invalid format string"))
TJS_MSG_DECL(TJSDivideByZero, TJS_WS("Division by zero"))
TJS_MSG_DECL(TJSNotReconstructiveRandomizeData, TJS_WS("Cannot reconstruct random seeds"))
TJS_MSG_DECL(TJSSymbol, TJS_WS("ID"))
TJS_MSG_DECL(TJSCallHistoryIsFromOutOfTJS3Script, TJS_WS("[out of TJS3 script]"))
TJS_MSG_DECL(TJSNObjectsWasNotFreed, TJS_WS("Total %1 Object(s) was not freed"))
TJS_MSG_DECL(TJSObjectCreationHistoryDelimiter, TJS_WS(" <-- "));
TJS_MSG_DECL(TJSObjectWasNotFreed, TJS_WS("Object %1 [%2] wes not freed / The object was created at : %2"))
TJS_MSG_DECL(TJSGroupByObjectTypeAndHistory, TJS_WS("Group by object type and location where the object was created"))
TJS_MSG_DECL(TJSGroupByObjectType, TJS_WS("Group by object type"))
TJS_MSG_DECL(TJSObjectCountingMessageGroupByObjectTypeAndHistory, TJS_WS("%1 time(s) : [%2] %3"))
TJS_MSG_DECL(TJSObjectCountingMessageTJSGroupByObjectType, TJS_WS("%1 time(s) : [%2]"))
TJS_MSG_DECL(TJSWarnRunningCodeOnDeletingObject, TJS_WS("%4: Running code on deleting-in-progress object %1[%2] / The object was created at : %3"))
TJS_MSG_DECL(TJSWriteError, TJS_WS("Write error"))
TJS_MSG_DECL(TJSReadError, TJS_WS("Read error"))
TJS_MSG_DECL(TJSSeekError, TJS_WS("Seek error"))
#endif

#undef TJS_MSG_DECL
//---------------------------------------------------------------------------

}
//---------------------------------------------------------------------------


#endif // #ifndef tjsErrorH




