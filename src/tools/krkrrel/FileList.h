//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

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
//---------------------------------------------------------------------------
void RisaXP4GetDefaultClassList(wxArrayString & dest);

void RisaGetFileListAt(iRisaProgressCallback * callback,
	const wxString & dir, std::vector<tRisaXP4WriterInputFile> & dest);

void RisaXP4ClassifyFiles(iRisaProgressCallback * callback,
	const wxArrayString & pattern,
	std::vector<tRisaXP4WriterInputFile> &  dest
	);

wxString RisaNormalizeXP4ArchiveStorageName(const wxString & name);

void RisaEnumerateArchiveFiles(const wxString & archivename,
	std::vector<wxString> & archives);

void RisaReadXP4Metadata(
	iRisaProgressCallback * callback,
	const wxString & archivename,
	std::map<wxString, tRisaXP4MetadataReaderStorageItem> &dest,
	wxString * targetdir = NULL);

void RisaCompareXP4StorageNameMap(
	iRisaProgressCallback * callback,
	std::map<wxString, tRisaXP4MetadataReaderStorageItem> &arc,
	std::vector<tRisaXP4WriterInputFile> & ref);

void RisaXP4MetadataReaderStorageItemToXP4WriterInputFile(
	const std::map<wxString, tRisaXP4MetadataReaderStorageItem> & input,
	std::vector<tRisaXP4WriterInputFile> & output);

void RisaDeleteArchiveSet(const wxString & archivename);
//---------------------------------------------------------------------------

#endif
