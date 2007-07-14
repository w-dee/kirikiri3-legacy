//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ファイルシステムマネージャ(ファイルシステムの根幹部分)
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/fs/common/FSManager.h"
#include "base/fs/osfs/OSFS.h"
#include "base/exception/RisaException.h"
#include "base/fs/common/FSManagerBind.h"
#include <vector>





namespace Risa {
RISSE_DEFINE_SOURCE_ID(57835,14019,1274,20023,25994,43742,64617,60148);
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tFileSystemManager::tFileSystemManager()
{
	// カレントディレクトリを / に設定
	CurrentDirectory = RISSE_WS("/");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tFileSystemManager::~tFileSystemManager()
{
	volatile tCriticalSection::tLocker holder(CS);

	// 全てのマウントポイントをアンマウントする
	// 全てのRisseオブジェクトをvectorにとり、最後にこれが削除されることにより
	// すべての Risse オブジェクトを解放する。
	gc_vector<tRefHolder<iRisseDispatch2> > objects;
	objects.reserve(MountPoints.GetCount());

	gc_vector<tString> points;

	tHashTable<tString, tFileSystemInfo>::tIterator i;
	for(i = MountPoints.GetFirst(); !i.IsNull(); i++)
	{
		objects.push_back(i.GetValue().Object);
	}

	//- MountPoints をクリア
	MountPoints.Clear();

	//- 全ての objects を invalidate
	for(gc_vector<tRefHolder<iRisseDispatch2> >::iterator i =
		objects.begin(); i != objects.end(); i++)
	{
		i->GetObjectNoAddRef()->Invalidate(0, NULL, NULL, NULL);
	}


	// この時点で objects に配置されたオブジェクトは全て解放される。
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::Mount(const tString & point,
	iRisseDispatch2 * fs_risseobj)
{
	// risse_obj がファイルシステムのインスタンスを持っているかどうかを
	// 確認する
	if(!fs_risseobj ||
		RISSE_FAILED(fs_risseobj->NativeInstanceSupport(
						RISSE_NIS_GETINSTANCE,
						tNI_FileSystemNativeInstance::ClassID,
						NULL)) )
	{
		// ファイルシステムのインスタンスを持っていない
		eRisaException::Throw(RISSE_WS_TR("the object given is not a filesystem object"));
	}

	// マウントポイントは / で始まって / で終わる (つまりディレクトリ) を
	// 表していなければならない。そうでない場合はその形式にする
	tString path(NormalizePath(point));
	if(!path.EndsWith(RISSE_WC('/'))) path += RISSE_WC('/');

	// スレッド保護
	volatile tCriticalSection::tLocker holder(CS);

	// すでにその場所にマウントが行われているかどうかをチェックする
	tFileSystemInfo * item = MountPoints.Find(path);
	if(item)
	{
		// ファイルシステムが見つかったのでそこにはマウントできない
		eRisaException::Throw(RISSE_WS_TR("can not mount filesystem: the mount point '%1' is already mounted"), path);
	}

	// マウントポイントを追加
	tFileSystemInfo info(fs_risseobj);
	MountPoints.Add(path, info);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::Unmount(const tString & point)
{
	// マウントポイントは / で始まって / で終わる (つまりディレクトリ) を
	// 表していなければならない。そうでない場合はその形式にする
	tString path(NormalizePath(point));
	if(!path.EndsWith(RISSE_WC('/'))) path += RISSE_WC('/');

	// スレッド保護
	volatile tCriticalSection::tLocker holder(CS);

	// その場所にマウントが行われているかどうかをチェックする
	tFileSystemInfo * item = MountPoints.Find(path);
	if(!item)
	{
		// そこにはなにもマウントされていない
		eRisaException::Throw(RISSE_WS_TR("there are no filesystem at mount point '%1'"), path);
	}

	// マウントポイントを削除
	volatile tRefHolder<iRisseDispatch2> risse_object_holder (item->Object);
	MountPoints.Delete(path);

	// risse_object_holder はここで削除される。
	// この時点で Object への参照が無くなり、Object が Invalidate
	// される可能性がある。tNI_FileSystemNativeInstance 内では
	// この際に Unmount(Object) を呼び出すが、この時点ではすでに
	// どのマウントポイントにもそのファイルシステムはマウントされていないので
	// 何も操作は行われない。
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::Unmount(iRisseDispatch2 * fs_risseobj)
{
	volatile tCriticalSection::tLocker holder(CS);

	// そのファイルシステムがマウントされているマウントポイントを調べる
	gc_vector<tString> points;

	tHashTable<tString, tFileSystemInfo>::tIterator i;
	for(i = MountPoints.GetFirst(); !i.IsNull(); i++)
	{
		if(i.GetValue().Object.GetObjectNoAddRef() == fs_risseobj)
			points.push_back(i.GetKey());
	}

	// 調べたマウントポイントをすべてアンマウントする
	for(gc_vector<tString>::iterator i = points.begin(); i != points.end(); i++)
	{
		Unmount(*i);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tFileSystemManager::NormalizePath(const tString & path)
{
	tString ret(path);

	// 相対ディレクトリかどうかをチェック
	// 先頭が '/' でなければ相対パスとみなし、パスの先頭に CurrentDirectory
	// を挿入する
	if(ret[0] != RISSE_WC('/'))
	{
		volatile tCriticalSection::tLocker holder(CS);
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

	ret.FixLen(); // tStringの内部状態を更新

	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tFileSystemManager::GetFileListAt(const tString & dirname,
	tFileSystemIterationCallback * callback, bool recursive)
{
	tString path(NormalizePath(dirname));

	if(!recursive)
	{
		// 再帰をしないならば
		return InternalGetFileListAt(path, callback); // 話は簡単
	}

	// 再帰をする場合
	class tIteratorCallback : public tFileSystemIterationCallback
	{
		gc_vector<tString> & List;
		tFileSystemIterationCallback *Destination;
		size_t Count;
		tString CurrentDirectory;

	public:
		tIteratorCallback(gc_vector<tString> &list,
			tFileSystemIterationCallback *destination)
				: List(list), Destination(destination), Count(0)
		{
			;
		}

		bool OnFile(const tString & filename)
		{
			Count ++;
			if(Destination)
				return Destination->OnFile(CurrentDirectory + filename);
			return true;
		}
		bool OnDirectory(const tString & dirname)
		{
			Count ++;
			tString dir(CurrentDirectory  + dirname);
			List.push_back(dir); // ディレクトリを list に push
			if(Destination)
				return Destination->OnDirectory(dir);
			return true;
		}

		size_t GetCount() const { return Count; }
	} ;
	gc_vector<tString> list; // ディレクトリのリスト
	list.push_back(tString()); // 空ディレクトリを push

	tIteratorCallback localcallback(list, callback);

	while(list.size()) // ディレクトリのリストに残りがある限り繰り返す
	{
		tString dir(path + list.back());
		list.pop_back();
		InternalGetFileListAt(dir, &localcallback);
	}

	return localcallback.GetCount();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tFileSystemManager::FileExists(const tString & filename)
{
	volatile tCriticalSection::tLocker holder(CS);

	tString fspath;
	tString fullpath(NormalizePath(filename));
	boost::shared_ptr<tFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(filename);
	try
	{
		return fs->FileExists(fspath);
	}
	catch(const eRisseError &e)
	{
		eRisaException::Throw(RISSE_WS_TR("failed to retrieve existence of file '%1' : %2"),
			fullpath, e.GetMessageString()); // this method never returns
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tFileSystemManager::DirectoryExists(const tString & dirname)
{
	volatile tCriticalSection::tLocker holder(CS);

	tString fspath;
	tString fullpath(NormalizePath(dirname));
	boost::shared_ptr<tFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		return fs->DirectoryExists(fspath);
	}
	catch(const eRisseError &e)
	{
		eRisaException::Throw(RISSE_WS_TR("failed to retrieve existence of directory '%1' : %2"),
			fullpath, e.GetMessageString()); // this method never returns
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::RemoveFile(const tString & filename)
{
	volatile tCriticalSection::tLocker holder(CS);

	tString fspath;
	tString fullpath(NormalizePath(filename));
	boost::shared_ptr<tFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		fs->RemoveFile(fspath);
	}
	catch(const eRisseError &e)
	{
		eRisaException::Throw(RISSE_WS_TR("failed to remove file '%1' : %2"),
			fullpath, e.GetMessageString());
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::RemoveDirectory(const tString & dirname, bool recursive)
{
	volatile tCriticalSection::tLocker holder(CS);

	tString fspath;
	tString fullpath(NormalizePath(dirname));
	boost::shared_ptr<tFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		fs->RemoveDirectory(fspath, recursive);
	}
	catch(const eRisseError &e)
	{
		eRisaException::Throw(RISSE_WS_TR("failed to remove directory '%1' : %2"),
			fullpath, e.GetMessageString());
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::CreateDirectory(const tString & dirname, bool recursive)
{
	volatile tCriticalSection::tLocker holder(CS);

	tString fspath;
	tString fullpath(NormalizePath(dirname));
	boost::shared_ptr<tFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		fs->CreateDirectory(fspath, recursive);
	}
	catch(const eRisseError &e)
	{
		eRisaException::Throw(RISSE_WS_TR("failed to create directory '%1' : %2"),
			fullpath, e.GetMessageString());
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::Stat(const tString & filename, tStatStruc & struc)
{
	volatile tCriticalSection::tLocker holder(CS);

	tString fspath;
	tString fullpath(NormalizePath(filename));
	boost::shared_ptr<tFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		fs->Stat(fspath, struc);
	}
	catch(const eRisseError &e)
	{
		eRisaException::Throw(RISSE_WS_TR("failed to stat '%1' : %2"),
			fullpath, e.GetMessageString());
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tBinaryStream * tFileSystemManager::CreateStream(const tString & filename,
	risse_uint32 flags)
{
	volatile tCriticalSection::tLocker holder(CS);

	// 先頭が native: で始まる場合は OSFS ストリーム経由で直接アクセスする
	if(filename.StartsWith(RISSE_WS("native:")))
	{
		// +7 = "native:" の文字数
		return new tOSNativeStream(tString(filename.c_str() + 7).AsWxString(), flags);
	}

	// 通常のファイルシステム経由のストリームの作成
	tString fspath;
	tString fullpath(NormalizePath(filename));
	boost::shared_ptr<tFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	try
	{
		return fs->CreateStream(fspath, flags);
	}
	catch(const eRisseError &e)
	{
		eRisaException::Throw(RISSE_WS_TR("failed to create stream of '%1' : %2"),
			fullpath, e.GetMessageString());
	}
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tFileSystemManager::InternalGetFileListAt(
	const tString & dirname,
	tFileSystemIterationCallback * callback)
{
	volatile tCriticalSection::tLocker holder(CS);

	tString fspath;
	boost::shared_ptr<tFileSystem> fs = GetFileSystemAt(dirname, &fspath);
	if(!fs) ThrowNoFileSystemError(dirname);

	try
	{
		return fs->GetFileListAt(fspath, callback);
	}
	catch(const eRisseError &e)
	{
		eRisaException::Throw(RISSE_WS_TR("failed to list files in directory '%1' : %2"),
			dirname, e.GetMessageString());
	}
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
boost::shared_ptr<tFileSystem> tFileSystemManager::GetFileSystemAt(
					const tString & fullpath, tString * fspath)
{
	// フルパスの最後からディレクトリを削りながら見ていき、最初に
	// マウントポイントに一致したディレクトリに対応するファイルシステムを
	// 返す。
	// マウントポイントに一致したディレクトリが本当に存在するかや、
	// fullpath で指定したディレクトリが本当に存在するかどうかは
	// チェックしない。
	const risse_char *start = fullpath.c_str();
	const risse_char *p = start + fullpath.GetLen();

	while(p >= start)
	{
		if(*p == RISSE_WC('/'))
		{
			// p が スラッシュ
			tString subpath(start, p - start + 1);
			tFileSystemInfo * item =
				MountPoints.Find(subpath);
			if(item)
			{
				if(fspath) *fspath = start + subpath.GetLen();

				// item->Object.GetObjectNoAddRef() が Risse オブジェクト
				// RisseオブジェクトからtNI_FileSystemNativeInstanceの
				// ネイティブインスタンスを得る
				iRisseDispatch2 * obj = item->Object.GetObjectNoAddRef();
				tNI_FileSystemNativeInstance * ni;
				if(obj)
				{
					if(RISSE_SUCCEEDED(obj->NativeInstanceSupport(
						RISSE_NIS_GETINSTANCE,
						tNI_FileSystemNativeInstance::ClassID,
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
	return boost::shared_ptr<tFileSystem>();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::ThrowNoFileSystemError(const tString & filename)
{
	eRisaException::Throw(
		RISSE_WS_TR("Could not find filesystem at path '%1'"), filename);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::RaiseNoSuchFileOrDirectoryError()
{
	eRisaException::Throw(
		RISSE_WS_TR("no such file or directory"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::SplitExtension(const tString & in, tString * other, tString * ext)
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
			if(other) *other = tString(start, p - start);
			if(ext)   *ext   = tString(p);
			return;
		}

		p--;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::SplitPathAndName(const tString & in, tString * path, tString * name)
{
	const risse_char * p = in.c_str() + in.GetLen();
	const risse_char * start = in.c_str();
	p --;
	while(p > start && *p != RISSE_WC('/')) p--;

	if(*p == RISSE_WC('/')) p++;

	if(path) *path = tString(start, p - start);
	if(name) *name = tString(p);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::TrimLastPathDelimiter(tString & path)
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
tString tFileSystemManager::ChopExtension(const tString & in)
{
	tString ret;
	SplitExtension(in, &ret, NULL);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tFileSystemManager::ExtractExtension(const tString & in)
{
	tString ret;
	SplitExtension(in, NULL, &ret);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tFileSystemManager::ExtractName(const tString & in)
{
	tString ret;
	SplitPathAndName(in, NULL, &ret);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tFileSystemManager::ExtractPath(const tString & in)
{
	tString ret;
	SplitPathAndName(in, &ret, NULL);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const tString & tFileSystemManager::GetCurrentDirectory()
{
	volatile tCriticalSection::tLocker holder(CS);

	return CurrentDirectory;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::SetCurrentDirectory(const tString &dir)
{
	volatile tCriticalSection::tLocker holder(CS);

	if(dir.EndsWith(RISSE_WC('/')))
		CurrentDirectory = dir;
	else
		CurrentDirectory = dir + RISSE_WS("/");
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa

