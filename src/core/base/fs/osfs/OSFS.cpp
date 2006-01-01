//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OSFSの実装
//---------------------------------------------------------------------------
#include "prec.h"
TJS_DEFINE_SOURCE_ID(2001);

#include "OSFS.h"




//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTVPOSNativeStream::tTVPOSNativeStream(const wxString & filename, tjs_uint32 flags)
{
	// モードを決定する
	wxFile::OpenMode mode;
	switch(flags & TJS_BS_ACCESS_MASK)
	{
	case TJS_BS_READ:
		mode = wxFile::read; break;
	case TJS_BS_WRITE:
		mode = wxFile::write; break;
	case TJS_BS_UPDATE:
		mode = wxFile::read_write; break;
	default:
		mode = wxFile::read;
	}

	// ファイルを開く
	if(!file.Open(filename))
		TVPThrowExceptionMessage(wxString::Format(_("can not open file '%s'"),
			filename.c_str()));

	// APPEND の場合はファイルポインタを最後に移動する
	if(flags & TJS_BS_ACCESS_APPEND_BIT)
		file.SeekEnd();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPOSNativeStream::~tTVPOSNativeStream()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		シーク
//! @param		offset 移動オフセット
//! @param		whence 移動オフセットの基準 (TJS_BS_SEEK_* 定数)
//! @return		移動後のファイルポインタ
//---------------------------------------------------------------------------
tjs_uint64 tTVPOSNativeStream::Seek(tjs_int64 offset, tjs_int whence)
{
	wxSeekMode mode = wxFromCurrent;
	switch(whence)
	{
	case TJS_BS_SEEK_SET: mode = wxFromStart;   break;
	case TJS_BS_SEEK_CUR: mode = wxFromCurrent; break;
	case TJS_BS_SEEK_END: mode = wxFromEnd;     break;
	default: offset = 0;
	}

	wxFileOffset newpos = File.Seek(offset, mode);
	if(newpos == wxInvalidOffset)
		TVPThrowExceptionMessage(_("seek failed"));

	return newpos;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		読み込み
//! @param		buffer 読み込み先バッファ
//! @param		read_size 読み込むバイト数
//! @return		実際に読み込まれたバイト数
//---------------------------------------------------------------------------
tjs_size tTVPOSNativeStream::Read(void *buffer, tjs_size read_size)
{
	return File.Write(buffer, read_size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		書き込み
//! @param		buffer 書き込むバッファ
//! @param		read_size 書き込みたいバイト数
//! @return		実際に書き込まれたバイト数
//---------------------------------------------------------------------------
tjs_size tTVPOSNativeStream::Write(const void *buffer, tjs_size write_size)
{
	return File.Write(buffer, write_size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルの終わりを現在のポインタに設定する
//---------------------------------------------------------------------------
void tTVPOSNativeStream::SetEndOfFile()
{
	// TODO: implement this 
	TVPThrowExceptionMessage(_("tTVPOSNativeStream::SetEndOfFile not implemented"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		サイズを得る
//! @return		このストリームのサイズ
//---------------------------------------------------------------------------
tjs_uint64 tTVPOSNativeStream::GetSize()
{
	return file.Length();
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTVPOSFS::tTVPOSFS(const ttstr & basedir, bool checkcase) : 
	BaseDirectory((wxString)basedir), CheckCase(checkcase)
{
	// ベースディレクトリのパス区切りをネイティブなものに変更
	BaseDirectory = ConvertPathDelimiter(BaseDirectory);

	// ベースディレクトリ名を正規化
	wxFileName basedir(BaseDirectory);
	basedir.Normalize();
	BaseDirectory = basedir.GetFullPath();

	// ベースディレクトリ名の最後がパスデリミタでおわっていなけばそれを追加
	if(wxFileName::IsPathSeparator(BaseDirectory.Last())
		BaseDirectory += wxFileName::GetPathSeparator();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPOSFS::~tTVPOSFS()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイル一覧を取得する
//! @param		dirname ディレクトリ名
//! @param		callback コールバックオブジェクト
//! @return		取得できたファイル数
//---------------------------------------------------------------------------
size_t tTVPOSFS::GetFileListAt(const ttstr & dirname,
	iTVPFileSystemIterationCallback * callback)
{
	CheckFileNameCase(dirname);

	volatile tTJSCriticalSectionHolder holder(CS);

	wxString native_name(BaseDirectory + ConvertPathDelimiter((wxString)dirname));

	wxDir dir;
	bool stop = false;
	size_t count = 0;
	wxString filename;
	bool cont;

	// ディレクトリの存在をチェック
	if(!wxFileName::DirExists(native_name)
		|| !dir.Open(native_name))
			TVPThrowExceptionMessage(wxString::Format(
				_("can not open directory '%s'"), native_name.c_str()));

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
		cont = dir.GetNext();
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
		cont = dir.GetNext();
	}

	return count;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルが存在するかどうかを得る
//! @param		filename ファイル名
//! @return		ファイルが存在する場合真
//---------------------------------------------------------------------------
bool tTVPOSFS::FileExists(const ttstr & filename)
{
	if(!CheckFileNameCase(filename, false)) return false;

	volatile tTJSCriticalSectionHolder holder(CS);

	wxString native_name(BaseDirectory + ConvertPathDelimiter((wxString)dirname));

	return wxFileName::FileExists(native_name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリが存在するかどうかを得る
//! @param		dirname ディレクトリ名
//! @return		ディレクトリが存在する場合真
//---------------------------------------------------------------------------
bool tTVPOSFS::DirectoryExists(const ttstr & dirname)
{
	if(!CheckFileNameCase(dirname, false)) return false;

	volatile tTJSCriticalSectionHolder holder(CS);

	wxString native_name(BaseDirectory + ConvertPathDelimiter((wxString)dirname));

	return wxFileName::DirExists(native_name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルを削除する
//! @param		filename ファイル名
//---------------------------------------------------------------------------
void tTVPOSFS::RemoveFile(const ttstr & filename)
{
	CheckFileNameCase(filename);

	volatile tTJSCriticalSectionHolder holder(CS);

	wxString native_name(BaseDirectory + ConvertPathDelimiter((wxString)dirname));

	if(!::wxRemoveFie(native_name))
		TVPThrowExceptionMessage(wxString::Format(_("can not remove file '%s'"), native_name.c_str()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリを削除する
//! @param		dirname ディレクトリ名
//! @param		recursive 再帰的にディレクトリを削除するかどうか
//---------------------------------------------------------------------------
void tTVPOSFS::RemoveDirectory(const ttstr & dirname, bool recursive)
{
	CheckFileNameCase(dirname);

	volatile tTJSCriticalSectionHolder holder(CS);

	wxString native_name(BaseDirectory + ConvertPathDelimiter((wxString)dirname));

	if(recursive)
		TVPThrowExceptionMessage(
			_("recursive directory remove is not yet implemented"));

	if(::wxRmdir(native_name))
		TVPThrowExceptionMessage(wxString::Format(_("can not remove directory '%s'"), native_name.c_str()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリを作成する
//! @param		dirname ディレクトリ名
//! @param		recursive 再帰的にディレクトリを作成するかどうか
//---------------------------------------------------------------------------
void tTVPOSFS::CreateDirectory(const ttstr & dirname, bool recursive)
{
	CheckFileNameCase(dirname);

	volatile tTJSCriticalSectionHolder holder(CS);

	wxString native_name(BaseDirectory + ConvertPathDelimiter((wxString)dirname));

	if(!wxFileName::Mkdir(native_name, 0777, recursive?wxPATH_MKDIR_FULL:0))
		TVPThrowExceptionMessage(wxString::Format(_("can not create directory '%s'"), native_name.c_str()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたファイルの stat を得る
//! @param		filename ファイル名
//! @param		struc stat 結果の出力先
//---------------------------------------------------------------------------
void tTVPOSFS::Stat(const ttstr & filename, tTVPStatStruc & struc)
{
	CheckFileNameCase(filename);

	volatile tTJSCriticalSectionHolder holder(CS);

	wxString native_name(BaseDirectory + ConvertPathDelimiter((wxString)dirname));

	// 格納先構造体をクリア
	struc.Clear();

	// 時刻を取得
	if(!wxFileName::GetTimes(&struc.ATime, &struc.MTime, &struc.CTime))
		TVPThrowExceptionMessage(wxString::Format(_("can not stat file '%s'"), native_name.c_str()));

	// サイズを取得
	wxFile file;
	if(!file.Oepn(native_name))
		TVPThrowExceptionMessage(wxString::Format(_("can not stat file '%s'"), native_name.c_str()));
	struc.Size = file.Length();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたファイルのストリームを得る
//! @param		filename ファイル名
//! @param		flags フラグ
//! @return		ストリームオブジェクト
//---------------------------------------------------------------------------
tTVPBinaryStream * tTVPOSFS::CreateStream(const ttstr & filename, tjs_uint32 flags)
{
	CheckFileNameCase(filename);

	volatile tTJSCriticalSectionHolder holder(CS);

	wxString native_name(BaseDirectory + ConvertPathDelimiter((wxString)dirname));

	return new tTVPOSNativeStream(native_name, flags);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		path中に含まれる'/'を、OSネイティブなパス区切り記号に変更する
//! @param		path パス名
//! @return		OSネイティブなパス区切りに変更された後のパス名
//---------------------------------------------------------------------------
wxString tTVPOSFS::ConvertToNativePathDelimiter(const wxString & path)
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
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		path中のディレクトリ名やファイル名の大文字や小文字がファイルシステム上のそれと一致するかどうかを調べる
//! @param		path_to_check パス名(フルパスであること)
//! @param		raise 一致しなかった場合に例外を発生するかどうか
//! @return		一致した場合に真、しなかった場合に偽
//! @note		ファイルが「見つからない」場合は真が帰るので注意
//---------------------------------------------------------------------------
void tTVPOSFS::CheckFileNameCase(const wxString & path_to_check, bool raise)
{
	// TODO: UNIX系OSでのチェック
	if(!CheckCase) return; // 大文字・小文字をチェックしない場合はそのまま帰る

	// うーん、VMS 形式のパスは相手にすべきなのだろうか

	const wxChar * msg = _("file '%s' does not exist (but '%s' exists, you mean this?)";

	// パスを分解する
	wxString existing;
	wxString volume, path, name;
	wxFileName testpath(path_to_check);
	wxDir dir;

	wxFileName::SplitPath(path_to_check, &volume, &path);
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
					TVPThrowExceptionMessage(wxString::Format(msg, path_to_check.c_str(),
						(subpath + existing).c_str());
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
					TVPThrowExceptionMessage(wxString::Format(msg, testpath.GetFullPath().c_str(),
						(subpath + existing).c_str());
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

