//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 吉里吉里 レンダリング済みフォント作成ツールアプリケーションメインルーチン
//---------------------------------------------------------------------------
#include "prec.h"
#include "KrkrFontApp.h"
#include "MainDialog.h"

//---------------------------------------------------------------------------
// アプリケーションメインルーチン定義
//---------------------------------------------------------------------------
IMPLEMENT_APP(wxKrkrFontApp)
wxLocale locale;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		アプリケーションが開始するとき
//---------------------------------------------------------------------------
bool wxKrkrFontApp::OnInit()
{
	locale.Init(wxLANGUAGE_DEFAULT);
	locale.AddCatalogLookupPathPrefix(wxT("locales")); 
	locale.AddCatalogLookupPathPrefix(wxT("../locales")); 
	locale.AddCatalog(wxT("krkrfont"));

	wxXmlResource::Get()->InitAllHandlers();
	InitXmlResource();
	wxMainDialog *dialog = new wxMainDialog();
	dialog->Show(true);

	return true;
}
//---------------------------------------------------------------------------
 
//---------------------------------------------------------------------------
//! @brief		アプリケーションが終了するとき
//---------------------------------------------------------------------------
int wxKrkrFontApp::OnExit()
{
	return 0;
}
//---------------------------------------------------------------------------


