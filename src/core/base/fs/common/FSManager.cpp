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


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTVPFileSystemManager::tTVPFileSystemManager()
{
	CurrentDirectory = TJS_W("/");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPFileSystemManager::~tTVPFileSystemManager()
{
	tTJSCriticalSectionHolder holder(CS);

	// すべてのファイルシステムを解放
	tTJSHashTable<ttstr, iTVPFileSystem *>::tIterator i;
	for(i = MountPoints.GetFirst(); !i.IsNull(); i++)
	{
		i->GetValue()->Release();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルシステムをマウントする
//! @param		point: マウントポイント
//! @param		fs: ファイルシステムオブジェクト
//---------------------------------------------------------------------------
void tTVPFileSystemManager::Mount(const ttstr & point, iTVPFileSystem * fs)
{
	tTJSCriticalSectionHolder holder(CS);

	// マウントポイントは / で始まって / で終わる (つまりディレクトリ) を
	// 表していなければならない。そうでない場合はその形式にする
	ttstr path(NormalizePath(point));
	if(!path.EndWith(TJS_W('/'))) path += TJS_W('/');

	// すでにその場所にマウントが行われているかどうかをチェックする
	iTVPFileSystem ** item = MountPoints.Find(path);
	if(item)
	{
		// ファイルシステムが見つかったのでそこにはマウントできない
		TVPThrowExceptionMessage(_("can not mount filesystem: the mount point '$1' is already mounted"), path);
	}

	// マウントポイントを追加
	MountPoints.Add(path, fs);

	// fs を AddRef
	fs->AddRef();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルシステムをアンマウントする
//! @param		point: マウントポイント
//---------------------------------------------------------------------------
void tTVPFileSystemManager::Unmount(const ttstr & point)
{
	tTJSCriticalSectionHolder holder(CS);

	// マウントポイントは / で始まって / で終わる (つまりディレクトリ) を
	// 表していなければならない。そうでない場合はその形式にする
	ttstr path(NormalizePath(point));
	if(!path.EndWith(TJS_W('/'))) path += TJS_W('/');

	// その場所にマウントが行われているかどうかをチェックする
	iTVPFileSystem ** item = MountPoints.Find(path);
	if(!item)
	{
		// そこにはなにもマウントされていない
		TVPThrowExceptionMessage(_("there are no filesystem at mount point '$1'"), path);
	}

	// マウントポイントを削除
	MountPoints.Delete(path);

	// ファイルシステムを Release
	(*item)->Release();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		パスを正規化する
//! @param		path: 正規化したいパス
//! @return		正規化されたパス
//---------------------------------------------------------------------------
ttstr tTVPFileSystemManager::NormalizePath(const ttstr & path)
{
	ttstr ret(path);

	// 相対ディレクトリかどうかをチェック
	// 先頭が '/' でなければ相対パスとみなし、パスの先頭に CurrentDirectory
	// を挿入する
	if(path[0] != TJS_W('/'))
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
		if(s[0] == TJS_W('/'))
		{
			// *s が /
			if(s[1] == TJS_W('/'))
			{
				// s[1] も /
				// / が重複している
				s+=2;
				while(*s  == TJS_W('/')) s++;
				s--;
			}
			else if(s[1] == TJS_W('.') && s[2] == TJS_W('.') &&
				(s[3] == 0 || s[3] == TJS_W('/')))
			{
				// s[2] 以降が ..
				s += 3;
				// d を巻き戻す
				while(d > start && *d != TJS_W('/')) d--;
				// この時点で d は '/' を指している
			}
			else if(s[1] == TJS_W('.') &&
				(s[2] == 0 || s[2] == TJS_W('/')))
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
	if(d == start) *(d++) = TJS_W('/'); // 処理によっては最初の / が消えてしまうので
	*d = 0; // 文字列を終結

	ret.FixLen(); // ttstrの内部状態を更新

	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイル一覧を取得する
//! @param		dirname: ディレクトリ名
//! @param		callback: コールバックオブジェクト
//! @param		recursive: 再帰的にファイル一覧を得るかどうか
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
//! @param		filename: ファイル名
//! @return		ファイルが存在する場合真
//---------------------------------------------------------------------------
bool tTVPFileSystemManager::FileExists(const ttstr & filename)
{
	tTJSCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(filename));
	tTVPFileSystem * fs = GetFileSystemAtNoAddRef(fullpath, &fspath);
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
//! @param		dirname: ディレクトリ名
//! @return		ディレクトリが存在する場合真
//---------------------------------------------------------------------------
bool tTVPFileSystemManager::DirectoryExists(const ttstr & dirname)
{
	tTJSCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(dirname));
	tTVPFileSystem * fs = GetFileSystemAtNoAddRef(fullpath, &fspath);
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
//! @param		filename: ファイル名
//---------------------------------------------------------------------------
void tTVPFileSystemManager::RemoveFile(const ttstr & filename)
{
	tTJSCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(filename));
	tTVPFileSystem * fs = GetFileSystemAtNoAddRef(fullpath, &fspath);
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
//! @param		dirname: ディレクトリ名
//! @param		recursive: 再帰的にディレクトリを削除するかどうか
//---------------------------------------------------------------------------
void tTVPFileSystemManager::RemoveDirectory(const ttstr & dirname, bool recursive)
{
	tTJSCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(dirname));
	tTVPFileSystem * fs = GetFileSystemAtNoAddRef(fullpath, &fspath);
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
//! @param		dirname: ディレクトリ名
//! @param		recursive: 再帰的にディレクトリを作成するかどうか
//---------------------------------------------------------------------------
void tTVPFileSystemManager::CreateDirectory(const ttstr & dirname, bool recursive)
{
	tTJSCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(dirname));
	tTVPFileSystem * fs = GetFileSystemAtNoAddRef(fullpath, &fspath);
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
//! @param		filename: ファイル名
//! @param		struc: stat 結果の出力先
//---------------------------------------------------------------------------
void tTVPFileSystemManager::Stat(const ttstr & filename, tTVPStatStruc & struc)
{
	tTJSCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(filename));
	tTVPFileSystem * fs = GetFileSystemAtNoAddRef(fullpath, &fspath);
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
//! @param		filename: ファイル名
//! @param		flags: フラグ
//! @return		ストリームオブジェクト
//---------------------------------------------------------------------------
iTVPBinaryStream * tTVPFileSystemManager::CreateStream(const ttstr & filename, tjs_uint32 flags)
{
	tTJSCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(filename));
	tTVPFileSystem * fs = GetFileSystemAtNoAddRef(fullpath, &fspath);
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
//! @param		dirname: ディレクトリ名(正規化されているべきこと)
//! @callback	コールバックオブジェクト
//---------------------------------------------------------------------------
size_t tTVPFileSystemManager::InternalGetFileListAt(
	const ttstr & dirname,
	iTVPFileSystemIterationCallback * callback)
{
	tTJSCriticalSectionHolder holder(CS);

	ttstr fspath;
	tTVPFileSystem * fs = GetFileSystemAtNoAddRef(dirname, &fspath);
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
//! @param		fullpath: 正規フルパス
//! @param		fspath: ファイルシステム内におけるパス(興味がない場合はNULL可)
//! @return		ファイルシステムインスタンス(AddRefされないので注意すること)
//! @note		ここから返されるファイルシステムインスタンスは AddRef されない。
//!				返されたインスタンスを使い終わるまでは、そのインスタンスが
//!				存在し続けることを確実にするため、tTV
//---------------------------------------------------------------------------
iTVPFileSystem * tTVPFileSystemManager::GetFileSystemAtNoAddRef(const ttstr & fullpath, ttstr * fspath)
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
		if(*p == TJS_W('/'))
		{
			// p が スラッシュ
			ttstr subpath(start, p - start + 1);
			iTVPFileSystem ** item =
				MountPoints.Find(subpath);
			if(item)
			{
				/* (*item)->AddRef(); */ // AddRef() されないので注意
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
//! @param		filename:マウントポイント
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
//! @brief		パス名をパスと名前に分離する ( /path/to/file を /path/to と file に分離する )
//! @param		in: 入力パス名
//! @param		path: (出力) パス
//! @param		name: (出力) 名前
//---------------------------------------------------------------------------
void tTVPFileSystemManager::SplitPathAndName(const ttstr & in, ttstr & path, ttstr & name)
{
	const tjs_char * p = in.c_str() + in.GetLen();
	const tjs_char * pp = p;
	const tjs_char * start = in.c_str();
	p --;
	while(p > start && *p != TJS_W('/')) p--;

	if(*p == TJS_W('/')) p++;

	path = ttstr(start, p - start);
	name = ttstr(p);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		パス名の最後のパスデリミタ ('/') を取り去る ( /path/is/here/ を /path/is/here にする )
//! @param		path: パス
//---------------------------------------------------------------------------
static void tTVPFileSystemManager::TrimLastPathDelimiter(ttstr & path)
{
	if(path.EndWith(TJS_W('/')))
	{
		tjs_char *s = path.Independ();
		tjs_char *p = s + path.GetLen() - 1;
		while(p >= s && *p == TJS_W('/')) p--;
		p++;
		*p = 0;
		path.FixLen();
	}
}
//---------------------------------------------------------------------------
