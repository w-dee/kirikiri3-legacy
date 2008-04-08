//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risseスクリプトエンジンの開始・終了・スクリプト実行などのインターフェース
//---------------------------------------------------------------------------
#ifndef ENGINEH
#define ENGINEH

#include "risse/include/risseScriptEngine.h"
#include "risa/common/Singleton.h"
#include "risse/include/risseObjectBase.h"
#include "risse/include/risseNativeBinder.h"
#include "risse/include/risseStaticStrings.h"
#include "risse/include/risseStringTemplate.h"
#include "risse/include/risseClass.h"
#include "risse/include/risseModule.h"
#include "risse/include/risseObjectClass.h"
#include "risse/include/risseExceptionClass.h"
#include "risse/include/builtin/risseBuiltinPackageInitializer.h"


namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		Risseスクリプトエンジンへのインターフェース
//---------------------------------------------------------------------------
class tRisseScriptEngine : public singleton_base<tRisseScriptEngine>
{
	tScriptEngine *ScriptEngine; //!< スクリプトエンジンインスタンス
	const tRTTI * DefaultRTTI; //!< デフォルトの RTTI (スクリプトエンジンへのポインタを含んでいるだけ)

public:
	//! @brief		コンストラクタ
	tRisseScriptEngine();

	//! @brief		デストラクタ
	~tRisseScriptEngine();

	//! @brief		シャットダウン
	void Shutdown();

	tScriptEngine * GetScriptEngine() { return ScriptEngine; } //!< スクリプトエンジンを返す

	const tRTTI * GetDefaultRTTI() const { return DefaultRTTI; }
		//!< デフォルトの RTTI を帰す

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


	//! @brief		スクリプトファイルを評価する
	//! @param		script			スクリプトの内容
	//! @param		result			実行の結果(NULL可)
	//! @param		binding			バインディング情報(NULLの場合はグローバルバインディング)
	//! @param		is_expression	式モードかどうか(Risseのように文と式の区別を
	//!								する必要がない言語ではfalseでよい)
	void EvaluateFile(const tString & filename,
					tVariant * result = NULL,
					const tBindingInfo * binding = NULL, bool is_expression = false);
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		パッケージのメンバを初期化するためのインターフェース
//---------------------------------------------------------------------------
class tPackageMemberInitializer : public tCollectee
{
public:
	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tPackageMemberInitializer() {;}

	//! @brief		パッケージのメンバを初期化する
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		パッケージ名
	//! @param		global		パッケージグローバル
	virtual void Initialize(tScriptEngine * engine, const tString & name,
		const tVariant & global) = 0;
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief		パッケージをRisseスクリプトエンジンに登録するためのシングルトンインスタンス
//---------------------------------------------------------------------------
class tPackage : public tBuiltinPackageInitializer
{
	gc_vector<tPackageMemberInitializer *> Initializers; //!< このパッケージに登録するメンバの一覧

	tString PackageName; //!< パッケージ名
	tScriptEngine * ScriptEngine; //!< スクリプトエンジンインスタンス(パッケージが初期化されるよりも前の状態ではNULL)
public:
	//! @brief		コンストラクタ
	//! @param		name		パッケージ名
	tPackage(const tString & name) :
		tBuiltinPackageInitializer(name), Initializers(),
		PackageName(name), ScriptEngine(NULL) {}

