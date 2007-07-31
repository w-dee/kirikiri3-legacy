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

	info.InitializeSuperClass();

	// モードを決定する
	wxFile::OpenMode mode;
	switch(flags & tFileOpenModes::omAccessMask)
	{
	case tFileOpenModes::omRead:
		mode = wxFile::read; break;
	case tFileOpenModes::omWrite:
		mode = wxFile::write; break;
	case tFileOpenModes::omUpdate:
		mode = wxFile::read_write; break;
	default:
		mode = wxFile::read;
	}

	// ファイルを開く
	Internal = new tInternal();
	if(!Internal->File.Open(path.AsWxString(), mode))
	{
		delete Internal; Internal = NULL;
		tIOExceptionClass::Throw(
			tString(RISSE_WS_TR("can not open file %1"), path));
	}

	// 名前を

	// APPEND の場合はファイルポインタを最後に移動する
	if(flags & tFileOpenModes::omAppendBit)
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
	if(Internal)
		delete Internal, Internal = NULL;
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
risse_size tOSNativeStreamInstance::get(const tOctet & buf)
{
	volatile tSynchronizer sync(this); // sync

	if(!Internal) tIOExceptionClass::ThrowStreamIsClosed();

	// buf の示すポインタに「直接」書き込みを行う。
	// tOctet をメモリバッファとして使うかなり危ない使い方だが、
	// これに限ってはこういう使い方をすると言うことになっている。
	// というかそうした。

	ssize_t read = Internal->File.Read(const_cast<risse_uint8*>(buf.Pointer()), buf.GetLength());

	if(read < 0 || read == wxInvalidOffset) read = 0; // エラーと見なす

	return read;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tOSNativeStreamInstance::put(const tOctet & buf)
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
void tOSNativeStreamInstance::flush()
{
	volatile tSynchronizer sync(this); // sync

	if(!Internal) tIOExceptionClass::ThrowStreamIsClosed();

	Internal->File.Flush();
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
	BindFunction(this, ss_get, &tOSNativeStreamInstance::get);
	BindFunction(this, ss_put, &tOSNativeStreamInstance::put);
	BindFunction(this, ss_truncate, &tOSNativeStreamInstance::truncate);
	BindProperty(this, ss_size, &tOSNativeStreamInstance::get_size);
	BindFunction(this, ss_flush, &tOSNativeStreamInstance::flush);
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
	BaseDirectoryLengthWx = base_directory.length();
	BaseDirectory = base_directory;
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
size_t tOSFSInstance::walkAt(const tString & dirname,
	const tMethodArgument &args)
{
	wxString wxdirname(dirname.AsWxString());

	CheckFileNameCase(wxdirname);

	volatile tSynchronizer sync(this); // sync

	args.ExpectBlockArgumentCount(1); // ブロック引数が一つなければならない

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter((wxString)wxdirname));

	wxDir dir;
	size_t count = 0;
	wxString filename;
	bool cont;

	tVariant callback(args.GetBlockArgument(0));

	// ディレクトリの存在をチェック
	if(!wxFileName::DirExists(native_name)
		|| !dir.Open(native_name))
			tIOExceptionClass::Throw(tString(
				RISSE_WS_TR("can not open directory '%1'"), native_name.c_str()));

	// ファイルを列挙
	cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
	while(cont)
	{
		count ++;
		callback.Do(GetRTTI()->GetScriptEngine(),
			ocFuncCall, NULL, tString::GetEmptyString(), 0,
			tMethodArgument::New(tString(filename), false));

		cont = dir.GetNext(&filename);
	}

	// ディレクトリを列挙
	cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
	while(cont)
	{
		if(filename != wxT("..") && filename != wxT("."))
		{
			count ++;
			callback.Do(GetRTTI()->GetScriptEngine(),
				ocFuncCall, NULL, tString::GetEmptyString(), 0,
				tMethodArgument::New(tString(filename), true));
		}
		cont = dir.GetNext(&filename);
	}

	return count;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tOSFSInstance::isFile(const tString & filename)
{
	wxString wxfilename(filename.AsWxString());

	if(!CheckFileNameCase(wxfilename, false)) return false;

	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(wxfilename));

	return wxFileName::FileExists(native_name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tOSFSInstance::isDirectory(const tString & dirname)
{
	wxString wxdirname(dirname.AsWxString());

	if(!CheckFileNameCase(wxdirname, false)) return false;

	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(wxdirname));

	return wxFileName::DirExists(native_name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSFSInstance::removeFile(const tString & filename)
{
	wxString wxfilename(filename.AsWxString());

	CheckFileNameCase(wxfilename);

	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(wxfilename));

	if(!::wxRemoveFile(native_name))
		tIOExceptionClass::Throw(tString(
				RISSE_WS_TR("can not remove file '%1'"), native_name.c_str()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSFSInstance::removeDirectory(const tString & dirname, const tMethodArgument &args)
{
	wxString wxdirname(dirname.AsWxString());

	CheckFileNameCase(wxdirname);

	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(wxdirname));

	bool recursive = args.HasArgument(1) ? (bool)args[1] : false;

	if(recursive)
		tIOExceptionClass::Throw(
			RISSE_WS_TR("recursive directory remove is not yet implemented"));

	if(::wxRmdir(native_name))
		tIOExceptionClass::Throw(tString(
			RISSE_WS_TR("can not remove directory '%1'"), native_name.c_str()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSFSInstance::createDirectory(const tString & dirname)
{
	wxString wxdirname(dirname.AsWxString());

	CheckFileNameCase(wxdirname);

	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(wxdirname));

	if(!wxFileName::Mkdir(native_name, 0777, 0))
		tIOExceptionClass::Throw(tString(
			RISSE_WS_TR("can not create directory '%1'"), native_name.c_str()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tObjectInterface * tOSFSInstance::stat(const tString & filename)
{
	wxString wxfilename(filename.AsWxString());

	CheckFileNameCase(wxfilename);

	volatile tSynchronizer sync(this); // sync

	tIOExceptionClass::Throw(tString(RISSE_WS_TR("stat is not yet implemented")));

	return NULL;
/*
	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(wxfilename));

	// 格納先構造体をクリア
	struc.Clear();

	// 時刻を取得
	wxFileName filename_obj(native_name);

	if(!filename_obj.GetTimes(&struc.ATime, &struc.MTime, &struc.CTime))
		tIOExceptionClass::Throw(tString(wxString::Format(RISSE_WS_TR("can not stat file '%s'"), native_name.c_str())));

	// サイズを取得
	wxFile file;
	if(!file.Open(native_name))
		tIOExceptionClass::Throw(tString(wxString::Format(RISSE_WS_TR("can not stat file '%s'"), native_name.c_str())));

	struc.Size = file.Length();
*/
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStreamInstance * tOSFSInstance::open(const tString & filename, risse_uint32 flags)
{
	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(filename.AsWxString()));
	CheckFileNameCase(native_name);

	// OSNativeStreamClass からインスタンスを生成して返す
	tVariant obj =
		tRisseFSClassRegisterer<tOSFSClass>::instance()->GetClassInstance()->
			GetOSNativeStreamClass()->Invoke(ss_new, tString(native_name.c_str()), (risse_int64)flags);
	obj.AssertClass(tRisseFSClassRegisterer<tOSFSClass>::instance()->GetClassInstance()->
									GetOSNativeStreamClass());
	return reinterpret_cast<tStreamInstance *>(obj.GetObjectInterface());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSFSInstance::flush()
{
	// 現状、何もしない。
	// 本当は sync() を発行したほうがいいのかもしれないけれども………
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

	const wxChar * msg = RISSE_WS_TR("file or directory '%1' does not exist (but '%2' exists, did you mean this?)");

	// パスを分解する
	wxFileName testpath(path_to_check);
	testpath.Normalize();
	wxString normalized_path = testpath.GetFullPath();

	// ここでは自前でパスの分解を行う
	wxDir dir;
	const wxChar * start = normalized_path.c_str();
	const wxChar * p = normalized_path.c_str() + BaseDirectoryLengthWx;
	const wxChar * pp = p;

	for(;;)
	{
		while(*p && !wxFileName::IsPathSeparator(*p)) p++;
		// この時点で p は \0 か パスデリミタ
		// pp から p までがパスコンポーネント
		// start から pp までがパスコンポーネントを含むディレクトリ
		if(!dir.Open(wxString(start, pp - start))) return true; // ディレクトリをオープンできなかった
		wxString existing;
		wxString path_comp(p, p - pp);
		if(!dir.GetFirst(&existing, path_comp)) return true; // 見つからなかった
		if(existing != path_comp)
		{
			// 大文字と小文字が違う
			if(raise)
				tIOExceptionClass::Throw(tString(msg, wxString(start, p - start).c_str(),
								(wxString(start, pp - start) + existing).c_str()));
			else
				return false;
		}
		if(!*p) break;
		p++;
		pp = p + 1;
	}

	// エラーは見つからなかった
	return true;
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

	BindFunction(this, tSS<'w','a','l','k','A','t'>(), &tOSFSInstance::walkAt);
	BindFunction(this, tSS<'i','s','F','i','l','e'>(), &tOSFSInstance::isFile);
	BindFunction(this, tSS<'i','s','D','i','r','e','c','t','o','r','y'>(), &tOSFSInstance::isDirectory);
	BindFunction(this, tSS<'r','e','m','o','v','e','F','i','l','e'>(), &tOSFSInstance::removeFile);
	BindFunction(this, tSS<'r','e','m','o','v','e','D','i','r','e','c','t','o','r','y'>(), &tOSFSInstance::removeDirectory);
	BindFunction(this, tSS<'c','r','e','a','t','e','D','i','r','e','c','t','o','r','y'>(), &tOSFSInstance::createDirectory);
	BindFunction(this, tSS<'s','t','a','t'>(), &tOSFSInstance::stat);
	BindFunction(this, tSS<'o','p','e','n'>(), &tOSFSInstance::open);
	BindFunction(this, tSS<'f','l','u','s','h'>(), &tOSFSInstance::flush);

	BindProperty(this, tSS<'s','o','u','r','c','e'>(), &tOSFSInstance::get_source);

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


