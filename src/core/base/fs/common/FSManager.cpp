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
#include "prec.h"
TJS_DEFINE_SOURCE_ID(2000);

//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTVPFileSystemManager::tTVPFileSystemManager()
{
	CurrentDirectory = TJS_WS("/");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPFileSystemManager::~tTVPFileSystemManager()
{
	tTJSCriticalSectionHolder holder(CS);

	// すべてのファイルシステムを解放
	tTJSHashTable<ttstr, boost::shared_ptr<iTVPFileSystem> >::tIterator i;
	for(i = MountPoints.GetFirst(); !i.IsNull(); i++)
	{
		i->GetValue()->Release();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルシステムをマウントする
//! @param		point マウントポイント
//! @param		fs ファイルシステムオブジェクト
//---------------------------------------------------------------------------
void tTVPFileSystemManager::Mount(const ttstr & point, boost::shared_ptr<iTVPFileSystem> fs)
{
	tTJSCriticalSectionHolder holder(CS);

	// マウントポイントは / で始まって / で終わる (つまりディレクトリ) を
	// 表していなければならない。そうでない場合はその形式にする
	ttstr path(NormalizePath(point));
	if(!path.EndWith(TJS_WC('/'))) path += TJS_WC('/');

	// すでにその場所にマウントが行われているかどうかをチェックする
	boost::shared_ptr<iTVPFileSystem> * item = MountPoints.Find(path);
	if(item)
	{
		// ファイルシステムが見つかったのでそこにはマウントできない
		TVPThrowExceptionMessage(_("can not mount filesystem: the mount point '$1' is already mounted"), path);
	}

	// マウントポイントを追加
	MountPoints.Add(path, fs);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルシステムをアンマウントする
//! @param		point マウントポイント
//---------------------------------------------------------------------------
void tTVPFileSystemManager::Unmount(const ttstr & point)
{
	tTJSCriticalSectionHolder holder(CS);

	// マウントポイントは / で始まって / で終わる (つまりディレクトリ) を
	// 表していなければならない。そうでない場合はその形式にする
	ttstr path(NormalizePath(point));
	if(!path.EndWith(TJS_WC('/'))) path += TJS_WC('/');

	// その場所にマウントが行われているかどうかをチェックする
	boost::shared_ptr<iTVPFileSystem> * item = MountPoints.Find(path);
	if(!item)
	{
		// そこにはなにもマウントされていない
		TVPThrowExceptionMessage(_("there are no filesystem at mount point '$1'"), path);
	}

	// マウントポイントを削除
	MountPoints.Delete(path);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		パスを正規化する
//! @param		path 正規化したいパス
//! @return		正規化されたパス
//---------------------------------------------------------------------------
ttstr tTVPFileSystemManager::NormalizePath(const ttstr & path)
{
	ttstr ret(path);

	// 相対ディレクトリかどうかをチェック
	// 先頭が '/' でなければ相対パスとみなし、パスの先頭に CurrentDirectory
	// を挿入する
	if(path[0] != TJS_WC('/'))
	{
		tTJSCriticalSectionHolder holder(CS);
		path = CurrentDirectory + path;
	}

	// これから後の変換は、文字列が短くなる方向にしか働かない
	tjs_char *d = ret.Independ();
	tjs_char *s = d;
	tjs_char *start = d;

	// 行う作業は
	// ・ 重複する / の除去
	// ・ ./ の除去
	// ・ ../ の巻き戻し
	while(*s)
	{
		if(s[0] == TJS_WC('/'))
		{
			// *s が /
			if(s[1] == TJS_WC('/'))
			{
				// s[1] も /
				// / が重複している
				s+=2;
				while(*s  == TJS_WC('/')) s++;
				s--;
			}
			else if(s[1] == TJS_WC('.') && s[2] == TJS_WC('.') &&
				(s[3] == 0 || s[3] == TJS_WC('/')))
			{
				// s[2] 以降が ..
				s += 3;
				// d を巻き戻す
				while(d > start && *d != TJS_WC('/')) d--;
				// この時点で d は '/' を指している
			}
			else if(s[1] == TJS_WC('.') &&
				(s[2] == 0 || s[2] == TJS_WC('/')))
			{
				// s[2] 以降が .
				s += 2; // 読み飛ばす
			}
			else
			{
				*(d++) = *s;
				s++;
			}
		}
		else
		{
			*(d++) = *s;
			s++;
		}
	}
	if(d == start) *(d++) = TJS_WC('/'); // 処理によっては最初の / が消えてしまうので
	*d = 0; // 文字列を終結

	ret.FixLen(); // ttstrの内部状態を更新

	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイル一覧を取得する
//! @param		dirname ディレクトリ名
//! @param		callback コールバックオブジェクト
//! @param		recursive 再帰的にファイル一覧を得るかどうか
//! @return		取得できたファイル数
//---------------------------------------------------------------------------
size_t GetFileListAt(const ttstr & dirname,
	iTVPFileSystemIterationCallback * callback, bool recursive = false)
{
	ttstr path(NormalizePath(dirname));

	if(!recursive)
	{
		// 再帰をしないならば
		return InternalGetFileListAt(path, callback); // 話は簡単
	}

	// 再帰をする場合
	class tIteratorCallback : public iTVPFileSystemIterationCallback
	{
		std::vector<ttstr> & List;
		iTVPFileSystemIterationCallback *Destination;
		size_t Count;
		ttstr CurrentDirectory;

		tIteratorCallback(std::vector<ttstr> &list,
			iTVPFileSystemIterationCallback *destination)
				: List(list), Destination(destination), Count(0);
		{
		}

		bool OnFile(const ttstr & filename)
		{
			Count ++;
			if(dest)
				return dest->OnFile(CurrentDirectory + filename);
			return true;
		}
		bool OnDirectory(const ttstr & dirname)
		{
			Count ++;
			ttstr dir(CurrentDirectory  + filename);
			if(dest)
				return dest->OnDirectory(dir);
			List.push_back(dir); // ディレクトリを list に push
		}
	} 
	std::vector<ttstr> list; // ディレクトリのリスト
	list.push_back(ttstr()); // 空ディレクトリを push

	tIteratorCallback localcallback(list, callback);

	while(list.size()) // ディレクトリのリストに残りがある限り繰り返す
	{
		ttstr dir(path + list.back());
		list.pop_back();
		InternalGetFileListAt(dir, &localcallback);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルが存在するかどうかを得る
//! @param		filename ファイル名
//! @return		ファイルが存在する場合真
//---------------------------------------------------------------------------
bool tTVPFileSystemManager::FileExists(const ttstr & filename)
{
	tTJSCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(filename));
	boost::shared_ptr<iTVPFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(filename);
	try
	{
		return fs->FileExists(fspath);
	}
	catch(const eTJSError &e)
	{
		TVPThrowExpceptionMessage(_("failed to retrieve existence of file '%1' : %2"),
			fullpath, e.GetMessage()); // this method never returns
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリが存在するかどうかを得る
//! @param		dirname ディレクトリ名
//! @return		ディレクトリが存在する場合真
//---------------------------------------------------------------------------
bool tTVPFileSystemManager::DirectoryExists(const ttstr & dirname)
{
	tTJSCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(dirname));
	boost::shared_ptr<iTVPFileSystem> GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		return fs->DirectoryExists(fspath);
	}
	catch(const eTJSError &e)
	{
		TVPThrowExpceptionMessage(_("failed to retrieve existence of directory '%1' : %2"),
			fullpath, e.GetMessage()); // this method never returns
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルを削除する
//! @param		filename ファイル名
//---------------------------------------------------------------------------
void tTVPFileSystemManager::RemoveFile(const ttstr & filename)
{
	tTJSCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(filename));
	boost::shared_ptr<iTVPFileSystem> GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		fs->RemoveFile(fspath);
	}
	catch(const eTJSError &e)
	{
		TVPThrowExpceptionMessage(_("failed to remove file '%1' : %2"),
			fullpath, e.GetMessage());
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリを削除する
//! @param		dirname ディレクトリ名
//! @param		recursive 再帰的にディレクトリを削除するかどうか
//---------------------------------------------------------------------------
void tTVPFileSystemManager::RemoveDirectory(const ttstr & dirname, bool recursive)
{
	tTJSCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(dirname));
	boost::shared_ptr<iTVPFileSystem> GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		fs->RemoveDirectory(fspath, recursive);
	}
	catch(const eTJSError &e)
	{
		TVPThrowExpceptionMessage(_("failed to remove directory '%1' : %2"),
			fullpath, e.GetMessage());
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリを作成する
//! @param		dirname ディレクトリ名
//! @param		recursive 再帰的にディレクトリを作成するかどうか
//---------------------------------------------------------------------------
void tTVPFileSystemManager::CreateDirectory(const ttstr & dirname, bool recursive)
{
	tTJSCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(dirname));
	boost::shared_ptr<iTVPFileSystem> GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		fs->CreateDirectory(fspath, recursive);
	}
	catch(const eTJSError &e)
	{
		TVPThrowExpceptionMessage(_("failed to create directory '%1' : %2"),
			fullpath, e.GetMessage());
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたファイルの stat を得る
//! @param		filename ファイル名
//! @param		struc stat 結果の出力先
//---------------------------------------------------------------------------
void tTVPFileSystemManager::Stat(const ttstr & filename, tTVPStatStruc & struc)
{
	tTJSCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(filename));
	boost::shared_ptr<iTVPFileSystem> GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		fs->Stat(fspath, struc);
	}
	catch(const eTJSError &e)
	{
		TVPThrowExpceptionMessage(_("failed to stat '%1' : %2"),
			fullpath, e.GetMessage());
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたファイルのストリームを得る
//! @param		filename ファイル名
//! @param		flags フラグ
//! @return		ストリームオブジェクト
//---------------------------------------------------------------------------
tTVPBinaryStream * tTVPFileSystemManager::CreateStream(const ttstr & filename, tjs_uint32 flags)
{
	tTJSCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(filename));
	boost::shared_ptr<iTVPFileSystem> GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		return fs->CreateStream(fspath, flags);
	}
	catch(const eTJSError &e)
	{
		TVPThrowExpceptionMessage(_("failed to create stream of '%1' : %2"),
			fullpath, e.GetMessage());
	}
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイル一覧を取得する(内部関数)
//! @param		dirname ディレクトリ名(正規化されているべきこと)
//! @param		callback コールバック先
//! @callback	コールバックオブジェクト
//---------------------------------------------------------------------------
size_t tTVPFileSystemManager::InternalGetFileListAt(
	const ttstr & dirname,
	iTVPFileSystemIterationCallback * callback)
{
	tTJSCriticalSectionHolder holder(CS);

	ttstr fspath;
	boost::shared_ptr<iTVPFileSystem> fs = GetFileSystemAt(dirname, &fspath);
	if(!fs) ThrowNoFileSystemError(dirname);
	try
	{
		return fs->GetFileListAt(fspath, callcack);
	}
	catch(const eTJSError &e)
	{
		TVPThrowExpceptionMessage(_("failed to list files in directory '%1' : %2"),
			dirname, e.GetMessage());
	}
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定された正規フルパスに対応するファイルシステムを得る
//! @param		fullpath 正規フルパス
//! @param		fspath ファイルシステム内におけるパス(興味がない場合はNULL可、最初の / は含まれない)
//! @return		ファイルシステムインスタンス
//---------------------------------------------------------------------------
boost::shared_ptr<iTVPFileSystem> tTVPFileSystemManager::GetFileSystemAt(
					const ttstr & fullpath, ttstr * fspath)
{
	// フルパスの最後からディレクトリを削りながら見ていき、最初に
	// マウントポイントに一致したディレクトリに対応するファイルシステムを
	// 返す。
	// マウントポイントに一致したディレクトリが本当に存在するかや、
	// fullpath で指定したディレクトリが本当に存在するかどうかは
	// チェックしない。
	tTJSCriticalSectionHolder holder(CS);

	const tjs_char *start = fullpath.c_str();
	const tjs_char *p = start + fullPath.GetLen();

	while(p >= start)
	{
		if(*p == TJS_WC('/'))
		{
			// p が スラッシュ
			ttstr subpath(start, p - start + 1);
			boost::shared_ptr<iTVPFileSystem> * item =
				MountPoints.Find(subpath);
			if(item)
			{
				if(fspath) *fspath = start + subpath.GetLen();
				return *item; // ファイルシステムが見つかった
			}
		}
		p--;
	}

	// 最低でも / (ルート) に割り当てられているファイルシステムが見つからないと
	// おかしいので、ここに来るはずはないのだが ...
	// (fullpath に空文字列が渡されるとここに来るかもしれない)
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「ファイルシステムが指定されたパスはない」例外を発生させる
//! @param		filenameマウントポイント
//! @note		この関数は例外を発生させるため呼び出し元には戻らない
//---------------------------------------------------------------------------
void tTVPFileSystemManager::ThrowNoFileSystemError(const ttstr & filename)
{
	TVPThrowExceptionMessage(
		_("Could not find filesystem at path '%1'"), mountpoint);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「そのようなファイルやディレクトリは無い」例外を発生させる
//---------------------------------------------------------------------------
void tTVPFileSystemManager::RaiseNoSuchFileOrDirectoryError()
{
	TVPThrowExceptionMessage(
		_("no such file or directory"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		パス名を拡張子より前と拡張子に分離する ( /path/to/file.ext を /path/to/file と .ext に分離する )
//! @param		in 入力パス名
//! @param		other [out] 拡張子より前の部分へポインタ(興味ない場合はNULL可)
//! @param		name [out] 拡張子へのポインタ(興味ない場合はNULL可)  拡張子には .(ドット) を含む。拡張子がない場合は空文字列になる
//! @note		in と そのほかのパラメータに同じ文字列を指定しないこと
//---------------------------------------------------------------------------
void tTVPFileSystemManager::SplitExtension(const ttstr & in, ttstr * other, ttstr * ext)
{
	const tjs_char * p = in.c_str() + in.GetLen();
	const tjs_char * pp = p;
	const tjs_char * start = in.c_str();

	// パス名を最後からスキャン
	while(true)
	{
		if(p < start || *p == TJS_WC('/'))
		{
			// * ファイル名の先頭を超えて前に行った
			// * '/' にぶつかった
			// このファイル名は拡張子を持っていない
			if(other) *other = in;
			if(ext)   ext->Clear();
			return;
		}

		if(*p == TJS_WC('.'))
		{
			// * '.' にぶつかった
			if(other) *other = ttstr(start, p - start);
			if(ext)   *ext   = ttstr(p);
			return;
		}

		p--;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		パス名をパスと名前に分離する ( /path/to/file を /path/to と file に分離する )
//! @param		in 入力パス名
//! @param		path [out] パスへのポインタ(興味ない場合はNULL可)
//! @param		name [out] 名前へのポインタ(興味ない場合はNULL可)
//! @note		in と そのほかのパラメータに同じ文字列を指定しないこと
//---------------------------------------------------------------------------
void tTVPFileSystemManager::SplitPathAndName(const ttstr & in, ttstr * path, ttstr * name)
{
	const tjs_char * p = in.c_str() + in.GetLen();
	const tjs_char * pp = p;
	const tjs_char * start = in.c_str();
	p --;
	while(p > start && *p != TJS_WC('/')) p--;

	if(*p == TJS_WC('/')) p++;

	if(path) *path = ttstr(start, p - start);
	if(name) *name = ttstr(p);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		パス名の最後のパスデリミタ ('/') を取り去る ( /path/is/here/ を /path/is/here にする )
//! @param		path パス
//---------------------------------------------------------------------------
static void tTVPFileSystemManager::TrimLastPathDelimiter(ttstr & path)
{
	if(path.EndWith(TJS_WC('/')))
	{
		tjs_char *s = path.Independ();
		tjs_char *p = s + path.GetLen() - 1;
		while(p >= s && *p == TJS_WC('/')) p--;
		p++;
		*p = 0;
		path.FixLen();
	}
}
//---------------------------------------------------------------------------
