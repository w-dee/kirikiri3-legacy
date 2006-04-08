//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ファイルシステムマネージャ(ファイルシステムの根幹部分)
//---------------------------------------------------------------------------
#ifndef _FSMANAGER_H_
#define _FSMANAGER_H_

#include <wx/file.h>
#include <wx/datetime.h>
#include "risse/include/risse.h"
#include "risse/include/risseHashSearch.h"
#include "base/utils/Singleton.h"
#include "base/utils/RisaThread.h"
#include "base/script/RisseEngine.h"
#include <boost/smart_ptr.hpp>


//---------------------------------------------------------------------------
//! @brief		tRisaFileSystem::GetFileListAt で用いられるコールバックインターフェース
//---------------------------------------------------------------------------
class tRisaFileSystemIterationCallback
{
public:
	virtual ~tRisaFileSystemIterationCallback() {;}
	virtual bool OnFile(const ttstr & filename) = 0;
	virtual bool OnDirectory(const ttstr & dirname) = 0;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tRisaFileSystem::Stat で返される構造体
//---------------------------------------------------------------------------
struct tRisaStatStruc
{
	wxFileOffset	Size;	//!< ファイルサイズ (wxFileOffset)-1 の場合は無効
	wxDateTime		MTime;	//!< ファイル修正時刻 (wxDateTime::IsValidで有効性をチェックのこと)
	wxDateTime		ATime;	//!< アクセス時刻 (wxDateTime::IsValidで有効性をチェックのこと)
	wxDateTime		CTime;	//!< 作成時刻 (wxDateTime::IsValidで有効性をチェックのこと)

	tRisaStatStruc() { Clear(); }
	void Clear() {  Size = (wxFileOffset) - 1; MTime = ATime = CTime = wxDateTime(); }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルシステム基底クラス
//---------------------------------------------------------------------------
class tRisaFileSystem
{
	//---------- このクラスで実装する物
protected:

public:

	//---------- 各サブクラスで(も)実装すべき物
public:
	virtual ~tRisaFileSystem() {;}

