//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

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


//---------------------------------------------------------------------------
//! @brief		FreeType フォント face
//---------------------------------------------------------------------------
class tTVPFreeTypeFace
{
	wxString FontName;		//!< フォント名
	tTVPBaseFreeTypeFace * Face; //!< Face オブジェクト
	FT_Face FTFace; //!< FreeType Face オブジェクト
	tjs_uint32 Options; //!< フラグ

	typedef std::vector<FT_ULong> tGlyphIndexToCharcodeVector;
	tGlyphIndexToCharcodeVector * GlyphIndexToCharcodeVector;		//!< グリフインデックスから文字コードへの変換マップ
	tjs_int Height;		//!< フォントサイズ(高さ) in pixel

	tjs_uint (*UnicodeToLocalChar)(tjs_char in); //!< SJISなどをUnicodeに変換する関数
	tjs_char (*LocalCharToUnicode)(tjs_uint in); //!< UnicodeをSJISなどに変換する関数

public:
	tTVPFreeTypeFace(const wxString &fontname, tjs_uint32 options);
	~tTVPFreeTypeFace();

	tjs_uint GetGlyphCount();
	tjs_char GetCharcodeFromGlyphIndex(tjs_uint index);

	void GetFaceNameList(wxArrayString &dest);

	tjs_int GetHeight() { return Height; }
	void SetHeight(int height);

	tTVPGlyphBitmap * GetGlyphFromCharcode(tjs_char code);
	bool GetGlyphMetricsFromCharcode(tjs_char code, tTVPGlyphMetrics & metrics);

private:
	bool LoadGlyphSlotFromCharcode(tjs_char code);
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		FreeType ライブラリラッパー(singleton)
//---------------------------------------------------------------------------
class tTVPFreeTypeLibrary
{
	static tTVPFreeTypeLibrary * GlobalLibrary;	//!< FreeType ライブラリ

	tjs_int RefCount;	//!< 参照カウンタ
	FT_Library Library;	//!< FreeType ライブラリ

public:
	static void AddRef();
	static void Release();

	//! @brief	FreeType ライブラリを返す
	//! @return	ライブラリインスタンス
	static FT_Library Get() { if(!GlobalLibrary) return NULL;
								return GlobalLibrary->Library; }

private:
	tTVPFreeTypeLibrary();
	~tTVPFreeTypeLibrary();

	void Hook();
	tjs_int Unhook();
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief		FreeType フォントドライバ
//---------------------------------------------------------------------------
class tTVPFreeTypeFontDriver : public tTVPBaseFreeTypeFontDriver
{
public:
	tTVPFreeTypeFontDriver();
	~tTVPFreeTypeFontDriver();

private:
};
//---------------------------------------------------------------------------








#endif /*_FREETYPE_H_*/
