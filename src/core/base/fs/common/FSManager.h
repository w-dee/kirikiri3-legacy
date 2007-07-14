//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ファイルシステムマネージャ(ファイルシステムの根幹部分)
//---------------------------------------------------------------------------
#ifndef _FSMANAGER_H_
#define _FSMANAGER_H_

#include "basetypes.h"
#include <wx/file.h>
#include <wx/datetime.h>
#include "risse/include/risseHashTable.h"
#include "base/utils/Singleton.h"
#include "base/utils/RisaThread.h"
#include "base/script/RisseEngine.h"
#include "risse/include/risseString.h"
#include <boost/smart_ptr.hpp>

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		tFileSystem::GetFileListAt で用いられるコールバックインターフェース
//---------------------------------------------------------------------------
class tFileSystemIterationCallback
{
public:
	virtual ~tFileSystemIterationCallback() {;}
	virtual bool OnFile(const tString & filename) = 0;
	virtual bool OnDirectory(const tString & dirname) = 0;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tFileSystem::Stat で返される構造体
//---------------------------------------------------------------------------
struct tStatStruc
{
	wxFileOffset	Size;	//!< ファイルサイズ (wxFileOffset)-1 の場合は無効
	wxDateTime		MTime;	//!< ファイル修正時刻 (wxDateTime::IsValidで有効性をチェックのこと)
	wxDateTime		ATime;	//!< アクセス時刻 (wxDateTime::IsValidで有効性をチェックのこと)
	wxDateTime		CTime;	//!< 作成時刻 (wxDateTime::IsValidで有効性をチェックのこと)

	tStatStruc() { Clear(); }
	void Clear() {  Size = (wxFileOffset) - 1; MTime = ATime = CTime = wxDateTime(); }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルシステム基底クラス
//---------------------------------------------------------------------------
class tFileSystem
{
	//---------- このクラスで実装する物
protected:

public:

	//---------- 各サブクラスで(も)実装すべき物
public:
	virtual ~tFileSystem() {;}

