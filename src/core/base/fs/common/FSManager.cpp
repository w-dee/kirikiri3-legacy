//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ファイルシステムマネージャ(ファイルシステムの根幹部分)
//---------------------------------------------------------------------------
#include "prec.h"
#include "FSManager.h"
#include "TVPException.h"
#include "FSManagerBind.h"
#include <vector>

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

	;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルシステムをマウントする
//! @param		point マウントポイント
//! @param		fs_tjsobj ファイルシステムオブジェクトを表すTJSオブジェクト
//! @note		メインスレッド以外から呼び出さないこと
//---------------------------------------------------------------------------
void tTVPFileSystemManager::Mount(const ttstr & point,
	iTJSDispatch2 * fs_tjsobj)
{
	tTJSCriticalSectionHolder holder(CS);

	// tjs_obj がファイルシステムのインスタンスを持っているかどうかを
	// 確認する
	if(!fs_tjsobj ||
		TJS_FAILED(fs_tjsobj->NativeInstanceSupport(
						TJS_NIS_GETINSTANCE,
						tTJSNI_FileSystemNativeInstance::ClassID,
						NULL)) )
	{
		// ファイルシステムのインスタンスを持っていない
		eTVPException::Throw(TJS_WS_TR("the object given is not a filesystem object"));
	}

	// マウントポイントは / で始まって / で終わる (つまりディレクトリ) を
	// 表していなければならない。そうでない場合はその形式にする
	ttstr path(NormalizePath(point));
	if(!path.EndsWith(TJS_WC('/'))) path += TJS_WC('/');

	// すでにその場所にマウントが行われているかどうかをチェックする
	tFileSystemInfo * item = MountPoints.Find(path);
	if(item)
	{
		// ファイルシステムが見つかったのでそこにはマウントできない
		eTVPException::Throw(TJS_WS_TR("can not mount filesystem: the mount point '$1' is already mounted"), path);
	}

	// マウントポイントを追加
	tFileSystemInfo info(fs_tjsobj);
	MountPoints.Add(path, info);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルシステムをアンマウントする
//! @param		point マウントポイント
//! @note		メインスレッド以外から呼び出さないこと
//---------------------------------------------------------------------------
void tTVPFileSystemManager::Unmount(const ttstr & point)
{
	tTJSCriticalSectionHolder holder(CS);

	// マウントポイントは / で始まって / で終わる (つまりディレクトリ) を
	// 表していなければならない。そうでない場合はその形式にする
	ttstr path(NormalizePath(point));
	if(!path.EndsWith(TJS_WC('/'))) path += TJS_WC('/');

	// その場所にマウントが行われているかどうかをチェックする
	tFileSystemInfo * item = MountPoints.Find(path);
	if(!item)
	{
		// そこにはなにもマウントされていない
		eTVPException::Throw(TJS_WS_TR("there are no filesystem at mount point '$1'"), path);
	}

	// マウントポイントを削除
	tTJSRefHolder<iTJSDispatch2> tjs_object_holder (item->TJSObject);
	MountPoints.Delete(path);

	// tjs_object_holder はここで削除される。
	// この時点で TJSObject への参照が無くなり、TJSObject が Invalidate
	// される可能性がある。tTJSNI_FileSystemNativeInstance 内では
	// この際に Unmount(TJSObject) を呼び出すが、この時点ではすでに
	// どのマウントポイントにもそのファイルシステムはマウントされていないので
	// 何も操作は行われない。
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルシステムをアンマウントする
//! @param		fs_tjsobj アンマウントしたいファイルシステムを表すTJSオブジェクト
//! @note		メインスレッド以外から呼び出さないこと
//---------------------------------------------------------------------------
void tTVPFileSystemManager::Unmount(iTJSDispatch2 * fs_tjsobj)
{
	tTJSCriticalSectionHolder holder(CS);

	// そのファイルシステムがマウントされているマウントポイントを調べる
	std::vector<ttstr> points;

	tTJSHashTable<ttstr, tFileSystemInfo>::tIterator i;
	for(i = MountPoints.GetFirst(); !i.IsNull(); i++)
	{
		if(i.GetValue().TJSObject.GetObjectNoAddRef() == fs_tjsobj)
			points.push_back(i.GetKey());
	}

	// 調べたマウントポイントをすべてアンマウントする
	for(std::vector<ttstr>::iterator i = points.begin(); i != points.end(); i++)
	{
		Unmount(*i);
	}
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
	if(ret[0] != TJS_WC('/'))
	{
		tTJSCriticalSectionHolder holder(CS);
		ret = CurrentDirectory + ret;
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
size_t tTVPFileSystemManager::GetFileListAt(const ttstr & dirname,
	tTVPFileSystemIterationCallback * callback, bool recursive)
{
	ttstr path(NormalizePath(dirname));

	if(!recursive)
	{
		// 再帰をしないならば
		return InternalGetFileListAt(path, callback); // 話は簡単
	}

	// 再帰をする場合
	class tIteratorCallback : public tTVPFileSystemIterationCallback
	{
		std::vector<ttstr> & List;
		tTVPFileSystemIterationCallback *Destination;
		size_t Count;
		ttstr CurrentDirectory;

	public:
		tIteratorCallback(std::vector<ttstr> &list,
			tTVPFileSystemIterationCallback *destination)
				: List(list), Destination(destination), Count(0)
		{
			;
		}

		bool OnFile(const ttstr & filename)
		{
			Count ++;
			if(Destination)
				return Destination->OnFile(CurrentDirectory + filename);
			return true;
		}
		bool OnDirectory(const ttstr & dirname)
		{
			Count ++;
			ttstr dir(CurrentDirectory  + dirname);
			if(Destination)
				return Destination->OnDirectory(dir);
			List.push_back(dir); // ディレクトリを list に push
		}
	} ;
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
	boost::shared_ptr<tTVPFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(filename);
	try
	{
		return fs->FileExists(fspath);
	}
	catch(const eTJSError &e)
	{
		eTVPException::Throw(TJS_WS_TR("failed to retrieve existence of file '%1' : %2"),
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
	boost::shared_ptr<tTVPFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		return fs->DirectoryExists(fspath);
	}
	catch(const eTJSError &e)
	{
		eTVPException::Throw(TJS_WS_TR("failed to retrieve existence of directory '%1' : %2"),
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
	boost::shared_ptr<tTVPFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		fs->RemoveFile(fspath);
	}
	catch(const eTJSError &e)
	{
		eTVPException::Throw(TJS_WS_TR("failed to remove file '%1' : %2"),
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
	boost::shared_ptr<tTVPFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		fs->RemoveDirectory(fspath, recursive);
	}
	catch(const eTJSError &e)
	{
		eTVPException::Throw(TJS_WS_TR("failed to remove directory '%1' : %2"),
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
	boost::shared_ptr<tTVPFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		fs->CreateDirectory(fspath, recursive);
	}
	catch(const eTJSError &e)
	{
		eTVPException::Throw(TJS_WS_TR("failed to create directory '%1' : %2"),
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
	boost::shared_ptr<tTVPFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		fs->Stat(fspath, struc);
	}
	catch(const eTJSError &e)
	{
		eTVPException::Throw(TJS_WS_TR("failed to stat '%1' : %2"),
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
tTJSBinaryStream * tTVPFileSystemManager::CreateStream(const ttstr & filename,
	tjs_uint32 flags)
{
	tTJSCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(filename));
	boost::shared_ptr<tTVPFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		return fs->CreateStream(fspath, flags);
	}
	catch(const eTJSError &e)
	{
		eTVPException::Throw(TJS_WS_TR("failed to create stream of '%1' : %2"),
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
	tTVPFileSystemIterationCallback * callback)
{
	tTJSCriticalSectionHolder holder(CS);

	ttstr fspath;
	boost::shared_ptr<tTVPFileSystem> fs = GetFileSystemAt(dirname, &fspath);
	if(!fs) ThrowNoFileSystemError(dirname);
	try
	{
		return fs->GetFileListAt(fspath, callback);
	}
	catch(const eTJSError &e)
	{
		eTVPException::Throw(TJS_WS_TR("failed to list files in directory '%1' : %2"),
			dirname, e.GetMessage());
	}
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定された正規フルパスに対応するファイルシステムを得る
//! @param		fullpath 正規フルパス
//! @param		fspath ファイルシステム内におけるパス(興味がない場合はNULL可、最初の / は含まれない)
//! @return		ファイルシステムインスタンス
//---------------------------------------------------------------------------
boost::shared_ptr<tTVPFileSystem> tTVPFileSystemManager::GetFileSystemAt(
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
	const tjs_char *p = start + fullpath.GetLen();

	while(p >= start)
	{
		if(*p == TJS_WC('/'))
		{
			// p が スラッシュ
			ttstr subpath(start, p - start + 1);
			tFileSystemInfo * item =
				MountPoints.Find(subpath);
			if(item)
			{
				if(fspath) *fspath = start + subpath.GetLen();

				// item->TJSObject.GetObjectNoAddRef() が TJS オブジェクト
				// TJSオブジェクトからtTJSNI_FileSystemNativeInstanceの
				// ネイティブインスタンスを得る
				iTJSDispatch2 * obj = item->TJSObject.GetObjectNoAddRef();
				tTJSNI_FileSystemNativeInstance * ni;
				if(obj)
				{
					if(TJS_SUCCEEDED(obj->NativeInstanceSupport(
						TJS_NIS_GETINSTANCE,
						tTJSNI_FileSystemNativeInstance::ClassID,
						(iTJSNativeInstance**)&ni)) )
					{
						return ni->GetFileSystem(); // ファイルシステムが見つかった
					}
				}
			}
			break;
		}
		p--;
	}

	// 最低でも / (ルート) に割り当てられているファイルシステムが見つからないと
	// おかしいので、ここに来るはずはないのだが ...
	// (fullpath に空文字列が渡されるとここに来るかもしれない)
	return boost::shared_ptr<tTVPFileSystem>();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「ファイルシステムが指定されたパスはない」例外を発生させる
//! @param		filename  マウントポイント
//! @note		この関数は例外を発生させるため呼び出し元には戻らない
//---------------------------------------------------------------------------
void tTVPFileSystemManager::ThrowNoFileSystemError(const ttstr & filename)
{
	eTVPException::Throw(
		TJS_WS_TR("Could not find filesystem at path '%1'"), filename);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「そのようなファイルやディレクトリは無い」例外を発生させる
//---------------------------------------------------------------------------
void tTVPFileSystemManager::RaiseNoSuchFileOrDirectoryError()
{
	eTVPException::Throw(
		TJS_WS_TR("no such file or directory"));
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
void tTVPFileSystemManager::TrimLastPathDelimiter(ttstr & path)
{
	if(path.EndsWith(TJS_WC('/')))
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


//---------------------------------------------------------------------------
//! @brief		拡張子を切り落とす
//! @param		in 処理したいファイル名
//! @return		拡張子が取り落とされたファイル名
//---------------------------------------------------------------------------
ttstr tTVPFileSystemManager::ChopExtension(const ttstr & in)
{
	ttstr ret;
	SplitExtension(in, &ret, NULL);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		拡張子を取り出す
//! @param		in 処理したいファイル名
//! @return		拡張子(ドットも含む; 拡張子が無い場合は空文字)
//---------------------------------------------------------------------------
ttstr tTVPFileSystemManager::ExtractExtension(const ttstr & in)
{
	ttstr ret;
	SplitExtension(in, NULL, &ret);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイル名を返す (パスの部分を含まない)
//! @param		in 処理したいファイル名
//! @return		ファイル名
//---------------------------------------------------------------------------
ttstr tTVPFileSystemManager::ExtractName(const ttstr & in)
{
	ttstr ret;
	SplitPathAndName(in, NULL, &ret);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイル名のパス名を返す
//! @param		in 処理したいファイル名
//! @return		パス名
//---------------------------------------------------------------------------
ttstr tTVPFileSystemManager::ExtractPath(const ttstr & in)
{
	ttstr ret;
	SplitPathAndName(in, &ret, NULL);
	return ret;
}
//---------------------------------------------------------------------------

