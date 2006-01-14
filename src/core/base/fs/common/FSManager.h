//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ファイルシステムマネージャ(ファイルシステムの根幹部分)
//---------------------------------------------------------------------------
#ifndef _FSMANAGER_H_
#define _FSMANAGER_H_

#include <wx/datetime.h>
#include "risse.h"
#include "risseUtils.h"
#include "risseHashSearch.h"
#include "Singleton.h"
#include <boost/smart_ptr.hpp>


//---------------------------------------------------------------------------
//! @brief		tTVPFileSystem::GetFileListAt で用いられるコールバックインターフェース
//---------------------------------------------------------------------------
class tTVPFileSystemIterationCallback
{
public:
	virtual bool OnFile(const ttstr & filename) = 0;
	virtual bool OnDirectory(const ttstr & dirname) = 0;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tTVPFileSystem::Stat で返される構造体
//---------------------------------------------------------------------------
struct tTVPStatStruc
{
	wxFileOffset	Size;	//!< ファイルサイズ (wxFileOffset)-1 の場合は無効
	wxDateTime		MTime;	//!< ファイル修正時刻 (wxDateTime::IsValidで有効性をチェックのこと)
	wxDateTime		ATime;	//!< アクセス時刻 (wxDateTime::IsValidで有効性をチェックのこと)
	wxDateTime		CTime;	//!< 作成時刻 (wxDateTime::IsValidで有効性をチェックのこと)

	tTVPStatStruc() { Clear(); }
	void Clear() {  Size = (wxFileOffset) - 1; MTime = ATime = CTime = wxDateTime(); }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルシステム基底クラス
//---------------------------------------------------------------------------
class tTVPFileSystem
{
	//---------- このクラスで実装する物
protected:

public:

	//---------- 各サブクラスで(も)実装すべき物
public:
	virtual ~tTVPFileSystem() {;}

	virtual size_t GetFileListAt(const ttstr & dirname,
		tTVPFileSystemIterationCallback * callback) = 0; //!< ファイル一覧を取得する
	virtual bool FileExists(const ttstr & filename) = 0; //!< ファイルが存在するかどうかを得る
	virtual bool DirectoryExists(const ttstr & dirname) = 0; //!< ディレクトリが存在するかどうかを得る
	virtual void RemoveFile(const ttstr & filename) = 0; //!< ファイルを削除する
	virtual void RemoveDirectory(const ttstr & dirname, bool recursive = false) = 0; //!< ディレクトリを削除する
	virtual void CreateDirectory(const ttstr & dirname, bool recursive = false) = 0; //!< ディレクトリを作成する
	virtual void Stat(const ttstr & filename, tTVPStatStruc & struc) = 0; //!< 指定されたファイルの stat を得る
	virtual tRisseBinaryStream * CreateStream(const ttstr & filename, risse_uint32 flags) = 0; //!< 指定されたファイルのストリームを得る
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルシステムマネージャクラス
//---------------------------------------------------------------------------
class tTVPFileSystemManager
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

	tRisseCriticalSection CS; //!< このファイルシステムマネージャを保護するクリティカルセクション

public:
	tTVPFileSystemManager();
	~tTVPFileSystemManager();

private:
	tTVPSingletonObjectLifeTracer<tTVPFileSystemManager> singleton_object_life_tracer;
public:
	static boost::shared_ptr<tTVPFileSystemManager> & instance() { return
		tTVPSingleton<tTVPFileSystemManager>::instance();
			} //!< このシングルトンのインスタンスを返す

	void Mount(const ttstr & point, iRisseDispatch2 * fs_risseobj);
	void Unmount(const ttstr & point);
	void Unmount(iRisseDispatch2 * fs_risseobj);

	ttstr NormalizePath(const ttstr & path);

	size_t GetFileListAt(const ttstr & dirname,
		tTVPFileSystemIterationCallback * callback, bool recursive = false);
	bool FileExists(const ttstr & filename);
	bool DirectoryExists(const ttstr & dirname);
	void RemoveFile(const ttstr & filename);
	void RemoveDirectory(const ttstr & dirname, bool recursive = false);
	void CreateDirectory(const ttstr & dirname, bool recursive = false);
	void Stat(const ttstr & filename, tTVPStatStruc & struc);
	tRisseBinaryStream * CreateStream(const ttstr & filename, risse_uint32 flags);

private:
	size_t InternalGetFileListAt(const ttstr & dirname,
		tTVPFileSystemIterationCallback * callback);
	boost::shared_ptr<tTVPFileSystem> GetFileSystemAt(const ttstr & fullpath, ttstr * fspath = NULL);
	static void ThrowNoFileSystemError(const ttstr & filename);

public:
	static void RaiseNoSuchFileOrDirectoryError();
	static void SplitExtension(const ttstr & in, ttstr * other, ttstr * ext);
	static void SplitPathAndName(const ttstr & in, ttstr * path, ttstr * name);
	static void TrimLastPathDelimiter(ttstr & path);

	static ttstr ChopExtension(const ttstr & in);
	static ttstr ExtractExtension(const ttstr & in);
	static ttstr ExtractName(const ttstr & in);
	static ttstr ExtractPath(const ttstr & in);

	ttstr GetFullPath(const ttstr & in) { return NormalizePath(in); }

	const ttstr & GetCurrentDirectory();
	void SetCurrentDirectory(const ttstr &dir);
};
//---------------------------------------------------------------------------

#endif
