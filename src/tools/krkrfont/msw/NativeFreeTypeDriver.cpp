//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Win32 GDI 経由でのFreeType Face
//! @note フォント名からフォントファイル名を得る動作がOSごとに異なるため、
//! tTVPFreeTypeFace もプラットフォームごとに異なった実装となる。
//---------------------------------------------------------------------------
#include "prec.h"
#include "NativeFreeTypeDriver.h"


// 以下の二つのdefineはOpenType フォントを表していて、
// EnumFontsProc の lpntme->ntmTm.ntmFlags にビットセットとして渡されてくる。
// (OpenTypeがサポートされた Windows 2000 以降で存在)
// tTVPBaseFreeTypeFontDriver::EnumerateFonts ではTrueType フォントとともに
// これらのフォントも列挙される。

#ifndef NTM_PS_OPENTYPE
#define NTM_PS_OPENTYPE     0x00020000 //!< PostScript 形式 OpenType フォント
#endif

#ifndef NTM_TT_OPENTYPE
#define NTM_TT_OPENTYPE     0x00040000 //!< TrueType 形式 OpenType フォント
#endif


//---------------------------------------------------------------------------
//! @brief		tTVPBaseFreeTypeFontDriver::EnumerateFonts で内部的に使われるクラス
//---------------------------------------------------------------------------
class tTVPFreeTypeFontEnumeraterHelper
{
private:
	wxArrayString & Dest; //!< 格納先配列
	tjs_uint32 Flags; //!< 列挙フラグ (tvpfontstruc.h の TVP_FSF_XXXXX 定数の bitor )
	wxFontEncoding Encoding; //!< エンコーディング
	wxString PrevFontName; //!< 直前に列挙したフォント名(重複をはじくために使う)

public:
	tTVPFreeTypeFontEnumeraterHelper(wxArrayString & dest,
		tjs_uint32 Flags, wxFontEncoding encoding);
	~tTVPFreeTypeFontEnumeraterHelper();

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
//! @brief		コンストラクタ
//! @param		flags	列挙フラグ (tvpfontstruc.h の TVP_FSF_XXXXX 定数の bitor )
//! @param		encoding	エンコーディング (wxFontEncoding)
//! @param		dest	格納先配列 (配列はクリアされる)
//---------------------------------------------------------------------------
tTVPFreeTypeFontEnumeraterHelper::tTVPFreeTypeFontEnumeraterHelper(wxArrayString & dest,
	tjs_uint32 flags, wxFontEncoding encoding ) :
		 Dest(dest), Flags(flags), Encoding(encoding)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPFreeTypeFontEnumeraterHelper::~tTVPFreeTypeFontEnumeraterHelper()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		フォントを列挙する
//---------------------------------------------------------------------------
void tTVPFreeTypeFontEnumeraterHelper::DoEnumerate()
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
//! @brief		フォント列挙コールバック
//! @param		lplf	LPLOGFONT
//! @param		lptm	LPTEXTMETRIC
//! @param		dwStyle	DWORD style
//! @param		lParam	User-defined data
//---------------------------------------------------------------------------
int CALLBACK tTVPFreeTypeFontEnumeraterHelper::CallbackProc(
			ENUMLOGFONTEX *lpelfe,    // pointer to logical-font data
			NEWTEXTMETRICEX *lpntme,  // pointer to physical-font data
			int FontType,             // type of font
			LPARAM lParam  )
{
	tTVPFreeTypeFontEnumeraterHelper * _this =
		reinterpret_cast<tTVPFreeTypeFontEnumeraterHelper*>(lParam);

	if(_this->Flags & TVP_FSF_FIXEDPITCH)
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

	if(_this->Flags & TVP_FSF_NOVERTICAL)
	{
		// not to list vertical fonts up ?
		if(lpelfe->elfLogFont.lfFaceName[0] == '@') return 1;
	}

	if(_this->Flags & TVP_FSF_OUTLINEONLY)
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
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTVPBaseFreeTypeFontDriver::tTVPBaseFreeTypeFontDriver()
{
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPBaseFreeTypeFontDriver::~tTVPBaseFreeTypeFontDriver()
{
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		FreeType フォントドライバで使用可能なフォントを列挙する
//! @param		dest	格納先配列 (配列はクリアされる)
//! @param		flags	列挙フラグ (tvpfontstruc.h の TVP_FSF_XXXXX 定数の bitor )
//! @param		encoding	エンコーディング (wxFontEncoding)
//! @note		encoding を文字列から変換するには wxFontMapper::CharsetToEncoding を
//!				使うことができる。
//!				flags に指定できるのは、TVP_FSF_FIXEDPITCH 、 TVP_FSF_NOVERTICAL のみ。
//---------------------------------------------------------------------------
void tTVPBaseFreeTypeFontDriver::EnumerateFonts(wxArrayString & dest,
		tjs_uint32 flags, wxFontEncoding encoding)
{
	dest.Clear();

	flags |= TVP_FSF_OUTLINEONLY; // 常に(FreeTypeで使用可能な)アウトラインフォントのみを列挙する

	tTVPFreeTypeFontEnumeraterHelper helper(dest, flags, encoding);
	helper.DoEnumerate(); // 列挙を行う
}
//---------------------------------------------------------------------------




