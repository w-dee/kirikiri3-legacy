//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risseスクリプトエンジンの開始・終了・スクリプト実行などのインターフェース
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/script/RisseEngine.h"
#include "risse/include/risseError.h"

RISSE_DEFINE_SOURCE_ID(50344,48369,3431,18494,14208,60463,45295,19784);


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaRisseScriptEngine::tRisaRisseScriptEngine()
{
	Engine = new tRisse();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaRisseScriptEngine::~tRisaRisseScriptEngine()
{
	if(Engine)
	{
/*
#ifdef __WXDEBUG__
		wxFprintf(stderr, wxT("warning: tRisaRisseScriptEngine::instance().Shutdown() should be called before main() ends.\n"));
#endif
*/
		Shutdown();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		シャットダウン
//---------------------------------------------------------------------------
void tRisaRisseScriptEngine::Shutdown()
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
void tRisaRisseScriptEngine::RegisterGlobalObject(const risse_char *name,
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
