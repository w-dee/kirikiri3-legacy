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
#include <vector>





namespace Risa {
RISSE_DEFINE_SOURCE_ID(57835,14019,1274,20023,25994,43742,64617,60148);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief	ファイルシステム用例外クラス
/*! @note
	ファイルシステム関連の例外クラスとしては、ここの FileSystemException
	(extends RuntimeException) 以外に、IOException がある(Risseエンジン内で定義)。
*/
//---------------------------------------------------------------------------
RISA_DEFINE_EXCEPTION_SUBCLASS(tFileSystemExceptionClass,
	(tSS<'F','i','l','e','S','y','s','t','e','m','E','x','c','e','p','t','i','o','n'>()),
	tRisseScriptEngine::instance()->GetScriptEngine()->RuntimeExceptionClass)
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
tFileSystemManager::tFileSystemManager()
{
	// カレントディレクトリを / に設定
	CurrentDirectory = RISSE_WS("/");
}
//---------------------------------------------------------------------------

/*
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
*/

//---------------------------------------------------------------------------
void tFileSystemManager::Mount(const tString & point,
	tFileSystemInstance * fs_risseobj)
{
	// マウントポイントは / で始まって / で終わる (つまりディレクトリ) を
	// 表していなければならない。そうでない場合はその形式にする
	tString path(NormalizePath(point));
	if(!path.EndsWith(RISSE_WC('/'))) path += RISSE_WC('/');

	// スレッド保護
	volatile tCriticalSection::tLocker holder(CS);

	// すでにその場所にマウントが行われているかどうかをチェックする
	tFileSystemInstance ** item = MountPoints.Find(path);
	if(item)
	{
		// ファイルシステムが見つかったのでそこにはマウントできない
		tFileSystemExceptionClass::Throw(tString(RISSE_WS_TR("can not mount filesystem: the mount point '%1' is already mounted"), path));
	}

	// マウントポイントを追加
	MountPoints.Add(path, fs_risseobj);
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
	tFileSystemInstance ** item = MountPoints.Find(path);
	if(!item)
	{
		// そこにはなにもマウントされていない
		tFileSystemExceptionClass::Throw(tString(RISSE_WS_TR("there are no filesystem at mount point '%1'"), path));
	}

	// マウントポイントを削除
	MountPoints.Delete(path);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::Unmount(tFileSystemInstance * fs_risseobj)
{
	volatile tCriticalSection::tLocker holder(CS);

	// そのファイルシステムがマウントされているマウントポイントを調べる
	gc_vector<tString> points;

	tHashTable<tString, tFileSystemInstance *>::tIterator i(MountPoints);
	for(; !i.End(); ++i)
	{
		if(i.GetValue() == fs_risseobj)
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

	ret.FixLength(); // tStringの内部状態を更新

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

		virtual ~tIteratorCallback() {;}

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
	tFileSystemInstance * fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(filename);
//	try
//	{
		return fs->FileExists(fspath);
//	}
//	catch(const tVariant * e)
//	{
//		tFileSystemExceptionClass::Throw(tString(RISSE_WS_TR("failed to retrieve existence of file '%1' : %2"),
//			fullpath, e->operator tString())); // this method never returns
//	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tFileSystemManager::DirectoryExists(const tString & dirname)
{
	volatile tCriticalSection::tLocker holder(CS);

	tString fspath;
	tString fullpath(NormalizePath(dirname));
	tFileSystemInstance * fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
//	try
//	{
		return fs->DirectoryExists(fspath);
//	}
//	catch(const tVariant * e)
//	{
//		tFileSystemExceptionClass::Throw(tString(RISSE_WS_TR("failed to retrieve existence of directory '%1' : %2"),
//			fullpath, e.GetMessageString())); // this method never returns
//	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::RemoveFile(const tString & filename)
{
	volatile tCriticalSection::tLocker holder(CS);

	tString fspath;
	tString fullpath(NormalizePath(filename));
	tFileSystemInstance * fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
//	try
//	{
		fs->RemoveFile(fspath);
//	}
//	catch(const tVariant * e)
//	{
//		tFileSystemExceptionClass::Throw(RISSE_WS_TR("failed to remove file '%1' : %2"),
//			fullpath, e.GetMessageString());
//	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::RemoveDirectory(const tString & dirname, bool recursive)
{
	volatile tCriticalSection::tLocker holder(CS);

	tString fspath;
	tString fullpath(NormalizePath(dirname));
	tFileSystemInstance * fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
//	try
//	{
		fs->RemoveDirectory(fspath, recursive);
//	}
//	catch(const eRisseError &e)
//	{
//		eRisaException::Throw(RISSE_WS_TR("failed to remove directory '%1' : %2"),
//			fullpath, e.GetMessageString());
//	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::CreateDirectory(const tString & dirname, bool recursive)
{
	volatile tCriticalSection::tLocker holder(CS);

	tString fspath;
	tString fullpath(NormalizePath(dirname));
	tFileSystemInstance * fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
//	try
//	{
		fs->CreateDirectory(fspath, recursive);
//	}
//	catch(const eRisseError &e)
//	{
//		eRisaException::Throw(RISSE_WS_TR("failed to create directory '%1' : %2"),
//			fullpath, e.GetMessageString());
//	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::Stat(const tString & filename, tStatStruc & struc)
{
	volatile tCriticalSection::tLocker holder(CS);

	tString fspath;
	tString fullpath(NormalizePath(filename));
	tFileSystemInstance * fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
//	try
//	{
		fs->Stat(fspath, struc);
//	}
//	catch(const eRisseError &e)
//	{
//		eRisaException::Throw(RISSE_WS_TR("failed to stat '%1' : %2"),
//			fullpath, e.GetMessageString());
//	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tBinaryStream * tFileSystemManager::CreateStream(const tString & filename,
	risse_uint32 flags)
{
	volatile tCriticalSection::tLocker holder(CS);

	// 先頭が native: で始まる場合は OSFS ストリーム経由で直接アクセスする
	if(filename.StartsWith(tSS<'n','a','t','i','v','e',':'>()))
	{
		// +7 = "native:" の文字数
		return new tOSNativeStream(tString(filename.c_str() + 7).AsWxString(), flags);
	}

	// 通常のファイルシステム経由のストリームの作成
	tString fspath;
	tString fullpath(NormalizePath(filename));
	tFileSystemInstance * fs = GetFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
//	try
//	{
		return fs->CreateStream(fspath, flags);
//	}
//	catch(const eRisseError &e)
//	{
//		eRisaException::Throw(RISSE_WS_TR("failed to create stream of '%1' : %2"),
//			fullpath, e.GetMessageString());
//	}
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
	tFileSystemInstance * fs = GetFileSystemAt(dirname, &fspath);
	if(!fs) ThrowNoFileSystemError(dirname);

//	try
//	{
		return fs->GetFileListAt(fspath, callback);
//	}
//	catch(const eRisseError &e)
//	{
//		eRisaException::Throw(RISSE_WS_TR("failed to list files in directory '%1' : %2"),
//			dirname, e.GetMessageString());
//	}
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tFileSystemInstance * tFileSystemManager::GetFileSystemAt(
					const tString & fullpath, tString * fspath)
{
	// フルパスの最後からディレクトリを削りながら見ていき、最初に
	// マウントポイントに一致したディレクトリに対応するファイルシステムを
	// 返す。
	// マウントポイントに一致したディレクトリが本当に存在するかや、
	// fullpath で指定したディレクトリが本当に存在するかどうかは
	// チェックしない。
	const risse_char *start = fullpath.Pointer();
	const risse_char *p = start + fullpath.GetLength();

	while(p >= start)
	{
		if(*p == RISSE_WC('/'))
		{
			// p が スラッシュ
			tString subpath(fullpath, 0, p - start + 1);
			tFileSystemInstance ** item =
				MountPoints.Find(subpath);
			if(item)
			{
				if(fspath)
					*fspath =
						tString(fullpath, p-start, fullpath.GetLength() - (p-start));

				return *item;
			}
		}
		p--;
	}

	// 通常はここにこない
	// ここにくるのは以下のどちらか
	// ・  / (ルート) に割り当てられているファイルシステムが見つからない
	// ・  fullpath にが渡された

	if(fullpath.GetLength() != 0)
		tFileSystemExceptionClass::Throw(
			RISSE_WS_TR("Could not find the root filesystem"));
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::ThrowNoFileSystemError(const tString & filename)
{
	tFileSystemExceptionClass::Throw(
		tString(RISSE_WS_TR("Could not find filesystem at path '%1'"), filename));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::RaiseNoSuchFileOrDirectoryError()
{
	tFileSystemExceptionClass::Throw(
		tString(RISSE_WS_TR("no such file or directory")));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::SplitExtension(const tString & in, tString * other, tString * ext)
{
	// TODO: ここの最適化(tStringの部分文字列共有機能を使えばもっと高速化出来る)

	const risse_char * p = in.c_str() + in.GetLength();
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
	// TODO: ここの最適化(tStringの部分文字列共有機能を使えばもっと高速化出来る)

	const risse_char * p = in.c_str() + in.GetLength();
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
	// TODO: ここの最適化(tStringの部分文字列共有機能を使えばもっと高速化出来る)

	if(path.EndsWith(RISSE_WC('/')))
	{
		risse_char *s = path.Independ();
		risse_char *p = s + path.GetLength() - 1;
		while(p >= s && *p == RISSE_WC('/')) p--;
		p++;
		*p = 0;
		path.FixLength();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tFileSystemManager::ChopExtension(const tString & in)
{
	// TODO: ここの最適化(tStringの部分文字列共有機能を使えばもっと高速化出来る)

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
void tFileSystemInstance::construct()
{
	// デフォルトではなにもしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------














//---------------------------------------------------------------------------
tFileSystemClass::tFileSystemClass(tScriptEngine * engine) :
	tClassBase(tSS<'F','i','l','e','S','y','s','t','e','m'>(), engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tFileSystemClass::ovulate);
	BindFunction(this, ss_construct, &tFileSystemInstance::construct);
	BindFunction(this, ss_initialize, &tFileSystemInstance::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tFileSystemClass::ovulate()
{
	// このクラスのインスタンスは作成できないので例外を投げる
	tInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		FileSystem クラスレジストラ
template class tRisseClassRegisterer<tFileSystemClass>;
//---------------------------------------------------------------------------

















//---------------------------------------------------------------------------
tFileClass::tFileClass(tScriptEngine * engine) :
	tClassBase(tSS<'F','i','l','e'>(), engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tFileClass::ovulate);
	BindFunction(this, ss_construct, &tFileClass::construct);
	BindFunction(this, ss_initialize, &tFileClass::initialize);

	BindFunction(this, tSS<'m','o','u','n','t'>(), &tFileClass::mount);
	BindFunction(this, tSS<'u','n','m','o','u','n','t'>(), &tFileClass::unmount);
	BindFunction(this, tSS<'n','o','r','m','a','l','i','z','e'>(), &tFileClass::normalize);
	BindFunction(this, tSS<'e','x','i','s','t','s'>(), &tFileClass::exists);
	BindFunction(this, tSS<'i','s','F','i','l','e'>(), &tFileClass::isFile);
	BindFunction(this, tSS<'i','s','D','i','r','e','c','t','o','r','y'>(), &tFileClass::isDirectory);
	BindFunction(this, tSS<'c','h','o','p','E','x','t','e','n','s','i','o','n'>(), &tFileClass::chopExtension);
	BindFunction(this, tSS<'e','x','t','r','a','c','t','E','x','t','e','n','s','i','o','n'>(), &tFileClass::extractExtension);
	BindFunction(this, tSS<'e','x','t','r','a','c','t','N','a','m','e'>(), &tFileClass::extractName);
	BindProperty(this, tSS<'c','w','d'>(), &tFileClass::get_cwd, &tFileClass::set_cwd);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tFileClass::ovulate()
{
	// このクラスのインスタンスは作成できないので例外を投げる
	tInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileClass::mount(const tString & point, const tVariant & fs)
{
	fs.AssertClass(tRisseClassRegisterer<tFileSystemClass>::instance()->GetClassInstance());
	tFileSystemManager::instance()->Mount(point, reinterpret_cast<tFileSystemInstance *>(fs.GetObjectInterface()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		File クラスレジストラ
template class tRisseClassRegisterer<tFileClass>;
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace Risa

