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
#include "base/fs/osfs/OSFSBind.h"
#include "base/exception/RisaException.h"
#include <wx/filename.h>
#include <wx/dir.h>

RISSE_DEFINE_SOURCE_ID(49572,65271,56057,18682,27296,33314,20965,8152);



//---------------------------------------------------------------------------
tRisaOSNativeStream::tRisaOSNativeStream(const wxString & filename, risse_uint32 flags)
{
	// モードを決定する
	wxFile::OpenMode mode;
	switch(flags & RISSE_BS_ACCESS_MASK)
	{
	case RISSE_BS_READ:
		mode = wxFile::read; break;
	case RISSE_BS_WRITE:
		mode = wxFile::write; break;
	case RISSE_BS_UPDATE:
		mode = wxFile::read_write; break;
	default:
		mode = wxFile::read;
	}

	// ファイルを開く
	if(!File.Open(filename))
		eRisaException::Throw(ttstr(wxString::Format(RISSE_WS_TR("can not open file '%s'"),
			filename.c_str())));

	// APPEND の場合はファイルポインタを最後に移動する
	if(flags & RISSE_BS_ACCESS_APPEND_BIT)
		File.SeekEnd();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaOSNativeStream::~tRisaOSNativeStream()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tRisaOSNativeStream::Seek(risse_int64 offset, risse_int whence)
{
	wxSeekMode mode = wxFromCurrent;
	switch(whence)
	{
	case RISSE_BS_SEEK_SET: mode = wxFromStart;   break;
	case RISSE_BS_SEEK_CUR: mode = wxFromCurrent; break;
	case RISSE_BS_SEEK_END: mode = wxFromEnd;     break;
	default: offset = 0;
	}

	wxFileOffset newpos = File.Seek(offset, mode);
	if(newpos == wxInvalidOffset)
		eRisaException::Throw(RISSE_WS_TR("seek failed"));

	return newpos;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tRisaOSNativeStream::Read(void *buffer, risse_size read_size)
{
	return File.Read(buffer, read_size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tRisaOSNativeStream::Write(const void *buffer, risse_size write_size)
{
	return File.Write(buffer, write_size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaOSNativeStream::SetEndOfFile()
{
	// TODO: implement this 
	eRisaException::Throw(RISSE_WS_TR("tRisaOSNativeStream::SetEndOfFile not implemented"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tRisaOSNativeStream::GetSize()
{
	return File.Length();
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
tRisaOSFS::tRisaOSFS(const ttstr & basedir, bool checkcase) : 
	BaseDirectory(basedir.AsWxString()), CheckCase(checkcase)
{
	// ベースディレクトリのパス区切りをネイティブなものに変更
	BaseDirectory = ConvertToNativePathDelimiter(BaseDirectory);

	// ベースディレクトリ名の最後がパスデリミタでおわっていなけばそれを追加
	if(!wxFileName::IsPathSeparator(BaseDirectory.Last()))
		BaseDirectory += wxFileName::GetPathSeparator();

	// ベースディレクトリ名を正規化
	wxFileName normalized(BaseDirectory);
	normalized.Normalize();
	BaseDirectory = normalized.GetFullPath();

	// ベースディレクトリ名の最後がパスデリミタでおわっていなけばそれを追加
	if(!wxFileName::IsPathSeparator(BaseDirectory.Last()))
		BaseDirectory += wxFileName::GetPathSeparator();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaOSFS::~tRisaOSFS()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tRisaOSFS::GetFileListAt(const ttstr & dirname,
	tRisaFileSystemIterationCallback * callback)
{
	wxString wxdirname(dirname.AsWxString());

	CheckFileNameCase(wxdirname);

	volatile tRisaCriticalSection::tLocker holder(CS);

	wxString native_name(BaseDirectory + ConvertToNativePathDelimiter((wxString)wxdirname));

	wxDir dir;
	size_t count = 0;
	wxString filename;
	bool cont;

	// ディレクトリの存在をチェック
	if(!wxFileName::DirExists(native_name)
		|| !dir.Open(native_name))
			eRisaException::Throw(ttstr(wxString::Format(
				RISSE_WS_TR("can not open directory '%s'"), native_name.c_str())));

	// ファイルを列挙
	cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
	while(cont)
	{
		count ++;
		if(callback)
		{
			if(!callback->OnFile(ttstr(filename)))
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
				if(!callback->OnDirectory(ttstr(filename)))
					return count;
			}
		}
		cont = dir.GetNext(&filename);
	}

	return count;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisaOSFS::FileExists(const ttstr & filename)
{
	wxString wxfilename(filename.AsWxString());

	if(!CheckFileNameCase(wxfilename, false)) return false;

	volatile tRisaCriticalSection::tLocker holder(CS);

	wxString native_name(BaseDirectory + ConvertToNativePathDelimiter(wxfilename));

	return wxFileName::FileExists(native_name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisaOSFS::DirectoryExists(const ttstr & dirname)
{
	wxString wxdirname(dirname.AsWxString());

	if(!CheckFileNameCase(wxdirname, false)) return false;

	volatile tRisaCriticalSection::tLocker holder(CS);

	wxString native_name(BaseDirectory + ConvertToNativePathDelimiter(wxdirname));

	return wxFileName::DirExists(native_name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaOSFS::RemoveFile(const ttstr & filename)
{
	wxString wxfilename(filename.AsWxString());

	CheckFileNameCase(wxfilename);

	volatile tRisaCriticalSection::tLocker holder(CS);

	wxString native_name(BaseDirectory + ConvertToNativePathDelimiter(wxfilename));

	if(!::wxRemoveFile(native_name))
		eRisaException::Throw(ttstr(wxString::Format(RISSE_WS_TR("can not remove file '%s'"), native_name.c_str())));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaOSFS::RemoveDirectory(const ttstr & dirname, bool recursive)
{
	wxString wxdirname(dirname.AsWxString());

	CheckFileNameCase(wxdirname);

	volatile tRisaCriticalSection::tLocker holder(CS);

	wxString native_name(BaseDirectory + ConvertToNativePathDelimiter(wxdirname));

	if(recursive)
		eRisaException::Throw(
			RISSE_WS_TR("recursive directory remove is not yet implemented"));

	if(::wxRmdir(native_name))
		eRisaException::Throw(ttstr(wxString::Format(RISSE_WS_TR("can not remove directory '%s'"), native_name.c_str())));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaOSFS::CreateDirectory(const ttstr & dirname, bool recursive)
{
	wxString wxdirname(dirname.AsWxString());

	CheckFileNameCase(wxdirname);

	volatile tRisaCriticalSection::tLocker holder(CS);

	wxString native_name(BaseDirectory + ConvertToNativePathDelimiter(wxdirname));

	if(!wxFileName::Mkdir(native_name, 0777, recursive?wxPATH_MKDIR_FULL:0))
		eRisaException::Throw(ttstr(wxString::Format(RISSE_WS_TR("can not create directory '%s'"), native_name.c_str())));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaOSFS::Stat(const ttstr & filename, tRisaStatStruc & struc)
{
	wxString wxfilename(filename.AsWxString());

	CheckFileNameCase(wxfilename);

	volatile tRisaCriticalSection::tLocker holder(CS);

	wxString native_name(BaseDirectory + ConvertToNativePathDelimiter(wxfilename));

	// 格納先構造体をクリア
	struc.Clear();

	// 時刻を取得
	wxFileName filename_obj(native_name);

	if(!filename_obj.GetTimes(&struc.ATime, &struc.MTime, &struc.CTime))
		eRisaException::Throw(ttstr(wxString::Format(RISSE_WS_TR("can not stat file '%s'"), native_name.c_str())));

	// サイズを取得
	wxFile file;
	if(!file.Open(native_name))
		eRisaException::Throw(ttstr(wxString::Format(RISSE_WS_TR("can not stat file '%s'"), native_name.c_str())));

	struc.Size = file.Length();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseBinaryStream * tRisaOSFS::CreateStream(const ttstr & filename, risse_uint32 flags)
{
	wxString wxfilename(filename.AsWxString());

	CheckFileNameCase(wxfilename);

	volatile tRisaCriticalSection::tLocker holder(CS);

	wxString native_name(BaseDirectory + ConvertToNativePathDelimiter(wxfilename));

	return new tRisaOSNativeStream(native_name, flags);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
wxString tRisaOSFS::ConvertToNativePathDelimiter(const wxString & path)
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
bool tRisaOSFS::CheckFileNameCase(const wxString & path_to_check, bool raise)
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
					eRisaException::Throw(ttstr(wxString::Format(msg, path_to_check.c_str(),
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
					eRisaException::Throw(ttstr(wxString::Format(msg, testpath.GetFullPath().c_str(),
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

