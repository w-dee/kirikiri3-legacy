//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

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
void TVPXP4GetDefaultClassList(wxArrayString & dest);

void TVPGetFileListAt(iTVPProgressCallback * callback,
	const wxString & dir, std::vector<tTVPXP4WriterInputFile> & dest);

void TVPXP4ClassifyFiles(iTVPProgressCallback * callback,
	const wxArrayString & pattern,
	std::vector<tTVPXP4WriterInputFile> &  dest
	);

wxString TVPNormalizeXP4ArchiveStorageName(const wxString & name);

void TVPEnumerateArchiveFiles(const wxString & archivename,
	std::vector<wxString> & archives);

void TVPReadXP4Metadata(
	iTVPProgressCallback * callback,
	const wxString & archivename,
	std::map<wxString, tTVPXP4MetadataReaderStorageItem> &dest);

void TVPCompareXP4StorageNameMap(
	iTVPProgressCallback * callback,
	std::map<wxString, tTVPXP4MetadataReaderStorageItem> &arc,
	std::vector<tTVPXP4WriterInputFile> & ref);

void TVPXP4MetadataReaderStorageItemToXP4WriterInputFile(
	const std::map<wxString, tTVPXP4MetadataReaderStorageItem> & input,
	std::vector<tTVPXP4WriterInputFile> & output);

void TVPDeleteArchiveSet(const wxString & archivename);
//---------------------------------------------------------------------------

#endif
