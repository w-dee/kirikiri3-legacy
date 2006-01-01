//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 失敗時に例外を発生する機構を備えた wxFile のラッパー
//---------------------------------------------------------------------------

#ifndef _wxFileWrapper_H_
#define _wxFileWrapper_H_

//---------------------------------------------------------------------------
//! @brief 失敗時に例外を発生する機構を備えた wxFile のラッパークラス
//---------------------------------------------------------------------------
class wxFileEx : public wxFile
{
	typedef wxFile inherited;

	wxString FileName;

public:
	wxFileEx();
	wxFileEx(const wxChar* filename,
		wxFile::OpenMode mode = wxFile::read);
	~wxFileEx();
	void Open(const wxChar* filename, wxFile::OpenMode mode = wxFile::read);
	void Create(const wxChar* filename, bool overwrite = false,
		int access = wxS_DEFAULT);
	void Close();
	void Attach(int fd);
	void Detach();
	void SetPosition(wxFileOffset ofs);
	void ReadBuffer(void* buffer, size_t count);
	void WriteBuffer(const void* buffer, wxFileOffset count);
	void Write(const wxString& s, wxMBConv& conv = wxConvUTF8);

private:
	void RaiseOpenError(const wxChar * filename, wxFile::OpenMode);

public:
	// ここ以降はオリジナルに無い機能
	void Align(wxFileOffset alignbytes, unsigned int padding = 0);
	void Fill(wxFileOffset fillbytes, unsigned int padding = 0);
};
//---------------------------------------------------------------------------

#endif

