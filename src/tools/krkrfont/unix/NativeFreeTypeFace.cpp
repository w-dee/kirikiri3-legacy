//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief fontconfig 経由でのFreeType Face
//! @note フォント名からフォントファイル名を得る動作がOSごとに異なるため、
//! tRisaFreeTypeFace もプラットフォームごとに異なった実装となる。
//---------------------------------------------------------------------------
#include "prec.h"
#include "NativeFreeTypeFace.h"
#include "FreeType.h"
#include <fontconfig/fontconfig.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TAGS_H
#include FT_TRUETYPE_TABLES_H
#include FT_TRUETYPE_UNPATENTED_H


namespace {

struct CFcFontSet
{
public:
	FcFontSet *fs;
	CFcFontSet ( FcFontSet *fs )
		:fs(fs) {}
	~CFcFontSet() { FcFontSetDestroy(fs ) ; }
};

};

//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		fontname フォント名
//! @param		options オプション
//---------------------------------------------------------------------------
tRisaNativeFreeTypeFace::tRisaNativeFreeTypeFace(const wxString &fontname,
	risse_uint32 options)
	:FaceName(fontname)
{
	(void)options;
	// フィールドのクリア
	Face = NULL;

	FcChar8 *filename; // TTFのファイル名
	wxWX2MBbuf fontname_mbs_buffer = fontname.mbc_str();

	FcObjectSet *os = FcObjectSetBuild( FC_FILE, (char*)0 );
	FcPattern *pat = FcPatternCreate();

	FcPatternAddString( pat, FC_FAMILY,
			    reinterpret_cast<FcChar8 const *>(
				    static_cast<char const *>(fontname_mbs_buffer)
				    ) );

	FcFontSet *fs = FcFontList( 0, pat, os );
	CFcFontSet cfs( fs );

	FcPatternDestroy( pat );
	FcObjectSetDestroy( os );

	if ( fs->nfont == 0 ) {
		wxString msg;
		msg.sprintf(_("Font '%1$s' cannot be used"), static_cast<char const *>(fontname_mbs_buffer) );
		throw msg;
	}

	if ( FcPatternGetString(fs->fonts[0],FC_FILE,0,&filename) != FcResultMatch ) {
		wxString msg;
		msg.sprintf(_("Font '%1$s' cannot be used"), static_cast<char const *>(fontname_mbs_buffer) );
		throw msg;
	}

	// TrueType ライブラリをフック
	tRisaFreeTypeLibrary::AddRef();
	FT_Error err = FT_New_Face( tRisaFreeTypeLibrary::Get(), reinterpret_cast<char const*>(filename), 0, &Face );

	if ( err != 0 ) {
		wxString msg;
		msg.sprintf(_("Font '%1$s' cannot be used"), static_cast<char const *>(fontname_mbs_buffer) );
		throw msg;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaNativeFreeTypeFace::~tRisaNativeFreeTypeFace()
{
	Clear();
	tRisaFreeTypeLibrary::Release();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		FreeType の Face オブジェクトを返す
//---------------------------------------------------------------------------
FT_Face tRisaNativeFreeTypeFace::GetFTFace() const
{
	return Face;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		このフォントファイルが持っているフォントを配列として返す
//! @param		dest 格納先配列
//---------------------------------------------------------------------------
void tRisaNativeFreeTypeFace::GetFaceNameList(wxArrayString & dest) const
{
	// このFaceの場合、既にFaceは特定されているため、利用可能な
	// Face 数は常に1で、フォント名はこのオブジェクトが構築された際に渡された
	// フォント名となる
	dest.Clear();
	dest.Add(FaceName);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		全てのオブジェクトを解放する
//---------------------------------------------------------------------------
void tRisaNativeFreeTypeFace::Clear()
{
	if(Face) FT_Done_Face(Face), Face = NULL;
}
//---------------------------------------------------------------------------






