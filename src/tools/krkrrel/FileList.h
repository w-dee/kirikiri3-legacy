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
//---------------------------------------------------------------------------
void TVPGetFileListAt(iTVPProgressCallback * callback,
	const wxString & dir, std::vector<tTVPXP4WriterInputFile> & dest);
void TVPClassifyFiles(iTVPProgressCallback * callback,
	const wxArrayString & excludepat,
	const wxArrayString & compresspat,
	std::vector<tTVPXP4WriterInputFile> &  dest
	);
wxString TVPNormalizeXP4ArchiveStorageName(const wxString & name);
//---------------------------------------------------------------------------

#endif
