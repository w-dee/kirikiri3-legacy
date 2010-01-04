//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief fontconfig 経由でのFreeType Driver
/**
 * @note	フォント名からフォントファイル名を得る動作がOSごとに異なるため、
 *			tFreeTypeDriver もプラットフォームごとに異なった実装となる。
 */
#include "../prec.h"
#include "NativeFreeTypeDriver.h"
#include <fontconfig/fontconfig.h>

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * コンストラクタ
 */
tBaseFreeTypeFontDriver::tBaseFreeTypeFontDriver()
{
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * デストラクタ
 */
tBaseFreeTypeFontDriver::~tBaseFreeTypeFontDriver()
{
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * FreeType フォントドライバで使用可能なフォントを列挙する
 * @param dest		格納先配列 (配列はクリアされる)
 * @param flags		列挙フラグ (tvpfontstruc.h の RISA_FSF_XXXXX 定数の bitor )
 * @param encoding	エンコーディング (wxFontEncoding)
 * @note	encoding を文字列から変換するには wxFontMapper::CharsetToEncoding を
 *			使うことができる。
 *			flags に指定できるのは、RISA_FSF_FIXEDPITCH 、 RISA_FSF_NOVERTICAL のみ。
 */
void tBaseFreeTypeFontDriver::EnumerateFonts(wxArrayString & dest,
		risse_uint32 flags, wxFontEncoding encoding)
{
	dest.Clear();

	flags |= RISA_FSF_OUTLINEONLY; // 常に(FreeTypeで使用可能な)アウトラインフォントのみを列挙する
	if ( ! FcInit() ) {
		wxString msg(_("FcInit failed"));
		throw msg;
	}

	FcObjectSet *os = FcObjectSetBuild( FC_FAMILY, (char*)0 );
	FcPattern *pat = FcPatternCreate();

	if(flags & RISA_FSF_FIXEDPITCH)
	{
		// fixed pitch only ?
		FcPatternAddInteger( pat, FC_SPACING, 90 );
	}

	if( encoding != wxFONTENCODING_SYSTEM )
	{
		// 指定されたエンコーディングでは使用できないフォント
		// TODO 
	}

	if(flags & RISA_FSF_NOVERTICAL)
	{
		// not to list vertical fonts up ?
		FcPatternAddBool( pat, FC_VERTICAL_LAYOUT, FcTrue );
	}

	if(flags & RISA_FSF_OUTLINEONLY)
	{
		FcPatternAddBool( pat, FC_OUTLINE, FcTrue );
	}

	FcFontSet *fs = FcFontList( 0, pat, os );

	FcPatternDestroy( pat );
	FcObjectSetDestroy( os );

	{
		for ( int i=0; i<fs->nfont; i++ ) {
			FcChar8 *family;
			FcPatternGetString( fs->fonts[i], FC_FAMILY, 0, &family );
			char *family_str = reinterpret_cast<char*>(family);
			dest.Add( wxString(family_str,wxConvLibc,strlen(family_str)) );
		}
	}
	FcFontSetDestroy( fs );
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risa


