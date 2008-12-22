//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risaで内部的に用いている例外クラスの管理など
//---------------------------------------------------------------------------
#ifndef _RISAExceptionH_
#define _RISAExceptionH_

#include "risseExceptionClass.h"
#include "risa/common/UnhandledException.h"

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * Risa用の汎用例外クラス
 * @note	暫定的な利用。最終的にはこれは無くす。
 */
class eRisaException
{
	eRisaException();

public:
	/**
	 * eRisaException型の例外を投げる
	 */
	static void Throw(const tString &msg);

	/**
	 * eRisaException型の例外を投げる
	 * @param str	文字列 (中に %1 などの指令を埋め込む)
	 * @param s1	文字列中の %1 と置き換えたい文字列
	 */
	static void Throw(const tString &msg, const tString & s1);

	/**
	 * eRisaException型の例外を投げる
	 * @param str	文字列 (中に %1 などの指令を埋め込む)
	 * @param s1	文字列中の %1 と置き換えたい文字列
	 * @param s2	文字列中の %2 と置き換えたい文字列
	 */
	static void Throw(const tString &msg, const tString & s1, const tString & s2);

	/**
	 * eRisaException型の例外を投げる
	 * @param str	文字列 (中に %1 などの指令を埋め込む)
	 * @param s1	文字列中の %1 と置き換えたい文字列
	 * @param s2	文字列中の %2 と置き換えたい文字列
	 * @param s3	文字列中の %3 と置き換えたい文字列
	 */
	static void Throw(const tString &msg, const tString & s1, const tString & s2, const tString & s3);

	/**
	 * eRisaException型の例外を投げる
	 * @param str	文字列 (中に %1 などの指令を埋め込む)
	 * @param s1	文字列中の %1 と置き換えたい文字列
	 * @param s2	文字列中の %2 と置き換えたい文字列
	 * @param s3	文字列中の %3 と置き換えたい文字列
	 * @param s4	文字列中の %4 と置き換えたい文字列
	 */
	static void Throw(const tString &msg, const tString & s1, const tString & s2, const tString & s3, const tString & s4);

	/**
	 * 内部エラー例外を発生させる
	 * @param line		エラーの起こった行
	 * @param filename	エラーの起こったファイル名
	 */
	static void ThrowInternalErrorException(int line, const char * filename);
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
/**
 * 内部エラーをthrowするマクロ
 */
#define ThrowInternalError eRisaException::ThrowInternalErrorException(__LINE__, __FILE__)
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
/**
 * 例外を捕捉し、必要ならばエラー表示を行うマクロ
 */
#define RISA_CATCH_AND_SHOW_SCRIPT_EXCEPTION(origin) \
	catch(const tVariant * e) \
	{ \
		tUnhandledExceptionHandler::ShowScriptException(e); \
	} \


/*\
	catch(eRisseScriptException &e) \
	{ \
		e.AddTrace(tString(origin)); \
		tUnhandledExceptionHandler::Process(e); \
	} \
	catch(eRisseScriptError &e) \
	{ \
		e.AddTrace(tString(origin)); \
		tUnhandledExceptionHandler::Process(e); \
	} \
	catch(eRisse &e) \
	{ \
		tUnhandledExceptionHandler::Process(e); \
	} \
	catch(...) \
	{ \
		throw; \
	}*/
// TODO:implement this
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 例外を捕捉し、強制的にエラー表示を行うマクロ
 */
#define RISA_CATCH_AND_SHOW_SCRIPT_EXCEPTION_FORCE_SHOW_EXCEPTION(origin) \
	catch(...) { throw; }

/*\
	catch(eRisseScriptError &e) \
	{ \
		e.AddTrace(tString(origin)); \
		tUnhandledExceptionHandler::ShowScriptException(e); \
	} \
	catch(eRisse &e) \
	{ \
		tUnhandledExceptionHandler::ShowScriptException(e); \
	} \
	catch(...) \
	{ \
		throw; \
	}*/
// TODO: implement this
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 既存の例外クラスから新しい例外クラスを派生させるためのマクロ
 */
#define RISA_DEFINE_EXCEPTION_SUBCLASS(CPP_CLASSNAME, RISSE_PACKAGENAME_STRING, RISSE_CLASSNAME_STRING, PARENT_CLASS) \
class CPP_CLASSNAME : public tClassBase       {                                   \
	typedef tClassBase inherited;                                                 \
public:                                                                           \
	CPP_CLASSNAME(tScriptEngine * engine) :                                       \
		tClassBase(RISSE_CLASSNAME_STRING, PARENT_CLASS) { RegisterMembers(); }   \
	void RegisterMembers()  {                                                     \
		inherited::RegisterMembers();                                             \
		BindFunction(this, ss_construct, &CPP_CLASSNAME::construct);              \
		BindFunction(this, ss_initialize, &CPP_CLASSNAME::initialize);            \
	}                                                                             \
public:                                                                           \
	static void construct(const tNativeCallInfo & info){}                         \
	static void initialize(const tNativeCallInfo & info)                          \
		{info.InitializeSuperClass(info.args);}                                   \
public:                                                                           \
	static void Throw(tScriptEngine * engine, const tString &msg) {               \
		tTemporaryException * e =                                                 \
			new tTemporaryException(                                              \
				RISSE_PACKAGENAME_STRING, RISSE_CLASSNAME_STRING, msg);           \
		if(engine) e->ThrowConverted(engine); else throw e;                       \
	}                                                                             \
	static void Throw(const tString &msg) { Throw(NULL, msg); }                   \
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
/**
 * 例外を捕捉し、例外メッセージの先頭に指定のメッセージを追加するためのマクロ
 */
#define RISA_PREPEND_EXCEPTION_MESSAGE_BEGIN() try{try{

#define RISA_PREPEND_EXCEPTION_MESSAGE_END(ENGINE, MESSAGE) \
	} catch(const tTemporaryException * te) { te->ThrowConverted(ENGINE); } \
	} catch(const tVariant * e) { e->PrependMessage(MESSAGE); throw e; }
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
} // namespace Risa



#endif
