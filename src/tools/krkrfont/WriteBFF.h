//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief BFF (Risa3 Bitmap Font File) の書き出し
//---------------------------------------------------------------------------

#ifndef _WRITEBFF_H_
#define _WRITEBFF_H_

void RisaWriteGlyphBitmap(tRisaFreeTypeFace * face, const wxString & out_file,
	bool write_bitmap,
	bool write_kerning_vector, wxWindow * parent);

#endif
