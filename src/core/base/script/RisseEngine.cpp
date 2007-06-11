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
#include "base/log/Log.h"
#include "risse/include/risseObjectBase.h"


RISSE_DEFINE_SOURCE_ID(50344,48369,3431,18494,14208,60463,45295,19784);


//---------------------------------------------------------------------------
tRisaRisseScriptEngine::tRisaRisseScriptEngine()
{
	Engine = new tRisseScriptEngine();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaRisseScriptEngine::~tRisaRisseScriptEngine()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaRisseScriptEngine::Shutdown()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaRisseScriptEngine::RegisterGlobalObject(const tRisseString & name, const tRisseVariant & object)
{
	if(!Engine) return;
	// グローバルオブジェクトは tRisseObjectBase のはず・・・
	RISSE_ASSERT(dynamic_cast<tRisseObjectBase *>(Engine->GetGlobalObject().GetObjectInterface()) != NULL);
	static_cast<tRisseObjectBase *>(Engine->GetGlobalObject().GetObjectInterface())->RegisterNormalMember(name, object);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaRisseScriptEngine::EvaluateExpresisonAndPrintResultToConsole(const tRisseString & expression)
{
	// execute the expression
	tRisseString result_str;
	tRisseVariant result;
	try
	{
		Engine->Evaluate(expression, RISSE_WS_TR("console"), 0, &result, NULL, true);
	}
	catch(const tRisseVariant * e)
	{
		// An exception had been occured in console quick Risse expression evaluation
		result_str = tRisseString(RISSE_WS_TR("(Console)")) + expression +
			tRisseString(RISSE_WS_TR(" = (exception) ")) +
			e->operator tRisseString();
		tRisaLogger::Log(result_str, tRisaLogger::llError);
		return;
	}
	catch(...)
	{
		throw;
	}

	// success in console quick Risse expression evaluation
	result_str = tRisseString(RISSE_WS_TR("(Console) ")) + expression +
		tRisseString(RISSE_WS_TR(" = ")) + result.AsHumanReadable();
	tRisaLogger::Log(result_str);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaRisseScriptEngine::Evaluate(const tRisseString & script, const tRisseString & name,
				risse_size lineofs,
				tRisseVariant * result,
				const tRisseBindingInfo * binding, bool is_expression)
{
	Engine->Evaluate(script, name, lineofs, result, binding, is_expression);
}
//---------------------------------------------------------------------------

