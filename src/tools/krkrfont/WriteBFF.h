//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief BFF (TVP3 Bitmap Font File) の書き出し
//---------------------------------------------------------------------------

#ifndef _WRITEBFF_H_
#define _WRITEBFF_H_

void TVPWriteGlyphBitmap(tTVPFreeTypeFace * face, const wxString & out_file,
	bool write_bitmap,
	bool write_kerning_vector, wxWindow * parent);

#endif
