//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ハンドルされなかった例外のハンドル
//---------------------------------------------------------------------------

#include "risa/prec.h"
#include "risa/common/RisseEngine.h"
#include "risa/common/RisaException.h"
#include "risa/common/UnhandledException.h"
#include "risa/packages/risa/event/Event.h"
#include "risa/packages/risa/log/Log.h"
#include "risa/common/ui/editor/ScriptEditor.h"
#include "risseStaticStrings.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(28545,30194,3434,19984,56458,2209,37707,53134);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * スクリプト例外を表示する
 * @param e	例外オブジェクト
 */
void tUnhandledExceptionHandler::ShowScriptException(const tVariant * e)
{
	// イベント配信を無効に
	tEventSystem::instance()->SetCanDeliverEvents(false);

	// ログ
	tString message = e->operator tString();

	tString exception_head =
		tString(RISSE_WS_TR("An exception %1 had been occured"), e->GetClassName());

	tLogger::Log(exception_head, tLogger::llError);
	tLogger::Log(message, tLogger::llError);

	// スクリプトエディタはメインスレッドから投げられた例外のみに対して表示
	// (そのほかはログに表示するのみとする)
	if(::wxIsMainThread())
	{
		// 例外オブジェクトはトレースを持っているか？
		// 持っていればスクリプトエディタ
		tVariant trace_array = e->GetPropertyDirect_Object(ss_trace);
		tVariant top_sp = trace_array.Invoke_Object(mnIGet, risse_int64(0));
			// 先頭の source point

		if(!top_sp.IsVoid() && top_sp.GetType() == tVariant::vtObject)
		{
			// トレースを持っている
			tScriptEngine * engine = top_sp.GetObjectInterface()->GetRTTI()->GetScriptEngine();
			tVariant sb = top_sp.GetPropertyDirect(engine, ss_scriptBlock);
			tString script = (tString)sb.GetPropertyDirect(engine, ss_script);
			tVariant position = top_sp.GetPropertyDirect(engine, ss_position);
			risse_size line = (risse_int64)sb.Invoke(engine, ss_positionToLine, position);

			// スクリプトエディタを表示
			tScriptEditorFrame *editor = new tScriptEditorFrame();
			editor->SetContent(script.AsWxString());
			editor->SetReadOnly(true);
			editor->SetLinePosition(line);
			editor->SetStatusString(message.AsWxString());
			editor->Show(true);
		}
	}

	// メッセージボックスを表示
	wxMessageBox(exception_head.AsWxString() + wxT("\n") +
		message.AsWxString(), _("Exception"), wxICON_ERROR);

}
//---------------------------------------------------------------------------

#if 0
//---------------------------------------------------------------------------
/**
 * ハンドルされなかった例外を処理する
 * @param e	例外オブジェクト
 * @note	Process は 3つ同じような内容が並ぶが、
 *			例外オブジェクトを取得するstart～例外オブジェクトを取得するend
 *			間の記述内容が違うだけである。
 */
