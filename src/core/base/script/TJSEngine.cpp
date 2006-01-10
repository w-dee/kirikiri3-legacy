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
	Engine->Shutdown();
	Engine->Release();
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
	tTJSVariant val(object, NULL);
	iTJSDispatch2 * global = Engine->GetGlobalNoAddRef();
	tjs_error er;
	er = global->PropSet(TJS_MEMBERENSURE, name, NULL, &val, global);
	if(TJS_FAILED(er))
		TJSThrowFrom_tjs_error(er, name);
}
//---------------------------------------------------------------------------
