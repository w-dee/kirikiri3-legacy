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

#include "risse/include/risseScriptEngine.h"
#include "base/utils/Singleton.h"

//---------------------------------------------------------------------------
//! @brief		Risseスクリプトエンジンへのインターフェース
//---------------------------------------------------------------------------
class tRisaRisseScriptEngine : public singleton_base<tRisaRisseScriptEngine>
{
	tRisseScriptEngine *Engine; //!< スクリプトエンジンインスタンス

public:
	//! @brief		コンストラクタ
	tRisaRisseScriptEngine();

	//! @brief		デストラクタ
	~tRisaRisseScriptEngine();

	//! @brief		シャットダウン
	void Shutdown();

	tRisseScriptEngine * GetEngine() { return Engine; } //!< スクリプトエンジンを返す

	const tRisseVariant & GetGlobalObject()
		{ if(!Engine) return tRisseVariant::GetNullObject(); return Engine->GetGlobalObject(); }
		//!< グローバルオブジェクトを返す

	//! @brief		グローバルにオブジェクトを登録する
	//! @param		name    オブジェクトにつけたい名前
	//! @param		object  その名前で登録したいオブジェクト
	void RegisterGlobalObject(const tRisseString & name, const tRisseVariant & object);

	//! @brief		式を評価して結果をコンソールに表示する
	//! @param		expression 式
	void EvaluateExpresisonAndPrintResultToConsole(const tRisseString & expression);

	//! @brief		スクリプトを評価する
	//! @param		script			スクリプトの内容
	//! @param		name			スクリプトブロックの名称
	//! @param		lineofs			行オフセット(ドキュメント埋め込みスクリプト用に、
	//!								スクリプトのオフセットを記録できる)
	//! @param		result			実行の結果(NULL可)
	//! @param		binding			バインディング情報(NULLの場合はグローバルバインディング)
	//! @param		is_expression	式モードかどうか(Risseのように文と式の区別を
	//!								する必要がない言語ではfalseでよい)
	void Evaluate(const tRisseString & script, const tRisseString & name,
					risse_size lineofs = 0,
					tRisseVariant * result = NULL,
					const tRisseBindingInfo * binding = NULL, bool is_expression = false);
};
//---------------------------------------------------------------------------



#endif
