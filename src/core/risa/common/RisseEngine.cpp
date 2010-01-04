//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risseスクリプトエンジンの開始・終了・スクリプト実行などのインターフェース
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/common/RisseEngine.h"
#include "risa/packages/risa/log/Log.h"
#include "risseObjectBase.h"
#include "risa/packages/risa/fs/FSManager.h"
#include "risseExceptionClass.h"

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
void tRisseScriptEngine::EvaluateFile(const tString & filename,
				tVariant * result, const tBindingInfo * binding, bool is_expression)
{
	// ファイルを読み込む
	// TODO: ここは正規のテキスト読み込みルーチンを通じてファイルを読み込むべき
	tStreamAdapter stream(tFileSystemManager::instance()->Open(filename, tFileOpenModes::omRead));

	risse_uint64 size64 = stream.GetSize();
	risse_size size = static_cast<risse_size>(size64);

	if(size != size64) tIOExceptionClass::Throw(tString(RISSE_WS_TR("too large file: %1"), filename));

	unsigned char * buf = new (PointerFreeGC) unsigned char [size + 1];
	stream.ReadBuffer(buf, size);
	buf[size] = '\0';

	// 評価を行う
	Evaluate(reinterpret_cast<char*>(buf), filename, 0, result, binding, is_expression);
}
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
} // namespace Risa

