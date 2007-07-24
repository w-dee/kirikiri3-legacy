//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OSFSの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/fs/osfs/OSFS.h"
#include "base/exception/RisaException.h"
#include "risse/include/risseExceptionClass.h"
#include "risse/include/risseStaticStrings.h"
#include <wx/filename.h>
#include <wx/dir.h>



namespace Risa {
RISSE_DEFINE_SOURCE_ID(49572,65271,56057,18682,27296,33314,20965,8152);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSNativeStreamInstance::initialize(const tString & path, risse_uint32 flags, const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// モードを決定する
	wxFile::OpenMode mode;
	switch(flags & tFileSystem::omAccessMask)
	{
	case tFileSystem::omRead:
		mode = wxFile::read; break;
	case tFileSystem::omWrite:
		mode = wxFile::write; break;
	case tFileSystem::omUpdate:
		mode = wxFile::read_write; break;
	default:
		mode = wxFile::read;
	}

	// ファイルを開く
	Internal = new tInternal();
	if(!Internal->File.Open(path.AsWxString()))
	{
		delete Internal; Internal = NULL;
		tIOExceptionClass::Throw(
			tString(RISSE_WS_TR("can not open file %1"), path));
	}

	// 名前を

	// APPEND の場合はファイルポインタを最後に移動する
	if(flags & tFileSystem::omAppendBit)
		Internal->File.SeekEnd();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOSNativeStreamInstance::~tOSNativeStreamInstance()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSNativeStreamInstance::dispose()
{
	volatile tSynchronizer sync(this); // sync
	if(!Internal) tIOExceptionClass::ThrowStreamIsClosed();
	delete Internal; Internal = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tOSNativeStreamInstance::seek(risse_int64 offset, tOrigin whence)
{
	volatile tSynchronizer sync(this); // sync

	if(!Internal) tIOExceptionClass::ThrowStreamIsClosed();

	wxSeekMode mode = wxFromCurrent;
	switch(whence)
	{
	case soSet: mode = wxFromStart;   break;
	case soCur: mode = wxFromCurrent; break;
	case soEnd: mode = wxFromEnd;     break;
	default: offset = 0;
	}

	wxFileOffset newpos = Internal->File.Seek(offset, mode);
	if(newpos == wxInvalidOffset)
		return false;

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tOSNativeStreamInstance::tell()
{
	volatile tSynchronizer sync(this); // sync

	if(!Internal) tIOExceptionClass::ThrowStreamIsClosed();

	return Internal->File.Tell();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tOSNativeStreamInstance::read(const tOctet & buf)
{
	volatile tSynchronizer sync(this); // sync

	if(!Internal) tIOExceptionClass::ThrowStreamIsClosed();

	// buf の示すポインタに「直接」書き込みを行う。
	// tOctet をメモリバッファとして使うかなり危ない使い方だが、
	// これに限ってはこういう使い方をすると言うことになっている。
	// というかそうした。

	return Internal->File.Read(const_cast<risse_uint8*>(buf.Pointer()), buf.GetLength());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tOSNativeStreamInstance::write(const tOctet & buf)
{
	volatile tSynchronizer sync(this); // sync

	if(!Internal) tIOExceptionClass::ThrowStreamIsClosed();

	return Internal->File.Write(buf.Pointer(), buf.GetLength());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSNativeStreamInstance::truncate()
{
	// TODO: implement this 
	if(!Internal) tIOExceptionClass::ThrowStreamIsClosed();

	tIOExceptionClass::Throw(RISSE_WS_TR("tOSNativeStreamInstance::Truncate not implemented"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tOSNativeStreamInstance::get_size()
{
	volatile tSynchronizer sync(this); // sync

	if(!Internal) tIOExceptionClass::ThrowStreamIsClosed();

	return Internal->File.Length();
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tOSNativeStreamClass::tOSNativeStreamClass(tScriptEngine * engine) :
	tClassBase(tSS<'O','S','N','a','t','i','v','e','S','t','r','e','a','m'>(), engine->StreamClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSNativeStreamClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tOSNativeStreamClass::ovulate);
	BindFunction(this, ss_construct, &tOSNativeStreamInstance::construct);
	BindFunction(this, ss_initialize, &tOSNativeStreamInstance::initialize);
	BindFunction(this, ss_dispose, &tOSNativeStreamInstance::dispose);
	BindFunction(this, ss_seek, &tOSNativeStreamInstance::seek);
	BindFunction(this, ss_tell, &tOSNativeStreamInstance::tell);
	BindFunction(this, ss_read, &tOSNativeStreamInstance::read);
	BindFunction(this, ss_write, &tOSNativeStreamInstance::write);
	BindFunction(this, ss_truncate, &tOSNativeStreamInstance::truncate);
	BindProperty(this, ss_size, &tOSNativeStreamInstance::get_size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tOSNativeStreamClass::ovulate()
{
	return tVariant(new tOSNativeStreamInstance());
}
//---------------------------------------------------------------------------
















//---------------------------------------------------------------------------
tOSFSInstance::tOSFSInstance()
{
	CheckCase = true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSFSInstance::SetOptions(const tString & basedir, bool checkcase)
{
	CheckCase = checkcase;

	// ベースディレクトリのパス区切りをネイティブなものに変更
	wxString base_directory;
	base_directory = ConvertToNativePathDelimiter(basedir.AsWxString());

	// ベースディレクトリ名の最後がパスデリミタでおわっていなけばそれを追加
	if(!wxFileName::IsPathSeparator(base_directory.Last()))
		base_directory += wxFileName::GetPathSeparator();

	// ベースディレクトリ名を正規化
	wxFileName normalized(base_directory);
	normalized.Normalize();
	base_directory = normalized.GetFullPath();

	// ベースディレクトリ名の最後がパスデリミタでおわっていなけばそれを追加
	if(!wxFileName::IsPathSeparator(base_directory.Last()))
		base_directory += wxFileName::GetPathSeparator();

	// ベースディレクトリを保存
	BaseDirectory = base_directory;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tOSFSInstance::GetFileListAt(const tString & dirname,
	tFileSystemIterationCallback * callback)
{
	wxString wxdirname(dirname.AsWxString());

	CheckFileNameCase(wxdirname);

	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter((wxString)wxdirname));

	wxDir dir;
	size_t count = 0;
	wxString filename;
	bool cont;

	// ディレクトリの存在をチェック
	if(!wxFileName::DirExists(native_name)
		|| !dir.Open(native_name))
			eRisaException::Throw(tString(wxString::Format(
				RISSE_WS_TR("can not open directory '%s'"), native_name.c_str())));

	// ファイルを列挙
	cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
	while(cont)
	{
		count ++;
		if(callback)
		{
			if(!callback->OnFile(tString(filename)))
				return count;
		}
		cont = dir.GetNext(&filename);
	}

	// ディレクトリを列挙
	cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
	while(cont)
	{
		if(filename != wxT("..") && filename != wxT("."))
		{
			count ++;
			if(callback)
			{
				if(!callback->OnDirectory(tString(filename)))
					return count;
			}
		}
		cont = dir.GetNext(&filename);
	}

	return count;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tOSFSInstance::FileExists(const tString & filename)
{
	wxString wxfilename(filename.AsWxString());

	if(!CheckFileNameCase(wxfilename, false)) return false;

	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(wxfilename));

	return wxFileName::FileExists(native_name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tOSFSInstance::DirectoryExists(const tString & dirname)
{
	wxString wxdirname(dirname.AsWxString());

	if(!CheckFileNameCase(wxdirname, false)) return false;

	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(wxdirname));

	return wxFileName::DirExists(native_name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSFSInstance::RemoveFile(const tString & filename)
{
	wxString wxfilename(filename.AsWxString());

	CheckFileNameCase(wxfilename);

	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(wxfilename));

	if(!::wxRemoveFile(native_name))
		eRisaException::Throw(tString(wxString::Format(RISSE_WS_TR("can not remove file '%s'"), native_name.c_str())));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSFSInstance::RemoveDirectory(const tString & dirname, bool recursive)
{
	wxString wxdirname(dirname.AsWxString());

	CheckFileNameCase(wxdirname);

	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(wxdirname));

	if(recursive)
		eRisaException::Throw(
			RISSE_WS_TR("recursive directory remove is not yet implemented"));

	if(::wxRmdir(native_name))
		eRisaException::Throw(tString(wxString::Format(RISSE_WS_TR("can not remove directory '%s'"), native_name.c_str())));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSFSInstance::CreateDirectory(const tString & dirname, bool recursive)
{
	wxString wxdirname(dirname.AsWxString());

	CheckFileNameCase(wxdirname);

	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(wxdirname));

	if(!wxFileName::Mkdir(native_name, 0777, recursive?wxPATH_MKDIR_FULL:0))
		eRisaException::Throw(tString(wxString::Format(RISSE_WS_TR("can not create directory '%s'"), native_name.c_str())));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSFSInstance::Stat(const tString & filename, tStatStruc & struc)
{
	wxString wxfilename(filename.AsWxString());

	CheckFileNameCase(wxfilename);

	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(wxfilename));

	// 格納先構造体をクリア
	struc.Clear();

	// 時刻を取得
	wxFileName filename_obj(native_name);

	if(!filename_obj.GetTimes(&struc.ATime, &struc.MTime, &struc.CTime))
		eRisaException::Throw(tString(wxString::Format(RISSE_WS_TR("can not stat file '%s'"), native_name.c_str())));

	// サイズを取得
	wxFile file;
	if(!file.Open(native_name))
		eRisaException::Throw(tString(wxString::Format(RISSE_WS_TR("can not stat file '%s'"), native_name.c_str())));

	struc.Size = file.Length();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStreamInstance * tOSFSInstance::CreateStream(const tString & filename, risse_uint32 flags)
{
	wxString wxfilename(filename.AsWxString());

	CheckFileNameCase(wxfilename);

	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(wxfilename));

	// OSNativeStreamClass からインスタンスを生成して返す
	tVariant obj =
		tRisseFSClassRegisterer<tOSFSClass>::instance()->GetClassInstance()->
			GetOSNativeStreamClass()->Invoke(ss_new, native_name.c_str(), (risse_int64)flags);
	obj.AssertClass(tRisseFSClassRegisterer<tOSFSClass>::instance()->GetClassInstance()->
									GetOSNativeStreamClass());
	return reinterpret_cast<tStreamInstance *>(obj.GetObjectInterface());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
wxString tOSFSInstance::ConvertToNativePathDelimiter(const wxString & path)
{
	wxString ret(path);
	if(ret.Length() > 0)
	{
		wxChar pathsep = static_cast<wxChar>(wxFileName::GetPathSeparator());
		for(size_t n = 0; ret.GetChar(n); n++)
		{
			if(ret.GetChar(n) == wxT('/'))
				ret.GetWritableChar(n) = pathsep;
		}
	}
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tOSFSInstance::CheckFileNameCase(const wxString & path_to_check, bool raise)
{
	// TODO: UNIX系OSでのチェック
	if(!CheckCase) return true; // 大文字・小文字をチェックしない場合はそのまま帰る

	// うーん、VMS 形式のパスは相手にすべきなのだろうか

	const wxChar * msg = RISSE_WS_TR("file '%s' does not exist (but '%s' exists, you mean this?");

	// パスを分解する
	wxString existing;
	wxString volume, path, name;
	wxFileName testpath(path_to_check);
	wxDir dir;

	wxFileName::SplitPath(path_to_check, &volume, &path, NULL, NULL);
	name = testpath.GetFullName();

	// まずファイル名をチェック
	if(!name.IsEmpty())
	{
		wxString subpath(testpath.GetPath(wxPATH_GET_SEPARATOR));
		if(wxFileName::DirExists(subpath) &&
			dir.Open(subpath) && dir.GetFirst(&existing, name))
		{
			if(existing != name)
			{
				// ファイル名が違う
				if(raise)
					eRisaException::Throw(tString(wxString::Format(msg, path_to_check.c_str(),
						(subpath + existing).c_str())));
				else
					return false;
			}
		}
	}

	// ディレクトリをチェック
	testpath.Assign(testpath.GetPath());
	while(testpath.GetDirCount())
	{
		wxString subpath(testpath.GetPath(wxPATH_GET_SEPARATOR));
		if(wxFileName::DirExists(subpath) &&
			dir.Open(subpath) && dir.GetFirst(&existing, testpath.GetFullName()))
		{
			if(existing != name)
			{
				// ファイル名が違う
				if(raise)
					eRisaException::Throw(tString(wxString::Format(msg, testpath.GetFullPath().c_str(),
						(subpath + existing).c_str())));
				else
					return false;
			}
		}
		testpath.RemoveLastDir();
	}

	// エラーは見つからなかった
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSFSInstance::construct()
{
	// とくにやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSFSInstance::initialize(const tString & base_dir, const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();

	// SetOptions を呼ぶ
	bool check_case = info.args.HasArgument(1) ? (bool)info.args[1] : true;
	SetOptions(base_dir, check_case);
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tOSFSClass::tOSFSClass(tScriptEngine * engine) :
	tClassBase(tSS<'O','S','F','S'>(),
		tRisseClassRegisterer<tFileSystemClass>::instance()->GetClassInstance())
{
	OSNativeStreamClass = new tOSNativeStreamClass(engine);

	RegisterMembers();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void tOSFSClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tOSFSClass::ovulate);
	BindFunction(this, ss_construct, &tOSFSInstance::construct);
	BindFunction(this, ss_initialize, &tOSFSInstance::initialize);

	// OSNativeStream を登録する
	RegisterNormalMember(tSS<'O','S','N','a','t','i','v','e','S','t','r','e','a','m'>(),
			tVariant(OSNativeStreamClass), tMemberAttribute(), true);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tOSFSClass::ovulate()
{
	return tVariant(new tOSFSInstance());
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief		OSFS クラスレジストラ
template class tRisseFSClassRegisterer<tOSFSClass>;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


