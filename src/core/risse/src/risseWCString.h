//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief wchar_t ベースの文字列のC++クラス
//---------------------------------------------------------------------------

#ifndef risseWCStringH
#define risseWCStringH

/*! @note
	このクラスは非常にシンプルな wchar_t ベースの文字列クラスを実装する。
	パフォーマンスはあまりよくなく、また immutable な文字列クラスであるが、
	GC との相性はよいという特徴がある。

	tString にくらべ:
		+ シンプル(あんまり機能がない)
		+ tString が wchar_t が 16bit での環境でも 32bit 幅のコードポイントを
		  扱うのに対し、tWCString はあくまで wchar_t 型の文字列を扱う
	wxString にくらめ:
		+ GC aware である

	これらの特徴から、tString であつかう UTF-32 文字列よりも wchar_t 指向
	の文字列を半固定的に保持したい場合に使う。
*/

#include "risseCharUtils.h"
#include "risseTypes.h"
#include "risseAssert.h"
#include "risseString.h"
#include "risseGC.h"
#ifdef RISSE_SUPPORT_WX
	#include <wx/string.h>
#endif

namespace Risse
{
//---------------------------------------------------------------------------
//! @brief	wchar_tベースの文字列クラス
//---------------------------------------------------------------------------
class tWCString : public tCollectee
{
	wchar_t * Buffer; //!< バッファ

public:
	//! @brief		コンストラクタ
	tWCString() { Buffer = 0; }

	//! @brief		コピーコンストラクタ
	//! @param		ref		基準となる文字列
	explicit tWCString(const tWCString & ref) { Buffer = ref.Buffer; }

	//! @brief		コンストラクタ(tStringから)
	//! @param		ref		基準となる文字列
	explicit tWCString(const tString & ref);

#ifdef RISSE_SUPPORT_WX
	//! @brief		コンストラクタ(wxStringから)
	//! @param		ref		基準となる文字列
	explicit tWCString(const wxString & ref);
#endif

	//! @brief		wchar_t への変換
	operator const wchar_t * () const { return Buffer; }

	//! @brief		tString への変換
	operator tString () const { return tString(Buffer); }

#ifdef RISSE_SUPPORT_WX
	//! @brief		wxString への変換
	operator wxString () const { return wxString(Buffer); }
#endif
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risse


#endif
