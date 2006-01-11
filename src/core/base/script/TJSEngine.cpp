//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief TJS3スクリプトエンジンの開始・終了・スクリプト実行などのインターフェース
//---------------------------------------------------------------------------
#include "prec.h"
#include "TJSEngine.h"
#include "tjsError.h"
TJS_DEFINE_SOURCE_ID(2200);


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTVPTJS3ScriptEngine::tTVPTJS3ScriptEngine()
{
	Engine = new tTJS();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPTJS3ScriptEngine::~tTVPTJS3ScriptEngine()
{
	if(Engine)
	{
#ifdef __WXDEBUG__
		wxFprintf(stderr, wxT("warning: tTVPTJS3ScriptEngine::instance().Shutdown() should be called before main() ends.\n"));
#endif
		Shutdown();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		シャットダウン
//---------------------------------------------------------------------------
void tTVPTJS3ScriptEngine::Shutdown()
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
void tTVPTJS3ScriptEngine::RegisterGlobalObject(const tjs_char *name,
	iTJSDispatch2 * object)
{
	if(!Engine) return;
	tTJSVariant val(object, NULL);
	iTJSDispatch2 * global = Engine->GetGlobalNoAddRef();
	tjs_error er;
	er = global->PropSet(TJS_MEMBERENSURE, name, NULL, &val, global);
	if(TJS_FAILED(er))
		TJSThrowFrom_tjs_error(er, name);
}
//---------------------------------------------------------------------------
