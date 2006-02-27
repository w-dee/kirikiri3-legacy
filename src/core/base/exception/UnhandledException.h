//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ハンドルされなかった例外のハンドル
//---------------------------------------------------------------------------
#ifndef _UnhandledH_
#define _UnhandledH_

#include "base/utils/Singleton.h"
#include "risse/include/risseError.h"

//---------------------------------------------------------------------------
//! @brief ハンドルされなかった例外のハンドルを行うクラス
//---------------------------------------------------------------------------
class tRisaUnhandledExceptionHandler
{
public:
	static void ShowScriptException(eRisse &e);
	static void ShowScriptException(eRisseScriptError &e);

	static void Process(eRisseScriptException &e);
	static void Process(eRisseScriptError &e);
	static void Process(eRisse &e);
};
//---------------------------------------------------------------------------


#define RisaThrowInternalError eRisaException::ThrowInternalError(__LINE__, __FILE__)

#endif
