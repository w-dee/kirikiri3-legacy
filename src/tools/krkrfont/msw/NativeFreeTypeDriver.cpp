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

//---------------------------------------------------------------------------
//! @brief		tTVPBaseFreeTypeFontDriver::EnumerateFonts で内部的に使われるクラス
//---------------------------------------------------------------------------
class tTVPFreeTypeFontEnumeraterHelper
{
private:
	wxArrayString & Dest; //!< 格納先配列
	tjs_uint32 Flags; //!< 列挙フラグ (tvpfontstruc.h の TVP_FSF_XXXXX 定数の bitor )
	wxFontEncoding Encoding; //!< エンコーディング

public:
	tTVPFreeTypeFontEnumeraterHelper(wxArrayString & dest,
		tjs_uint32 Flags, wxFontEncoding encoding);
	~tTVPFreeTypeFontEnumeraterHelper();

	void DoEnumerate();

private:
	static int CALLBACK CallbackProc(LPLOGFONT lplf, LPTEXTMETRIC lptm,
                                  DWORD WXUNUSED(dwStyle), LONG lParam);

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
#if defined(__GNUWIN32__) && !defined(__CYGWIN10__) && !wxCHECK_W32API_VERSION( 1, 1 ) && !wxUSE_NORLANDER_HEADERS
    #define wxFONTENUMPROC int(*)(ENUMLOGFONTEX *, NEWTEXTMETRICEX*, int, LPARAM)
#else
    #define wxFONTENUMPROC FONTENUMPROC
#endif
void tTVPFreeTypeFontEnumeraterHelper::DoEnumerate()
{
	HDC refdc = GetDC(NULL); // ディスプレイの DC を参照元 DC として取得
	try
	{
		::EnumFonts(refdc, NULL, (wxFONTENUMPROC)CallbackProc,
			reinterpret_cast<LPARAM>(this));
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
int CALLBACK tTVPFreeTypeFontEnumeraterHelper::CallbackProc(LPLOGFONT lplf,
	LPTEXTMETRIC lptm, DWORD dwStyle, LONG lParam)
{
	tTVPFreeTypeFontEnumeraterHelper * _this =
		reinterpret_cast<tTVPFreeTypeFontEnumeraterHelper*>(lParam);

	if(_this->Flags & TVP_FSF_FIXEDPITCH)
	{
		// fixed pitch only ?
		// TMPF_FIXED_PITCH はフラグが立っていないときに固定ピッチを
		// 表しているので注意
		if(lptm->tmPitchAndFamily & TMPF_FIXED_PITCH) return 1;
	}

	if(_this->Encoding != wxFONTENCODING_SYSTEM &&
		 wxGetFontEncFromCharSet(lplf->lfCharSet) != _this->Encoding)
	{
		// 指定されたエンコーディングでは使用できないフォント
		return 1;
	}

	if(_this->Flags & TVP_FSF_NOVERTICAL)
	{
		// not to list vertical fonts up ?
		if(lplf->lfFaceName[0] == '@') return 1;
	}

	if(_this->Flags & TVP_FSF_TRUETYPEONLY)
	{
		// true type only ?
		if(!(dwStyle & TRUETYPE_FONTTYPE)) return 1;
	}

	_this->Dest.Add(lplf->lfFaceName);

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

	flags |= TVP_FSF_TRUETYPEONLY; // 常に TrueType フォントのみを列挙する

	tTVPFreeTypeFontEnumeraterHelper helper(dest, flags, encoding);
	helper.DoEnumerate(); // 列挙を行う
}
//---------------------------------------------------------------------------




