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
#include "prec.h"
#include "base/fs/common/FSManager.h"
#include "base/fs/pathfs/PathFS.h"
#include "base/exception/RisaException.h"
#include <algorithm>

RISSE_DEFINE_SOURCE_ID(65407,38273,27682,16596,13498,36425,59585,4169);




//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaPathFS::tRisaPathFS()
{
	NeedRebuild = true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaPathFS::~tRisaPathFS()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイル一覧を取得する
//! @param		dirname ディレクトリ名
//! @param		callback コールバックオブジェクト
//! @return		取得できたファイル数
//---------------------------------------------------------------------------
size_t tRisaPathFS::GetFileListAt(const ttstr & dirname,
	tRisaFileSystemIterationCallback * callback)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

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
//! @brief		ファイルが存在するかどうかを得る
//! @param		filename ファイル名
//! @return		ファイルが存在する場合真
//---------------------------------------------------------------------------
bool tRisaPathFS::FileExists(const ttstr & filename)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	ttstr fn;
	if(filename.StartsWith(RISSE_WC('/')))
		fn = filename.c_str() + 1; // 先頭の '/' を取り除く
	else
		fn = filename;

	return Hash.Find(fn) != NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリが存在するかどうかを得る
//! @param		dirname ディレクトリ名
//! @return		ディレクトリが存在する場合真
//---------------------------------------------------------------------------
bool tRisaPathFS::DirectoryExists(const ttstr & dirname)
{
	// PathFS にはサブディレクトリは存在しない
	if(dirname != RISSE_WS("/") || !dirname.IsEmpty())
		return false;
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルを削除する
//! @param		filename ファイル名
//---------------------------------------------------------------------------
void tRisaPathFS::RemoveFile(const ttstr & filename)
{
	eRisaException::Throw(RISSE_WS_TR("can not delete file (filesystem is read-only)"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリを削除する
//! @param		dirname ディレクトリ名
//! @param		recursive 再帰的にディレクトリを削除するかどうか
//---------------------------------------------------------------------------
void tRisaPathFS::RemoveDirectory(const ttstr & dirname, bool recursive)
{
	eRisaException::Throw(RISSE_WS_TR("can not delete directory (filesystem is read-only)"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリを作成する
//! @param		dirname ディレクトリ名
//! @param		recursive 再帰的にディレクトリを作成するかどうか
//---------------------------------------------------------------------------
void tRisaPathFS::CreateDirectory(const ttstr & dirname, bool recursive)
{
	eRisaException::Throw(RISSE_WS_TR("can not make directory (filesystem is read-only)"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたファイルの stat を得る
//! @param		filename ファイル名
//! @param		struc stat 結果の出力先
//---------------------------------------------------------------------------
void tRisaPathFS::Stat(const ttstr & filename, tRisaStatStruc & struc)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	ttstr fn;
	if(filename.StartsWith(RISSE_WC('/')))
		fn = filename.c_str() + 1; // 先頭の '/' を取り除く
	else
		fn = filename;

	Ensure();

	ttstr * mapped_filename = Hash.Find(fn);
	if(!mapped_filename)
		tRisaFileSystemManager::RaiseNoSuchFileOrDirectoryError();

	tRisaFileSystemManager::instance()->Stat(*mapped_filename, struc);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたファイルのストリームを得る
//! @param		filename ファイル名
//! @param		flags フラグ
//! @return		ストリームオブジェクト
//---------------------------------------------------------------------------
tRisseBinaryStream * tRisaPathFS::CreateStream(const ttstr & filename, risse_uint32 flags)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	ttstr fn;
	if(filename.StartsWith(RISSE_WC('/')))
		fn = filename.c_str() + 1; // 先頭の '/' を取り除く
	else
		fn = filename;

	Ensure();

	ttstr * mapped_filename = Hash.Find(fn);
	if(!mapped_filename)
		tRisaFileSystemManager::RaiseNoSuchFileOrDirectoryError();

	return tRisaFileSystemManager::instance()->CreateStream(*mapped_filename, flags);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		パスにディレクトリを追加する
//! @brief		name: 名前
//! @brief		recursive: 再帰的に名前を検索するかどうか
//---------------------------------------------------------------------------
void tRisaPathFS::Add(const ttstr & name, bool recursive)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	// ディレクトリ名の最後に '/' がついていなければ追加
	ttstr fn(name);
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
//! @brief		パスからディレクトリを削除する
//! @param		name 名前
//---------------------------------------------------------------------------
void tRisaPathFS::Remove(const ttstr & name)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	std::vector<ttstr>::iterator i;

	// ディレクトリ名の最後に '/' がついていなければ追加
	ttstr fn(name);
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
//! @brief		ファイルシステム中のファイル名と実際のファイル名の対応表を作り直す
//---------------------------------------------------------------------------
void tRisaPathFS::Ensure()
{
	if(!NeedRebuild) return; // 作り直す必要はない

	// コールバックを準備
	class tCallback : public tRisaFileSystemIterationCallback
	{
		tHash & Hash;
		ttstr DirName;
	public:
		tCallback(tHash & hash) : Hash(hash) {;}

		bool OnFile(const ttstr & filename)
		{
			// filename の名前だけを取り出す
			ttstr basename;
			tRisaFileSystemManager::SplitPathAndName(filename, NULL, &basename);

			// 表に追加
			Hash.Add(basename, DirName + filename);
			return true;
		}
		bool OnDirectory(const ttstr & dirname)
		{
			// ディレクトリには興味はない
			return true;
		}
		void SetDirName(const ttstr & name) { DirName = name; }
	} callback(Hash);

	// 表をクリア
	Hash.Clear();

	// 全てのパスに対して
	for(std::vector<ttstr>::iterator i = Paths.begin(); i != Paths.end(); i++)
	{
		bool recursive = (i->c_str()[0] == static_cast<risse_char>(RISSE_WC('+')));
		ttstr dirname(i->c_str() + 1);
		tRisaFileSystemManager::instance()->GetFileListAt(dirname, &callback, recursive);
	}

	// フラグを倒す
	NeedRebuild = false;
}
//---------------------------------------------------------------------------











