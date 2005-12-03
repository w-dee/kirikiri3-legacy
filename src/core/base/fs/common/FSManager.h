//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ファイルシステムマネージャ(ファイルシステムの根幹部分)
//---------------------------------------------------------------------------

#ifndef _FSMANAGER_H_
#define _FSMANAGER_H_



//---------------------------------------------------------------------------
//! @brief		ファイルシステムマネージャクラス
//---------------------------------------------------------------------------
class tTVPFileSystemManager
{
	tTJSHashTable<ttstr, iTVPFileSystem *> MountPoints; //!< マウントポイントのハッシュ表
	ttstr CurrentDirectory; //!< カレントディレクトリ (パスの最後に '/' を含む)

	tTJSCriticalSection CS;

public:
	tTVPFileSystemManager();
	~tTVPFileSystemManager();

	void Mount(const ttstr & point, iTVPFileSystem * fs);
	void Unmount(const ttstr & point);

	ttstr NormalizePath(const ttstr & path);

	size_t GetFileListAt(const ttstr & dirname,
		iTVPFileSystemIterationCallback * callback, bool recursive = false); //!< ファイル一覧を取得
	bool FileExists(const ttstr & filename);
	bool DirectoryExists(const ttstr & dirname);
	void RemoveFile(const ttstr & filename);
	void RemoveDirectory(const ttstr & dirname, bool recursive = false);
	void CreateDirectory(const ttstr & dirname, bool recursive = false);
	void Stat(const ttstr & filename, tTVPStatStruc & struc);
	iTVPBinaryStream * CreateStream(const ttstr & filename, tjs_uint32 flags);

private:
	iTVPFileSystem * GetFileSystemAtNoAddRef(const ttstr & fullpath, ttstr * fspath = NULL);
	static void ThrowNoFileSystemError(const ttstr & filename);
};
//---------------------------------------------------------------------------

#endif
