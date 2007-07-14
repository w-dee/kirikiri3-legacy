//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ハンドルされなかった例外のハンドル
//---------------------------------------------------------------------------

#include "prec.h"
#include "base/script/RisseEngine.h"
#include "base/exception/RisaException.h"
#include "base/exception/UnhandledException.h"
#include "base/system/SystemBind.h"
#include "base/event/Event.h"
#include "base/log/Log.h"
#include "base/ui/editor/ScriptEditor.h"
#include "risse/include/risseStaticStrings.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(28545,30194,3434,19984,56458,2209,37707,53134);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		スクリプト例外を表示する
//! @param		e		例外オブジェクト
//---------------------------------------------------------------------------
void tRisaUnhandledExceptionHandler::ShowScriptException(const tVariant * e)
{
	// イベント配信を無効に
	tRisaEventSystem::instance()->SetCanDeliverEvents(false);

	// ログ
	tString message = e->operator tString();

	tRisaLogger::Log(
		RISSE_WS_TR("An exception had been occured"), tRisaLogger::llError);
	tRisaLogger::Log(
		message, tRisaLogger::llError);

	// 例外オブジェクトはトレースを持っているか？
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
		tRisaScriptEditorFrame *editor = new tRisaScriptEditorFrame();
		editor->SetContent(script.AsWxString());
		editor->SetReadOnly(true);
		editor->SetLinePosition(line);
		editor->SetStatusString(message.AsWxString());
		editor->Show(true);
	}

	// メッセージボックスを表示
	wxMessageBox(wxString(_("An exception had been occured")) + wxT("\n") +
		message.AsWxString(), _("Exception"), wxICON_ERROR);

}
//---------------------------------------------------------------------------


#if 0
//---------------------------------------------------------------------------
//! @brief		スクリプト例外を表示する
//! @param		e		例外オブジェクト
//---------------------------------------------------------------------------
void tRisaUnhandledExceptionHandler::ShowScriptException(eRisseScriptError &e)
{
	// イベント配信を無効に
	if(tRisaEventSystem::pointer r = tRisaEventSystem::instance())
		r->SetCanDeliverEvents(false);

	// ログ
	if(tRisaLogger::alive())
	{
		tRisaLogger::Log(
			RISSE_WS_TR("An exception had been occured"), tRisaLogger::llError);
		tRisaLogger::Log(
			e.GetMessageString(), tRisaLogger::llError);
		if(e.GetTrace().GetLen() != 0)
			tRisaLogger::Log(
				tString(RISSE_WS_TR("Trace: ")) + e.GetTrace(), tRisaLogger::llError);
	}

	// スクリプトエディタを表示
	tRisaScriptEditorFrame *editor = new tRisaScriptEditorFrame();
	editor->SetContent(tString(e.GetBlockNoAddRef()->GetScript()).AsWxString());
	editor->SetReadOnly(true);
	editor->SetLinePosition(e.GetBlockNoAddRef()->SrcPosToLine(e.GetPosition() )
			- e.GetBlockNoAddRef()->GetLineOffset());
	editor->SetStatusString(e.GetMessageString().AsWxString());
	editor->Show(true);

	// メッセージボックスを表示
	wxMessageBox(wxString(_("An exception had been occured")) + wxT("\n") +
		e.GetMessageString().AsWxString(), _("Exception"), wxICON_ERROR);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ハンドルされなかった例外を処理する
//! @param		e		例外オブジェクト
//! @note		Process は 3つ同じような内容が並ぶが、
//!				例外オブジェクトを取得するstart～例外オブジェクトを取得するend
//!				間の記述内容が違うだけである。
//---------------------------------------------------------------------------
void tRisaUnhandledExceptionHandler::Process(eRisseScriptException &e)
{
	bool result = false;
	tVariantClosure clo;
	clo.Object = clo.ObjThis = NULL;

	do
	{
		try
		{
			// スクリプトエンジンを取得する
			tRisaRisseScriptEngine::pointer script_engine = tRisaRisseScriptEngine::instance();
			tRisaSystemRegisterer::pointer system_registerer = tRisaSystemRegisterer::instance();
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
//! @brief		ハンドルされなかった例外を処理する
//! @param		e		例外オブジェクト
//---------------------------------------------------------------------------
void tRisaUnhandledExceptionHandler::Process(eRisseScriptError &e)
{
	bool result = false;
	tVariantClosure clo;
	clo.Object = clo.ObjThis = NULL;

	do
	{
		try
		{
			// スクリプトエンジンを取得する
			tRisaRisseScriptEngine::pointer script_engine = tRisaRisseScriptEngine::instance();
			tRisaSystemRegisterer::pointer system_registerer = tRisaSystemRegisterer::instance();
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
//! @brief		ハンドルされなかった例外を処理する
//! @param		e		例外オブジェクト
//---------------------------------------------------------------------------
void tRisaUnhandledExceptionHandler::Process(eRisse &e)
{
	bool result = false;
	tVariantClosure clo;
	clo.Object = clo.ObjThis = NULL;

	do
	{
		try
		{
			// スクリプトエンジンを取得する
			tRisaRisseScriptEngine::pointer script_engine = tRisaRisseScriptEngine::instance();
			tRisaSystemRegisterer::pointer system_registerer = tRisaSystemRegisterer::instance();
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


