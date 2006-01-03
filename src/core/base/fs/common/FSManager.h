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
#include "tjs.h"
#include "tjsUtils.h"
#include "tjsHashSearch.h"
#include <boost/pool/detail/singleton.hpp>
#include <boost/smart_ptr.hpp>

using namespace boost;

//---------------------------------------------------------------------------
//! @brief		iTVPFileSystem::GetFileListAt で用いられるコールバックインターフェース
//---------------------------------------------------------------------------
class iTVPFileSystemIterationCallback
{
public:
	virtual bool OnFile(const ttstr & filename) = 0;
	virtual bool OnDirectory(const ttstr & dirname) = 0;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		iTVPFileSystem::Stat で返される構造体
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
//! @brief		ファイルシステム純粋仮想クラス
//---------------------------------------------------------------------------
class iTVPFileSystem
{
public:
	virtual ~iTVPFileSystem() {;} //!< デストラクタ

	virtual size_t GetFileListAt(const ttstr & dirname,
		iTVPFileSystemIterationCallback * callback) = 0; //!< ファイル一覧を取得する
	virtual bool FileExists(const ttstr & filename) = 0; //!< ファイルが存在するかどうかを得る
	virtual bool DirectoryExists(const ttstr & dirname) = 0; //!< ディレクトリが存在するかどうかを得る
	virtual void RemoveFile(const ttstr & filename) = 0; //!< ファイルを削除する
	virtual void RemoveDirectory(const ttstr & dirname, bool recursive = false) = 0; //!< ディレクトリを削除する
	virtual void CreateDirectory(const ttstr & dirname, bool recursive = false) = 0; //!< ディレクトリを作成する
	virtual void Stat(const ttstr & filename, tTVPStatStruc & struc) = 0; //!< 指定されたファイルの stat を得る
	virtual tTJSBinaryStream * CreateStream(const ttstr & filename, tjs_uint32 flags) = 0; //!< 指定されたファイルのストリームを得る
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルシステムマネージャクラス
//---------------------------------------------------------------------------
class tTVPFileSystemManager
{
	tTJSHashTable<ttstr, boost::shared_ptr<iTVPFileSystem> > MountPoints; //!< マウントポイントのハッシュ表
	ttstr CurrentDirectory; //!< カレントディレクトリ (パスの最後に '/' を含む)

	tTJSCriticalSection CS; //!< このファイルシステムマネージャを保護するクリティカルセクション

public:
	tTVPFileSystemManager();
	~tTVPFileSystemManager();

	void Mount(const ttstr & point, boost::shared_ptr<iTVPFileSystem> fs);
	void Unmount(const ttstr & point);

	ttstr NormalizePath(const ttstr & path);

	size_t GetFileListAt(const ttstr & dirname,
		iTVPFileSystemIterationCallback * callback, bool recursive = false);
	bool FileExists(const ttstr & filename);
	bool DirectoryExists(const ttstr & dirname);
	void RemoveFile(const ttstr & filename);
	void RemoveDirectory(const ttstr & dirname, bool recursive = false);
	void CreateDirectory(const ttstr & dirname, bool recursive = false);
	void Stat(const ttstr & filename, tTVPStatStruc & struc);
	tTJSBinaryStream * CreateStream(const ttstr & filename, tjs_uint32 flags);

private:
	size_t InternalGetFileListAt(const ttstr & dirname,
		iTVPFileSystemIterationCallback * callback);
	boost::shared_ptr<iTVPFileSystem> GetFileSystemAt(const ttstr & fullpath, ttstr * fspath = NULL);
	static void ThrowNoFileSystemError(const ttstr & filename);

public:
	static void RaiseNoSuchFileOrDirectoryError();
	static void SplitExtension(const ttstr & in, ttstr * other, ttstr * ext);
	static void SplitPathAndName(const ttstr & in, ttstr * path, ttstr * name);
	static void TrimLastPathDelimiter(ttstr & path);

};
//---------------------------------------------------------------------------

#endif
