//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 失敗時に例外を発生する機構を備えた wxFile のラッパー
//---------------------------------------------------------------------------
#include "prec.h"
#include "wxFileWrapper.h"

#define RAISE_EXCEPTION(msg) throw wxString(msg)


//---------------------------------------------------------------------------
wxFileEx::wxFileEx()
{
}
//---------------------------------------------------------------------------
wxFileEx::wxFileEx(const wxChar* filename,
	wxFile::OpenMode mode) :
		inherited(filename, mode == write_append ? read_write : mode)
{
	FileName = wxEmptyString;
	if(!inherited::IsOpened()) RaiseOpenError(filename, mode);
	FileName = filename;

	if(mode == write_append)
		SeekEnd();
			// this is to avoid a problem that the Tell()
			// always returns 0 right after opening with write_append

}
//---------------------------------------------------------------------------
wxFileEx::~wxFileEx()
{
}
//---------------------------------------------------------------------------
void wxFileEx::Open(const wxChar* filename, wxFile::OpenMode mode)
{
	FileName = wxEmptyString;
	if(!inherited::Open(filename, mode == write_append ? read_write : mode))
		RaiseOpenError(filename, mode);
	FileName = filename;
	if(mode == write_append)
		SeekEnd();
}
//---------------------------------------------------------------------------
void wxFileEx::Create(const wxChar* filename, bool overwrite, int access)
{
	FileName = wxEmptyString;
	if(!inherited::Create(filename, overwrite, access))
		RAISE_EXCEPTION(
			wxString::Format(
				_("can not create file '%s'"), filename));
	FileName = filename;
}
//---------------------------------------------------------------------------
void wxFileEx::Close()
{
	FileName = wxEmptyString;
	inherited::Close();
}
//---------------------------------------------------------------------------
void wxFileEx::Attach(int fd)
{
	FileName = wxEmptyString;
	if(fd == wxFile::fd_stdin)
		FileName = _("Standard Input");
	else if(fd == wxFile::fd_stdout)
		FileName = _("Standard Output");
	else if(fd == wxFile::fd_stderr)
		FileName = _("Standard Error Output");
	else
		FileName = wxString::Format(_("File Descriptor %d"), fd);
	inherited::Attach(fd);
}
//---------------------------------------------------------------------------
void wxFileEx::Detach()
{
	FileName = wxEmptyString;
	inherited::Detach();
}
//---------------------------------------------------------------------------
void wxFileEx::SetPosition(wxFileOffset ofs)
{
	if(inherited::Seek(ofs) != ofs)
		RAISE_EXCEPTION(
			wxString::Format(_("failed to seek on file '%s', pointer %lld"),
				FileName.c_str(), (wxUint64)ofs));
}
//---------------------------------------------------------------------------
void wxFileEx::ReadBuffer(void* buffer, size_t count)
{
	if((size_t)inherited::Read(buffer, count) != count)
		RAISE_EXCEPTION(
			wxString::Format(_("failed to read from file '%s'"), FileName.c_str()));
}
//---------------------------------------------------------------------------
void wxFileEx::WriteBuffer(const void* buffer, wxFileOffset count)
{
	if(inherited::Write(buffer, count) != count)
		RAISE_EXCEPTION(
			wxString::Format(_("failed to write to file '%s'"), FileName.c_str()));
}
//---------------------------------------------------------------------------
void wxFileEx::Write(const wxString& s, wxMBConv& conv)
{
	if(!inherited::Write(s, conv))
		RAISE_EXCEPTION(
			wxString::Format(_("failed to write to file '%s'"), FileName.c_str()));
}
//---------------------------------------------------------------------------
void wxFileEx::RaiseOpenError(const wxChar * filename, wxFile::OpenMode mode)
{
	if(mode == wxFile::read)
		RAISE_EXCEPTION(
			wxString::Format(
				_("can not open file '%s' for reading"), filename));
	else
		RAISE_EXCEPTION(
			wxString::Format(
				_("can not open file '%s' for writing"), filename));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルポインタのアラインメントを行う
//! @param		alignbytes: アラインメント数
//! @param		padding: 詰め物として書き込む文字
//---------------------------------------------------------------------------
void wxFileEx::Align(wxFileOffset alignbytes, unsigned int padding)
{
	wxFileOffset padbytes = 
		alignbytes - (Tell() % alignbytes);
	if(padbytes >= alignbytes) padbytes -= alignbytes;

	// padbytes 分の padding を書き込む
	Fill(padbytes, padding);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定数の同じ文字を書き込む
//! @param		fillbytes: 書き込むバイト数
//! @param		padding: 書き込む文字
//---------------------------------------------------------------------------
void wxFileEx::Fill(wxFileOffset fillbytes, unsigned int padding)
{
	unsigned char buf[4096];
	memset(buf, padding, fillbytes > sizeof(buf) ? sizeof(buf): (size_t)fillbytes);
	while(fillbytes > 0)
	{
		size_t onesize = fillbytes > sizeof(buf) ? sizeof(buf): (size_t)fillbytes;
		WriteBuffer(buf, onesize);
		fillbytes -= onesize;
	}
}
//---------------------------------------------------------------------------

