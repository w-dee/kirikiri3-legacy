//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Win32 GDI 経由でのFreeType Face
/**
 * @note	フォント名からフォントファイル名を得る動作がOSごとに異なるため、
 *			tFreeTypeFace もプラットフォームごとに異なった実装となる。
 */
#include "../prec.h"
#include "NativeFreeTypeDriver.h"


// wxGetFontEncFromCharSet の前方定義。本当は
// msw/private.h の中にある。さすがに公式なAPIではないので将来的には
// リンクできなくなるかもしれないが、そのときはそのときで別に考えよう。
WXDLLEXPORT wxFontEncoding wxGetFontEncFromCharSet(int charset);

namespace Risa {
//---------------------------------------------------------------------------

// 以下の二つのdefineはOpenType フォントを表していて、
// EnumFontsProc の lpntme->ntmTm.ntmFlags にビットセットとして渡されてくる。
// (OpenTypeがサポートされた Windows 2000 以降で存在)
// tBaseFreeTypeFontDriver::EnumerateFonts ではTrueType フォントとともに
// これらのフォントも列挙される。

#ifndef NTM_PS_OPENTYPE
#define NTM_PS_OPENTYPE     0x00020000 //!< PostScript 形式 OpenType フォント
#endif

#ifndef NTM_TT_OPENTYPE
#define NTM_TT_OPENTYPE     0x00040000 //!< TrueType 形式 OpenType フォント
#endif


//---------------------------------------------------------------------------
/**
 * tBaseFreeTypeFontDriver::EnumerateFonts で内部的に使われるクラス
 */
class tFreeTypeFontEnumeraterHelper
{
private:
	wxArrayString & Dest; //!< 格納先配列
	risse_uint32 Flags; //!< 列挙フラグ (tvpfontstruc.h の RISA__FSF_XXXXX 定数の bitor )
	wxFontEncoding Encoding; //!< エンコーディング
	wxString PrevFontName; //!< 直前に列挙したフォント名(重複をはじくために使う)

public:
	tFreeTypeFontEnumeraterHelper(wxArrayString & dest,
		risse_uint32 Flags, wxFontEncoding encoding);
	~tFreeTypeFontEnumeraterHelper();

	void DoEnumerate();

private:
	static int CALLBACK CallbackProc(
			ENUMLOGFONTEX *lpelfe,    // pointer to logical-font data
			NEWTEXTMETRICEX *lpntme,  // pointer to physical-font data
			int FontType,             // type of font
			LPARAM lParam             // application-defined data
		);


};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * コンストラクタ
 * @param flags		列挙フラグ (tvpfontstruc.h の RISA__FSF_XXXXX 定数の bitor )
 * @param encoding	エンコーディング (wxFontEncoding)
 * @param dest		格納先配列 (配列はクリアされる)
 */
tFreeTypeFontEnumeraterHelper::tFreeTypeFontEnumeraterHelper(wxArrayString & dest,
	risse_uint32 flags, wxFontEncoding encoding ) :
		 Dest(dest), Flags(flags), Encoding(encoding)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * デストラクタ
 */
tFreeTypeFontEnumeraterHelper::~tFreeTypeFontEnumeraterHelper()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * フォントを列挙する
 */
void tFreeTypeFontEnumeraterHelper::DoEnumerate()
{
	HDC refdc = GetDC(NULL); // ディスプレイの DC を参照元 DC として取得
	try
	{
		PrevFontName = wxEmptyString;

		LOGFONT l;
		l.lfHeight = -12;
		l.lfWidth = 0;
		l.lfEscapement = 0;
		l.lfOrientation = 0;
		l.lfWeight = 400;
		l.lfItalic = FALSE;
		l.lfUnderline = FALSE;
		l.lfStrikeOut = FALSE;
		l.lfCharSet = DEFAULT_CHARSET;
		l.lfOutPrecision = OUT_DEFAULT_PRECIS;
		l.lfQuality = DEFAULT_QUALITY;
		l.lfPitchAndFamily = 0;
		l.lfFaceName[0] = wxT('\0');

		::EnumFontFamiliesEx(refdc, &l,
			(OLDFONTENUMPROCW)CallbackProc, // ここのキャストでエラーを起こすかも; 直して
			reinterpret_cast<LPARAM>(this), 0);
	}
	catch(...)
	{
		ReleaseDC(0, refdc);
		throw;
	}
	ReleaseDC(0, refdc);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * フォント列挙コールバック
 * @param lplf		LPLOGFONT
 * @param lptm		LPTEXTMETRIC
 * @param dwStyle	DWORD style
 * @param lParam	User-defined data
 */
int CALLBACK tFreeTypeFontEnumeraterHelper::CallbackProc(
			ENUMLOGFONTEX *lpelfe,    // pointer to logical-font data
			NEWTEXTMETRICEX *lpntme,  // pointer to physical-font data
			int FontType,             // type of font
			LPARAM lParam  )
{
	tFreeTypeFontEnumeraterHelper * _this =
		reinterpret_cast<tFreeTypeFontEnumeraterHelper*>(lParam);

	if(_this->Flags & RISA__FSF_FIXEDPITCH)
	{
		// fixed pitch only ?
		// TMPF_FIXED_PITCH はフラグが立っていないときに固定ピッチを
		// 表しているので注意
		if(lpntme->ntmTm.tmPitchAndFamily & TMPF_FIXED_PITCH) return 1;
	}

	if(_this->Encoding != wxFONTENCODING_SYSTEM &&
		 wxGetFontEncFromCharSet(lpelfe->elfLogFont.lfCharSet) != _this->Encoding)
	{
		// 指定されたエンコーディングでは使用できないフォント
		return 1;
	}

	if(_this->Flags & RISA__FSF_NOVERTICAL)
	{
		// not to list vertical fonts up ?
		if(lpelfe->elfLogFont.lfFaceName[0] == '@') return 1;
	}

	if(_this->Flags & RISA__FSF_OUTLINEONLY)
	{
		// outline fonts only
		bool is_outline =
			(lpntme->ntmTm.ntmFlags &  NTM_PS_OPENTYPE) ||
			(lpntme->ntmTm.ntmFlags &  NTM_TT_OPENTYPE) ||
			(FontType & TRUETYPE_FONTTYPE);
		if(!is_outline) return 1;
	}

	wxString fontname = wxString(lpelfe->elfLogFont.lfFaceName);
	if(_this->PrevFontName != fontname)
		_this->Dest.Add(fontname);
	_this->PrevFontName = fontname;

	return 1;
}
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
 * @param flags		列挙フラグ (tvpfontstruc.h の RISA__FSF_XXXXX 定数の bitor )
 * @param encoding	エンコーディング (wxFontEncoding)
 * @note	encoding を文字列から変換するには wxFontMapper::CharsetToEncoding を
 *			使うことができる。
 *			flags に指定できるのは、RISA__FSF_FIXEDPITCH 、 RISA__FSF_NOVERTICAL のみ。
 */
void tBaseFreeTypeFontDriver::EnumerateFonts(wxArrayString & dest,
		risse_uint32 flags, wxFontEncoding encoding)
{
	dest.Clear();

	flags |= RISA__FSF_OUTLINEONLY; // 常に(FreeTypeで使用可能な)アウトラインフォントのみを列挙する

	tFreeTypeFontEnumeraterHelper helper(dest, flags, encoding);
	helper.DoEnumerate(); // 列挙を行う
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risa


