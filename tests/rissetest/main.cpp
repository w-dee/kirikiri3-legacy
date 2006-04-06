// openal を使って音声をストリーミング再生するテスト

#include "prec.h"
#include "risseCxxString.h"
#include "risseVariant.h"


RISSE_DEFINE_SOURCE_ID(1760,7877,28237,16679,32159,45258,11038,1907);










using namespace Risse;


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
	return true;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		アプリケーションメインルーチン
//! @return		終了コード
//---------------------------------------------------------------------------
int Application::OnRun()
{
	tRisseStringBlock str1("hoge");
	tRisseStringBlock str2(str1);
	str1 = "hage";
	tRisseStringBlock str3(str1, 1, 2);
//	str1 = "moge";

	for(int i = 0; i < 10; i++)
		str1 += str2;

	wxPrintf(wxT("str1 : %s\n"), str1.AsWxString().c_str());
	wxPrintf(wxT("str2 : %s\n"), str2.AsWxString().c_str());
	wxPrintf(wxT("str3 : %s\n"), str3.AsWxString().c_str());

	tRisseVariant v;
	v.prtsizes();
	return 0;
}
//---------------------------------------------------------------------------
