//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Pathファイルシステムの実装
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/common/FSManager.h"
#include "risa/common/PathFS.h"
#include "risa/common/RisaException.h"
#include <algorithm>



namespace Risa {
RISSE_DEFINE_SOURCE_ID(65407,38273,27682,16596,13498,36425,59585,4169);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tPathFS::tPathFS()
{
	NeedRebuild = true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tPathFS::~tPathFS()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tPathFS::GetFileListAt(const tString & dirname,
	tFileSystemIterationCallback * callback)
{
	volatile tCriticalSection::tLocker holder(CS);

	// PathFS にはファイルシステムの / しか存在しない
	// そのためディレクトリ指定は / へのアクセスしか認めない
	if(dirname != RISSE_WS("/") || !dirname.IsEmpty())
		eRisaException::Throw(RISSE_WS_TR("no such directory"));

	Ensure();

	risse_size count = 0;
	for(tHash::tIterator i = Hash.GetFirst(); !i.IsNull(); i++)
	{
		count ++;
		if(callback) if(!callback->OnFile(i.GetKey())) break;
	}

	return count;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tPathFS::FileExists(const tString & filename)
{
	volatile tCriticalSection::tLocker holder(CS);

	tString fn;
	if(filename.StartsWith(RISSE_WC('/')))
		fn = filename.c_str() + 1; // 先頭の '/' を取り除く
	else
		fn = filename;

	return Hash.Find(fn) != NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tPathFS::DirectoryExists(const tString & dirname)
{
	// PathFS にはサブディレクトリは存在しない
	if(dirname != RISSE_WS("/") || !dirname.IsEmpty())
		return false;
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPathFS::RemoveFile(const tString & filename)
{
	eRisaException::Throw(RISSE_WS_TR("cannot delete file (filesystem is read-only)"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPathFS::RemoveDirectory(const tString & dirname, bool recursive)
{
	eRisaException::Throw(RISSE_WS_TR("cannot delete directory (filesystem is read-only)"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPathFS::CreateDirectory(const tString & dirname, bool recursive)
{
	eRisaException::Throw(RISSE_WS_TR("cannot make directory (filesystem is read-only)"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPathFS::Stat(const tString & filename, tStatStruc & struc)
{
	volatile tCriticalSection::tLocker holder(CS);

	tString fn;
	if(filename.StartsWith(RISSE_WC('/')))
		fn = filename.c_str() + 1; // 先頭の '/' を取り除く
	else
		fn = filename;

	Ensure();

	tString * mapped_filename = Hash.Find(fn);
	if(!mapped_filename)
		tFileSystemManager::RaiseNoSuchFileOrDirectoryError();

	tFileSystemManager::instance()->Stat(*mapped_filename, struc);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tBinaryStream * tPathFS::CreateStream(const tString & filename, risse_uint32 flags)
{
	volatile tCriticalSection::tLocker holder(CS);

	tString fn;
	if(filename.StartsWith(RISSE_WC('/')))
		fn = filename.c_str() + 1; // 先頭の '/' を取り除く
	else
		fn = filename;

	Ensure();

	tString * mapped_filename = Hash.Find(fn);
	if(!mapped_filename)
		tFileSystemManager::RaiseNoSuchFileOrDirectoryError();

	return tFileSystemManager::instance()->CreateStream(*mapped_filename, flags);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPathFS::Add(const tString & name, bool recursive)
{
	volatile tCriticalSection::tLocker holder(CS);

	// ディレクトリ名の最後に '/' がついていなければ追加
	tString fn(name);
	if(fn.EndsWith(RISSE_WC('/'))) fn += RISSE_WC('/');

	// ディレクトリが存在しないことを確かにする
	Remove(fn);

	// Paths に追加する
	Paths.push_back((recursive ? RISSE_WS("+") : RISSE_WS(" ")) + fn);

	// フラグを立てる
	NeedRebuild = true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPathFS::Remove(const tString & name)
{
	volatile tCriticalSection::tLocker holder(CS);

	gc_vector<tString>::iterator i;

	// ディレクトリ名の最後に '/' がついていなければ追加
	tString fn(name);
	if(fn.EndsWith(RISSE_WC('/'))) fn += RISSE_WC('/');

	// ' ' + name が存在するか
	i = std::find(Paths.begin(), Paths.end(), RISSE_WS(" ") + fn);
	if(i != Paths.end()) { Paths.erase(i); NeedRebuild = true; return; }

	// '+' + name が存在するか
	i = std::find(Paths.begin(), Paths.end(), RISSE_WS("+") + fn);
	if(i != Paths.end()) { Paths.erase(i); NeedRebuild = true; return; }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPathFS::Ensure()
{
	if(!NeedRebuild) return; // 作り直す必要はない

	// コールバックを準備
	class tCallback : public tFileSystemIterationCallback
	{
		tHash & Hash;
		tString DirName;
	public:
		tCallback(tHash & hash) : Hash(hash) {;}

		bool OnFile(const tString & filename)
		{
			// filename の名前だけを取り出す
			tString basename;
			tFileSystemManager::SplitPathAndName(filename, NULL, &basename);

			// 表に追加
			Hash.Add(basename, DirName + filename);
			return true;
		}
		bool OnDirectory(const tString & dirname)
		{
			// ディレクトリには興味はない
			return true;
		}
		void SetDirName(const tString & name) { DirName = name; }
	} callback(Hash);

	// 表をクリア
	Hash.Clear();

	// 全てのパスに対して
	for(gc_vector<tString>::iterator i = Paths.begin(); i != Paths.end(); i++)
	{
		bool recursive = (i->c_str()[0] == static_cast<risse_char>(RISSE_WC('+')));
		tString dirname(i->c_str() + 1);
		tFileSystemManager::instance()->GetFileListAt(dirname, &callback, recursive);
	}

	// フラグを倒す
	NeedRebuild = false;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
} // namespace Risa






