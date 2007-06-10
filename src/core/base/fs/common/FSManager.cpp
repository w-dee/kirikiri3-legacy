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

RISSE_DEFINE_SOURCE_ID(57835,14019,1274,20023,25994,43742,64617,60148);












//---------------------------------------------------------------------------
tRisaFileSystemManager::tRisaFileSystemManager()
{
	// カレントディレクトリを / に設定
	CurrentDirectory = RISSE_WS("/");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaFileSystemManager::~tRisaFileSystemManager()
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	// 全てのマウントポイントをアンマウントする
	// 全てのRisseオブジェクトをvectorにとり、最後にこれが削除されることにより
	// すべての Risse オブジェクトを解放する。
	gc_vector<tRisseRefHolder<iRisseDispatch2> > objects;
	objects.reserve(MountPoints.GetCount());

	gc_vector<tRisseString> points;

	tRisseHashTable<tRisseString, tFileSystemInfo>::tIterator i;
	for(i = MountPoints.GetFirst(); !i.IsNull(); i++)
	{
		objects.push_back(i.GetValue().RisseObject);
	}

	//- MountPoints をクリア
	MountPoints.Clear();

	//- 全ての objects を invalidate
	for(gc_vector<tRisseRefHolder<iRisseDispatch2> >::iterator i =
		objects.begin(); i != objects.end(); i++)
	{
		i->GetObjectNoAddRef()->Invalidate(0, NULL, NULL, NULL);
	}


	// この時点で objects に配置されたオブジェクトは全て解放される。
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaFileSystemManager::Mount(const tRisseString & point,
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

	// マウントポイントは / で始まって / で終わる (つまりディレクトリ) を
	// 表していなければならない。そうでない場合はその形式にする
	tRisseString path(NormalizePath(point));
	if(!path.EndsWith(RISSE_WC('/'))) path += RISSE_WC('/');

	// スレッド保護
	volatile tRisaCriticalSection::tLocker holder(CS);

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
void tRisaFileSystemManager::Unmount(const tRisseString & point)
{
	// マウントポイントは / で始まって / で終わる (つまりディレクトリ) を
	// 表していなければならない。そうでない場合はその形式にする
	tRisseString path(NormalizePath(point));
	if(!path.EndsWith(RISSE_WC('/'))) path += RISSE_WC('/');

	// スレッド保護
	volatile tRisaCriticalSection::tLocker holder(CS);

	// その場所にマウントが行われているかどうかをチェックする
	tFileSystemInfo * item = MountPoints.Find(path);
	if(!item)
	{
		// そこにはなにもマウントされていない
		eRisaException::Throw(RISSE_WS_TR("there are no filesystem at mount point '%1'"), path);
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
void tRisaFileSystemManager::Unmount(iRisseDispatch2 * fs_risseobj)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	// そのファイルシステムがマウントされているマウントポイントを調べる
	gc_vector<tRisseString> points;

	tRisseHashTable<tRisseString, tFileSystemInfo>::tIterator i;
	for(i = MountPoints.GetFirst(); !i.IsNull(); i++)
	{
		if(i.GetValue().RisseObject.GetObjectNoAddRef() == fs_risseobj)
			points.push_back(i.GetKey());
	}

	// 調べたマウントポイントをすべてアンマウントする
	for(gc_vector<tRisseString>::iterator i = points.begin(); i != points.end(); i++)
	{
		Unmount(*i);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisaFileSystemManager::NormalizePath(const tRisseString & path)
{
	tRisseString ret(path);

	// 相対ディレクトリかどうかをチェック
	// 先頭が '/' でなければ相対パスとみなし、パスの先頭に CurrentDirectory
	// を挿入する
	if(ret[0] != RISSE_WC('/'))
	{
		volatile tRisaCriticalSection::tLocker holder(CS);
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

	ret.FixLen(); // tRisseStringの内部状態を更新

	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tRisaFileSystemManager::GetFileListAt(const tRisseString & dirname,
	tRisaFileSystemIterationCallback * callback, bool recursive)
{
	tRisseString path(NormalizePath(dirname));

	if(!recursive)
	{
		// 再帰をしないならば
		return InternalGetFileListAt(path, callback); // 話は簡単
	}

	// 再帰をする場合
	class tIteratorCallback : public tRisaFileSystemIterationCallback
	{
		gc_vector<tRisseString> & List;
		tRisaFileSystemIterationCallback *Destination;
		size_t Count;
		tRisseString CurrentDirectory;

	public:
		tIteratorCallback(gc_vector<tRisseString> &list,
			tRisaFileSystemIterationCallback *destination)
				: List(list), Destination(destination), Count(0)
		{
			;
		}

		bool OnFile(const tRisseString & filename)
		{
			Count ++;
			if(Destination)
				return Destination->OnFile(CurrentDirectory + filename);
			return true;
		}
		bool OnDirectory(const tRisseString & dirname)
		{
			Count ++;
			tRisseString dir(CurrentDirectory  + dirname);
			List.push_back(dir); // ディレクトリを list に push
			if(Destination)
				return Destination->OnDirectory(dir);
			return true;
		}

		size_t GetCount() const { return Count; }
	} ;
	gc_vector<tRisseString> list; // ディレクトリのリスト
	list.push_back(tRisseString()); // 空ディレクトリを push

	tIteratorCallback localcallback(list, callback);

	while(list.size()) // ディレクトリのリストに残りがある限り繰り返す
	{
		tRisseString dir(path + list.back());
		list.pop_back();
		InternalGetFileListAt(dir, &localcallback);
	}

	return localcallback.GetCount();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisaFileSystemManager::FileExists(const tRisseString & filename)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	tRisseString fspath;
	tRisseString fullpath(NormalizePath(filename));
	boost::shared_ptr<tRisaFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
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
bool tRisaFileSystemManager::DirectoryExists(const tRisseString & dirname)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	tRisseString fspath;
	tRisseString fullpath(NormalizePath(dirname));
	boost::shared_ptr<tRisaFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
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
void tRisaFileSystemManager::RemoveFile(const tRisseString & filename)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	tRisseString fspath;
	tRisseString fullpath(NormalizePath(filename));
	boost::shared_ptr<tRisaFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
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
void tRisaFileSystemManager::RemoveDirectory(const tRisseString & dirname, bool recursive)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	tRisseString fspath;
	tRisseString fullpath(NormalizePath(dirname));
	boost::shared_ptr<tRisaFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
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
void tRisaFileSystemManager::CreateDirectory(const tRisseString & dirname, bool recursive)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	tRisseString fspath;
	tRisseString fullpath(NormalizePath(dirname));
	boost::shared_ptr<tRisaFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
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
void tRisaFileSystemManager::Stat(const tRisseString & filename, tRisaStatStruc & struc)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	tRisseString fspath;
	tRisseString fullpath(NormalizePath(filename));
	boost::shared_ptr<tRisaFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
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
tRisseBinaryStream * tRisaFileSystemManager::CreateStream(const tRisseString & filename,
	risse_uint32 flags)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	// 先頭が native: で始まる場合は OSFS ストリーム経由で直接アクセスする
	if(filename.StartsWith(RISSE_WS("native:")))
	{
		// +7 = "native:" の文字数
		return new tRisaOSNativeStream(tRisseString(filename.c_str() + 7).AsWxString(), flags);
	}

	// 通常のファイルシステム経由のストリームの作成
	tRisseString fspath;
	tRisseString fullpath(NormalizePath(filename));
	boost::shared_ptr<tRisaFileSystem> fs = GetFileSystemAt(fullpath, &fspath);
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
size_t tRisaFileSystemManager::InternalGetFileListAt(
	const tRisseString & dirname,
	tRisaFileSystemIterationCallback * callback)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	tRisseString fspath;
	boost::shared_ptr<tRisaFileSystem> fs = GetFileSystemAt(dirname, &fspath);
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
boost::shared_ptr<tRisaFileSystem> tRisaFileSystemManager::GetFileSystemAt(
					const tRisseString & fullpath, tRisseString * fspath)
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
			tRisseString subpath(start, p - start + 1);
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
void tRisaFileSystemManager::ThrowNoFileSystemError(const tRisseString & filename)
{
	eRisaException::Throw(
		RISSE_WS_TR("Could not find filesystem at path '%1'"), filename);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaFileSystemManager::RaiseNoSuchFileOrDirectoryError()
{
	eRisaException::Throw(
		RISSE_WS_TR("no such file or directory"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaFileSystemManager::SplitExtension(const tRisseString & in, tRisseString * other, tRisseString * ext)
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
			if(other) *other = tRisseString(start, p - start);
			if(ext)   *ext   = tRisseString(p);
			return;
		}

		p--;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaFileSystemManager::SplitPathAndName(const tRisseString & in, tRisseString * path, tRisseString * name)
{
	const risse_char * p = in.c_str() + in.GetLen();
	const risse_char * start = in.c_str();
	p --;
	while(p > start && *p != RISSE_WC('/')) p--;

	if(*p == RISSE_WC('/')) p++;

	if(path) *path = tRisseString(start, p - start);
	if(name) *name = tRisseString(p);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaFileSystemManager::TrimLastPathDelimiter(tRisseString & path)
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
tRisseString tRisaFileSystemManager::ChopExtension(const tRisseString & in)
{
	tRisseString ret;
	SplitExtension(in, &ret, NULL);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisaFileSystemManager::ExtractExtension(const tRisseString & in)
{
	tRisseString ret;
	SplitExtension(in, NULL, &ret);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisaFileSystemManager::ExtractName(const tRisseString & in)
{
	tRisseString ret;
	SplitPathAndName(in, NULL, &ret);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisaFileSystemManager::ExtractPath(const tRisseString & in)
{
	tRisseString ret;
	SplitPathAndName(in, &ret, NULL);
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const tRisseString & tRisaFileSystemManager::GetCurrentDirectory()
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	return CurrentDirectory;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaFileSystemManager::SetCurrentDirectory(const tRisseString &dir)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	if(dir.EndsWith(RISSE_WC('/')))
		CurrentDirectory = dir;
	else
		CurrentDirectory = dir + RISSE_WS("/");
}
//---------------------------------------------------------------------------
