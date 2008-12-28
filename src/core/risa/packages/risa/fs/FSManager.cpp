//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ファイルシステムマネージャ(ファイルシステムの根幹部分)
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/fs/FSManager.h"
#include "risa/packages/risa/fs/osfs/OSFS.h"
#include "risa/packages/risa/fs/FileSystem.h"
#include "builtin/stream/risseStreamClass.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(54267,15059,65338,18354,34733,17665,1145,27805);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * ファイルシステム用例外クラス
 */
/*! @note
	ファイルシステム関連の例外クラスとしては、ここの FileSystemException
	(extends RuntimeException) 以外に、IOException がある(Risseエンジン内で定義)。
*/
//---------------------------------------------------------------------------
RISA_DEFINE_EXCEPTION_SUBCLASS(tFileSystemExceptionClass,
	(tSS<'r','i','s','a','.','f','s'>()),
	(tSS<'F','i','l','e','S','y','s','t','e','m','E','x','c','e','p','t','i','o','n'>()),
	tRisseScriptEngine::instance()->GetScriptEngine()->RuntimeExceptionClass)
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * FileSystemException クラスレジストラ
 */
template class tClassRegisterer<
	tSS<'r','i','s','a','.','f','s'>,
	tFileSystemExceptionClass>;
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
				tString(RISSE_WS_TR("cannot mount filesystem: the mount point '%1' is not directory"), path));
		}
	}

	// すでにその場所にマウントが行われているかどうかをチェックする
	tFileSystemInstance ** item = MountPoints.Find(path);
	if(item)
	{
		// ファイルシステムが見つかったのでそこにはマウントできない
		tFileSystemExceptionClass::Throw(tString(RISSE_WS_TR("cannot mount filesystem: the mount point '%1' is already mounted"), path));
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
				// /.. で終わっている、または /../ がある
				s += 3;
				// d を巻き戻す
				if(d > start)
				{
					d --;
					while(d > start && *d != RISSE_WC('/')) d--;
					// この時点で d は '/' を指している
				}
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

	RISA_PREPEND_EXCEPTION_MESSAGE_BEGIN()
	{
		return fs->Invoke(tSS<'i','s','F','i','l','e'>(), fspath);
	}
	RISA_PREPEND_EXCEPTION_MESSAGE_END(tRisseScriptEngine::instance()->GetScriptEngine(),
			tString(RISSE_WS_TR("failed to retrieve existence of file '%1': "), fullpath))

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

	RISA_PREPEND_EXCEPTION_MESSAGE_BEGIN()
	{
		return fs->Invoke(tSS<'i','s','D','i','r','e','c','t','o','r','y'>(), fspath);
	}
	RISA_PREPEND_EXCEPTION_MESSAGE_END(tRisseScriptEngine::instance()->GetScriptEngine(),
			tString(RISSE_WS_TR("failed to retrieve existence of directory '%1': "), fullpath))

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

	RISA_PREPEND_EXCEPTION_MESSAGE_BEGIN()
	{
		fs->Do(ocFuncCall, NULL, tSS<'r','e','m','o','v','e','F','i','l','e'>(),
			0, tMethodArgument::New(fspath));
	}
	RISA_PREPEND_EXCEPTION_MESSAGE_END(tRisseScriptEngine::instance()->GetScriptEngine(),
			tString(RISSE_WS_TR("Failed to remove file '%1': "), fullpath))
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
		tFileSystemExceptionClass::Throw(tString(RISSE_WS_TR("cannot remove mount point '%1'"),
			fullpath));
	}

	if(recursive)
	{
		// 削除しようとしているディレクトリ以下にマウントポイントがあるかどうかを探す
		tHashTable<tString, tFileSystemInstance *>::tIterator i(MountPoints);
		for(; !i.End(); ++i)
		{
			if(i.GetKey().StartsWith(fullpath))
				tFileSystemExceptionClass::Throw(tString(RISSE_WS_TR("cannot remove mount point '%1'"),
					i.GetKey()));
		}
	}

	RISA_PREPEND_EXCEPTION_MESSAGE_BEGIN()
	{
		fs->Do(ocFuncCall, NULL, tSS<'r','e','m','o','v','e','D','i','r','e','c','t','o','r','y'>(),
			0, tMethodArgument::New(fspath, recursive));
	}
	RISA_PREPEND_EXCEPTION_MESSAGE_END(tRisseScriptEngine::instance()->GetScriptEngine(),
			tString(RISSE_WS_TR("failed to remove directory '%1': "), fullpath))
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
		RISA_PREPEND_EXCEPTION_MESSAGE_BEGIN()
		{
			fs->Do(ocFuncCall, NULL, tSS<'c','r','e','a','t','e','D','i','r','e','c','t','o','r','y'>(),
				0, tMethodArgument::New(fspath, recursive));
		}
		RISA_PREPEND_EXCEPTION_MESSAGE_END(tRisseScriptEngine::instance()->GetScriptEngine(),
				tString(RISSE_WS_TR("failed to create directory '%1': "), fullpath))
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
					CreateDirectory(n_dirname, false);
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

	RISA_PREPEND_EXCEPTION_MESSAGE_BEGIN()
	{
		return fs->Invoke(tSS<'s','t','a','t'>(), fspath).GetObjectInterface();
	}
	RISA_PREPEND_EXCEPTION_MESSAGE_END(tRisseScriptEngine::instance()->GetScriptEngine(),
			tString(RISSE_WS_TR("failed to stat '%1': "), fullpath))
	return NULL;
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

	RISA_PREPEND_EXCEPTION_MESSAGE_BEGIN()
	{
		val = fs->Invoke(tSS<'o','p','e','n'>(), fspath, (risse_int64)flags);
	}
	RISA_PREPEND_EXCEPTION_MESSAGE_END(tRisseScriptEngine::instance()->GetScriptEngine(),
			tString(RISSE_WS_TR("failed to open '%1': "), fullpath))

	val.AssertClass(static_cast<tClassBase*>(tRisseScriptEngine::instance()->GetScriptEngine()->GetPackageGlobal(tSS<'s','t','r','e','a','m'>()).
		GetPropertyDirect(tRisseScriptEngine::instance()->GetScriptEngine(), tSS<'S','t','r','e','a','m'>()).GetObjectInterface()));
	return static_cast<tStreamInstance*>(val.GetObjectInterface());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStreamInstance * tFileSystemManager::Open(const tVariant & filename, risse_uint32 flags)
{
	if(filename.InstanceOf(tRisseScriptEngine::instance()->GetScriptEngine(),
		tVariant(static_cast<tClassBase*>(tRisseScriptEngine::instance()->GetScriptEngine()->GetPackageGlobal(tSS<'s','t','r','e','a','m'>()).
		GetPropertyDirect(tRisseScriptEngine::instance()->GetScriptEngine(), tSS<'S','t','r','e','a','m'>()).GetObjectInterface()))))
	{
		// filename は Stream クラスのインスタンス
		// そのまま帰す
		return static_cast<tStreamInstance*>(filename.GetObjectInterface());
	}
	else
	{
		// filename を文字列化して Open に渡す
		return Open((tString)filename, flags);
	}
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

	RISA_PREPEND_EXCEPTION_MESSAGE_BEGIN()
	{
		tMethodArgumentOf<1,1> args;
		tVariant fspath_v(fspath);
		tVariant callback_v(callback);
		args.SetArgument(0, &fspath_v);
		args.SetBlockArgument(0, &callback_v);
		tVariant ret;
		fs->Do(ocFuncCall, &ret, tSS<'w','a','l','k','A','t'>(), 0, args);
		return ret.operator risse_int64();
	}
	RISA_PREPEND_EXCEPTION_MESSAGE_END(tRisseScriptEngine::instance()->GetScriptEngine(),
			tString(RISSE_WS_TR("failed to list '%1': "), dirname))

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

	tString fullpath(NormalizePath(dir));

	if(fullpath.EndsWith(RISSE_WC('/')))
		CurrentDirectory = fullpath;
	else
		CurrentDirectory = fullpath + RISSE_WS("/");
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
/**
 * risa.fs の static メソッド群
 */
struct tRisaFsStaticMethods
{

	static void mount(const tString & point, const tVariant & fs)
	{
		fs.AssertClass(tClassHolder<tFileSystemClass>::instance()->GetClass());
		tFileSystemManager::instance()->Mount(point,
			static_cast<tFileSystemInstance *>(fs.GetObjectInterface()));
	}

	static void unmount(const tString & point)
	{
		tFileSystemManager::instance()->Unmount(point);
	}

	static tString normalize(const tString & path)
	{
		return tFileSystemManager::instance()->NormalizePath(path);
	}

	static size_t walkAt(const tString & dirname,
		const tMethodArgument &args, tScriptEngine * engine)
	{
		// オプションをとってくる
		args.ExpectBlockArgumentCount(1);
		tVariant callback = args.GetBlockArgument(0);
		bool recursive = args.Get(1, false);

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

	static bool exists(const tString & filename)
	{
		return tFileSystemManager::instance()->IsFile(filename) ||
			tFileSystemManager::instance()->IsDirectory(filename);
	}

	static bool isFile(const tString & filename)
	{
		return tFileSystemManager::instance()->IsFile(filename);
	}

	static bool isDirectory(const tString & dirname)
	{
		return tFileSystemManager::instance()->IsDirectory(dirname);
	}

	static void removeFile(const tString & filename)
	{
		return tFileSystemManager::instance()->RemoveFile(filename);
	}

	static void removeDirectory(const tString & dirname, const tMethodArgument &args)
	{
		bool recursive = args.HasArgument(1) ? (bool)args[1] : false;
		tFileSystemManager::instance()->RemoveDirectory(dirname, recursive);
	}

	static void createDirectory(const tString & dirname,
			const tMethodArgument &args)
	{
		bool recursive = args.HasArgument(1) ? (bool)args[1] : false;
		tFileSystemManager::instance()->CreateDirectory(dirname, recursive);
	}

	static tObjectInterface * stat(const tString & filename)
	{
		return tFileSystemManager::instance()->Stat(filename);
	}

	static tVariant open(const tString & filename, const tNativeCallInfo &info)
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

	static tFileSystemInstance * getFileSystemAt(const tString & path)
	{
		return tFileSystemManager::instance()->GetFileSystemAt(path);
	}

	static tString chopExtension(const tString & filename)
	{
		return tFileSystemManager::instance()->ChopExtension(filename);
	}

	static tString extractExtension(const tString & filename)
	{
		return tFileSystemManager::instance()->ExtractExtension(filename);
	}

	static tString extractName(const tString & filename)
	{
		return tFileSystemManager::instance()->ExtractName(filename);
	}

	static tString extractPath(const tString & filename)
	{
		return tFileSystemManager::instance()->ExtractPath(filename);
	}

	static tString get_cwd()
	{
		return tFileSystemManager::instance()->GetCurrentDirectory();
	}

	static void set_cwd(const tString & dirname)
	{
		tFileSystemManager::instance()->SetCurrentDirectory(dirname);
	}

};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
/**
 * risa.fs のパッケージのメンバを初期化するためのシングルトンインスタンス
 */
class tRisaFsPackageMemberInitializer : public tPackageMemberInitializer,
	public singleton_base<tRisaFsPackageMemberInitializer>,
	private tFileOpenModes
{
public:
	/**
	 * コンストラクタ
	 */
	tRisaFsPackageMemberInitializer()
	{
		tPackageRegisterer<tSS<'r','i','s','a','.','f','s'> >::instance()->AddInitializer(this);
	}

	void Initialize(tScriptEngine * engine, const tString & name,
		const tVariant & global)
	{
		tObjectBase * g = static_cast<tObjectBase *>(global.GetObjectInterface());

		tMemberAttribute final_const (	tMemberAttribute(tMemberAttribute::mcConst)|
									tMemberAttribute(tMemberAttribute::ocFinal));

		BindFunction(g, tSS<'m','o','u','n','t'>(), &tRisaFsStaticMethods::mount, final_const);
		BindFunction(g, tSS<'u','n','m','o','u','n','t'>(), &tRisaFsStaticMethods::unmount, final_const);
		BindFunction(g, tSS<'n','o','r','m','a','l','i','z','e'>(), &tRisaFsStaticMethods::normalize, final_const);
		BindFunction(g, tSS<'w','a','l','k','A','t'>(), &tRisaFsStaticMethods::walkAt, final_const);
		BindFunction(g, tSS<'e','x','i','s','t','s'>(), &tRisaFsStaticMethods::exists, final_const);
		BindFunction(g, tSS<'i','s','F','i','l','e'>(), &tRisaFsStaticMethods::isFile, final_const);
		BindFunction(g, tSS<'i','s','D','i','r','e','c','t','o','r','y'>(), &tRisaFsStaticMethods::isDirectory, final_const);
		BindFunction(g, tSS<'r','e','m','o','v','e','F','i','l','e'>(), &tRisaFsStaticMethods::removeFile, final_const);
		BindFunction(g, tSS<'r','e','m','o','v','e','D','i','r','e','c','t','o','r','y'>(), &tRisaFsStaticMethods::removeDirectory, final_const);
		BindFunction(g, tSS<'c','r','e','a','t','e','D','i','r','e','c','t','o','r','y'>(), &tRisaFsStaticMethods::createDirectory, final_const);
		BindFunction(g, tSS<'s','t','a','t'>(), &tRisaFsStaticMethods::stat, final_const);
		BindFunction(g, tSS<'o','p','e','n'>(), &tRisaFsStaticMethods::open, final_const);
		BindFunction(g, tSS<'g','e','t','F','i','l','e','S','y','s','t','e','m','A','t'>(), &tRisaFsStaticMethods::getFileSystemAt, final_const);
		BindFunction(g, tSS<'c','h','o','p','E','x','t','e','n','s','i','o','n'>(), &tRisaFsStaticMethods::chopExtension, final_const);
		BindFunction(g, tSS<'e','x','t','r','a','c','t','E','x','t','e','n','s','i','o','n'>(), &tRisaFsStaticMethods::extractExtension, final_const);
		BindFunction(g, tSS<'e','x','t','r','a','c','t','N','a','m','e'>(), &tRisaFsStaticMethods::extractName, final_const);
		BindFunction(g, tSS<'e','x','t','r','a','c','t','P','a','t','h'>(), &tRisaFsStaticMethods::extractPath, final_const);
		BindProperty(g, tSS<'c','w','d'>(), &tRisaFsStaticMethods::get_cwd, &tRisaFsStaticMethods::set_cwd/*, final_const*/);
		// TODO: property の final_const なプロパティってちゃんと動作してる？

		global.RegisterFinalConstMember(
				tSS<'o','m','R','e','a','d'>(),
				tVariant((risse_int64)omRead));
		global.RegisterFinalConstMember(
				tSS<'o','m','W','r','i','t','e'>(),
				tVariant((risse_int64)omWrite));
		global.RegisterFinalConstMember(
				tSS<'o','m','U','p','d','a','t','e'>(),
				tVariant((risse_int64)omUpdate));
		global.RegisterFinalConstMember(
				tSS<'o','m','A','c','c','e','s','s','M','a','s','k'>(),
				tVariant((risse_int64)omAccessMask));
		global.RegisterFinalConstMember(
				tSS<'o','m','A','p','p','e','n','d'>(),
				tVariant((risse_int64)omAppend));
		global.RegisterFinalConstMember(
				tSS<'o','m','R','e','a','d','B','i','t'>(),
				tVariant((risse_int64)omReadBit));
		global.RegisterFinalConstMember(
				tSS<'o','m','W','r','i','t','e','B','i','t'>(),
				tVariant((risse_int64)omWriteBit));
		global.RegisterFinalConstMember(
				tSS<'o','m','A','p','p','e','n','d','B','i','t'>(),
				tVariant((risse_int64)omAppendBit));
	}
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
/**
 * FileSystem クラスレジストラ
 */
template class tClassRegisterer<
	tSS<'r','i','s','a','.','f','s'>,
	tFileSystemClass>;
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
} // namespace Risa

