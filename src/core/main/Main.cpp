//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risa (吉里吉里3) メインルーチン
//---------------------------------------------------------------------------
#include "prec.h"
#include <wx/filename.h>
#include <wx/app.h>
#include "risa/packages/risa/event/Event.h"
#include "risa/packages/risa/event/IdleEvent.h"
#include "risa/packages/risa/event/TickCount.h"
#include "base/gc/RisaGC.h"
#include "base/script/RisseEngine.h"
#include "base/ui/console/Console.h"
#include "base/ui/editor/ScriptEditor.h"
#include "risa/packages/risa/log/Log.h"
#include "risa/packages/risa/fs/FSManager.h"
#include "risa/packages/risa/fs/osfs/OSFS.h"
#include "risa/packages/risa/fs/tmpfs/TmpFS.h"

namespace Risa {
RISSE_DEFINE_SOURCE_ID(17420,39507,42749,18842,4255,44341,64162,32476);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	GC を初期化するための構造体
//! @note	GC を初期化するためのstaticな構造体。この構造体の初期化がmain前に
//!			呼ばれることにより、tApplication の構築が正常に終了する
//!			ことを期待する。C++ においてstatic な構造体の初期化順序は不定なので、
//!			これよりも先に他の場所でクローバルな構造体があり、その中で GC の機能を
//!			使用した場合の定義は未定義である。基本的にはグローバルに置かれた初期化が
//!			必要な構造体は使用しないこと。
//---------------------------------------------------------------------------
struct tGCInitializer
{
	tGCInitializer() { GC_init(); }
} GCInitializer;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	アプリケーションクラス
//! @note	このクラスのインスタンスへのポインタは wxTheApp に登録されるため、
//!			GC が見つけ出すことが出来るはず。
//---------------------------------------------------------------------------
class tApplication : public wxApp/*, public tCollectee*/
{
public:
	bool OnInit();
	int OnExit();
	bool ProcessIdle();

private:
	void OnActivate(wxActivateEvent & event);
	void OnActivateApp(wxActivateEvent & event);

	wxLocale locale;

	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// アプリケーションメインルーチン定義
//---------------------------------------------------------------------------
} // namespace Risa
// リンカが main 関数を探してくれるように名前空間外に出す
IMPLEMENT_APP(Risa::tApplication)
namespace Risa {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// アプリケーションイベントテーブル
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tApplication, wxApp)
	EVT_ACTIVATE(			tApplication::OnActivate)
	EVT_ACTIVATE_APP(		tApplication::OnActivateApp)
END_EVENT_TABLE()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		アプリケーションが開始するとき
//---------------------------------------------------------------------------
bool tApplication::OnInit()
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

	// 引数を取得 (実行するスクリプト名が指定されているか？)
	// (暫定実装)
	wxFileName script_filename;
	bool script_filename_set = false;
	bool assertion_enabled = false;
	for(int i = 1; i < argc; i++)
	{
		// 先頭が - で始まっていないオプションを探し、それを
		// 実行するスクリプト名であると見なす
		if(argv[i][0] != wxT('-'))
		{
			script_filename = wxFileName(argv[i]);
			script_filename_set = true;
		}
		else
		{
			// 先頭が '-' で始まっている → オプション
			if(argv[i][1] == wxT('a'))
			{
				// -a オプション : Risse の assert 文を有効にする
				assertion_enabled = true;
			}
		}
	}

