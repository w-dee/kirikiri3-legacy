//!@ brief TJS3 テスト用プログラム

#include "prec.h"
#include "TJSEngine.h"
#include "tjsNative.h"

TJS_DEFINE_SOURCE_ID(1);





//---------------------------------------------------------------------------
//! @brief		TJS テスト関数
//---------------------------------------------------------------------------
class TestFunc : public tTJSNativeFunction
{
public:
    tjs_error Process(tTJSVariant *result, tjs_int numparams,
        tTJSVariant **param, iTJSDispatch2 *objthis)
    {
        if(numparams < 1) return TJS_E_BADPARAMCOUNT; // 引数が足りない

		wxPrintf(wxT("%s\n"), param[0]->AsWxString().c_str());

        return TJS_S_OK; // 正常に終わったことを示すため TJS_S_OK を返す
    }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		アプリケーションクラス
//---------------------------------------------------------------------------
class Application : public wxAppConsole
{
public:
	virtual bool OnInit();
	virtual int OnRun();
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// アプリケーションメインルーチン定義
//---------------------------------------------------------------------------
IMPLEMENT_APP_CONSOLE(Application)
wxLocale locale;
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		アプリケーションが開始するとき
//! @return		成功すれば真
//---------------------------------------------------------------------------
bool Application::OnInit()
{
	locale.Init(wxLANGUAGE_DEFAULT);
	locale.AddCatalogLookupPathPrefix(wxT("locales")); 
	locale.AddCatalogLookupPathPrefix(wxT("../locales")); 
	locale.AddCatalog(wxT("krkrrel"));
	return true;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		アプリケーションメインルーチン
//! @return		終了コード
//---------------------------------------------------------------------------
int Application::OnRun()
{
	try
	{
		iTJSDispatch2 * global = tTVPTJS3ScriptEngine::instance().GetGlobalNoAddRef();
			// グローバルオブジェクトを取得

		iTJSDispatch2 *func = new TestFunc(); // TestFunc のオブジェクトを作成
		tTJSVariant func_var(func); // tTJSVariant 型 func_var にオブジェクトを設定
		func->Release(); // func を Release

		TJS_THROW_IF_ERROR(
			global->PropSet(TJS_MEMBERENSURE, TJS_WS("test"), NULL, &func_var, NULL));
				// 登録

		tTVPTJS3ScriptEngine::instance().GetEngineNoAddRef()->EvalExpression(
			TJS_WS("test((string)(FileSystem.TmpFS)+(string)(FileSystem.XP4FS)+(string)(FileSystem.PathFS)+(string)(FileSystem.OSFS))"),
			NULL, NULL, NULL);
			// tTJS::EvalExpression を使って式を実行
	}
	catch(const eTJS &e)
	{
		wxFprintf(stderr, wxT("error : %s\n"), e.GetMessage().AsWxString().c_str());
	}

	return 0;
}
//---------------------------------------------------------------------------


