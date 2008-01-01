//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ファイルリストに関する処理を行う
//---------------------------------------------------------------------------
#ifndef _FILELIST_H_
#define _FILELIST_H_

#include "WriteXP4.h"
#include "ReadXP4Meta.h"

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void XP4GetDefaultClassList(wxArrayString & dest);

void GetFileListAt(iRisaProgressCallback * callback,
	const wxString & dir, std::vector<tXP4WriterInputFile> & dest);

void XP4ClassifyFiles(iRisaProgressCallback * callback,
	const wxArrayString & pattern,
	std::vector<tXP4WriterInputFile> &  dest
	);

wxString NormalizeXP4ArchiveStorageName(const wxString & name);

void EnumerateArchiveFiles(const wxString & archivename,
	std::vector<wxString> & archives);

void ReadXP4Metadata(
	iRisaProgressCallback * callback,
	const wxString & archivename,
	std::map<wxString, tXP4MetadataReaderStorageItem> &dest,
	wxString * targetdir = NULL);

void CompareXP4StorageNameMap(
	iRisaProgressCallback * callback,
	std::map<wxString, tXP4MetadataReaderStorageItem> &arc,
	std::vector<tXP4WriterInputFile> & ref);

void XP4MetadataReaderStorageItemToXP4WriterInputFile(
	const std::map<wxString, tXP4MetadataReaderStorageItem> & input,
	std::vector<tXP4WriterInputFile> & output);

void DeleteArchiveSet(const wxString & archivename);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif
