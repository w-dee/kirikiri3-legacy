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
#include "prec.h"
#include "base/script/RisseEngine.h"
#include "base/exception/RisaException.h"
#include "base/exception/UnhandledException.h"
#include "base/system/SystemBind.h"
#include "base/event/Event.h"
#include "base/log/Log.h"
#include "base/ui/editor/ScriptEditor.h"
#include "risse/include/risseScriptBlock.h"

RISSE_DEFINE_SOURCE_ID(28545,30194,3434,19984,56458,2209,37707,53134);



//---------------------------------------------------------------------------
//! @brief		スクリプト例外を表示する
//! @param		e		例外オブジェクト
//---------------------------------------------------------------------------
void tRisaUnhandledExceptionHandler::ShowScriptException(eRisse &e)
{
	// イベント配信を無効に
	if(tRisaEventSystem::alive())
		tRisaEventSystem::instance()->SetCanDeliverEvents(false);

	// ログ
	if(tRisaLogger::alive())
	{
		tRisaLogger::instance()->Log(
			RISSE_WS_TR("An exception had been occured"), tRisaLogger::llError);
		tRisaLogger::instance()->Log(
			e.GetMessageString(), tRisaLogger::llError);
	}

	// メッセージボックスを表示
	wxMessageBox(wxString(_("An exception had been occured")) + wxT("\n") +
		e.GetMessageString().AsWxString(), _("Exception"), wxICON_ERROR);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スクリプト例外を表示する
//! @param		e		例外オブジェクト
//---------------------------------------------------------------------------
void tRisaUnhandledExceptionHandler::ShowScriptException(eRisseScriptError &e)
{
	// イベント配信を無効に
	if(tRisaEventSystem::alive())
		tRisaEventSystem::instance()->SetCanDeliverEvents(false);

	// ログ
	if(tRisaLogger::alive())
	{
		tRisaLogger::instance()->Log(
			RISSE_WS_TR("An exception had been occured"), tRisaLogger::llError);
		tRisaLogger::instance()->Log(
			e.GetMessageString(), tRisaLogger::llError);
		if(e.GetTrace().GetLen() != 0)
			tRisaLogger::instance()->Log(
				ttstr(RISSE_WS_TR("Trace: ")) + e.GetTrace(), tRisaLogger::llError);
	}

	// スクリプトエディタを表示
	tRisaScriptEditorFrame *editor = new tRisaScriptEditorFrame();
	editor->SetContent(ttstr(e.GetBlockNoAddRef()->GetScript()).AsWxString());
	editor->SetReadOnly(true);
	editor->SetLinePosition(1+e.GetBlockNoAddRef()->SrcPosToLine(e.GetPosition() )
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
	bool result;
	tRisseVariantClosure clo;
	clo.Object = clo.ObjThis = NULL;

	try
	{
		// スクリプトエンジンを取得する
		if(!tRisaRisseScriptEngine::alive()) throw e; // スクリプトエンジンはすでに終了
		if(!tRisaSystemRegisterer::alive()) throw e; // "System" レジストラも終了

		// get System.exceptionHandler
		if(!tRisaSystemRegisterer::instance()->GetExceptionHandlerObject(clo))
			throw e; // System.exceptionHandler cannot be retrieved

		//---- 例外オブジェクトを取得するstart ----
		tRisseVariant obj;
		tRisseVariant msg(e.GetMessageString());
		tRisseVariant trace(e.GetTrace());
		RisseGetExceptionObject(
					tRisaRisseScriptEngine::instance()->GetEngineNoAddRef(),
					&obj, msg, &trace);
		//---- 例外オブジェクトを取得するend ----

		// execute clo
		tRisseVariant *pval[] =  { &obj };

		tRisseVariant res;

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

	if(!result) ShowScriptException(e);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ハンドルされなかった例外を処理する
//! @param		e		例外オブジェクト
//---------------------------------------------------------------------------
void tRisaUnhandledExceptionHandler::Process(eRisseScriptError &e)
{
	bool result;
	tRisseVariantClosure clo;
	clo.Object = clo.ObjThis = NULL;

	try
	{
		// スクリプトエンジンを取得する
		if(!tRisaRisseScriptEngine::alive()) throw e; // スクリプトエンジンはすでに終了
		if(!tRisaSystemRegisterer::alive()) throw e; // "System" レジストラも終了

		// get System.exceptionHandler
		if(!tRisaSystemRegisterer::instance()->GetExceptionHandlerObject(clo))
			throw e; // System.exceptionHandler cannot be retrieved

		//---- 例外オブジェクトを取得するstart ----
		tRisseVariant obj;
		tRisseVariant msg(e.GetMessageString());
		tRisseVariant trace(e.GetTrace());
		RisseGetExceptionObject(
				tRisaRisseScriptEngine::instance()->GetEngineNoAddRef(),
				&obj, msg);
		//---- 例外オブジェクトを取得するend ----

		// execute clo
		tRisseVariant *pval[] =  { &obj };

		tRisseVariant res;

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

	if(!result) ShowScriptException(e);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ハンドルされなかった例外を処理する
//! @param		e		例外オブジェクト
//---------------------------------------------------------------------------
void tRisaUnhandledExceptionHandler::Process(eRisse &e)
{
	bool result;
	tRisseVariantClosure clo;
	clo.Object = clo.ObjThis = NULL;

	try
	{
		// スクリプトエンジンを取得する
		if(!tRisaRisseScriptEngine::alive()) throw e; // スクリプトエンジンはすでに終了
		if(!tRisaSystemRegisterer::alive()) throw e; // "System" レジストラも終了

		// get System.exceptionHandler
		if(!tRisaSystemRegisterer::instance()->GetExceptionHandlerObject(clo))
			throw e; // System.exceptionHandler cannot be retrieved

		//---- 例外オブジェクトを取得するstart ----
		tRisseVariant obj;
		tRisseVariant msg(e.GetMessageString());
		RisseGetExceptionObject(
			tRisaRisseScriptEngine::instance()->GetEngineNoAddRef(),
			&obj, msg);
		//---- 例外オブジェクトを取得するend ----

		// execute clo
		tRisseVariant *pval[] =  { &obj };

		tRisseVariant res;

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

	if(!result) ShowScriptException(e);
}
//---------------------------------------------------------------------------


