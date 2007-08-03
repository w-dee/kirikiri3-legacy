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
#include "risse/include/risseStreamClass.h"
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
tFileSystemIterationCallback::tRetValue
	tFileSystemIterationCallback::Operate(RISSE_OBJECTINTERFACE_OPERATE_IMPL_ARG)
{
	if(name.IsEmpty())
	{
		if(code == ocFuncCall) // このオブジェクトに対するメソッド呼び出しか？
		{
			args.ExpectArgumentCount(2);

			tString name = args[0];
			bool is_directory = args[1];
			bool ret;
			if(is_directory)
				OnDirectory(name);
			else
				OnFile(name);
			if(result) *result = ret;

			return rvNoError;
		}
	}

	// そのほかの場合はなにもしない
	return rvMemberNotFound;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tFileOpenModeConstsModule::tFileOpenModeConstsModule(tScriptEngine * engine) :
	tModuleBase(tSS<'F','i','l','e','O','p','e','n','M','o','d','e','C','o','n','s','t','s'>(), engine)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileOpenModeConstsModule::RegisterMembers()
{
	GetInstance()->RegisterFinalConstMember(
			tSS<'o','m','R','e','a','d'>(),
			tVariant((risse_int64)omRead), true);
	GetInstance()->RegisterFinalConstMember(
			tSS<'o','m','W','r','i','t','e'>(),
			tVariant((risse_int64)omWrite), true);
	GetInstance()->RegisterFinalConstMember(
			tSS<'o','m','U','p','d','a','t','e'>(),
			tVariant((risse_int64)omUpdate), true);
	GetInstance()->RegisterFinalConstMember(
			tSS<'o','m','A','c','c','e','s','s','M','a','s','k'>(),
			tVariant((risse_int64)omAccessMask), true);
	GetInstance()->RegisterFinalConstMember(
			tSS<'o','m','A','p','p','e','n','d'>(),
			tVariant((risse_int64)omAppend), true);
	GetInstance()->RegisterFinalConstMember(
			tSS<'o','m','R','e','a','d','B','i','t'>(),
			tVariant((risse_int64)omReadBit), true);
	GetInstance()->RegisterFinalConstMember(
			tSS<'o','m','W','r','i','t','e','B','i','t'>(),
			tVariant((risse_int64)omWriteBit), true);
	GetInstance()->RegisterFinalConstMember(
			tSS<'o','m','A','p','p','e','n','d','B','i','t'>(),
			tVariant((risse_int64)omAppendBit), true);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		FileOpenModeConsts モジュールレジストラ
template class tRisseModuleRegisterer<tFileOpenModeConstsModule>;
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
tFileSystemManager::tFileSystemManager()
{
	// カレントディレクトリを / に設定
	CurrentDirectory = RISSE_WS("/");
}
//---------------------------------------------------------------------------


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

	// その場所がディレクトリかどうかをチェックする
	if(path != tSS<'/'>())
	{
		if(!IsDirectory(path))
		{
			// もしディレクトリでなければ、そこにディレクトリの作成を試みる
			CreateDirectory(path, false);
		}

		if(!IsDirectory(path))
		{
			// 再チェック
			tFileSystemExceptionClass::Throw(
				tString(RISSE_WS_TR("can not mount filesystem: the mount point '%1' is not directory"), path));
		}
	}

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
size_t tFileSystemManager::WalkAt(const tString & dirname,
	tFileSystemIterationCallback * callback, bool recursive)
{
	tString path(NormalizePath(dirname));
	if(!path.EndsWith(RISSE_WC('/'))) path += RISSE_WC('/');

	if(!recursive)
	{
		// 再帰をしないならば
		return InternalList(path, callback); // 話は簡単
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

		void SetCurrentDirectory(const tString & dir) { CurrentDirectory = dir; }

		void OnFile(const tString & filename)
		{
			Count ++;
			Destination->Do(ocFuncCall, NULL, tString::GetEmptyString(), 0,
				tMethodArgument::New(CurrentDirectory + filename, false));
		}
		void OnDirectory(const tString & dirname)
		{
			Count ++;
			List.push_back(CurrentDirectory + dirname + tSS<'/'>()); // ディレクトリ名を list に push
			Destination->Do(ocFuncCall, NULL, tString::GetEmptyString(), 0,
				tMethodArgument::New(CurrentDirectory + dirname, true));
		}

		size_t GetCount() const { return Count; }
	} ;
	gc_vector<tString> list; // ディレクトリのリスト
	list.push_back(tString()); // 空ディレクトリを push

	tIteratorCallback localcallback(list, callback);

	while(list.size()) // ディレクトリのリストに残りがある限り繰り返す
	{
		tString relative_dir(list.back());
		tString dir(path + relative_dir);
		list.pop_back();
		localcallback.SetCurrentDirectory(relative_dir);
		InternalList(dir, &localcallback);
	}

	return localcallback.GetCount();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tFileSystemManager::IsFile(const tString & filename)
{
	volatile tCriticalSection::tLocker holder(CS);

	tString fspath;
	tString fullpath(NormalizePath(filename));
	tFileSystemInstance * fs = FindFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(filename);
//	try
//	{
		return fs->Invoke(tSS<'i','s','F','i','l','e'>(), fspath);
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
bool tFileSystemManager::IsDirectory(const tString & dirname)
{
	volatile tCriticalSection::tLocker holder(CS);

	tString fspath;
	tString fullpath(NormalizePath(dirname));
	tFileSystemInstance * fs = FindFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	TrimLastPathDelimiter(fullpath);
//	try
//	{
		return fs->Invoke(tSS<'i','s','D','i','r','e','c','t','o','r','y'>(), fspath);
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
	tFileSystemInstance * fs = FindFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
//	try
//	{
		fs->Do(ocFuncCall, NULL, tSS<'r','e','m','o','v','e','F','i','l','e'>(),
			0, tMethodArgument::New(fspath));
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
	if(!fullpath.EndsWith(RISSE_WC('/'))) fullpath += RISSE_WC('/');
	tFileSystemInstance * fs = FindFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);

	// ディレクトリを削除する際、マウントポイントをまたがる場合があるので
	// 注意が必要。
	// ディレクトリを削除する場合、マウントポイントを削除しようと
	// した場合は、何も削除しようとせずにエラーにする。
	if(fspath.IsEmpty())
	{
		// fs 内のパスが空文字列ということは、ファイルシステムのルートを
		// 削除しようとしていると言うこと
		tFileSystemExceptionClass::Throw(tString(RISSE_WS_TR("can not remove mount point '%1'"),
			fullpath));
	}

	if(recursive)
	{
		// 削除しようとしているディレクトリ以下にマウントポイントがあるかどうかを探す
		tHashTable<tString, tFileSystemInstance *>::tIterator i(MountPoints);
		for(; !i.End(); ++i)
		{
			if(i.GetKey().StartsWith(fullpath))
				tFileSystemExceptionClass::Throw(tString(RISSE_WS_TR("can not remove mount point '%1'"),
					i.GetKey()));
		}
	}

//	try
//	{
		fs->Do(ocFuncCall, NULL, tSS<'r','e','m','o','v','e','D','i','r','e','c','t','o','r','y'>(),
			0, tMethodArgument::New(fspath, recursive));
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

	tString fullpath(NormalizePath(dirname));
	TrimLastPathDelimiter(fullpath);

	if(!recursive)
	{
		// 再帰をしない場合
		tString fspath;
		tFileSystemInstance * fs = FindFileSystemAt(fullpath, &fspath);
		if(!fs) ThrowNoFileSystemError(fullpath);
	//	try
	//	{
			fs->Do(ocFuncCall, NULL, tSS<'c','r','e','a','t','e','D','i','r','e','c','t','o','r','y'>(),
				0, tMethodArgument::New(fspath, recursive));
	//	}
	//	catch(const eRisseError &e)
	//	{
	//		eRisaException::Throw(RISSE_WS_TR("failed to create directory '%1' : %2"),
	//			fullpath, e.GetMessageString());
	//	}
	}
	else
	{
		// 再帰する場合
		// / から順にトラバースする
		const risse_char *p = fullpath.Pointer();
		const risse_char *p_start = p;
		const risse_char *p_limit = p + fullpath.GetLength();
		const risse_char *pp = p;
		RISSE_ASSERT(*p == '/');
		p++;
		pp++; // skip first '/'

		while( p < p_limit )
		{
			while(p < p_limit && *p != RISSE_WC('/')) p++; // 次の / までスキップ
			if(pp != p)
			{
				// この時点で:
				// p_start と pp の間  作成しようとするディレクトリの親ディレクトリ名の終わり
				// pp と p の間        作成しようとするディレクトリ名
				// すこし効率が悪いが、この関数を自分自身で呼び出してディレクトリを作成する
				tString n_dirname(fullpath, 0, p - p_start);
				if(!IsDirectory(n_dirname))
					CreateDirectory(fullpath, false);
			}
			p ++; // skip '/'
			pp = p;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tObjectInterface * tFileSystemManager::Stat(const tString & filename)
{
	volatile tCriticalSection::tLocker holder(CS);

	tString fspath;
	tString fullpath(NormalizePath(filename));
	tFileSystemInstance * fs = FindFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
//	try
//	{
		return fs->Invoke(tSS<'s','t','a','t'>(), fspath).GetObjectInterface();
//	}
//	catch(const eRisseError &e)
//	{
//		eRisaException::Throw(RISSE_WS_TR("failed to stat '%1' : %2"),
//			fullpath, e.GetMessageString());
//	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStreamInstance * tFileSystemManager::Open(const tString & filename,
	risse_uint32 flags)
{
	volatile tCriticalSection::tLocker holder(CS);

	// 通常のファイルシステム経由のストリームの作成
	tString fspath;
	tString fullpath(NormalizePath(filename));
	tFileSystemInstance * fs = FindFileSystemAt(fullpath, &fspath);
	if(!fs) ThrowNoFileSystemError(fullpath);
	tVariant val;
//	try
//	{
		val = fs->Invoke(tSS<'o','p','e','n'>(), fspath, (risse_int64)flags);
//	}
//	catch(const eRisseError &e)
//	{
//		eRisaException::Throw(RISSE_WS_TR("failed to create stream of '%1' : %2"),
//			fullpath, e.GetMessageString());
//	}
	val.AssertClass(tRisseScriptEngine::instance()->GetScriptEngine()->StreamClass);
	return reinterpret_cast<tStreamInstance*>(val.GetObjectInterface());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tFileSystemInstance * tFileSystemManager::GetFileSystemAt(const tString & path)
{
	volatile tCriticalSection::tLocker holder(CS);

	tString fullpath(NormalizePath(path));
	tFileSystemInstance * fs = FindFileSystemAt(fullpath, NULL);

	return fs;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tFileSystemManager::InternalList(
	const tString & dirname,
	tFileSystemIterationCallback * callback)
{
	volatile tCriticalSection::tLocker holder(CS);

	tString fspath;
	tFileSystemInstance * fs = FindFileSystemAt(dirname, &fspath);
	if(!fs) ThrowNoFileSystemError(dirname);

//	try
//	{
		tMethodArgumentOf<1,1> args;
		tVariant fspath_v(fspath);
		tVariant callback_v(callback);
		args.SetArgument(0, &fspath_v);
		args.SetBlockArgument(0, &callback_v);
		tVariant ret;
		fs->Do(ocFuncCall, &ret, tSS<'w','a','l','k','A','t'>(), 0, args);
		return ret.operator risse_int64();
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
tFileSystemInstance * tFileSystemManager::FindFileSystemAt(
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
						tString(fullpath, p - start + 1, risse_size_max);

				return *item;
			}
		}
		p--;
	}

	// 通常はここにこない
	// ここにくるのは以下のどちらか
	// ・  / (ルート) に割り当てられているファイルシステムが見つからない
	// ・  fullpath に空文字が渡された

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
		tString(RISSE_WS_TR("could not find filesystem at path '%1'"), filename));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::RaiseNoSuchFileOrDirectoryError()
{
	tIOExceptionClass::Throw(
		tString(RISSE_WS_TR("no such file or directory")));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::SplitExtension(const tString & in, tString * other, tString * ext)
{
	if(in.IsEmpty())
	{
		// in が空文字列なので分解できない
		if(other) other->Clear();
		if(ext) ext->Clear();
		return;
	}

	const risse_char * start = in.Pointer();
	const risse_char * p = start + in.GetLength();

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
			if(other) *other = tString(in, 0, p - start);
			if(ext)   *ext   = tString(in, p - start, risse_size_max);
			return;
		}

		p--;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::SplitPathAndName(const tString & in, tString * path, tString * name)
{
	if(in.IsEmpty())
	{
		// in が空文字列なので分解できない
		if(path) path->Clear();
		if(name) name->Clear();
		return;
	}

	const risse_char * start = in.Pointer();
	const risse_char * p = start + in.GetLength();

	p --;
	while(p > start && *p != RISSE_WC('/')) p--;

	if(*p == RISSE_WC('/')) p++;

	if(path) *path = tString(in, 0, p - start);
	if(name) *name = tString(in, p - start, risse_size_max);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemManager::TrimLastPathDelimiter(tString & path)
{
	if(path.EndsWith(RISSE_WC('/')))
	{
		const risse_char *s = path.Pointer();
		const risse_char *p = s + path.GetLength() - 1;
		while(p >= s && *p == RISSE_WC('/')) p--;
		p++;
		path = tString(path, 0, p - s);
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
size_t tFileSystemInstance::walkAt(const tString & dirname,
	const tMethodArgument &args)
{
	// 実装されていない; 下位の Risse クラスで実装すること
	tUnsupportedOperationExceptionClass::ThrowOperationIsNotImplemented();
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tFileSystemInstance::isFile(const tString & filename)
{
	// 実装されていない; 下位の Risse クラスで実装すること
	tUnsupportedOperationExceptionClass::ThrowOperationIsNotImplemented();
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tFileSystemInstance::isDirectory(const tString & dirname)
{
	// 実装されていない; 下位の Risse クラスで実装すること
	tUnsupportedOperationExceptionClass::ThrowOperationIsNotImplemented();
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemInstance::removeFile(const tString & filename)
{
	// 実装されていない; 下位の Risse クラスで実装すること
	tUnsupportedOperationExceptionClass::ThrowOperationIsNotImplemented();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemInstance::removeDirectory(const tString & dirname,
	const tMethodArgument &args)
{
	// 実装されていない; 下位の Risse クラスで実装すること
	tUnsupportedOperationExceptionClass::ThrowOperationIsNotImplemented();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemInstance::createDirectory(const tString & dirname)
{
	// 実装されていない; 下位の Risse クラスで実装すること
	tUnsupportedOperationExceptionClass::ThrowOperationIsNotImplemented();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tObjectInterface * tFileSystemInstance::stat(const tString & filename)
{
	// 実装されていない; 下位の Risse クラスで実装すること
	tUnsupportedOperationExceptionClass::ThrowOperationIsNotImplemented();
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStreamInstance * tFileSystemInstance::open(const tString & filename,
	risse_uint32 flags)
{
	// 実装されていない; 下位の Risse クラスで実装すること
	tUnsupportedOperationExceptionClass::ThrowOperationIsNotImplemented();
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemInstance::flush()
{
	// 標準の flush メソッドは何もしない
}
//---------------------------------------------------------------------------














//---------------------------------------------------------------------------
tFileSystemClass::tFileSystemClass(tScriptEngine * engine) :
	tClassBase(tSS<'F','i','l','e','S','y','s','t','e','m'>(), engine->ObjectClass)
{
	RegisterMembers();

	// FileOpenModeConsts を include する
	Do(ocFuncCall, NULL, ss_include, 0,
		tMethodArgument::New(
			tRisseModuleRegisterer<tFileOpenModeConstsModule>::instance()->GetModuleInstance()));
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

	BindFunction(this, tSS<'w','a','l','k','A','t'>(), &tFileSystemInstance::walkAt);
	BindFunction(this, tSS<'i','s','F','i','l','e'>(), &tFileSystemInstance::isFile);
	BindFunction(this, tSS<'i','s','D','i','r','e','c','t','o','r','y'>(), &tFileSystemInstance::isDirectory);
	BindFunction(this, tSS<'r','e','m','o','v','e','F','i','l','e'>(), &tFileSystemInstance::removeFile);
	BindFunction(this, tSS<'r','e','m','o','v','e','D','i','r','e','c','t','o','r','y'>(), &tFileSystemInstance::removeDirectory);
	BindFunction(this, tSS<'c','r','e','a','t','e','D','i','r','e','c','t','o','r','y'>(), &tFileSystemInstance::createDirectory);
	BindFunction(this, tSS<'s','t','a','t'>(), &tFileSystemInstance::stat);
	BindFunction(this, tSS<'o','p','e','n'>(), &tFileSystemInstance::open);
	BindFunction(this, tSS<'f','l','u','s','h'>(), &tFileSystemInstance::flush);
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

	// FileOpenModeConsts を include する
	Do(ocFuncCall, NULL, ss_include, 0,
		tMethodArgument::New(
			tRisseModuleRegisterer<tFileOpenModeConstsModule>::instance()->GetModuleInstance()));
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

	tMemberAttribute final_const (	tMemberAttribute(tMemberAttribute::vcConst)|
								tMemberAttribute(tMemberAttribute::ocFinal));
	BindFunction(this, tSS<'m','o','u','n','t'>(), &tFileClass::mount, final_const);
	BindFunction(this, tSS<'u','n','m','o','u','n','t'>(), &tFileClass::unmount, final_const);
	BindFunction(this, tSS<'n','o','r','m','a','l','i','z','e'>(), &tFileClass::normalize, final_const);
	BindFunction(this, tSS<'w','a','l','k','A','t'>(), &tFileClass::walkAt, final_const);
	BindFunction(this, tSS<'e','x','i','s','t','s'>(), &tFileClass::exists, final_const);
	BindFunction(this, tSS<'i','s','F','i','l','e'>(), &tFileClass::isFile, final_const);
	BindFunction(this, tSS<'i','s','D','i','r','e','c','t','o','r','y'>(), &tFileClass::isDirectory, final_const);
	BindFunction(this, tSS<'r','e','m','o','v','e','F','i','l','e'>(), &tFileClass::removeFile, final_const);
	BindFunction(this, tSS<'r','e','m','o','v','e','D','i','r','e','c','t','o','r','y'>(), &tFileClass::removeDirectory, final_const);
	BindFunction(this, tSS<'c','r','e','a','t','e','D','i','r','e','c','t','o','r','y'>(), &tFileClass::createDirectory, final_const);
	BindFunction(this, tSS<'s','t','a','t'>(), &tFileClass::stat, final_const);
	BindFunction(this, tSS<'o','p','e','n'>(), &tFileClass::open, final_const);
	BindFunction(this, tSS<'g','e','t','F','i','l','e','S','y','s','t','e','m','A','t'>(), &tFileClass::getFileSystemAt, final_const);
	BindFunction(this, tSS<'c','h','o','p','E','x','t','e','n','s','i','o','n'>(), &tFileClass::chopExtension, final_const);
	BindFunction(this, tSS<'e','x','t','r','a','c','t','E','x','t','e','n','s','i','o','n'>(), &tFileClass::extractExtension, final_const);
	BindFunction(this, tSS<'e','x','t','r','a','c','t','N','a','m','e'>(), &tFileClass::extractName, final_const);
	BindFunction(this, tSS<'e','x','t','r','a','c','t','P','a','t','h'>(), &tFileClass::extractPath, final_const);
	BindProperty(this, tSS<'c','w','d'>(), &tFileClass::get_cwd, &tFileClass::set_cwd, final_const);
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
size_t tFileClass::walkAt(const tString & dirname,
	const tMethodArgument &args, tScriptEngine * engine)
{
	// オプションをとってくる
	args.ExpectBlockArgumentCount(1);
	tVariant callback = args.GetBlockArgument(0);
	bool recursive = args.HasArgument(1) ? (bool)args[1] : false;

	// tFileSystemManager::WalkAt が受け取るのは tFileSystemIterationCallback *
	// なので、そのままでは tVariant callback を渡せないので、変換させる
	class tCallbackGW : public tFileSystemIterationCallback
	{
		tScriptEngine * engine;
		tVariant & callback;
	public:
		tCallbackGW(tScriptEngine * eng, tVariant & cb) : engine(eng), callback(cb) {;}
	protected:
		void OnFile(const tString & filename)
		{
			callback.Do(engine,
				ocFuncCall, NULL, tString::GetEmptyString(), 0,
				tMethodArgument::New(tString(filename), false));
		}
		void OnDirectory(const tString & dirname)
		{
			callback.Do(engine,
				ocFuncCall, NULL, tString::GetEmptyString(), 0,
				tMethodArgument::New(tString(dirname), true));
		}
	};

	tCallbackGW gw(engine, callback);

	// tFileSystemManager::WalkAt を呼ぶ
	return tFileSystemManager::instance()->WalkAt(dirname, &gw, recursive);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileClass::removeDirectory(const tString & dirname, const tMethodArgument &args)
{
	bool recursive = args.HasArgument(1) ? (bool)args[1] : false;
	tFileSystemManager::instance()->RemoveDirectory(dirname, recursive);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileClass::createDirectory(const tString & dirname,
			const tMethodArgument &args)
{
	bool recursive = args.HasArgument(1) ? (bool)args[1] : false;
	tFileSystemManager::instance()->CreateDirectory(dirname, recursive);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tFileClass::open(const tString & filename, const tNativeCallInfo &info)
{
	risse_uint32 flags = info.args.HasArgument(1) ?
		(risse_uint32)(risse_int64)info.args[1] : tFileOpenModes::omRead;
	tVariant stream_v(tFileSystemManager::instance()->Open(filename, flags));
	if(info.args.HasBlockArgument(0))
	{
		// ブロック付き呼び出しの場合は、ブロックを呼び出し、呼んだら
		// stream_v の dispose メソッドを呼ぶ
		try
		{
			tVariant block(info.args.GetBlockArgument(0));
			block.Do(info.engine,
					ocFuncCall, NULL, tString::GetEmptyString(), 0,
					tMethodArgument::New(stream_v));
		}
		catch(...)
		{
			stream_v.Do(info.engine, ocFuncCall, NULL, ss_dispose);
			throw;
		}
		stream_v.Do(info.engine, ocFuncCall, NULL, ss_dispose);
		return info.This; // this を帰す
	}
	else
	{
		// ブロック無し呼び出しの場合は、オープンしたオブジェクトをそのまま帰す
		return stream_v;
	}
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		File クラスレジストラ
template class tRisseClassRegisterer<tFileClass>;
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace Risa