	//! @brief		パッケージにメンバを初期化するためのインターフェースを登録する
	//! @param		initializer		パッケージにメンバを初期化するためのインターフェース
	void AddInitializer(tPackageMemberInitializer * initializer)
	{
		// パッケージが初期化前ならば Initializers に追加、
		// 初期化後ならば直接パッケージグローバルに追加
		if(!ScriptEngine)
			Initializers.push_back(initializer);
		else
			initializer->Initialize(ScriptEngine, PackageName,
				ScriptEngine->GetPackageGlobal(PackageName));
	}

protected:
	//! @brief		パッケージを初期化する(スクリプトエンジンのtPackageManagerから呼ばれる)
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		パッケージ名
	//! @param		global		パッケージグローバル
	//! @note		オーバーライドしても良いがスーパークラスのこれも忘れずに呼ぶこと
	virtual void Initialize(tScriptEngine * engine, const tString & name,
		const tVariant & global)
	{
		// Initializers を 順にパッケージに追加
		for(gc_vector<tPackageMemberInitializer *>::iterator i = Initializers.begin();
			i != Initializers.end(); i++)
			(*i)->Initialize(engine, name, global);

		// スクリプトエンジンを設定(以降は AddInitializer を呼んだら
		// 直接パッケージグローバルにメンバが登録される)
		ScriptEngine = engine;
	}

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tPackage をスクリプトエンジンに登録するための
//!				シングルトンインスタンスのテンプレートクラス
//---------------------------------------------------------------------------
template <typename NameT>
class tPackageRegisterer :
	public singleton_base<tPackageRegisterer<NameT> >,
	depends_on<tRisseScriptEngine>
{
	tPackage * Package; //!< tPackage インスタンス
public:
	//! @brief		コンストラクタ
	tPackageRegisterer()
	{
		// ここらへんのプロセスについては tScriptEngine のコンストラクタも参照のこと
		tScriptEngine * engine = tRisseScriptEngine::instance()->GetScriptEngine();
		Package = new tPackage(NameT());
		RISSE_ASSERT(dynamic_cast<tPackage *>(Package));
		Package->RegisterInstance(engine);
	}

	//! @brief	GetPackage()->AddInitializer() へのショートカット
	//! @param		initializer		パッケージにメンバを初期化するためのインターフェース
	void AddInitializer(tPackageMemberInitializer * initializer)
	{
		Package->AddInitializer(initializer);
	}

	tPackage * GetPackage() const { return Package; } //!< tPackageインスタンスを得る
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		Risseクラス一つを保持するテンプレートクラス
//! @note		このクラスを一度でもどこかで使うと、
//!				そのクラスに対するシングルトンインスタンスが作成される。
//!				Risse 組み込みクラスに関してはこれではなく
//!				Risse の tScriptEngine からクラスインスタンスを取得すること。
//!				さもないと Risse と Risaシングルトン管理機構の２カ所でクラス
//!				インスタンスが作成されてしまう。
//---------------------------------------------------------------------------
template <typename ClassT>
class tClassHolder :
	public singleton_base<tClassHolder<ClassT> >,
	depends_on<tRisseScriptEngine>
{
	ClassT * Class; //!< クラスインスタンス

public:
	//! @brief		コンストラクタ
	tClassHolder()
	{
		Class = new ClassT(tRisseScriptEngine::instance()->GetScriptEngine());
	}


	//! @brief		クラスインスタンスを取得する
	//! @return		クラスインスタンス
	ClassT * GetClass() const { return Class; }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		Risseクラス一つを特定パッケージに登録するための
//!				シングルトンインスタンスのテンプレートクラス
//! @note		クラスインスタンスのシングルトンインスタンスも自動的に作成される。
//!				クラスインスタンスを取得するときは tClassHolder<> 経由で取得すること。
//---------------------------------------------------------------------------
template <typename NameT, typename ClassT>
class tClassRegisterer :
	private tPackageMemberInitializer,
	public singleton_base<tClassRegisterer<NameT, ClassT> >,
	depends_on<tPackageRegisterer<NameT> >,
	depends_on<tClassHolder<ClassT> >
{
public:
	//! @brief		コンストラクタ
	tClassRegisterer()
	{
		tPackageRegisterer<NameT>::instance()->
			GetPackage()->AddInitializer(this);
	}

private:
	//! @brief		パッケージのメンバを初期化する
	//!				(tPackageMemberInitializer::Initialize()オーバーライド)
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		パッケージ名
	//! @param		global		パッケージグローバル
	virtual void Initialize(tScriptEngine * engine, const tString & name,
		const tVariant & global)
	{
		tClassHolder<ClassT>::instance()->GetClass()->RegisterInstance(global);
	}
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa


#endif
