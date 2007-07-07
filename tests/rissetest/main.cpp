// openal を使って音声をストリーミング再生するテスト

#include "prec.h"
#include "risseString.h"
#include "risseVariant.h"
#include "gc_cpp.h"
#include <wx/file.h>
#include <wx/filename.h>



#include "risseScriptEngine.h"
#include "risseClass.h"
#include "risseStaticStrings.h"
#include "risseNativeBinder.h"
#include "risseStringTemplate.h"
#include "risseObjectClass.h"
#include "risseExceptionClass.h"

RISSE_DEFINE_SOURCE_ID(1760,7877,28237,16679,32159,45258,11038,1907);


using namespace Risse;






//---------------------------------------------------------------------------
//! @brief		暫定 Script クラス
//---------------------------------------------------------------------------
class tRisseScriptClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisseScriptClass(tRisseScriptEngine * engine) :
		tRisseClassBase(tRisseSS<'S','c','r','i','p','t'>(), engine->ObjectClass)
	{
		RegisterMembers();
	}

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers()
	{
		RisseBindFunction(this, ss_construct, &tRisseScriptClass::construct);
		RisseBindFunction(this, ss_initialize, &tRisseScriptClass::initialize);
		RisseBindFunction(this, tRisseSS<'r','e','q','u','i','r','e'>(), &tRisseScriptClass::require);
		RisseBindFunction(this, tRisseSS<'p','r','i','n','t'>(), &tRisseScriptClass::print);
	}

	static void construct()
	{
	}

	static void initialize()
	{
	}

	static void require(const tRisseString & name, tRisseScriptEngine * engine)
	{
		// name を取ってきて eval する
		wxFile file;
		if(file.Open(name.AsWxString()))
		{
			// 内容を読み込む
			size_t length = file.Length();
			char *buf = new (PointerFreeGC) char [length + 1];
			file.Read(buf, length);
			buf[length] = 0;

			// 内容を評価する
			fflush(stderr);
			fflush(stdout);
			engine->Evaluate((tRisseString)(buf), name, 0, NULL);
			fflush(stderr);
			fflush(stdout);
		}
		else
		{
			// TODO: IOException
		}
	}

	static void print(const tRisseMethodArgument & args)
	{
		fflush(stderr);
		fflush(stdout);
		for(risse_size i = 0; i < args.GetArgumentCount(); i++)
			RisseFPrint(stdout, args[i].operator tRisseString().c_str());
		fflush(stderr);
		fflush(stdout);
	}

public:
};
//---------------------------------------------------------------------------









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

		// Script クラスを追加する
		(new tRisseScriptClass(&engine))->
				RegisterClassInstance(engine.GetGlobalObject(),
					tRisseSS<'S','c','r','i','p','t'>());

		// 入力ファイルを開く
		wxFile file;
		if(file.Open(argv[1]))
		{
			// スクリプトのある場所をカレントディレクトリに指定する
			wxFileName filename(argv[1]);
			wxFileName::SetCwd(filename.GetPath());

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

