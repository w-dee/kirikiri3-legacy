//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
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
#include "base/utils/Thread.h"
#include "base/fs/common/FSManager.h"



//---------------------------------------------------------------------------
//! @brief		PathFS の実装
//---------------------------------------------------------------------------
class tRisaPathFS : public tRisaFileSystem
{
	tRisaCriticalSection CS; //!< このファイルシステムを保護するクリティカルセクション
	typedef tRisseHashTable<ttstr, ttstr> tHash; //!< ファイルシステム中のファイル名と実際のファイル名の対応表のtypedef
	tHash Hash;//!< ファイルシステム中のファイル名と実際のファイル名の対応表
	std::vector<ttstr> Paths; //!< パス(最初の文字が ' ' ならばディレクトリ単独、'+' ならばrecursive)
	bool NeedRebuild; //!< パスのハッシュ表を作り直す必要がある場合に真になる

public:
	tRisaPathFS();

	//-- tRisaFileSystem メンバ
	~tRisaPathFS();

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
public:
	void Add(const ttstr & name, bool recursive = false);
	void Remove(const ttstr & name);

private:
	void Ensure();
};
//---------------------------------------------------------------------------




#endif
