//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
//---------------------------------------------------------------------------
//! @brief		パッケージ検索のためのインターフェース
//---------------------------------------------------------------------------
class tPackageFileSystemInterface : public tCollectee
{
public:
	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tPackageFileSystemInterface() {}

	//! @brief		指定ディレクトリにあるファイル名をすべて列挙する
	//! @param		dir		ディレクトリ(区切りには '/' が用いられる)
	//! @param		files	そこにあるファイル/ディレクトリ名一覧
	//! @note		. で始まるディレクトリやファイル,隠しファイルは含めなくて良い。
	//!				ディレクトリの場合はfilesの最後を '/' で終わらせること。
	//!				files は呼び出し側で最初に clear() しておくこと。
	virtual void List(const tString & dir, gc_vector<tString> & files) = 0;

	//! @brief		ファイル種別を得る
	//! @param		file	ファイル名
	//! @return		種別(0=ファイルが存在しない, 1=ファイル, 2=ディレクトリ)
	virtual int GetType(const tString & file) = 0;

	//! @brief		ファイルを読み込む
	//! @param		file	ファイル名
	//! @return		読み込まれたファイルの中身
	virtual tString ReadFile(const tString & file) = 0;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		警告情報等の通知インターフェース
//---------------------------------------------------------------------------
class tLineOutputInterface : public tCollectee
{
public:
public:
	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tLineOutputInterface() {}

	//! @brief		情報を通知する
	//! @param		info		情報
	//! @note		警告情報は複数スレッドから同時に出力される可能性があるので注意
	virtual void Output(const tString & info) = 0;
};
//---------------------------------------------------------------------------




// クラスの前方定義を行う
#define RISSE_INTERNALCLASSES_CLASS(X) class t##X##Class;
#include "risseInternalClasses.inc"
#undef RISSE_INTERNALCLASSES_CLASS

//---------------------------------------------------------------------------
//! @brief		スクリプトエンジンクラス
//---------------------------------------------------------------------------
class tScriptEngine : public tCollectee
{
public:
	void * StartSentinel; //!< クラスインスタンスの開始位置

	// 各クラスインスタンスへのポインタを定義する
	#define RISSE_INTERNALCLASSES_CLASS(X) t##X##Class * X##Class;
	#include "risseInternalClasses.inc"
	#undef RISSE_INTERNALCLASSES_CLASS

	void * EndSentinel; //!< クラスインスタンスの開始位置

private:
	static bool CommonObjectsInitialized;

protected:
	tPackageManager * PackageManager; //!< パッケージマネージャオブジェクト
	tVariant RissePackageGlobal; //!< "risse" パッケージのグローバルオブジェクト
	tLineOutputInterface *WarningOutput; //!< 警告情報の出力先
	tPackageFileSystemInterface * PackageFileSystem; //!< パッケージ読み込み用のファイルシステムインターフェース

public:
	//! @brief		スクリプトエンジンの動作オプション用構造体
	struct tOptions
	{
		bool AssertEnabled;  //!< assert によるテストが有効かどうか

		//! @brief		デフォルトコンストラクタ
		tOptions()
		{
			AssertEnabled = false;
		}

		//! @brief		比較演算子
		bool operator ==(const tOptions & rhs) const
		{
			return AssertEnabled == rhs.AssertEnabled;
		}
	};

private:
	tOptions Options; //!< オプション

public:
	//! @brief		コンストラクタ
	tScriptEngine();

	//! @brief		"risse" パッケージのグローバルオブジェクトを得る
	const tVariant & GetRissePackageGlobal() const { return RissePackageGlobal; }

	//! @brief		スクリプトを評価する
	//! @param		script			スクリプトの内容
	//! @param		name			スクリプトブロックの名称
	//! @param		lineofs			行オフセット(ドキュメント埋め込みスクリプト用に、
	//!								スクリプトのオフセットを記録できる)
	//! @param		result			実行の結果(NULL可)
	//! @param		binding			バインディング情報(NULLの場合は"main"パッケージグローバル)
	//! @param		is_expression	式モードかどうか(Risseのように文と式の区別を
	//!								する必要がない言語ではfalseでよい)
	void Evaluate(
					const tString & script, const tString & name,
					risse_size lineofs = 0,
					tVariant * result = NULL,
					const tBindingInfo * binding = NULL, bool is_expression = false);

	//! @brief		警告情報の出力先を設定する
	//! @param		output		警告情報の出力先
	//! @note		警告情報は複数スレッドから同時に出力される可能性があるので注意
	void SetWarningOutput(tLineOutputInterface * output)  { WarningOutput = output; }

	//! @brief		警告情報の出力先を取得する
	//! @return		警告情報の出力先
	tLineOutputInterface * GetWarningOutput() const  { return WarningOutput; }

	//! @brief		パッケージ読み込み用のファイルシステムインターフェースを設定する
	//! @param		intf	パッケージ読み込み用のファイルシステムインターフェース
	void SetPackageFileSystem(tPackageFileSystemInterface * intf) { PackageFileSystem = intf; }

	//! @brief		パッケージ読み込み用のファイルシステムインターフェースを取得する
	//! @return		パッケージ読み込み用のファイルシステムインターフェース
	tPackageFileSystemInterface * GetPackageFileSystem() const { return PackageFileSystem; }

	//! @brief		警告情報を出力する
	//! @param		info	警告情報
	void OutputWarning(const tString & info) const
	{
		if(WarningOutput) WarningOutput->Output(info);
	}

	//! @brief		オプション情報を得る
	const tOptions & GetOptions() const { return Options; }

	//! @brief		assertion が有効かどうかを得る
	//! @return		assertion が有効かどうか
	bool GetAssertionEnabled() const { return Options.AssertEnabled; }

	//! @brief		assertion が有効かどうかを設定する
	//! @param		b		asssrtion が有効かどうか
	void SetAssertionEnabled(bool b) { Options.AssertEnabled = b; }

	//! @brief		パッケージマネージャを得る
	//! @return		パッケージマネージャ
	tPackageManager * GetPackageManager() const { return PackageManager; }
};
//---------------------------------------------------------------------------
} // namespace Risse

#endif

