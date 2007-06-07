//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risseスクリプトエンジンの開始・終了・スクリプト実行などのインターフェース
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/script/RisseEngine.h"
#include "risse/include/risseError.h"
#include "base/log/Log.h"

#if 0

RISSE_DEFINE_SOURCE_ID(50344,48369,3431,18494,14208,60463,45295,19784);


//---------------------------------------------------------------------------
tRisaRisseScriptEngine::tRisaRisseScriptEngine()
{
	Engine = new tRisse();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaRisseScriptEngine::~tRisaRisseScriptEngine()
{
	if(Engine)
	{
/*
#ifdef __WXDEBUG__
		wxFprintf(stderr, wxT("warning: tRisaRisseScriptEngine::instance()->Shutdown() should be called before main() ends.\n"));
#endif
*/
		Shutdown();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaRisseScriptEngine::Shutdown()
{
	if(Engine)
	{
		Engine->Shutdown();
		Engine->Release();
		Engine = NULL;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaRisseScriptEngine::RegisterGlobalObject(const risse_char *name,
	iRisseDispatch2 * object)
{
	if(!Engine) return;
	tRisseVariant val(object, NULL);
	iRisseDispatch2 * global = Engine->GetGlobalNoAddRef();
	risse_error er;
	er = global->PropSet(RISSE_MEMBERENSURE, name, NULL, &val, global);
	if(RISSE_FAILED(er))
		RisseThrowFrom_risse_error(er, name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaRisseScriptEngine::EvalExpresisonAndPrintResultToConsole(const tRisseString & expression)
{
	// execute the expression
	tRisseString result_str;
	tRisseVariant result;
	try
	{
		Engine->EvalExpression(expression, &result);
	}
	catch(eRisse &e)
	{
		// An exception had been occured in console quick Risse expression evaluation
		result_str = tRisseString(RISSE_WS_TR("(Console) ")) + expression +
			tRisseString(RISSE_WS_TR(" = (exception) ")) +
			e.GetMessageString();
		tRisaLogger::Log(result_str, tRisaLogger::llError);
		return;
	}
	catch(...)
	{
		throw;
	}

	// success in console quick Risse expression evaluation
	result_str = tRisseString(RISSE_WS_TR("(Console) ")) + expression +
		tRisseString(RISSE_WS_TR(" = ")) +
		RisseVariantToReadableString(result);
	tRisaLogger::Log(result_str);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaRisseScriptEngine::ExecuteScript(
		const tRisseString &script, tRisseVariant *result,
		iRisseDispatch2 *context,
		const tRisseString *name, risse_int lineofs)
{
	Engine->ExecScript(script, result, context, name, lineofs);
}
//---------------------------------------------------------------------------

#endif
