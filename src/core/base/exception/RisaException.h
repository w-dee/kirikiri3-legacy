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
	//! @brief		コンストラクタ
	eRisaException(const ttstr & msg) : eRisseError(msg) {;}

	//! @brief		コピーコンストラクタ
	eRisaException(const eRisaException & ref) : eRisseError(ref) {;}

	//! @brief		eRisaException型の例外を投げる
	static void Throw(const ttstr &msg);

	//! @brief		eRisaException型の例外を投げる
	//! @param		str  文字列 (中に %1 などの指令を埋め込む)
	//! @param		s1   文字列中の %1 と置き換えたい文字列
	static void Throw(const ttstr &msg, const ttstr & s1);

	//! @brief		eRisaException型の例外を投げる
	//! @param		str  文字列 (中に %1 などの指令を埋め込む)
	//! @param		s1   文字列中の %1 と置き換えたい文字列
	//! @param		s2   文字列中の %2 と置き換えたい文字列
	static void Throw(const ttstr &msg, const ttstr & s1, const ttstr & s2);

	//! @brief		eRisaException型の例外を投げる
	//! @param		str  文字列 (中に %1 などの指令を埋め込む)
	//! @param		s1   文字列中の %1 と置き換えたい文字列
	//! @param		s2   文字列中の %2 と置き換えたい文字列
	//! @param		s3   文字列中の %3 と置き換えたい文字列
	static void Throw(const ttstr &msg, const ttstr & s1, const ttstr & s2, const ttstr & s3);

	//! @brief		eRisaException型の例外を投げる
	//! @param		str  文字列 (中に %1 などの指令を埋め込む)
	//! @param		s1   文字列中の %1 と置き換えたい文字列
	//! @param		s2   文字列中の %2 と置き換えたい文字列
	//! @param		s3   文字列中の %3 と置き換えたい文字列
	//! @param		s4   文字列中の %4 と置き換えたい文字列
	static void Throw(const ttstr &msg, const ttstr & s1, const ttstr & s2, const ttstr & s3, const ttstr & s4);

	//! @brief		内部エラー例外を発生させる
	//! @param		line     エラーの起こった行
	//! @param		filename エラーの起こったファイル名
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