	virtual size_t GetFileListAt(const tString & dirname,
		tFileSystemIterationCallback * callback) = 0; //!< ファイル一覧を取得する
	virtual bool FileExists(const tString & filename) = 0; //!< ファイルが存在するかどうかを得る
	virtual bool DirectoryExists(const tString & dirname) = 0; //!< ディレクトリが存在するかどうかを得る
	virtual void RemoveFile(const tString & filename) = 0; //!< ファイルを削除する
	virtual void RemoveDirectory(const tString & dirname, bool recursive = false) = 0; //!< ディレクトリを削除する
	virtual void CreateDirectory(const tString & dirname, bool recursive = false) = 0; //!< ディレクトリを作成する
	virtual void Stat(const tString & filename, tStatStruc & struc) = 0; //!< 指定されたファイルの stat を得る
	virtual tBinaryStream * CreateStream(const tString & filename, risse_uint32 flags) = 0; //!< 指定されたファイルのストリームを得る
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルシステムマネージャクラス
//---------------------------------------------------------------------------
class tFileSystemManager :
	public singleton_base<tFileSystemManager>,
	protected depends_on<tRisseScriptEngine>
{
	//! @brief ファイルシステムマネージャ内で管理されるファイルシステムの情報
	struct tFileSystemInfo
	{
		tRefHolder<iRisseDispatch2> Object; //!< Risseオブジェクト
		tFileSystemInfo(iRisseDispatch2 *risse_obj) : 
			Object(risse_obj)  {;} //!< コンストラクタ
	};

	tHashTable<tString, tFileSystemInfo> MountPoints; //!< マウントポイントのハッシュ表
	tString CurrentDirectory; //!< カレントディレクトリ (パスの最後に '/' を含む)

	tCriticalSection CS; //!< このファイルシステムマネージャを保護するクリティカルセクション

public:
	//! @brief		コンストラクタ
	tFileSystemManager();

	//! @brief		デストラクタ
	~tFileSystemManager();

public:
	//! @brief		ファイルシステムをマウントする
	//! @param		point マウントポイント
	//! @param		fs_risseobj ファイルシステムオブジェクトを表すRisseオブジェクト
	//! @note		メインスレッド以外から呼び出さないこと
	void Mount(const tString & point, iRisseDispatch2 * fs_risseobj);

	//! @brief		ファイルシステムをアンマウントする
	//! @param		point マウントポイント
	//! @note		メインスレッド以外から呼び出さないこと
	void Unmount(const tString & point);

	//! @brief		ファイルシステムをアンマウントする
	//! @param		fs_risseobj アンマウントしたいファイルシステムを表すRisseオブジェクト
	//! @note		メインスレッド以外から呼び出さないこと
	void Unmount(iRisseDispatch2 * fs_risseobj);

	//! @brief		パスを正規化する
	//! @param		path 正規化したいパス
	//! @return		正規化されたパス
	tString NormalizePath(const tString & path);

	//! @brief		ファイル一覧を取得する
	//! @param		dirname ディレクトリ名
	//! @param		callback コールバックオブジェクト
	//! @param		recursive 再帰的にファイル一覧を得るかどうか
	//! @return		取得できたファイル数
	size_t GetFileListAt(const tString & dirname,
		tFileSystemIterationCallback * callback, bool recursive = false);

	//! @brief		ファイルが存在するかどうかを得る
	//! @param		filename ファイル名
	//! @return		ファイルが存在する場合真
	bool FileExists(const tString & filename);

	//! @brief		ディレクトリが存在するかどうかを得る
	//! @param		dirname ディレクトリ名
	//! @return		ディレクトリが存在する場合真
	bool DirectoryExists(const tString & dirname);

	//! @brief		ファイルを削除する
	//! @param		filename ファイル名
	void RemoveFile(const tString & filename);

	//! @brief		ディレクトリを削除する
	//! @param		dirname ディレクトリ名
	//! @param		recursive 再帰的にディレクトリを削除するかどうか
	void RemoveDirectory(const tString & dirname, bool recursive = false);

	//! @brief		ディレクトリを作成する
	//! @param		dirname ディレクトリ名
	//! @param		recursive 再帰的にディレクトリを作成するかどうか
	void CreateDirectory(const tString & dirname, bool recursive = false);

	//! @brief		指定されたファイルの stat を得る
	//! @param		filename ファイル名
	//! @param		struc stat 結果の出力先
	void Stat(const tString & filename, tStatStruc & struc);

	//! @brief		指定されたファイルのストリームを得る
	//! @param		filename ファイル名
	//! @param		flags フラグ
	//! @return		ストリームオブジェクト
	tBinaryStream * CreateStream(const tString & filename, risse_uint32 flags);

private:
	//! @brief		ファイル一覧を取得する(内部関数)
	//! @param		dirname ディレクトリ名(正規化されているべきこと)
	//! @param		callback コールバック先
	//! @callback	コールバックオブジェクト
	size_t InternalGetFileListAt(const tString & dirname,
		tFileSystemIterationCallback * callback);

	//! @brief		指定された正規フルパスに対応するファイルシステムを得る
	//! @param		fullpath 正規フルパス
	//! @param		fspath ファイルシステム内におけるパス(興味がない場合はNULL可、最初の / は含まれない)
	//! @return		ファイルシステムインスタンス
	//! @note		このメソッドはスレッド保護されていないため、このメソッドを呼ぶ場合は
	//!				CriticalSection 内で呼ぶこと！
	boost::shared_ptr<tFileSystem> GetFileSystemAt(const tString & fullpath, tString * fspath = NULL);

	//! @brief		「ファイルシステムが指定されたパスはない」例外を発生させる
	//! @param		filename  マウントポイント
	//! @note		この関数は例外を発生させるため呼び出し元には戻らない
	static void ThrowNoFileSystemError(const tString & filename);

public:

	//! @brief		「そのようなファイルやディレクトリは無い」例外を発生させる
	static void RaiseNoSuchFileOrDirectoryError();

	//! @brief		パス名を拡張子より前と拡張子に分離する ( /path/to/file.ext を /path/to/file と .ext に分離する )
	//! @param		in 入力パス名
	//! @param		other [out] 拡張子より前の部分へポインタ(興味ない場合はNULL可)
	//! @param		name [out] 拡張子へのポインタ(興味ない場合はNULL可)  拡張子には .(ドット) を含む。拡張子がない場合は空文字列になる
	//! @note		in と そのほかのパラメータに同じ文字列を指定しないこと
	static void SplitExtension(const tString & in, tString * other, tString * ext);

	//! @brief		パス名をパスと名前に分離する ( /path/to/file を /path/to と file に分離する )
	//! @param		in 入力パス名
	//! @param		path [out] パスへのポインタ(興味ない場合はNULL可)
	//! @param		name [out] 名前へのポインタ(興味ない場合はNULL可)
	//! @note		in と そのほかのパラメータに同じ文字列を指定しないこと
	static void SplitPathAndName(const tString & in, tString * path, tString * name);

	//! @brief		パス名の最後のパスデリミタ ('/') を取り去る ( /path/is/here/ を /path/is/here にする )
	//! @param		path パス
	static void TrimLastPathDelimiter(tString & path);

	//! @brief		拡張子を切り落とす
	//! @param		in 処理したいファイル名
	//! @return		拡張子が取り落とされたファイル名
	static tString ChopExtension(const tString & in);

	//! @brief		拡張子を取り出す
	//! @param		in 処理したいファイル名
	//! @return		拡張子(ドットも含む; 拡張子が無い場合は空文字)
	static tString ExtractExtension(const tString & in);

	//! @brief		ファイル名を返す (パスの部分を含まない)
	//! @param		in 処理したいファイル名
	//! @return		ファイル名
	static tString ExtractName(const tString & in);

	//! @brief		ファイル名のパス名を返す
	//! @param		in 処理したいファイル名
	//! @return		パス名
	static tString ExtractPath(const tString & in);

	//! @brief		フルパスを得る
	tString GetFullPath(const tString & in) { return NormalizePath(in); }

	//! @brief		現在の作業ディレクトリを得る
	//! @return		作業ディレクトリ
	const tString & GetCurrentDirectory();

	//! @brief		作業ディレクトリを設定する
	//! @param		dir   作業ディレクトリ
	//! @note		実際にそのディレクトリが存在するかどうかのチェックは行わない
	void SetCurrentDirectory(const tString &dir);
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif
