//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief スクリプトエンジン管理
//---------------------------------------------------------------------------
#ifndef risseScriptEngineH
#define risseScriptEngineH

#include "risseTypes.h"
#include "risseString.h"
#include "risseVariant.h"
#include "risseAssert.h"
#include "risseGC.h"
#include "risseScriptEngine.h"

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief		スクリプトエンジンクラス
//---------------------------------------------------------------------------
class tRisseScriptEngine : public tRisseCollectee
{
private:
	static bool CommonObjectsInitialized;

protected:
	tRisseVariant GlobalObject; //!< グローバルオブジェクト

public:
	//! @brief		コンストラクタ
	tRisseScriptEngine();

	//! @brief		グローバルオブジェクトを得る
	tRisseVariant & GetGlobalObject() { return GlobalObject; }

	//! @brief		スクリプトを評価する
	//! @param		script		スクリプトの内容
	//! @param		name		スクリプトブロックの名称
	//! @param		lineofs		行オフセット(ドキュメント埋め込みスクリプト用に、
	//!							スクリプトのオフセットを記録できる)
	//! @param		result			実行の結果(NULL可)
	//! @param		is_expression	式評価モードかどうか
	void Evaluate(const tRisseString & script, const tRisseString & name,
					risse_size lineofs = 0,
					tRisseVariant * result = NULL, bool is_expression = false,
					const tRisseVariant & context = tRisseVariant::GetNullObject());
};
//---------------------------------------------------------------------------
} // namespace Risse

#endif

