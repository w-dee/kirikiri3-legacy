//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief マルチスレッド関連ユーティリティ
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/utils/RisaThread.h"
#include "base/script/RisseEngine.h"
#include "risse/include/risseExceptionClass.h"
#include "risa/packages/risa/log/Log.h"

namespace Risa {
RISSE_DEFINE_SOURCE_ID(24795,6838,687,16805,21894,40786,6545,48673);
//---------------------------------------------------------------------------
void tThread::CallExecute()
{
	// execute を呼ぶが、発生した例外は受け取ってコンソールに表示する
	try
	{
		try
		{
			Execute();
		}
		catch(const tTemporaryException * te)
		{
			volatile tRisseScriptEngine::tLocker lock;
			if(tRisseScriptEngine::alive())
			{
				te->ThrowConverted(tRisseScriptEngine::instance()->GetScriptEngine());
			}
			else
			{
				throw new tVariant(te->GetExceptionClassName());
			}
		}
	}
	catch(const tVariant * e)
	{
		// 例外を受け取った
		tLogger::Log(tString(RISSE_WS_TR("Abnormal thread abortion due to unhandled exception : %1 (%2)"),
			e->operator tString(), e->GetClassName()), tLogger::llCritical);
	}
	catch(...)
	{
		// 例外を受け取った
		tLogger::Log(tString(RISSE_WS_TR("Abnormal thread abortion due to unhandled exception : unknown exception")),
				tLogger::llCritical);
	}
}
//---------------------------------------------------------------------------
} // namespace Risa



