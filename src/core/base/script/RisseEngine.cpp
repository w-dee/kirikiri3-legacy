//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

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

RISSE_DEFINE_SOURCE_ID(50344,48369,3431,18494,14208,60463,45295,19784);


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaRisseScriptEngine::tRisaRisseScriptEngine()
{
	Engine = new tRisse();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaRisseScriptEngine::~tRisaRisseScriptEngine()
{
	if(Engine)
	{
/*
#ifdef __WXDEBUG__
		wxFprintf(stderr, wxT("warning: tRisaRisseScriptEngine::instance().Shutdown() should be called before main() ends.\n"));
#endif
*/
		Shutdown();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		シャットダウン
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
//! @brief		グローバルにオブジェクトを登録する
//! @param		name    オブジェクトにつけたい名前
//! @param		object  その名前で登録したいオブジェクト
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
//! @brief		式を評価して結果をコンソールに表示する
//! @param		expression 式
//---------------------------------------------------------------------------
void tRisaRisseScriptEngine::EvalExpresisonAndPrintResultToConsole(const ttstr & expression)
{
	// execute the expression
	ttstr result_str;
	tRisseVariant result;
	try
	{
		Engine->EvalExpression(expression, &result);
	}
	catch(eRisse &e)
	{
		// An exception had been occured in console quick Risse expression evaluation
		result_str = ttstr(RISSE_WS_TR("(Console) ")) + expression +
			ttstr(RISSE_WS_TR(" = (exception) ")) +
			e.GetMessage();
		tRisaLogger::instance()->Log(result_str, tRisaLogger::llError);
		return;
	}
	catch(...)
	{
		throw;
	}

	// success in console quick Risse expression evaluation
	result_str = ttstr(RISSE_WS_TR("(Console) ")) + expression +
		ttstr(RISSE_WS_TR(" = ")) +
		RisseVariantToReadableString(result);
	tRisaLogger::instance()->Log(result_str);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スクリプトブロックを実行する
//! @param		script		スクリプト
//! @param		result		実行結果を受け取るtRisseVariant型オブジェクトへのポインタ(要らない場合はNULLを)
//! @param		context		実行を行うコンテキスト(globalで動かしたい場合はNULL)
//! @param		name		スクリプトブロック名
//! @param		lineofs		このスクリプトブロックの(オリジナルのファイル全体に対する)開始行
//---------------------------------------------------------------------------
void tRisaRisseScriptEngine::ExecuteScript(
		const ttstr &script, tRisseVariant *result,
		iRisseDispatch2 *context,
		const ttstr *name, risse_int lineofs)
{
	Engine->ExecScript(script, result, context, name, lineofs);
}
//---------------------------------------------------------------------------

