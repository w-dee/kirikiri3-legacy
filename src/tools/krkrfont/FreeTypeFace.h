//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief FreeType の Face 基底クラスの定義
//---------------------------------------------------------------------------

#ifndef FREETYPEFACE_H
#define FREETYPEFACE_H

#include <ft2build.h>
#include FT_FREETYPE_H

//---------------------------------------------------------------------------
//! @brief		FreeType Face の基底クラス
//---------------------------------------------------------------------------
class tTVPBaseFreeTypeFace
{
public:
	virtual FT_Face GetFTFace() const = 0; //!< FreeType の Face オブジェクトを返す
	virtual void GetFaceNameList(wxArrayString & dest) const = 0; //!< このフォントファイルが持っているフォントを配列として返す
};
//---------------------------------------------------------------------------


#endif

