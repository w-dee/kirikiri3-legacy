//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

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
class tBindingInfo;
class tPackageManager;
class tScriptEngine;
//---------------------------------------------------------------------------
/**
 * パッケージ検索のためのインターフェース
 */
class tPackageFileSystemInterface : public tCollectee
{
public:
	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	virtual ~tPackageFileSystemInterface() {}

	/**
	 * 指定ディレクトリにあるファイル名をすべて列挙する
	 * @param dir	ディレクトリ(区切りには '/' が用いられる)
	 * @param files	そこにあるファイル/ディレクトリ名一覧
	 * @note	. で始まるディレクトリやファイル,隠しファイルは含めなくて良い。
	 *			ディレクトリの場合はfilesの最後を '/' で終わらせること。
	 *			files は呼び出し側で最初に clear() しておくこと。
	 */
	virtual void List(const tString & dir, gc_vector<tString> & files) = 0;

	/**
	 * ファイル種別を得る
	 * @param file	ファイル名
	 * @return	種別(0=ファイルが存在しない, 1=ファイル, 2=ディレクトリ)
	 */
	virtual int GetType(const tString & file) = 0;

	/**
	 * ファイルを読み込む
	 * @param file	ファイル名
	 * @return	読み込まれたファイルの中身
	 */
	virtual tString ReadFile(const tString & file) = 0;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 組み込みパッケージのための初期化用インターフェース
 */
class tBuiltinPackageInitializerInterface : public tCollectee
{
public:
	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	virtual ~tBuiltinPackageInitializerInterface() {}

	/**
	 * パッケージを初期化する
	 * @param engine	スクリプトエンジンインスタンス
	 * @param name		パッケージ名
	 * @param global	パッケージグローバル
	 */
	virtual void Initialize(tScriptEngine * engine, const tString & name,
		const tVariant & global) = 0;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 警告情報等の通知インターフェース
 */
class tLineOutputInterface : public tCollectee
{
public:
public:
	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	virtual ~tLineOutputInterface() {}

	/**
	 * 情報を通知する
	 * @param info	情報
	 * @note	警告情報は複数スレッドから同時に出力される可能性があるので注意
	 */
	virtual void Output(const tString & info) = 0;
};
//---------------------------------------------------------------------------




// クラスの前方定義を行う
#define RISSE_BUILTINCLASSES_CLASS(X) class t##X##Class;
#include "risseBuiltinClasses.inc"
#undef RISSE_BUILTINCLASSES_CLASS
#define RISSE_BUILTINPACKAGES_PACKAGE(X) class t##X##PackageInitializer;
#include "risseBuiltinPackages.inc"
#undef RISSE_BUILTINPACKAGES_PACKAGE

//---------------------------------------------------------------------------
/**
 * スクリプトエンジンクラス
 */
class tScriptEngine : public tCollectee
{
public:
	void * StartSentinel; //!< クラスインスタンス・パッケージイニシャライザの開始位置

	// 各クラスインスタンスへのポインタを定義する
	#define RISSE_BUILTINCLASSES_CLASS(X) t##X##Class * X##Class;
	#include "risseBuiltinClasses.inc"
	#undef RISSE_BUILTINCLASSES_CLASS
	// 各パッケージイニシャライザへのポインタを定義する
	#define RISSE_BUILTINPACKAGES_PACKAGE(X) t##X##PackageInitializer * X##PackageInitializer;
	#include "risseBuiltinPackages.inc"
	#undef RISSE_BUILTINPACKAGES_PACKAGE

	void * EndSentinel; //!< クラスインスタンス・パッケージイニシャライザの開始位置

private:
	static bool CommonObjectsInitialized;

protected:
	tPackageManager * PackageManager; //!< パッケージマネージャオブジェクト
	tVariant RissePackageGlobal; //!< "risse" パッケージのグローバルオブジェクト
	tVariant MainPackageGlobal; //!< "main" パッケージのグローバルオブジェクト
	tLineOutputInterface *WarningOutput; //!< 警告情報の出力先
	tPackageFileSystemInterface * PackageFileSystem; //!< パッケージ読み込み用のファイルシステムインターフェース

public:
	/**
	 * スクリプトエンジンの動作オプション用構造体
	 */
	struct tOptions
	{
		bool AssertEnabled;  //!< assert によるテストが有効かどうか

