//!@ brief TJS3 テスト用プログラム

#include "prec.h"
#include "tjsNative.h"

TJS_DEFINE_SOURCE_ID(0);





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

		wxPrintf(wxT("%s\n"), ((ttstr)(*param[0])).AsWxString().c_str());

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
	tTJS *tjsengine = new tTJS();

	iTJSDispatch2 * global = tjsengine->GetGlobalNoAddRef();
		// グローバルオブジェクトを取得

	iTJSDispatch2 *func = new TestFunc(); // TestFunc のオブジェクトを作成
	tTJSVariant func_var(func); // tTJSVariant 型 func_var にオブジェクトを設定
	func->Release(); // func を Release

	TJS_THROW_IF_ERROR(
		global->PropSet(TJS_MEMBERENSURE, TJS_WS("test"), NULL, &func_var, NULL));
			// 登録

	tjsengine->EvalExpression(TJS_WS("test('ほげらもげ')"), NULL, NULL, NULL);
		// tTJS::EvalExpression を使って式を実行

	tjsengine->Release();
	return 0;
}
//---------------------------------------------------------------------------


