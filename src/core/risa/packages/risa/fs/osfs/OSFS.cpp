//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OSFSの実装
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/fs/osfs/OSFS.h"
#include "risa/common/RisaException.h"
#include "risseExceptionClass.h"
#include "risseStaticStrings.h"
#include <wx/filename.h>
#include <wx/dir.h>



namespace Risa {
RISSE_DEFINE_SOURCE_ID(36633,46200,38568,17703,26300,17488,6098,26811);
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
	bool check_case = info.args.Get(1, true);
	SetOptions(base_dir, check_case);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tOSFSInstance::walkAt(const tString & dirname,
	const tMethodArgument &args)
{
	volatile tSynchronizer sync(this); // sync

	args.ExpectBlockArgumentCount(1); // ブロック引数が一つなければならない

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(dirname.AsWxString()));
	CheckFileNameCase(native_name);

	wxDir dir;
	size_t count = 0;
	wxString filename;
	bool cont;

	tVariant callback(args.GetBlockArgument(0));

	// ディレクトリの存在をチェック
	if(!wxFileName::DirExists(native_name)
		|| !dir.Open(native_name))
			tIOExceptionClass::Throw(tString(
				RISSE_WS_TR("cannot open directory '%1'"), native_name.c_str()));

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
	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(filename.AsWxString()));
	CheckFileNameCase(native_name);

	return wxFileName::FileExists(native_name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tOSFSInstance::isDirectory(const tString & dirname)
{
	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(dirname.AsWxString()));
	CheckFileNameCase(native_name);

	return wxFileName::DirExists(native_name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSFSInstance::removeFile(const tString & filename)
{
	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(filename.AsWxString()));
	CheckFileNameCase(native_name);

	if(!::wxRemoveFile(native_name))
		tIOExceptionClass::Throw(tString(
				RISSE_WS_TR("cannot remove file '%1'"), native_name.c_str()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSFSInstance::removeDirectory(const tString & dirname, const tMethodArgument &args)
{
	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(dirname.AsWxString()));
	CheckFileNameCase(native_name);

	bool recursive = args.HasArgument(1) ? (bool)args[1] : false;

	if(recursive)
		tIOExceptionClass::Throw(
			RISSE_WS_TR("recursive directory remove is not yet implemented"));

	if(!::wxRmdir(native_name))
		tIOExceptionClass::Throw(tString(
			RISSE_WS_TR("cannot remove directory '%1'"), native_name.c_str()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSFSInstance::createDirectory(const tString & dirname)
{
	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(dirname.AsWxString()));
	CheckFileNameCase(native_name);

	if(!wxFileName::Mkdir(native_name, 0777, 0))
		tIOExceptionClass::Throw(tString(
			RISSE_WS_TR("cannot create directory '%1'"), native_name.c_str()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tObjectInterface * tOSFSInstance::stat(const tString & filename)
{
	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(filename.AsWxString()));
	CheckFileNameCase(native_name);

	tIOExceptionClass::Throw(tString(RISSE_WS_TR("stat is not yet implemented")));

	return NULL;
/*
	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(wxfilename));

	// 格納先構造体をクリア
	struc.Clear();

	// 時刻を取得
	wxFileName filename_obj(native_name);

	if(!filename_obj.GetTimes(&struc.ATime, &struc.MTime, &struc.CTime))
		tIOExceptionClass::Throw(tString(wxString::Format(RISSE_WS_TR("cannot stat file '%s'"), native_name.c_str())));

	// サイズを取得
	wxFile file;
	if(!file.Open(native_name))
		tIOExceptionClass::Throw(tString(wxString::Format(RISSE_WS_TR("cannot stat file '%s'"), native_name.c_str())));

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
		tClassHolder<tOSNativeStreamClass>::instance()->GetClass()->
			Invoke(ss_new, tString(native_name.c_str()), (risse_int64)flags);
	obj.AssertClass(tClassHolder<tOSNativeStreamClass>::instance()->GetClass());
	return static_cast<tStreamInstance *>(obj.GetObjectInterface());
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
	// 今のところ相手してない

	const wxChar * msg = RISSE_WS_TR("file or directory '%1' does not exist (but '%2' exists)");

	// ここでは自前でパスの分解を行う
	wxDir dir;
	const wxChar * start = path_to_check.c_str();
	const wxChar * p = path_to_check.c_str() + BaseDirectoryLengthWx;
	const wxChar * pp = p;

	for(;;)
	{
		while(*p && !wxFileName::IsPathSeparator(*p)) p++;
		// この時点で p は \0 か パスデリミタ
		// pp から p までがパスコンポーネント
		// start から pp までがパスコンポーネントを含むディレクトリ
		if(p - pp > 0)
		{
			if(!dir.Open(wxString(start, pp - start))) return true; // ディレクトリをオープンできなかった
			wxString existing;
			wxString path_comp(pp, p - pp);
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
		}
		if(!*p) break;
		p++;
		pp = p;
	}

	// エラーは見つからなかった
	return true;
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tOSFSClass, (tSS<'O','S','F','S'>()),
		tClassHolder<tFileSystemClass>::instance()->GetClass())
	OSNativeStreamClass = new tOSNativeStreamClass(GetRTTI()->GetScriptEngine());

	RISSE_BIND_CONSTRUCTORS
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
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
/**
 * OSFS クラスレジストラ
 */
template class tClassRegisterer<
	tSS<'r','i','s','a','.','f','s'>,
	tOSFSClass>;
//---------------------------------------------------------------------------















//---------------------------------------------------------------------------
} // namespace Risa


