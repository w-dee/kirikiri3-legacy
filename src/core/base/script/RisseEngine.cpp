//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risseスクリプトエンジンの開始・終了・スクリプト実行などのインターフェース
//---------------------------------------------------------------------------
#include "prec.h"
#include "RisseEngine.h"
#include "risseError.h"
RISSE_DEFINE_SOURCE_ID(2200);


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTVPRisseScriptEngine::tTVPRisseScriptEngine()
{
	Engine = new tRisse();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPRisseScriptEngine::~tTVPRisseScriptEngine()
{
	if(Engine)
	{
/*
#ifdef __WXDEBUG__
		wxFprintf(stderr, wxT("warning: tTVPRisseScriptEngine::instance().Shutdown() should be called before main() ends.\n"));
#endif
*/
		Shutdown();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		シャットダウン
//---------------------------------------------------------------------------
void tTVPRisseScriptEngine::Shutdown()
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
void tTVPRisseScriptEngine::RegisterGlobalObject(const risse_char *name,
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
