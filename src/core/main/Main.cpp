//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risa (吉里吉里3) メインルーチン
//---------------------------------------------------------------------------
#include "prec.h"
#include <wx/filename.h>
#include "RisseEngine.h"
#include "LogViewer.h"


RISSE_DEFINE_SOURCE_ID(17420,39507,42749,18842,4255,44341,64162,32476);


//---------------------------------------------------------------------------
//! @brief アプリケーションクラス
//---------------------------------------------------------------------------
class tRisaApplication:public wxApp
{
public:
	bool OnInit();
	int OnExit();
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// アプリケーションメインルーチン定義
//---------------------------------------------------------------------------
IMPLEMENT_APP(tRisaApplication)
wxLocale locale;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		アプリケーションが開始するとき
//---------------------------------------------------------------------------
bool tRisaApplication::OnInit()
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
	locale.AddCatalog(wxT("k3"));
	locale.AddCatalog(wxT("wxstd"));

	// すべてのシングルトンインスタンスを初期化する
	tRisaSingletonManager::InitAll(); // 全てのシングルトンインスタンスを初期化


	//---- ↓↓テストコード↓↓ ----
	// ファイルシステムのルートにカレントディレクトリをマウント
	tRisaRisseScriptEngine::instance()->GetEngineNoAddRef()->EvalExpression(
		RISSE_WS("FileSystem.mount('/', new FileSystem.OSFS('.'))"),
		NULL, NULL, NULL);

	// コンソールをメインウィンドウとして表示
	tRisaLogViewerFrame *frame = new tRisaLogViewerFrame();
	frame->Show();

	//---- ↑↑テストコード↑↑ ----

	return true;
}
//---------------------------------------------------------------------------
 
//---------------------------------------------------------------------------
//! @brief		アプリケーションが終了するとき
//---------------------------------------------------------------------------
int tRisaApplication::OnExit()
{
	return 0;
}
//---------------------------------------------------------------------------


