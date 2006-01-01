//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OSFSの実装
//---------------------------------------------------------------------------
#ifndef _OSFSH_
#define _OSFSH_

#include "FSManager.h"


//---------------------------------------------------------------------------
//! @brief		OS ネイティブファイルストリーム
//---------------------------------------------------------------------------
class tTVPOSNativeStream : public tTJSBinaryStream
{
protected:
	wxFile File;

public:
	tTVPOSNativeStream(const wxString & filename, tjs_uint32 flags);
	~tTVPOSNativeStream();

	tjs_uint64 Seek(tjs_int64 offset, tjs_int whence);
	tjs_size Read(void *buffer, tjs_size read_size);
	tjs_size Write(const void *buffer, tjs_size write_size);
	void SetEndOfFile();
	tjs_uint64 GetSize();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		OS ファイルシステム
//---------------------------------------------------------------------------
class tTVPOSFS : public iTVPFileSystem
{
	tTJSCriticalSection CS; //!< このファイルシステムを保護するクリティカルセクション

	wxString BaseDirectory; //!< このファイルシステムがマウントしているOSのディレクトリ
	bool CheckCase; //!< ファイル名の大文字・小文字をチェックする場合は真

public:
	tTVPOSFS(const ttstr & basedir, bool checkcase = true);

	//-- iTVPFileSystem メンバ
	~tTVPOSFS();

	size_t GetFileListAt(const ttstr & dirname,
		iTVPFileSystemIterationCallback * callback);
	bool FileExists(const ttstr & filename);
	bool DirectoryExists(const ttstr & dirname);
	void RemoveFile(const ttstr & filename);
	void RemoveDirectory(const ttstr & dirname, bool recursive = false);
	void CreateDirectory(const ttstr & dirname, bool recursive = false);
	void Stat(const ttstr & filename, tTVPStatStruc & struc);
	tTVPBinaryStream * CreateStream(const ttstr & filename, tjs_uint32 flags);

	//-- iTVPFileSystem メンバ ここまで
private:
	static wxString ConvertToNativePathDelimiter(const wxString & path);

	static bool CheckFileNameCase(const wxString & path_to_check, bool raise = true);
};
//---------------------------------------------------------------------------


#endif
