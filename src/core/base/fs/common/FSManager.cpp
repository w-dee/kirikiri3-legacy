//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ファイルシステムマネージャ(ファイルシステムの根幹部分)
//---------------------------------------------------------------------------
#include "prec.h"
#include "FSManager.h"
#include "RisaException.h"
#include "FSManagerBind.h"
#include <vector>

RISSE_DEFINE_SOURCE_ID(57835,14019,1274,20023,25994,43742,64617,60148);












//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaFileSystemManager::tRisaFileSystemManager()
{
	// カレントディレクトリを / に設定
	CurrentDirectory = RISSE_WS("/");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaFileSystemManager::~tRisaFileSystemManager()
{
	volatile tRisseCriticalSectionHolder holder(CS);

	// 全てのマウントポイントをアンマウントする
	// 全てのRisseオブジェクトをvectorにとり、最後にこれが削除されることにより
	// すべての Risse オブジェクトを解放する。
	std::vector<tRisseRefHolder<iRisseDispatch2> > objects;
	objects.reserve(MountPoints.GetCount());

	std::vector<ttstr> points;

	tRisseHashTable<ttstr, tFileSystemInfo>::tIterator i;
	for(i = MountPoints.GetFirst(); !i.IsNull(); i++)
	{
		objects.push_back(i.GetValue().RisseObject);
	}

	//- MountPoints をクリア
	MountPoints.Clear();

	//- 全ての objects を invalidate
	for(std::vector<tRisseRefHolder<iRisseDispatch2> >::iterator i =
		objects.begin(); i != objects.end(); i++)
	{
		i->GetObjectNoAddRef()->Invalidate(0, NULL, NULL, NULL);
	}


	// この時点で objects に配置されたオブジェクトは全て解放される。
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルシステムをマウントする
//! @param		point マウントポイント
//! @param		fs_risseobj ファイルシステムオブジェクトを表すRisseオブジェクト
//! @note		メインスレッド以外から呼び出さないこと
//---------------------------------------------------------------------------
void tRisaFileSystemManager::Mount(const ttstr & point,
	iRisseDispatch2 * fs_risseobj)
{
	// risse_obj がファイルシステムのインスタンスを持っているかどうかを
	// 確認する
	if(!fs_risseobj ||
		RISSE_FAILED(fs_risseobj->NativeInstanceSupport(
						RISSE_NIS_GETINSTANCE,
						tRisseNI_FileSystemNativeInstance::ClassID,
						NULL)) )
	{
		// ファイルシステムのインスタンスを持っていない
		eRisaException::Throw(RISSE_WS_TR("the object given is not a filesystem object"));
	}

	volatile tRisseCriticalSectionHolder holder(CS);

	// マウントポイントは / で始まって / で終わる (つまりディレクトリ) を
	// 表していなければならない。そうでない場合はその形式にする
	ttstr path(NormalizePath(point));
	if(!path.EndsWith(RISSE_WC('/'))) path += RISSE_WC('/');

	// すでにその場所にマウントが行われているかどうかをチェックする
	tFileSystemInfo * item = MountPoints.Find(path);
	if(item)
	{
		// ファイルシステムが見つかったのでそこにはマウントできない
		eRisaException::Throw(RISSE_WS_TR("can not mount filesystem: the mount point '$1' is already mounted"), path);
	}

	// マウントポイントを追加
	tFileSystemInfo info(fs_risseobj);
	MountPoints.Add(path, info);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルシステムをアンマウントする
//! @param		point マウントポイント
//! @note		メインスレッド以外から呼び出さないこと
//---------------------------------------------------------------------------
void tRisaFileSystemManager::Unmount(const ttstr & point)
{
	volatile tRisseCriticalSectionHolder holder(CS);

	// マウントポイントは / で始まって / で終わる (つまりディレクトリ) を
	// 表していなければならない。そうでない場合はその形式にする
	ttstr path(NormalizePath(point));
	if(!path.EndsWith(RISSE_WC('/'))) path += RISSE_WC('/');

	// その場所にマウントが行われているかどうかをチェックする
	tFileSystemInfo * item = MountPoints.Find(path);
	if(!item)
	{
		// そこにはなにもマウントされていない
		eRisaException::Throw(RISSE_WS_TR("there are no filesystem at mount point '$1'"), path);
	}

	// マウントポイントを削除
	volatile tRisseRefHolder<iRisseDispatch2> risse_object_holder (item->RisseObject);
	MountPoints.Delete(path);

	// risse_object_holder はここで削除される。
	// この時点で RisseObject への参照が無くなり、RisseObject が Invalidate
	// される可能性がある。tRisseNI_FileSystemNativeInstance 内では
	// この際に Unmount(RisseObject) を呼び出すが、この時点ではすでに
	// どのマウントポイントにもそのファイルシステムはマウントされていないので
	// 何も操作は行われない。
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルシステムをアンマウントする
//! @param		fs_risseobj アンマウントしたいファイルシステムを表すRisseオブジェクト
//! @note		メインスレッド以外から呼び出さないこと
//---------------------------------------------------------------------------
void tRisaFileSystemManager::Unmount(iRisseDispatch2 * fs_risseobj)
{
	volatile tRisseCriticalSectionHolder holder(CS);

	// そのファイルシステムがマウントされているマウントポイントを調べる
	std::vector<ttstr> points;

	tRisseHashTable<ttstr, tFileSystemInfo>::tIterator i;
	for(i = MountPoints.GetFirst(); !i.IsNull(); i++)
	{
		if(i.GetValue().RisseObject.GetObjectNoAddRef() == fs_risseobj)
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
ttstr tRisaFileSystemManager::NormalizePath(const ttstr & path)
{
	ttstr ret(path);

	// 相対ディレクトリかどうかをチェック
	// 先頭が '/' でなければ相対パスとみなし、パスの先頭に CurrentDirectory
	// を挿入する
	if(ret[0] != RISSE_WC('/'))
	{
		volatile tRisseCriticalSectionHolder holder(CS);
		ret = CurrentDirectory + ret;
	}

	// これから後の変換は、文字列が短くなる方向にしか働かない
	risse_char *d = ret.Independ();
	risse_char *s = d;
	risse_char *start = d;

	// 行う作業は
	// ・ 重複する / の除去
	// ・ ./ の除去
	// ・ ../ の巻き戻し
	while(*s)
	{
		if(s[0] == RISSE_WC('/'))
		{
			// *s が /
			if(s[1] == RISSE_WC('/'))
			{
				// s[1] も /
				// / が重複している
				s+=2;
				while(*s  == RISSE_WC('/')) s++;
				s--;
			}
			else if(s[1] == RISSE_WC('.') && s[2] == RISSE_WC('.') &&
				(s[3] == 0 || s[3] == RISSE_WC('/')))
			{
				// s[2] 以降が ..
				s += 3;
				// d を巻き戻す
				while(d > start && *d != RISSE_WC('/')) d--;
				// この時点で d は '/' を指している
			}
			else if(s[1] == RISSE_WC('.') &&
				(s[2] == 0 || s[2] == RISSE_WC('/')))
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
	if(d == start) *(d++) = RISSE_WC('/'); // 処理によっては最初の / が消えてしまうので
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
size_t tRisaFileSystemManager::GetFileListAt(const ttstr & dirname,
	tRisaFileSystemIterationCallback * callback, bool recursive)
{
	ttstr path(NormalizePath(dirname));

	if(!recursive)
	{
		// 再帰をしないならば
		return InternalGetFileListAt(path, callback); // 話は簡単
	}

	// 再帰をする場合
	class tIteratorCallback : public tRisaFileSystemIterationCallback
	{
		std::vector<ttstr> & List;
		tRisaFileSystemIterationCallback *Destination;
		size_t Count;
		ttstr CurrentDirectory;

	public:
		tIteratorCallback(std::vector<ttstr> &list,
			tRisaFileSystemIterationCallback *destination)
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
			List.push_back(dir); // ディレクトリを list に push
			if(Destination)
				return Destination->OnDirectory(dir);
			return true;
		}

		size_t GetCount() const { return Count; }
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

	return localcallback.GetCount();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルが存在するかどうかを得る
//! @param		filename ファイル名
//! @return		ファイルが存在する場合真
//---------------------------------------------------------------------------
bool tRisaFileSystemManager::FileExists(const ttstr & filename)
{
	volatile tRisseCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(filename));
	boost::shared_ptr<tRisaFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(filename);
	try
	{
		return fs->FileExists(fspath);
	}
	catch(const eRisseError &e)
	{
		eRisaException::Throw(RISSE_WS_TR("failed to retrieve existence of file '%1' : %2"),
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
bool tRisaFileSystemManager::DirectoryExists(const ttstr & dirname)
{
	volatile tRisseCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(dirname));
	boost::shared_ptr<tRisaFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		return fs->DirectoryExists(fspath);
	}
	catch(const eRisseError &e)
	{
		eRisaException::Throw(RISSE_WS_TR("failed to retrieve existence of directory '%1' : %2"),
			fullpath, e.GetMessage()); // this method never returns
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルを削除する
//! @param		filename ファイル名
//---------------------------------------------------------------------------
void tRisaFileSystemManager::RemoveFile(const ttstr & filename)
{
	volatile tRisseCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(filename));
	boost::shared_ptr<tRisaFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		fs->RemoveFile(fspath);
	}
	catch(const eRisseError &e)
	{
		eRisaException::Throw(RISSE_WS_TR("failed to remove file '%1' : %2"),
			fullpath, e.GetMessage());
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリを削除する
//! @param		dirname ディレクトリ名
//! @param		recursive 再帰的にディレクトリを削除するかどうか
//---------------------------------------------------------------------------
void tRisaFileSystemManager::RemoveDirectory(const ttstr & dirname, bool recursive)
{
	volatile tRisseCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(dirname));
	boost::shared_ptr<tRisaFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		fs->RemoveDirectory(fspath, recursive);
	}
	catch(const eRisseError &e)
	{
		eRisaException::Throw(RISSE_WS_TR("failed to remove directory '%1' : %2"),
			fullpath, e.GetMessage());
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリを作成する
//! @param		dirname ディレクトリ名
//! @param		recursive 再帰的にディレクトリを作成するかどうか
//---------------------------------------------------------------------------
void tRisaFileSystemManager::CreateDirectory(const ttstr & dirname, bool recursive)
{
	volatile tRisseCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(dirname));
	boost::shared_ptr<tRisaFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		fs->CreateDirectory(fspath, recursive);
	}
	catch(const eRisseError &e)
	{
		eRisaException::Throw(RISSE_WS_TR("failed to create directory '%1' : %2"),
			fullpath, e.GetMessage());
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたファイルの stat を得る
//! @param		filename ファイル名
//! @param		struc stat 結果の出力先
//---------------------------------------------------------------------------
void tRisaFileSystemManager::Stat(const ttstr & filename, tRisaStatStruc & struc)
{
	volatile tRisseCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(filename));
	boost::shared_ptr<tRisaFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		fs->Stat(fspath, struc);
	}
	catch(const eRisseError &e)
	{
		eRisaException::Throw(RISSE_WS_TR("failed to stat '%1' : %2"),
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
tRisseBinaryStream * tRisaFileSystemManager::CreateStream(const ttstr & filename,
	risse_uint32 flags)
{
	volatile tRisseCriticalSectionHolder holder(CS);

	ttstr fspath;
	ttstr fullpath(NormalizePath(filename));
	boost::shared_ptr<tRisaFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		return fs->CreateStream(fspath, flags);
	}
	catch(const eRisseError &e)
	{
		eRisaException::Throw(RISSE_WS_TR("failed to create stream of '%1' : %2"),
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
size_t tRisaFileSystemManager::InternalGetFileListAt(
	const ttstr & dirname,
	tRisaFileSystemIterationCallback * callback)
{
	volatile tRisseCriticalSectionHolder holder(CS);

	ttstr fspath;
	boost::shared_ptr<tRisaFileSystem> fs = GetFileSystemAt(dirname, &fspath);
	if(!fs) ThrowNoFileSystemError(dirname);
	try
	{
		return fs->GetFileListAt(fspath, callback);
	}
	catch(const eRisseError &e)
	{
		eRisaException::Throw(RISSE_WS_TR("failed to list files in directory '%1' : %2"),
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
boost::shared_ptr<tRisaFileSystem> tRisaFileSystemManager::GetFileSystemAt(
					const ttstr & fullpath, ttstr * fspath)
{
	// フルパスの最後からディレクトリを削りながら見ていき、最初に
	// マウントポイントに一致したディレクトリに対応するファイルシステムを
	// 返す。
	// マウントポイントに一致したディレクトリが本当に存在するかや、
	// fullpath で指定したディレクトリが本当に存在するかどうかは
	// チェックしない。
	volatile tRisseCriticalSectionHolder holder(CS);

	const risse_char *start = fullpath.c_str();
	const risse_char *p = start + fullpath.GetLen();

	while(p >= start)
	{
		if(*p == RISSE_WC('/'))
		{
			// p が スラッシュ
			ttstr subpath(start, p - start + 1);
			tFileSystemInfo * item =
				MountPoints.Find(subpath);
			if(item)
			{
				if(fspath) *fspath = start + subpath.GetLen();

				// item->RisseObject.GetObjectNoAddRef() が Risse オブジェクト
				// RisseオブジェクトからtRisseNI_FileSystemNativeInstanceの
				// ネイティブインスタンスを得る
				iRisseDispatch2 * obj = item->RisseObject.GetObjectNoAddRef();
				tRisseNI_FileSystemNativeInstance * ni;
				if(obj)
				{
					if(RISSE_SUCCEEDED(obj->NativeInstanceSupport(
						RISSE_NIS_GETINSTANCE,
						tRisseNI_FileSystemNativeInstance::ClassID,
						(iRisseNativeInstance**)&ni)) )
					{
						return ni->GetFileSystem(); // ファイルシステムが見つかった
					}
				}
			}
			break;
		}
		p--;
	}

	// 通常はここにこない
	// ここにくるのは以下のどちらか
	// ・  / (ルート) に割り当てられているファイルシステムが見つからない
	// ・  fullpath にが渡された

	if(fullpath.GetLen() != 0)
		eRisaException::Throw(
			RISSE_WS_TR("Could not find the root filesystem"));
	return boost::shared_ptr<tRisaFileSystem>();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「ファイルシステムが指定されたパスはない」例外を発生させる
//! @param		filename  マウントポイント
//! @note		この関数は例外を発生させるため呼び出し元には戻らない
//---------------------------------------------------------------------------
void tRisaFileSystemManager::ThrowNoFileSystemError(const ttstr & filename)
{
	eRisaException::Throw(
		RISSE_WS_TR("Could not find filesystem at path '%1'"), filename);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「そのようなファイルやディレクトリは無い」例外を発生させる
//---------------------------------------------------------------------------
void tRisaFileSystemManager::RaiseNoSuchFileOrDirectoryError()
{
	eRisaException::Throw(
		RISSE_WS_TR("no such file or directory"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		パス名を拡張子より前と拡張子に分離する ( /path/to/file.ext を /path/to/file と .ext に分離する )
//! @param		in 入力パス名
//! @param		other [out] 拡張子より前の部分へポインタ(興味ない場合はNULL可)
//! @param		name [out] 拡張子へのポインタ(興味ない場合はNULL可)  拡張子には .(ドット) を含む。拡張子がない場合は空文字列になる
//! @note		in と そのほかのパラメータに同じ文字列を指定しないこと
//---------------------------------------------------------------------------
void tRisaFileSystemManager::SplitExtension(const ttstr & in, ttstr * other, ttstr * ext)
{
	const risse_char * p = in.c_str() + in.GetLen();
	const risse_char * start = in.c_str();

	// パス名を最後からスキャン
	while(true)
	{
		if(p < start || *p == RISSE_WC('/'))
		{
			// * ファイル名の先頭を超えて前に行った
			// * '/' にぶつかった
			// このファイル名は拡張子を持っていない
			if(other) *other = in;
			if(ext)   ext->Clear();
			return;
		}

		if(*p == RISSE_WC('.'))
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
void tRisaFileSystemManager::SplitPathAndName(const ttstr & in, ttstr * path, ttstr * name)
{
	const risse_char * p = in.c_str() + in.GetLen();
	const risse_char * start = in.c_str();
	p --;
	while(p > start && *p != RISSE_WC('/')) p--;

	if(*p == RISSE_WC('/')) p++;

	if(path) *path = ttstr(start, p - start);
	if(name) *name = ttstr(p);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		パス名の最後のパスデリミタ ('/') を取り去る ( /path/is/here/ を /path/is/here にする )
//! @param		path パス
//---------------------------------------------------------------------------
void tRisaFileSystemManager::TrimLastPathDelimiter(ttstr & path)
{
	if(path.EndsWith(RISSE_WC('/')))
	{
		risse_char *s = path.Independ();
		risse_char *p = s + path.GetLen() - 1;
		while(p >= s && *p == RISSE_WC('/')) p--;
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
ttstr tRisaFileSystemManager::ChopExtension(const ttstr & in)
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
ttstr tRisaFileSystemManager::ExtractExtension(const ttstr & in)
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
ttstr tRisaFileSystemManager::ExtractName(const ttstr & in)
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
ttstr tRisaFileSystemManager::ExtractPath(const ttstr & in)
{
	ttstr ret;
	SplitPathAndName(in, &ret, NULL);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		現在の作業ディレクトリを得る
//! @return		作業ディレクトリ
//---------------------------------------------------------------------------
const ttstr & tRisaFileSystemManager::GetCurrentDirectory()
{
	volatile tRisseCriticalSectionHolder holder(CS);

	return CurrentDirectory;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		作業ディレクトリを設定する
//! @param		dir   作業ディレクトリ
//! @note		実際にそのディレクトリが存在するかどうかのチェックは行わない
//---------------------------------------------------------------------------
void tRisaFileSystemManager::SetCurrentDirectory(const ttstr &dir)
{
	volatile tRisseCriticalSectionHolder holder(CS);

	if(dir.EndsWith(RISSE_WC('/')))
		CurrentDirectory = dir;
	else
		CurrentDirectory = dir + RISSE_WS("/");
}
//---------------------------------------------------------------------------
