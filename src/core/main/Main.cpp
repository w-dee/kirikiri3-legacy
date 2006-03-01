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
#include <wx/app.h>
#include "base/event/Event.h"
#include "base/script/RisseEngine.h"
#include "base/ui/console/Console.h"
#include "base/ui/editor/ScriptEditor.h"
#include "base/log/Log.h"

RISSE_DEFINE_SOURCE_ID(17420,39507,42749,18842,4255,44341,64162,32476);


//---------------------------------------------------------------------------
//! @brief アプリケーションクラス
//---------------------------------------------------------------------------
class tRisaApplication : public wxApp
{
public:
	bool OnInit();
	int OnExit();
	bool ProcessIdle();
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

	// UpdateUIEvent と IdleEvent を指定されたウィンドウだけに送るように
	wxIdleEvent::SetMode(wxIDLE_PROCESS_SPECIFIED);
	wxUpdateUIEvent::SetMode(wxUPDATE_UI_PROCESS_SPECIFIED);

	// すべてのシングルトンインスタンスを初期化する
	try
	{
		// 先だって初期化しておきたい物
		tRisaWxLogProxy::ensure();

		// 残り全てのシングルトンインスタンスを初期化
		tRisaSingletonManager::InitAll();
	}
	catch(...)
	{
		// something ??
		fprintf(stderr, "something caught\n");
		return false;
	}

	//---- ↓↓テストコード↓↓ ----
	// ファイルシステムのルートにカレントディレクトリをマウント
	tRisaRisseScriptEngine::instance()->GetEngineNoAddRef()->EvalExpression(
		RISSE_WS("FileSystem.mount('/', new FileSystem.OSFS('.'))"),
		NULL, NULL, NULL);

	// コンソールをメインウィンドウとして表示
	tRisaConsoleFrame *console = new tRisaConsoleFrame();
//	wxFrame *frame = new wxFrame(NULL, wxID_ANY, wxT("hoge"));
	console->Show(true);

	tRisaScriptEditorFrame *editor = new tRisaScriptEditorFrame();
	editor->Show(true);

	//---- ↑↑テストコード↑↑ ----

	return true;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		アプリケーションが終了するとき
//---------------------------------------------------------------------------
int tRisaApplication::OnExit()
{
	printf("tRisaApplication::OnExit entered\n");

	// スクリプトエンジンをシャットダウンする
	tRisaRisseScriptEngine::instance()->Shutdown();

	// すべてのシングルトンインスタンスへの参照を切る
	tRisaSingletonManager::DisconnectAll();

	printf("all singletons should be destroyed within this time ...\n");

	// まだシステムに残っているシングルトンインスタンスを表示する
	tRisaSingletonManager::ReportAliveObjects();

	printf("tRisaApplication::OnExit ended\n");

	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Idleイベントを配信するとき
//---------------------------------------------------------------------------
bool tRisaApplication::ProcessIdle()
{
	bool cont = false;
	if(tRisaEventSystem::alive()) tRisaEventSystem::instance()->ProcessEvents();
	cont = wxApp::ProcessIdle() || cont;
	return cont;
}
//---------------------------------------------------------------------------














namespace boost
{

//---------------------------------------------------------------------------
//! @brief		boost の assertion failure を捕捉する
//---------------------------------------------------------------------------
void assertion_failed(char const * expr, char const * function, char const * file, long line)
{
	// user defined
	fprintf(stderr, "boost assertion failure on expression '%s' at function '%s' file %s line %ld\n",
		expr, function, file, line);
	abort();
}
//---------------------------------------------------------------------------


} // namespace boost

