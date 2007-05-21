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
class tRisseBindingInfo;
//---------------------------------------------------------------------------
//! @brief		警告情報等の通知インターフェース
//---------------------------------------------------------------------------
class tRisseLineOutputInterface : public tRisseCollectee
{
public:
	//! @brief		情報を通知する
	//! @param		info		情報
	virtual void Output(const tRisseString & info) = 0;
};
//---------------------------------------------------------------------------




// クラスの前方定義を行う
#define RISSE_INTERNALCLASSES_CLASS(X) class tRisse##X##Class;
#include "risseInternalClasses.inc"
#undef RISSE_INTERNALCLASSES_CLASS

//---------------------------------------------------------------------------
//! @brief		スクリプトエンジンクラス
//---------------------------------------------------------------------------
class tRisseScriptEngine : public tRisseCollectee
{
public:
	void * StartSentinel; //!< クラスインスタンスの開始位置

	// 各クラスインスタンスへのポインタを定義する
	#define RISSE_INTERNALCLASSES_CLASS(X) tRisse##X##Class * X##Class;
	#include "risseInternalClasses.inc"
	#undef RISSE_INTERNALCLASSES_CLASS

	void * EndSentinel; //!< クラスインスタンスの開始位置

private:
	static bool CommonObjectsInitialized;

protected:
	tRisseVariant GlobalObject; //!< グローバルオブジェクト
	tRisseLineOutputInterface *WarningOutput; //!< 警告情報の出力先

public:
	//! @brief		コンストラクタ
	tRisseScriptEngine();

	//! @brief		グローバルオブジェクトを得る
	tRisseVariant & GetGlobalObject() { return GlobalObject; }

	//! @brief		スクリプトを評価する
	//! @param		script			スクリプトの内容
	//! @param		name			スクリプトブロックの名称
	//! @param		lineofs			行オフセット(ドキュメント埋め込みスクリプト用に、
	//!								スクリプトのオフセットを記録できる)
	//! @param		result			実行の結果(NULL可)
	//! @param		binding			バインディング情報(NULLの場合はグローバルバインディング)
	//! @param		is_expresion	式モードかどうか(Risseのように文と式の区別を
	//!								する必要がない言語ではfalseでよい)
	void Evaluate(const tRisseString & script, const tRisseString & name,
					risse_size lineofs = 0,
					tRisseVariant * result = NULL,
					const tRisseBindingInfo * binding = NULL, bool is_expresion = false);

	//! @brief		警告情報の出力先を設定する
	//! @param		output		警告情報の出力先
	void SetWarningOutput(tRisseLineOutputInterface * output)  { WarningOutput = output; }

	//! @brief		警告情報の出力先を取得する
	//! @return		警告情報の出力先
	tRisseLineOutputInterface * GetWarningOutput() const  { return WarningOutput; }

	//! @brief		警告情報を出力する
	//! @param		info	警告情報
	void OutputWarning(const tRisseString & info) const
	{
		if(WarningOutput) WarningOutput->Output(info);
	}
};
//---------------------------------------------------------------------------
} // namespace Risse

#endif

