//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief BFF (3 Bitmap Font File) の書き出し
//---------------------------------------------------------------------------

#ifndef _WRITEBFF_H_
#define _WRITEBFF_H_

namespace Risa {
//---------------------------------------------------------------------------

void WriteGlyphBitmap(tFreeTypeFace * face, const wxString & out_file,
	bool write_bitmap,
	bool write_kerning_vector, wxWindow * parent);

//---------------------------------------------------------------------------
} // namespace Risa


#endif
