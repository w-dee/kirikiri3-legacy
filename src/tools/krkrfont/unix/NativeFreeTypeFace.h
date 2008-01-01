//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
// @brief fontconfig 経由でのFreeType Face
//---------------------------------------------------------------------------
#ifndef FONTCONFIG_NATIVEFREETYPEFACE_H_
#define FONTCONFIG_NATIVEFREETYPEFACE_H_

#include "tvpfontstruc.h"
#include "FreeTypeFace.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		fontconfig 経由でのFreeType Face クラス
//---------------------------------------------------------------------------
class tNativeFreeTypeFace : public tBaseFreeTypeFace
{
protected:
	wxString FaceName;	//!< Face名 = フォント名
	FT_Face Face;		//!< FreeType face オブジェクト

public:
	tNativeFreeTypeFace(const wxString &fontname, risse_uint32 options);
	virtual ~tNativeFreeTypeFace();

	virtual FT_Face GetFTFace() const;
	virtual void GetFaceNameList(wxArrayString & dest) const; 

	bool GetIsTTC() const { return false; }

private:
	void Clear();

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


#endif // FONTCONFIG_NATIVEFREETYPEFACE_H_
