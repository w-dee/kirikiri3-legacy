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



namespace Risa {
RISSE_DEFINE_SOURCE_ID(49572,65271,56057,18682,27296,33314,20965,8152);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
tOSNativeStream::tOSNativeStream(const wxString & filename, risse_uint32 flags)
{
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
	if(!File.Open(filename))
		eRisaException::Throw(tString(wxString::Format(RISSE_WS_TR("can not open file '%s'"),
			filename.c_str())));

	// APPEND の場合はファイルポインタを最後に移動する
	if(flags & tFileSystem::omAppendBit)
		File.SeekEnd();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOSNativeStream::~tOSNativeStream()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tOSNativeStream::Seek(risse_int64 offset, tOrigin whence)
{
	wxSeekMode mode = wxFromCurrent;
	switch(whence)
	{
	case soSet: mode = wxFromStart;   break;
	case soCur: mode = wxFromCurrent; break;
	case soEnd: mode = wxFromEnd;     break;
	default: offset = 0;
	}

	wxFileOffset newpos = File.Seek(offset, mode);
	if(newpos == wxInvalidOffset)
		return false;

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tOSNativeStream::Tell()
{
	return File.Tell();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tOSNativeStream::Read(void *buffer, risse_size read_size)
{
	return File.Read(buffer, read_size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tOSNativeStream::Write(const void *buffer, risse_size write_size)
{
	return File.Write(buffer, write_size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSNativeStream::Truncate()
{
	// TODO: implement this 
	tIOExceptionClass::Throw(RISSE_WS_TR("tOSNativeStream::Truncate not implemented"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tOSNativeStream::GetSize()
{
	return File.Length();
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
tOSFS::tOSFS(const tString & basedir, bool checkcase)
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
size_t tOSFS::GetFileListAt(const tString & dirname,
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
bool tOSFS::FileExists(const tString & filename)
{
	wxString wxfilename(filename.AsWxString());

	if(!CheckFileNameCase(wxfilename, false)) return false;

	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(wxfilename));

	return wxFileName::FileExists(native_name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tOSFS::DirectoryExists(const tString & dirname)
{
	wxString wxdirname(dirname.AsWxString());

	if(!CheckFileNameCase(wxdirname, false)) return false;

	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(wxdirname));

	return wxFileName::DirExists(native_name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSFS::RemoveFile(const tString & filename)
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
void tOSFS::RemoveDirectory(const tString & dirname, bool recursive)
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
void tOSFS::CreateDirectory(const tString & dirname, bool recursive)
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
void tOSFS::Stat(const tString & filename, tStatStruc & struc)
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
tBinaryStream * tOSFS::CreateStream(const tString & filename, risse_uint32 flags)
{
	wxString wxfilename(filename.AsWxString());

	CheckFileNameCase(wxfilename);

	volatile tSynchronizer sync(this); // sync

	wxString native_name(BaseDirectory.c_str() + ConvertToNativePathDelimiter(wxfilename));

	return new tOSNativeStream(native_name, flags);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
wxString tOSFS::ConvertToNativePathDelimiter(const wxString & path)
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
bool tOSFS::CheckFileNameCase(const wxString & path_to_check, bool raise)
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
} // namespace Risa