	// すべてのシングルトンインスタンスの作成や残りの設定を行う
	try
	{
		// 先だって初期化しておきたい物
		tRisseScriptEngine::ensure();
		tRisseScriptEngine::instance()->GetScriptEngine()->SetAssertionEnabled(assertion_enabled);

		try
		{
			tCollectorThread::ensure();
			tWxLogProxy::ensure();

			// 残り全てのシングルトンインスタンスを初期化
			singleton_manager::init_all();

			// ファイルシステム関連のパッケージのうち、必要な物を初期化しておく
			tRisseScriptEngine::instance()->GetScriptEngine()->GetPackageGlobal(
				tSS<'r','i','s','a','.','f','s'>());
			tRisseScriptEngine::instance()->GetScriptEngine()->GetPackageGlobal(
				tSS<'r','i','s','a','.','f','s','.','t','m','p','f','s'>());
			tRisseScriptEngine::instance()->GetScriptEngine()->GetPackageGlobal(
				tSS<'r','i','s','a','.','f','s','.','o','s','f','s'>());

			// / に TmpFS をマウント
			tFileSystemManager::instance()->Mount(tSS<'/'>(),
				tPackageInitializerRegisterer<tRisaTmpfsPackageInitializer>::instance()->GetInitializer()->
					TmpFSClass->Invoke(ss_new).
						ExpectAndGetObjectInterafce<tFileSystemInstance>(
							tPackageInitializerRegisterer<tRisaTmpfsPackageInitializer>::instance()->GetInitializer()->TmpFSClass
						)
			);

			if(script_filename_set)
			{
				wxString script_dir = script_filename.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);

				// /boot に 引数で与えられたファイル名が存在するディレクトリをマウントする
				tFileSystemManager::instance()->Mount(tSS<'/','b','o','o','t'>(),
					tPackageInitializerRegisterer<tRisaOsfsPackageInitializer>::instance()->GetInitializer()->
						OSFSClass->Invoke(ss_new, tString(script_dir), true).
							ExpectAndGetObjectInterafce<tFileSystemInstance>(
								tPackageInitializerRegisterer<tRisaOsfsPackageInitializer>::instance()->GetInitializer()->OSFSClass
							)
				);
			}

			if(!script_filename_set)
			{
				//---- ↓↓テストコード↓↓ ----
				// コンソールをメインウィンドウとして表示
				tConsoleFrame *console = new tConsoleFrame();
				console->Show(true);

				tScriptEditorFrame *editor = new tScriptEditorFrame();
				editor->Show(true);

				//---- ↑↑テストコード↑↑ ----
			}
			else
			{
				// スクリプトを実行する
				tRisseScriptEngine::instance()->EvaluateFile(
					tSS<'/','b','o','o','t','/'>().operator const tString &() +
						tString(script_filename.GetFullName()));

				// TODO: ウィンドウが何も表示されないならば終了するように
				return false; // すぐにアプリケーションを終了させる
			}

		}
		catch(const tTemporaryException * te)
		{
			// tTemporaryException は変換しないと tVariant * にならない
			te->ThrowConverted(tRisseScriptEngine::instance()->GetScriptEngine());
		}
	}
	catch(const tVariant * e)
	{
		wxFprintf(stderr, wxT("tVariant exception caught : %s (%s)\n"),
			e->operator tString().AsWxString().c_str(),
			e->GetClassName().AsWxString().c_str());
		return false;
	}
	catch(...)
	{
		// something ??
		fprintf(stderr, "something caught\n");
		return false;
	}



	return true;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		アプリケーションが終了するとき
//---------------------------------------------------------------------------
int tApplication::OnExit()
{
	printf("tApplication::OnExit entered\n");

	// スクリプトエンジンをシャットダウンする
//	tRisseScriptEngine::instance()->Shutdown();

	// すべてのシングルトンインスタンスへの参照を切る
	singleton_manager::disconnect_all();

	printf("all singletons should be destroyed within this time ...\n");

	// まだシステムに残っているシングルトンインスタンスを表示する
	singleton_manager::report_alive_objects();

	printf("tApplication::OnExit ended\n");

	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Idleイベントを処理するとき
//---------------------------------------------------------------------------
bool tApplication::ProcessIdle()
{
	bool cont = false;

	if(	tTickCount::alive() && tEventSystem::alive() &&
		tMainEventQueue::alive() && tIdleEventManager::alive())
	{
		// この回で呼び出すハンドラに渡すtickを得る
		risse_uint64 tick = tTickCount::instance()->Get();

		// 各サブシステムを呼び出す

		// イベントの配信
		cont = tMainEventQueue::instance()->GetEventQueueInstance()->ProcessEvents(tick) || cont;

		// アイドルイベントの配信
		cont = tIdleEventManager::instance()->Deliver(tick) || cont;
	}

	// デストラクタの呼び出し
	tMainThreadDestructorQueue::instance()->CallDestructors();

	cont = wxApp::ProcessIdle() || cont;
	return cont;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ウィンドウなどがアクティブ化/非アクティブ化したとき
//---------------------------------------------------------------------------
void tApplication::OnActivate(wxActivateEvent & event)
{
/*
	TODO: handle this
	if(!event.GetActive())
	{
		if(tCompactEventManager::pointer r = tCompactEventManager::instance())
			r->OnDeactivate();
	}
*/
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		アプリケーションがアクティブ化/非アクティブ化したとき
//---------------------------------------------------------------------------
void tApplication::OnActivateApp(wxActivateEvent & event)
{
/*
	TODO: handle this
	if(!event.GetActive())
	{
		if(tCompactEventManager::pointer r = tCompactEventManager::instance())
			r->OnDeactivateApp();
	}
*/
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa






