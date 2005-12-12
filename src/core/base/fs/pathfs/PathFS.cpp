//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Pathファイルシステムの実装
//---------------------------------------------------------------------------

#include "FSManager.h"

#include <algorithm>



//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTVPPathFS::tTVPPathFS()
{
	NeedRebuld = true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPPathFS::~tTVPPathFS()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイル一覧を取得する
//! @param		dirname ディレクトリ名
//! @param		callback コールバックオブジェクト
//! @return		取得できたファイル数
//---------------------------------------------------------------------------
size_t tTVPPathFS::GetFileListAt(const ttstr & dirname,
	iTVPFileSystemIterationCallback * callback)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	// PathFS にはファイルシステムの / しか存在しない
	// そのためディレクトリ指定は / へのアクセスしか認めない
	if(dirname != TJS_W("/") || !dirname.IsEmpty())
		TVPThrowExceptionMessage(_("no such directory"));

	Ensure();

	tjs_size count = 0;
	for(tHash::tIterator i = Hash.GetFirst(); !i.IsNull(); i++)
	{
		count ++;
		if(callback) if(!callback->OnFile(i->GetKey())) break;
	}

	return count;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルが存在するかどうかを得る
//! @param		filename ファイル名
//! @return		ファイルが存在する場合真
//---------------------------------------------------------------------------
bool tTVPPathFS::FileExists(const ttstr & filename)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	ttstr fn;
	if(filename.StartsWith(TJS_W('/')))
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
bool tTVPPathFS::DirectoryExists(const ttstr & dirname)
{
	// PathFS にはサブディレクトリは存在しない
	if(dirname != TJS_W("/") || !dirname.IsEmpty())
		return false;
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルを削除する
//! @param		filename ファイル名
//---------------------------------------------------------------------------
void tTVPPathFS::RemoveFile(const ttstr & filename)
{
	TVPThrowExceptionMessage(_("can not delete file (filesystem is read-only)"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリを削除する
//! @param		dirname ディレクトリ名
//! @param		recursive 再帰的にディレクトリを削除するかどうか
//---------------------------------------------------------------------------
void tTVPPathFS::RemoveDirectory(const ttstr & dirname, bool recursive)
{
	TVPThrowExceptionMessage(_("can not delete directory (filesystem is read-only)"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリを作成する
//! @param		dirname ディレクトリ名
//! @param		recursive 再帰的にディレクトリを作成するかどうか
//---------------------------------------------------------------------------
void tTVPPathFS::CreateDirectory(const ttstr & dirname, bool recursive)
{
	TVPThrowExceptionMessage(_("can not make directory (filesystem is read-only)"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたファイルの stat を得る
//! @param		filename ファイル名
//! @param		struc stat 結果の出力先
//---------------------------------------------------------------------------
void tTVPPathFS::Stat(const ttstr & filename, tTVPStatStruc & struc)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	ttstr fn;
	if(filename.StartsWith(TJS_W('/')))
		fn = filename.c_str() + 1; // 先頭の '/' を取り除く
	else
		fn = filename;

	Ensure();

	ttstr * mapped_filename = Hash.Find(fn);
	if(!mapped_filename)
		tTVPFileSystemManager::RaiseNoSuchFileOrDirectoryError();

	tTVPFileSystemManager::pointer fsman;
	fsman->Stat(*mapped_filename, struc);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたファイルのストリームを得る
//! @param		filename ファイル名
//! @param		flags フラグ
//! @return		ストリームオブジェクト
//---------------------------------------------------------------------------
tTVPBinaryStream * tTVPPathFS::CreateStream(const ttstr & filename, tjs_uint32 flags)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	ttstr fn;
	if(filename.StartsWith(TJS_W('/')))
		fn = filename.c_str() + 1; // 先頭の '/' を取り除く
	else
		fn = filename;

	Ensure();

	ttstr * mapped_filename = Hash.Find(fn);
	if(!mapped_filename)
		tTVPFileSystemManager::RaiseNoSuchFileOrDirectoryError();

	tTVPFileSystemManager::pointer fsman;
	return fsman->CreateStream(*mapped_filename, flags);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		パスにディレクトリを追加する
//! @brief		name: 名前
//! @brief		recursive: 再帰的に名前を検索するかどうか
//---------------------------------------------------------------------------
void tTVPPathFS::AddDirectory(const ttstr & name, bool recursive = false)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	// ディレクトリ名の最後に '/' がついていなければ追加
	ttstr fn(name);
	if(fn.EndsWith(TJS_W('/'))) fn += TJS_W('/');

	// ディレクトリが存在しないことを確かにする
	RemoveDirectory(fn);

	// Paths に追加する
	Paths.push_back((recursive ? TJS_W("+") : TJS_W(" ")) + fn);

	// フラグを立てる
	NeedRebuild = true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		パスからディレクトリを削除する
//! @param		name 名前
//---------------------------------------------------------------------------
void tTVPPathFS::RemoveDirectory(const ttstr & name)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	std::vector<ttstr>::iterator i;

	// ディレクトリ名の最後に '/' がついていなければ追加
	ttstr fn(name);
	if(fn.EndsWith(TJS_W('/'))) fn += TJS_W('/');

	// ' ' + name が存在するか
	i = std::find(Paths.begin(), Paths.end(), TJS_W(" ") + fn);
	if(i != Paths.end()) { Paths.erase(i); NeedRebuild = true; return; }

	// '+' + name が存在するか
	i = std::find(Paths.begin(), Paths.end(), TJS_W("+") + fn);
	if(i != Paths.end()) { Paths.erase(i); NeedRebuild = true; return; }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルシステム中のファイル名と実際のファイル名の対応表を作り直す
//---------------------------------------------------------------------------
void tTVPPathFS::Ensure()
{
	if(!NeedRebuild) return; // 作り直す必要はない

	tTVPFileSystemManager::pointer fsman;

	// コールバックを準備
	class tCallback : public iTVPFileSystemIterationCallback
	{
		tHash & Hash;
		ttstr DirName;
	public:
		tCallback(tHash & hash) : Hash(hash) {;}

		bool OnFile(const ttstr & filename)
		{
			// filename の名前だけを取り出す
			ttstr basename;
			tTVPFileSystemManager::SplitPathAndName(filename, NULL, &basename);

			// 表に追加
			Hash.Add(basename, DirName + filename);
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
		bool recursive = (i->c_str() == static_cast<tjs_char>(TJS_W('+')));
		ttstr dirname(i->c_str() + 1);
		fsman->GetFileListAt(dirname, &callback, recursive);
	}

	// フラグを倒す
	NeedRebuild = false;
}
//---------------------------------------------------------------------------











