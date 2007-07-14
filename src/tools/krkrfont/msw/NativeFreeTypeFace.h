//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
// @brief Win32 GDI 経由でのFreeType Face
//---------------------------------------------------------------------------
#ifndef _NATIVEFREETYPEFACE_H_
#define _NATIVEFREETYPEFACE_H_

#include "tvpfontstruc.h"
#include "FreeTypeFace.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		Win32 GDI 経由でのFreeType Face クラス
//---------------------------------------------------------------------------
class tRisaNativeFreeTypeFace : public tRisaBaseFreeTypeFace
{
protected:
	wxString FaceName;	//!< Face名 = フォント名
	FT_Face Face;	//!< FreeType face オブジェクト

private:
	HDC DC;			//!< デバイスコンテキスト
	HFONT OldFont;	//!< デバイスコンテキストに元々登録されていた古いフォント
	bool IsTTC;		//!< TTC(TrueTypeCollection)ファイルを扱っている場合に真
	FT_StreamRec Stream;

public:
	tRisaNativeFreeTypeFace(const wxString &fontname, risse_uint32 options);
	virtual ~tRisaNativeFreeTypeFace();

	virtual FT_Face GetFTFace() const;
	virtual void GetFaceNameList(wxArrayString & dest) const; 

	bool GetIsTTC() const { return IsTTC; }

private:
	void Clear();
	static unsigned long IoFunc(
			FT_Stream stream,
			unsigned long   offset,
			unsigned char*  buffer,
			unsigned long   count );
	static void CloseFunc( FT_Stream  stream );

	bool OpenFaceByIndex(int index);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa



#endif /*_NATIVEFREETYPEFACE_H_*/
