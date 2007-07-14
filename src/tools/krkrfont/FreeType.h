//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief FreeType フォントドライバ
//---------------------------------------------------------------------------
#ifndef _FREETYPE_H_
#define _FREETYPE_H_


#include "FontDriver.h"
#include "FreeTypeFace.h"
#include "NativeFreeTypeDriver.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		FreeType フォント face
//---------------------------------------------------------------------------
class tRisaFreeTypeFace
{
	wxString FontName;		//!< フォント名
	tRisaBaseFreeTypeFace * Face; //!< Face オブジェクト
	FT_Face FTFace; //!< FreeType Face オブジェクト
	risse_uint32 Options; //!< フラグ

	typedef std::vector<FT_ULong> tGlyphIndexToCharcodeVector;
	tGlyphIndexToCharcodeVector * GlyphIndexToCharcodeVector;		//!< グリフインデックスから文字コードへの変換マップ
	risse_int Height;		//!< フォントサイズ(高さ) in pixel

	risse_uint (*UnicodeToLocalChar)(risse_char in); //!< SJISなどをUnicodeに変換する関数
	risse_char (*LocalCharToUnicode)(risse_uint in); //!< UnicodeをSJISなどに変換する関数

public:
	tRisaFreeTypeFace(const wxString &fontname, risse_uint32 options);
	~tRisaFreeTypeFace();

	risse_uint GetGlyphCount();
	risse_char GetCharcodeFromGlyphIndex(risse_uint index);

	void GetFaceNameList(wxArrayString &dest);

	risse_int GetHeight() { return Height; }
	void SetHeight(int height);

	tRisaGlyphBitmap * GetGlyphFromCharcode(risse_char code);
	bool GetGlyphMetricsFromCharcode(risse_char code, tRisaGlyphMetrics & metrics);

private:
	bool LoadGlyphSlotFromCharcode(risse_char code);
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		FreeType ライブラリラッパー(singleton)
//---------------------------------------------------------------------------
class tRisaFreeTypeLibrary
{
	static tRisaFreeTypeLibrary * GlobalLibrary;	//!< FreeType ライブラリ

	risse_int RefCount;	//!< 参照カウンタ
	FT_Library Library;	//!< FreeType ライブラリ

public:
	static void AddRef();
	static void Release();

	//! @brief	FreeType ライブラリを返す
	//! @return	ライブラリインスタンス
	static FT_Library Get() { if(!GlobalLibrary) return NULL;
								return GlobalLibrary->Library; }

private:
	tRisaFreeTypeLibrary();
	~tRisaFreeTypeLibrary();

	void Hook();
	risse_int Unhook();
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief		FreeType フォントドライバ
//---------------------------------------------------------------------------
class tRisaFreeTypeFontDriver : public tRisaBaseFreeTypeFontDriver
{
public:
	tRisaFreeTypeFontDriver();
	~tRisaFreeTypeFontDriver();

private:
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
} // namespace Risa





#endif /*_FREETYPE_H_*/