	virtual size_t GetFileListAt(const ttstr & dirname,
		tRisaFileSystemIterationCallback * callback) = 0; //!< ファイル一覧を取得する
	virtual bool FileExists(const ttstr & filename) = 0; //!< ファイルが存在するかどうかを得る
	virtual bool DirectoryExists(const ttstr & dirname) = 0; //!< ディレクトリが存在するかどうかを得る
	virtual void RemoveFile(const ttstr & filename) = 0; //!< ファイルを削除する
	virtual void RemoveDirectory(const ttstr & dirname, bool recursive = false) = 0; //!< ディレクトリを削除する
	virtual void CreateDirectory(const ttstr & dirname, bool recursive = false) = 0; //!< ディレクトリを作成する
	virtual void Stat(const ttstr & filename, tRisaStatStruc & struc) = 0; //!< 指定されたファイルの stat を得る
	virtual tRisseBinaryStream * CreateStream(const ttstr & filename, risse_uint32 flags) = 0; //!< 指定されたファイルのストリームを得る
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルシステムマネージャクラス
//---------------------------------------------------------------------------
class tRisaFileSystemManager :
	public singleton_base<tRisaFileSystemManager>,
	protected depends_on<tRisaRisseScriptEngine>
{
	//! @brief ファイルシステムマネージャ内で管理されるファイルシステムの情報
	struct tFileSystemInfo
	{
		tRisseRefHolder<iRisseDispatch2> RisseObject; //!< Risseオブジェクト
		tFileSystemInfo(iRisseDispatch2 *risse_obj) : 
			RisseObject(risse_obj)  {;} //!< コンストラクタ
	};

	tRisseHashTable<ttstr, tFileSystemInfo> MountPoints; //!< マウントポイントのハッシュ表
	ttstr CurrentDirectory; //!< カレントディレクトリ (パスの最後に '/' を含む)

	tRisaCriticalSection CS; //!< このファイルシステムマネージャを保護するクリティカルセクション

public:
	//! @brief		コンストラクタ
	tRisaFileSystemManager();

	//! @brief		デストラクタ
	~tRisaFileSystemManager();

public:
	//! @brief		ファイルシステムをマウントする
	//! @param		point マウントポイント
	//! @param		fs_risseobj ファイルシステムオブジェクトを表すRisseオブジェクト
	//! @note		メインスレッド以外から呼び出さないこと
	void Mount(const ttstr & point, iRisseDispatch2 * fs_risseobj);

	//! @brief		ファイルシステムをアンマウントする
	//! @param		point マウントポイント
	//! @note		メインスレッド以外から呼び出さないこと
	void Unmount(const ttstr & point);

	//! @brief		ファイルシステムをアンマウントする
	//! @param		fs_risseobj アンマウントしたいファイルシステムを表すRisseオブジェクト
	//! @note		メインスレッド以外から呼び出さないこと
	void Unmount(iRisseDispatch2 * fs_risseobj);

	//! @brief		パスを正規化する
	//! @param		path 正規化したいパス
	//! @return		正規化されたパス
	ttstr NormalizePath(const ttstr & path);

	//! @brief		ファイル一覧を取得する
	//! @param		dirname ディレクトリ名
	//! @param		callback コールバックオブジェクト
	//! @param		recursive 再帰的にファイル一覧を得るかどうか
	//! @return		取得できたファイル数
	size_t GetFileListAt(const ttstr & dirname,
		tRisaFileSystemIterationCallback * callback, bool recursive = false);

	//! @brief		ファイルが存在するかどうかを得る
	//! @param		filename ファイル名
	//! @return		ファイルが存在する場合真
	bool FileExists(const ttstr & filename);

	//! @brief		ディレクトリが存在するかどうかを得る
	//! @param		dirname ディレクトリ名
	//! @return		ディレクトリが存在する場合真
	bool DirectoryExists(const ttstr & dirname);

	//! @brief		ファイルを削除する
	//! @param		filename ファイル名
	void RemoveFile(const ttstr & filename);

	//! @brief		ディレクトリを削除する
	//! @param		dirname ディレクトリ名
	//! @param		recursive 再帰的にディレクトリを削除するかどうか
	void RemoveDirectory(const ttstr & dirname, bool recursive = false);

	//! @brief		ディレクトリを作成する
	//! @param		dirname ディレクトリ名
	//! @param		recursive 再帰的にディレクトリを作成するかどうか
	void CreateDirectory(const ttstr & dirname, bool recursive = false);

	//! @brief		指定されたファイルの stat を得る
	//! @param		filename ファイル名
	//! @param		struc stat 結果の出力先
	void Stat(const ttstr & filename, tRisaStatStruc & struc);

	//! @brief		指定されたファイルのストリームを得る
	//! @param		filename ファイル名
	//! @param		flags フラグ
	//! @return		ストリームオブジェクト
	tRisseBinaryStream * CreateStream(const ttstr & filename, risse_uint32 flags);

private:
	//! @brief		ファイル一覧を取得する(内部関数)
	//! @param		dirname ディレクトリ名(正規化されているべきこと)
	//! @param		callback コールバック先
	//! @callback	コールバックオブジェクト
	size_t InternalGetFileListAt(const ttstr & dirname,
		tRisaFileSystemIterationCallback * callback);

	//! @brief		指定された正規フルパスに対応するファイルシステムを得る
	//! @param		fullpath 正規フルパス
	//! @param		fspath ファイルシステム内におけるパス(興味がない場合はNULL可、最初の / は含まれない)
	//! @return		ファイルシステムインスタンス
	//! @note		このメソッドはスレッド保護されていないため、このメソッドを呼ぶ場合は
	//!				CriticalSection 内で呼ぶこと！
	boost::shared_ptr<tRisaFileSystem> GetFileSystemAt(const ttstr & fullpath, ttstr * fspath = NULL);

	//! @brief		「ファイルシステムが指定されたパスはない」例外を発生させる
	//! @param		filename  マウントポイント
	//! @note		この関数は例外を発生させるため呼び出し元には戻らない
	static void ThrowNoFileSystemError(const ttstr & filename);

public:

	//! @brief		「そのようなファイルやディレクトリは無い」例外を発生させる
	static void RaiseNoSuchFileOrDirectoryError();

	//! @brief		パス名を拡張子より前と拡張子に分離する ( /path/to/file.ext を /path/to/file と .ext に分離する )
	//! @param		in 入力パス名
	//! @param		other [out] 拡張子より前の部分へポインタ(興味ない場合はNULL可)
	//! @param		name [out] 拡張子へのポインタ(興味ない場合はNULL可)  拡張子には .(ドット) を含む。拡張子がない場合は空文字列になる
	//! @note		in と そのほかのパラメータに同じ文字列を指定しないこと
	static void SplitExtension(const ttstr & in, ttstr * other, ttstr * ext);

	//! @brief		パス名をパスと名前に分離する ( /path/to/file を /path/to と file に分離する )
	//! @param		in 入力パス名
	//! @param		path [out] パスへのポインタ(興味ない場合はNULL可)
	//! @param		name [out] 名前へのポインタ(興味ない場合はNULL可)
	//! @note		in と そのほかのパラメータに同じ文字列を指定しないこと
	static void SplitPathAndName(const ttstr & in, ttstr * path, ttstr * name);

	//! @brief		パス名の最後のパスデリミタ ('/') を取り去る ( /path/is/here/ を /path/is/here にする )
	//! @param		path パス
	static void TrimLastPathDelimiter(ttstr & path);

	//! @brief		拡張子を切り落とす
	//! @param		in 処理したいファイル名
	//! @return		拡張子が取り落とされたファイル名
	static ttstr ChopExtension(const ttstr & in);

	//! @brief		拡張子を取り出す
	//! @param		in 処理したいファイル名
	//! @return		拡張子(ドットも含む; 拡張子が無い場合は空文字)
	static ttstr ExtractExtension(const ttstr & in);

	//! @brief		ファイル名を返す (パスの部分を含まない)
	//! @param		in 処理したいファイル名
	//! @return		ファイル名
	static ttstr ExtractName(const ttstr & in);

	//! @brief		ファイル名のパス名を返す
	//! @param		in 処理したいファイル名
	//! @return		パス名
	static ttstr ExtractPath(const ttstr & in);

	//! @brief		フルパスを得る
	ttstr GetFullPath(const ttstr & in) { return NormalizePath(in); }

	//! @brief		現在の作業ディレクトリを得る
	//! @return		作業ディレクトリ
	const ttstr & GetCurrentDirectory();

	//! @brief		作業ディレクトリを設定する
	//! @param		dir   作業ディレクトリ
	//! @note		実際にそのディレクトリが存在するかどうかのチェックは行わない
	void SetCurrentDirectory(const ttstr &dir);
};
//---------------------------------------------------------------------------

#endif
