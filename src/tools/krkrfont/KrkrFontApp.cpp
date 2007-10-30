//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 吉里吉里 レンダリング済みフォント作成ツールアプリケーションメインルーチン
//---------------------------------------------------------------------------
#include "prec.h"
#include "KrkrFontApp.h"
#include "MainDialog.h"

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// アプリケーションメインルーチン定義
//---------------------------------------------------------------------------
} // namespace Risa
// リンカが main 関数を探してくれるように名前空間外に出す
IMPLEMENT_APP(Risa::wxKrkrFontApp)
namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		アプリケーションが開始するとき
//---------------------------------------------------------------------------
bool wxKrkrFontApp::OnInit()
{
	// アプリケーションのディレクトリを得て、その下にある locale か、
	// <アプリケーションのあるディレクトリ>../share/locale をメッセージカタログ
	// の検索パスに指定する
	// wxApp::argv[0] がアプリケーションのファイル名を表していると仮定する
	wxFileName appfilename(argv[0]);
	wxString appdir = appfilename.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);

	locale.Init(wxLANGUAGE_DEFAULT);
	locale.AddCatalogLookupPathPrefix(appdir + wxT("locale")); 
	locale.AddCatalogLookupPathPrefix(appdir + wxT("../share/locale")); 

	// メッセージカタログを追加する
	locale.AddCatalog(wxT("krkrfont"));
	locale.AddCatalog(wxT("wxstd"));

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


//---------------------------------------------------------------------------
} // namespace Risa


