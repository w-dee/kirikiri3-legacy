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
#ifndef RisseENGINEH
#define RisseENGINEH

#include "risse/include/risse.h"
#include "base/utils/Singleton.h"

#if 0
//---------------------------------------------------------------------------
//! @brief		Risseスクリプトエンジンへのインターフェース
//---------------------------------------------------------------------------
class tRisaRisseScriptEngine : public singleton_base<tRisaRisseScriptEngine>
{
	tRisse *Engine;

public:
	//! @brief		コンストラクタ
	tRisaRisseScriptEngine();

	//! @brief		デストラクタ
	~tRisaRisseScriptEngine();

	//! @brief		シャットダウン
	void Shutdown();

	tRisse * GetEngineNoAddRef() { return Engine; } //!< スクリプトエンジンを返す
	iRisseDispatch2 * GetGlobalNoAddRef()
		{ if(!Engine) return NULL; return Engine->GetGlobalNoAddRef(); } //!< スクリプトエンジンを返す

	//! @brief		グローバルにオブジェクトを登録する
	//! @param		name    オブジェクトにつけたい名前
	//! @param		object  その名前で登録したいオブジェクト
	void RegisterGlobalObject(const risse_char *name, iRisseDispatch2 * object);

	//! @brief		式を評価して結果をコンソールに表示する
	//! @param		expression 式
	void EvalExpresisonAndPrintResultToConsole(const tRisseString & expression);

	//! @brief		スクリプトブロックを実行する
	//! @param		script		スクリプト
	//! @param		result		実行結果を受け取るtRisseVariant型オブジェクトへのポインタ(要らない場合はNULLを)
	//! @param		context		実行を行うコンテキスト(globalで動かしたい場合はNULL)
	//! @param		name		スクリプトブロック名
	//! @param		lineofs		このスクリプトブロックの(オリジナルのファイル全体に対する)開始行
	void ExecuteScript(const tRisseString &script, tRisseVariant *result = NULL,
		iRisseDispatch2 *context = NULL,
		const tRisseString *name = NULL, risse_int lineofs = 0);
};
//---------------------------------------------------------------------------

#endif


#endif
