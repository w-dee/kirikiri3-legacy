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

namespace Risa {
RISSE_DEFINE_SOURCE_ID(50344,48369,3431,18494,14208,60463,45295,19784);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tRisseScriptEngine::tRisseScriptEngine()
{
	ScriptEngine = new tScriptEngine();
	DefaultRTTI = new tRTTI(ScriptEngine);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseScriptEngine::~tRisseScriptEngine()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptEngine::Shutdown()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptEngine::RegisterGlobalObject(const tString & name, const tVariant & object)
{
	if(!ScriptEngine) return;
	// グローバルオブジェクトは tObjectBase のはず・・・
	RISSE_ASSERT(dynamic_cast<tObjectBase *>(ScriptEngine->GetGlobalObject().GetObjectInterface()) != NULL);
	static_cast<tObjectBase *>(ScriptEngine->GetGlobalObject().GetObjectInterface())->RegisterNormalMember(name, object);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptEngine::EvaluateExpresisonAndPrintResultToConsole(const tString & expression)
{
	// execute the expression
	tString result_str;
	tVariant result;
	try
	{
		ScriptEngine->Evaluate(expression, RISSE_WS_TR("console"), 0, &result, NULL, true);
	}
	catch(const tVariant * e)
	{
		// An exception had been occured in console quick Risse expression evaluation
		result_str = tString(RISSE_WS_TR("(Console)")) + expression +
			tString(RISSE_WS_TR(" = (exception %1) "), e->GetClassName()) +
			e->operator tString();
		tLogger::Log(result_str, tLogger::llError);
		return;
	}
	catch(...)
	{
		throw;
	}

	// success in console quick Risse expression evaluation
	result_str = tString(RISSE_WS_TR("(Console) ")) + expression +
		tString(RISSE_WS_TR(" = ")) + result.AsHumanReadable();
	tLogger::Log(result_str);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseScriptEngine::Evaluate(const tString & script, const tString & name,
				risse_size lineofs,
				tVariant * result,
				const tBindingInfo * binding, bool is_expression)
{
	ScriptEngine->Evaluate(script, name, lineofs, result, binding, is_expression);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa

