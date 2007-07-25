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
#ifndef ENGINEH
#define ENGINEH

#include "risse/include/risseScriptEngine.h"
#include "base/utils/Singleton.h"
#include "risse/include/risseObjectBase.h"
#include "risse/include/risseNativeBinder.h"
#include "risse/include/risseStaticStrings.h"
#include "risse/include/risseStringTemplate.h"
#include "risse/include/risseClass.h"
#include "risse/include/risseObjectClass.h"
#include "risse/include/risseExceptionClass.h"


namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		Risseスクリプトエンジンへのインターフェース
//---------------------------------------------------------------------------
class tRisseScriptEngine : public singleton_base<tRisseScriptEngine>
{
	tScriptEngine *ScriptEngine; //!< スクリプトエンジンインスタンス

public:
	//! @brief		コンストラクタ
	tRisseScriptEngine();

	//! @brief		デストラクタ
	~tRisseScriptEngine();

	//! @brief		シャットダウン
	void Shutdown();

	tScriptEngine * GetScriptEngine() { return ScriptEngine; } //!< スクリプトエンジンを返す

	const tVariant & GetGlobalObject()
		{ if(!ScriptEngine) return tVariant::GetNullObject();
		  return ScriptEngine->GetGlobalObject(); }
		//!< グローバルオブジェクトを返す

	//! @brief		グローバルにオブジェクトを登録する
	//! @param		name    オブジェクトにつけたい名前
	//! @param		object  その名前で登録したいオブジェクト
	void RegisterGlobalObject(const tString & name, const tVariant & object);

	//! @brief		式を評価して結果をコンソールに表示する
	//! @param		expression 式
	void EvaluateExpresisonAndPrintResultToConsole(const tString & expression);

	//! @brief		スクリプトを評価する
	//! @param		script			スクリプトの内容
	//! @param		name			スクリプトブロックの名称
	//! @param		lineofs			行オフセット(ドキュメント埋め込みスクリプト用に、
	//!								スクリプトのオフセットを記録できる)
	//! @param		result			実行の結果(NULL可)
	//! @param		binding			バインディング情報(NULLの場合はグローバルバインディング)
	//! @param		is_expression	式モードかどうか(Risseのように文と式の区別を
	//!								する必要がない言語ではfalseでよい)
	void Evaluate(const tString & script, const tString & name,
					risse_size lineofs = 0,
					tVariant * result = NULL,
					const tBindingInfo * binding = NULL, bool is_expression = false);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		クラスインスタンスやモジュールインスタンスを
//!				スクリプトエンジンに登録するためのテンプレートクラス
//---------------------------------------------------------------------------
template <typename ClassT>
class tRisseClassRegisterer :
	public singleton_base<tRisseClassRegisterer<ClassT> >,
	depends_on<tRisseScriptEngine>
{
	ClassT * ClassInstance;
public:
	//! @brief		コンストラクタ
	tRisseClassRegisterer()
	{
		// ここらへんのプロセスについては tScriptEngine のコンストラクタも参照のこと
		tScriptEngine * engine = tRisseScriptEngine::instance()->GetScriptEngine();
		ClassT *class_instance = new ClassT(engine);
		ClassInstance = class_instance;
		class_instance->RegisterInstance(engine->GetGlobalObject());
	}

	ClassT * GetClassInstance() const { return ClassInstance; } //!< クラスインスタンスを得る
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


#endif
