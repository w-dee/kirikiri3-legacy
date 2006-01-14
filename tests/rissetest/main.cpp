//!@ brief Risse テスト用プログラム

#include "prec.h"
#include "risseNative.h"

RISSE_DEFINE_SOURCE_ID(1);





//---------------------------------------------------------------------------
//! @brief		Risse テスト関数
//---------------------------------------------------------------------------
class TestFunc : public tRisseNativeFunction
{
public:
    risse_error Process(tRisseVariant *result, risse_int numparams,
        tRisseVariant **param, iRisseDispatch2 *objthis)
    {
        if(numparams < 1) return RISSE_E_BADPARAMCOUNT; // 引数が足りない

		wxPrintf(wxT("%s\n"), param[0]->AsWxString().c_str());

        return RISSE_S_OK; // 正常に終わったことを示すため RISSE_S_OK を返す
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
		tRisse *risseengine = new tRisse();

		iRisseDispatch2 * global = risseengine->GetGlobalNoAddRef();
			// グローバルオブジェクトを取得

		iRisseDispatch2 *func = new TestFunc(); // TestFunc のオブジェクトを作成
		tRisseVariant func_var(func); // tRisseVariant 型 func_var にオブジェクトを設定
		func->Release(); // func を Release

		RISSE_THROW_IF_ERROR(
			global->PropSet(RISSE_MEMBERENSURE, RISSE_WS("test"), NULL, &func_var, NULL));
				// 登録

		risseengine->EvalExpression(RISSE_WS("test('中３マキロン地獄！')"), NULL, NULL, NULL);
			// tRisse::EvalExpression を使って式を実行

		risseengine->Release();
	}
	catch(const eRisse & err)
	{
		wxFprintf(stderr, wxT("Error: %s\n"), err.GetMessage().AsWxString().c_str());
	}
	return 0;
}
//---------------------------------------------------------------------------