		/**
		 * デフォルトコンストラクタ
		 */
		tOptions()
		{
			AssertEnabled = false;
		}

		/**
		 * 比較演算子
		 */
		bool operator ==(const tOptions & rhs) const
		{
			return AssertEnabled == rhs.AssertEnabled;
		}
	};

private:
	tOptions Options; //!< オプション

public:
	/**
	 * コンストラクタ
	 */
	tScriptEngine();

	/**
	 * "risse" パッケージのグローバルオブジェクトを得る
	 */
	const tVariant & GetRissePackageGlobal() const { return RissePackageGlobal; }

	/**
	 * "main" パッケージのグローバルオブジェクトを得る
	 */
	const tVariant & GetMainPackageGlobal() const { return MainPackageGlobal; }

	/**
	 * スクリプトを評価する
	 * @param script		スクリプトの内容
	 * @param name			スクリプトブロックの名称
	 * @param lineofs		行オフセット(ドキュメント埋め込みスクリプト用に、
	 *						スクリプトのオフセットを記録できる)
	 * @param result		実行の結果(NULL可)
	 * @param binding		バインディング情報(NULLの場合は"main"パッケージグローバル)
	 * @param is_expression	式モードかどうか(Risseのように文と式の区別を
	 *						する必要がない言語ではfalseでよい)
	 */
	void Evaluate(
					const tString & script, const tString & name,
					risse_size lineofs = 0,
					tVariant * result = NULL,
					const tBindingInfo * binding = NULL, bool is_expression = false);

	/**
	 * 警告情報の出力先を設定する
	 * @param output	警告情報の出力先
	 * @note	警告情報は複数スレッドから同時に出力される可能性があるので注意
	 */
	void SetWarningOutput(tLineOutputInterface * output)  { WarningOutput = output; }

	/**
	 * 警告情報の出力先を取得する
	 * @return	警告情報の出力先
	 */
	tLineOutputInterface * GetWarningOutput() const  { return WarningOutput; }

	/**
	 * パッケージ読み込み用のファイルシステムインターフェースを設定する
	 * @param intf	パッケージ読み込み用のファイルシステムインターフェース
	 */
	void SetPackageFileSystem(tPackageFileSystemInterface * intf) { PackageFileSystem = intf; }

	/**
	 * パッケージ読み込み用のファイルシステムインターフェースを取得する
	 * @return	パッケージ読み込み用のファイルシステムインターフェース
	 */
	tPackageFileSystemInterface * GetPackageFileSystem() const { return PackageFileSystem; }

	/**
	 * 警告情報を出力する
	 * @param info	警告情報
	 */
	void OutputWarning(const tString & info) const
	{
		if(WarningOutput) WarningOutput->Output(info);
	}

	/**
	 * オプション情報を得る
	 */
	const tOptions & GetOptions() const { return Options; }

	/**
	 * assertion が有効かどうかを得る
	 * @return	assertion が有効かどうか
	 */
	bool GetAssertionEnabled() const { return Options.AssertEnabled; }

	/**
	 * assertion が有効かどうかを設定する
	 * @param b	asssrtion が有効かどうか
	 */
	void SetAssertionEnabled(bool b) { Options.AssertEnabled = b; }

	/**
	 * パッケージマネージャを得る
	 * @return	パッケージマネージャ
	 */
	tPackageManager * GetPackageManager() const { return PackageManager; }

	/**
	 * 指定されたパッケージのパッケージグローバルを得る
	 * @return	パッケージグローバル
	 * @note	もしパッケージが初期化されていない場合は初期化される。
	 *			tPackageManager::GetPackageGlobal() へのショートカット。
	 */
	tVariant GetPackageGlobal(const tString & name);

	/**
	 * 組み込みパッケージの情報を追加する
	 * @param package	パッケージ名
	 * @param init		イニシャライザ
	 *					(パッケージが初めて初期化される際に呼ばれる)
	 * @note	tPackageManager::AddBuiltinPackage() へのショートカット。
	 */
	void AddBuiltinPackage(const tString & package,
		tBuiltinPackageInitializerInterface * init);
};
//---------------------------------------------------------------------------
} // namespace Risse

#endif

