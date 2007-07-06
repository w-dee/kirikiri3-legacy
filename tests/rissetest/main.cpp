// openal を使って音声をストリーミング再生するテスト

#include "prec.h"
#include "risseString.h"
#include "risseVariant.h"
#include "gc_cpp.h"
#include <wx/file.h>



#include "risseScriptEngine.h"
#include "risseExceptionClass.h"

RISSE_DEFINE_SOURCE_ID(1760,7877,28237,16679,32159,45258,11038,1907);










using namespace Risse;

//---------------------------------------------------------------------------
//! @brief		警告情報の出力先インターフェース
//---------------------------------------------------------------------------
class tRisseWarningOutput : public tRisseLineOutputInterface
{
	void Output(const tRisseString & info)
	{
		fflush(stderr);
		fflush(stdout);
		RisseFPrint(stderr, RISSE_WS("warning: "));
		RisseFPrint(stderr, info.c_str());
		RisseFPrint(stderr, RISSE_WS("\n"));
		fflush(stderr);
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
	return true;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		アプリケーションメインルーチン
//! @return		終了コード
//---------------------------------------------------------------------------
int Application::OnRun()
{
	if(argc < 2)
	{
		fprintf(stderr, "Specify a file name to read.\n");
		return 0;
	}

	// Risse スクリプトエンジンを作成する
	try
	{
		tRisseScriptEngine engine;

		engine.SetWarningOutput(new tRisseWarningOutput());

		// 入力ファイルを開く
		wxFile file;
		if(file.Open(argv[1]))
		{
			// 内容を読み込む
			size_t length = file.Length();
			char *buf = new (PointerFreeGC) char [length + 1];
			file.Read(buf, length);
			buf[length] = 0;

			// 内容を評価する
			tRisseVariant result;
			engine.Evaluate((tRisseString)(buf), argv[1], 0, &result);
			RisseFPrint(stderr,(RISSE_WS("========== Result ==========\n")));
			fflush(stderr);
			fflush(stdout);
			RisseFPrint(stdout, result.AsHumanReadable().c_str());
		}

	}
	catch(const tRisseTemporaryException * te)
	{
		te->Dump();
	}
	catch(const tRisseVariant * e)
	{
		fflush(stderr);
		fflush(stdout);
		wxFprintf(stdout, wxT("exception: %s\n"), e->operator tRisseString().AsWxString().c_str());
	}
	return 0;
}
//---------------------------------------------------------------------------