void tUnhandledExceptionHandler::Process(eRisseScriptException &e)
{
	bool result = false;
	tVariantClosure clo;
	clo.Object = clo.ObjThis = NULL;

	do
	{
		try
		{
			// スクリプトエンジンを取得する
			tRisseScriptEngine::pointer script_engine = tRisseScriptEngine::instance();
			tSystemRegisterer::pointer system_registerer = tSystemRegisterer::instance();
			if(!script_engine) break; // スクリプトエンジンはすでに終了
			if(!system_registerer) break; // "System" レジストラも終了

			// get System.exceptionHandler
			if(!system_registerer->GetExceptionHandlerObject(clo))
				break; // System.exceptionHandler cannot be retrieved

			//---- 例外オブジェクトを取得するstart ----
			tVariant obj;
			tVariant msg(e.GetMessageString());
			tVariant trace(e.GetTrace());
			GetExceptionObject(
						script_engine->GetEngineNoAddRef(),
						&obj, msg, &trace);
			//---- 例外オブジェクトを取得するend ----

			// execute clo
			tVariant *pval[] =  { &obj };

			tVariant res;

			clo.FuncCall(0, NULL, NULL, &res, 1, pval, NULL);

			result = res.operator bool();
		}
		catch(eRisseScriptError &e)
		{
			clo.Release();
			ShowScriptException(e);
			return;
		}
		catch(eRisse &e)
		{
			clo.Release();
			ShowScriptException(e);
			return;
		}
		catch(...)
		{
			clo.Release();
			throw;
		}
		clo.Release();
	} while(0);

	if(!result) ShowScriptException(e);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * ハンドルされなかった例外を処理する
 * @param e	例外オブジェクト
 */
void tUnhandledExceptionHandler::Process(eRisseScriptError &e)
{
	bool result = false;
	tVariantClosure clo;
	clo.Object = clo.ObjThis = NULL;

	do
	{
		try
		{
			// スクリプトエンジンを取得する
			tRisseScriptEngine::pointer script_engine = tRisseScriptEngine::instance();
			tSystemRegisterer::pointer system_registerer = tSystemRegisterer::instance();
			if(!script_engine) break; // スクリプトエンジンはすでに終了
			if(!system_registerer) break; // "System" レジストラも終了

			// get System.exceptionHandler
			if(!system_registerer->GetExceptionHandlerObject(clo))
				break; // System.exceptionHandler cannot be retrieved

			//---- 例外オブジェクトを取得するstart ----
			tVariant obj;
			tVariant msg(e.GetMessageString());
			tVariant trace(e.GetTrace());
			GetExceptionObject(
					script_engine->GetEngineNoAddRef(),
					&obj, msg);
			//---- 例外オブジェクトを取得するend ----

			// execute clo
			tVariant *pval[] =  { &obj };

			tVariant res;

			clo.FuncCall(0, NULL, NULL, &res, 1, pval, NULL);

			result = res.operator bool();
		}
		catch(eRisseScriptError &e)
		{
			clo.Release();
			ShowScriptException(e);
			return;
		}
		catch(eRisse &e)
		{
			clo.Release();
			ShowScriptException(e);
			return;
		}
		catch(...)
		{
			clo.Release();
			throw;
		}
		clo.Release();
	} while(0);

	if(!result) ShowScriptException(e);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * ハンドルされなかった例外を処理する
 * @param e	例外オブジェクト
 */
void tUnhandledExceptionHandler::Process(eRisse &e)
{
	bool result = false;
	tVariantClosure clo;
	clo.Object = clo.ObjThis = NULL;

	do
	{
		try
		{
			// スクリプトエンジンを取得する
			tRisseScriptEngine::pointer script_engine = tRisseScriptEngine::instance();
			tSystemRegisterer::pointer system_registerer = tSystemRegisterer::instance();
			if(!script_engine) break; // スクリプトエンジンはすでに終了
			if(!system_registerer) break; // "System" レジストラも終了

			// get System.exceptionHandler
			if(!system_registerer->GetExceptionHandlerObject(clo))
				break; // System.exceptionHandler cannot be retrieved

			//---- 例外オブジェクトを取得するstart ----
			tVariant obj;
			tVariant msg(e.GetMessageString());
			GetExceptionObject(
				script_engine->GetEngineNoAddRef(),
				&obj, msg);
			//---- 例外オブジェクトを取得するend ----

			// execute clo
			tVariant *pval[] =  { &obj };

			tVariant res;

			clo.FuncCall(0, NULL, NULL, &res, 1, pval, NULL);

			result = res.operator bool();
		}
		catch(eRisseScriptError &e)
		{
			clo.Release();
			ShowScriptException(e);
			return;
		}
		catch(eRisse &e)
		{
			clo.Release();
			ShowScriptException(e);
			return;
		}
		catch(...)
		{
			clo.Release();
			throw;
		}
		clo.Release();
	} while(0);

	if(!result) ShowScriptException(e);
}
//---------------------------------------------------------------------------


#endif
//---------------------------------------------------------------------------
} // namespace Risa


