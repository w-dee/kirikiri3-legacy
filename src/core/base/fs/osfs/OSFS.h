//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OSFSの実装
//---------------------------------------------------------------------------
#ifndef _OSFSH_
#define _OSFSH_

#include "base/fs/common/FSManager.h"
#include "risse/include/risseNative.h"
#include <wx/file.h>

//---------------------------------------------------------------------------
//! @brief		OS ネイティブファイルストリーム
//---------------------------------------------------------------------------
class tRisaOSNativeStream : public tRisseBinaryStream
{
protected:
	wxFile File;

public:
	tRisaOSNativeStream(const wxString & filename, risse_uint32 flags);
	~tRisaOSNativeStream();

	risse_uint64 Seek(risse_int64 offset, risse_int whence);
	risse_size Read(void *buffer, risse_size read_size);
	risse_size Write(const void *buffer, risse_size write_size);
	void SetEndOfFile();
	risse_uint64 GetSize();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		OS ファイルシステム
//---------------------------------------------------------------------------
class tRisaOSFS : public tRisaFileSystem
{
	tRisseCriticalSection CS; //!< このファイルシステムを保護するクリティカルセクション

	wxString BaseDirectory; //!< このファイルシステムがマウントしているOSのディレクトリ
	bool CheckCase; //!< ファイル名の大文字・小文字をチェックする場合は真

public:
	tRisaOSFS(const ttstr & basedir, bool checkcase = true);

	//-- tRisaFileSystem メンバ
	~tRisaOSFS();

	size_t GetFileListAt(const ttstr & dirname,
		tRisaFileSystemIterationCallback * callback);
	bool FileExists(const ttstr & filename);
	bool DirectoryExists(const ttstr & dirname);
	void RemoveFile(const ttstr & filename);
	void RemoveDirectory(const ttstr & dirname, bool recursive = false);
	void CreateDirectory(const ttstr & dirname, bool recursive = false);
	void Stat(const ttstr & filename, tRisaStatStruc & struc);
	tRisseBinaryStream * CreateStream(const ttstr & filename, risse_uint32 flags);

	//-- tRisaFileSystem メンバ ここまで
private:
	static wxString ConvertToNativePathDelimiter(const wxString & path);

	bool CheckFileNameCase(const wxString & path_to_check, bool raise = true);
};
//---------------------------------------------------------------------------


#endif
