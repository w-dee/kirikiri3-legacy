//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risaで内部的に用いている例外クラスの管理など
//---------------------------------------------------------------------------
#ifndef _RisaExceptionH_
#define _RisaExceptionH_

#include "risse/include/risseError.h"
#include "base/exception/UnhandledException.h"

//---------------------------------------------------------------------------
//! @brief		Risa用の汎用例外クラス
//---------------------------------------------------------------------------
class eRisaException : public eRisseError
{
public:
	eRisaException(const ttstr & msg) : eRisseError(msg) {;}
	eRisaException(const eRisaException & ref) : eRisseError(ref) {;}

	static void Throw(const ttstr &msg);
	static void Throw(const ttstr &msg, const ttstr & s1);
	static void Throw(const ttstr &msg, const ttstr & s1, const ttstr & s2);
	static void Throw(const ttstr &msg, const ttstr & s1, const ttstr & s2, const ttstr & s3);
	static void Throw(const ttstr &msg, const ttstr & s1, const ttstr & s2, const ttstr & s3, const ttstr & s4);

	static void ThrowInternalError(int line, const char * filename);
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		内部エラーをthrowするマクロ
//---------------------------------------------------------------------------
#define RisaThrowInternalError eRisaException::ThrowInternalError(__LINE__, __FILE__)
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief		例外を捕捉し、必要ならばエラー表示を行うマクロ
//---------------------------------------------------------------------------
#define RISA_CATCH_AND_SHOW_SCRIPT_EXCEPTION(origin) \
	catch(eRisseScriptException &e) \
	{ \
		e.AddTrace(ttstr(origin)); \
		tRisaUnhandledExceptionHandler::Process(e); \
	} \
	catch(eRisseScriptError &e) \
	{ \
		e.AddTrace(ttstr(origin)); \
		tRisaUnhandledExceptionHandler::Process(e); \
	} \
	catch(eRisse &e) \
	{ \
		tRisaUnhandledExceptionHandler::Process(e); \
	} \
	catch(...) \
	{ \
		throw; \
	}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		例外を捕捉し、強制的にエラー表示を行うマクロ
//---------------------------------------------------------------------------
#define RISA_CATCH_AND_SHOW_SCRIPT_EXCEPTION_FORCE_SHOW_EXCEPTION(origin) \
	catch(eRisseScriptError &e) \
	{ \
		e.AddTrace(ttstr(origin)); \
		tRisaUnhandledExceptionHandler::ShowScriptException(e); \
	} \
	catch(eRisse &e) \
	{ \
		tRisaUnhandledExceptionHandler::ShowScriptException(e); \
	} \
	catch(...) \
	{ \
		throw; \
	}
//---------------------------------------------------------------------------



#endif
