//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Pathファイルシステムの実装
//---------------------------------------------------------------------------

#ifndef PATHFSH
#define PATHFSH

#include <boost/shared_ptr.hpp>
#include "FSManager.h"



//---------------------------------------------------------------------------
//! @brief		PathFS の実装
//---------------------------------------------------------------------------
class tTVPPathFS : public iTVPFileSystem
{
	tTJSCriticalSection CS; //!< このファイルシステムを保護するクリティカルセクション
	typedef tTJSHashTable<ttstr, ttstr> tHash; //!< ファイルシステム中のファイル名と実際のファイル名の対応表のtypedef
	tHash Hash;//!< ファイルシステム中のファイル名と実際のファイル名の対応表
	std::vector<ttstr> Paths; //!< パス(最初の文字が ' ' ならばディレクトリ単独、'+' ならばrecursive)
	bool NeedRebuld; //!< パスのハッシュ表を作り直す必要がある場合に真になる

public:
	tTVPPathFS();

	//-- iTVPFileSystem メンバ
	~tTVPPathFS();

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
public:
	void AddDirectory(const ttstr & name, bool recursive = false);
	void RemoveDirectory(const ttstr & name);

private:
	void Ensure();
};
//---------------------------------------------------------------------------




#endif
