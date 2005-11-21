#include "prec.h"
#include "ProgressCallback.h"
#include "WriteXP4.h"
#include "FileList.h"
#include "ReadXP4Meta.h"

//---------------------------------------------------------------------------
//! @brief		アプリケーションクラス
//---------------------------------------------------------------------------
class wxKrkrReleaserConsoleApp : public wxAppConsole
{
public:
    // don't use builtin cmd line parsing:
    virtual bool OnInit();
    virtual int OnRun();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// アプリケーションメインルーチン定義
//---------------------------------------------------------------------------
IMPLEMENT_APP_CONSOLE(wxKrkrReleaserConsoleApp)
wxLocale locale;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		進捗を表示するクラス
//---------------------------------------------------------------------------
class tTVPProgressCallback : public iTVPProgressCallback
{
public:
	void OnProgress(int percent); // パーセント単位での達成率
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// 進捗を表示する
//---------------------------------------------------------------------------
void tTVPProgressCallback::OnProgress(int percent)
{
	wxFprintf(stderr, _("%d percent done\r"), percent);
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		アプリケーションが開始するとき
//! @returns	成功すれば真
//---------------------------------------------------------------------------
bool wxKrkrReleaserConsoleApp::OnInit()
{
	setlocale(LC_ALL, "");
	locale.Init(wxLANGUAGE_DEFAULT);
	locale.AddCatalogLookupPathPrefix(wxT("locales")); 
	locale.AddCatalogLookupPathPrefix(wxT("../locales")); 
	locale.AddCatalog(wxT("krkrrel"));
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		アプリケーションメインルーチン
//! @returns	終了コード
//---------------------------------------------------------------------------
int wxKrkrReleaserConsoleApp::OnRun()
{
	try
	{
		wxArrayString excludepat;
		excludepat.Add(  wxT("/\\.")    );
		wxArrayString compresspat;
		compresspat.Add( wxT("\\.txt$") );
		compresspat.Add( wxT("\\.ks$")  );

		tTVPProgressCallback callback;

		tTVPProgressCallbackAggregator agg(&callback, 0, 4);

		wxString targetdir(wxT("C:\\bin"));

		std::vector<tTVPXP4WriterInputFile> filelist;
		TVPGetFileListAt(&agg, targetdir, filelist);

		agg.SetRange(4, 8);

		TVPClassifyFiles(&agg, excludepat, compresspat, filelist);

		agg.SetRange(8, 100);

		{
			tTVPXP4Writer writer(
				&agg,
				wxT("c:\\eclipse\\workspace\\krkrrel\\out"),
				0,
				filelist);

			writer.MakeArchive();
		}

		{
			tTVPXP4MetadataReaderArchive archive(
				wxT("c:\\eclipse\\workspace\\krkrrel\\out.xp4"));

			const std::vector<tTVPXP4MetadataReaderStorageItem> & vec =
				archive.GetItemVector();

			for(std::vector<tTVPXP4MetadataReaderStorageItem>::const_iterator
					i = vec.begin(); i != vec.end(); i++)
			{
				wxString datestring = i->GetTime().Format();
				wxPrintf(wxT("%s "), i->GetName().c_str());
				wxPrintf(wxT("%d "), (int)(i->GetFlags()));
				wxPrintf(wxT("%s\n"), datestring.c_str());
			}
		}
	}
	catch(const wxString & e)
	{
		wxLogError(e);
		return 3;
	}
	return 0;
}
//---------------------------------------------------------------------------
