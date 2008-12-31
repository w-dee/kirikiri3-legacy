//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 吉里吉里 レンダリング済みフォント作成ツールアプリケーションメインルーチン
//---------------------------------------------------------------------------

#ifndef RISA_KRKR_FONT_APP_H
#define RISA_KRKR_FONT_APP_H

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * アプリケーションクラス
 */
class wxKrkrFontApp:public wxApp
{
public:
	bool OnInit();
	int OnExit();

private:
	wxLocale locale;

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


#endif
