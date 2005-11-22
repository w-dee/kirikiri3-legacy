#include <wx/cmdline.h>
#include "prec.h"
#include "ProgressCallback.h"
#include "WriteXP4.h"
#include "FileList.h"
#include "XP4Archive.h"
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

protected:
	static void ListArchiveItems(const std::vector<tTVPXP4WriterInputFile> & files);
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
//! @brief		コマンドラインオプションの定義
//---------------------------------------------------------------------------
static const wxCmdLineEntryDesc TVPCmdLineDesc[] =
{
	{ wxCMD_LINE_SWITCH, wxT("h"), wxT("help"),
		_("show help message"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
	{ wxCMD_LINE_OPTION, wxT("s"), wxT("split"),
		_("split archive in given size (MB)"), wxCMD_LINE_VAL_NUMBER, 0 },
	{ wxCMD_LINE_OPTION, wxT("r"), wxT("revision"),
		_("patch revision"), wxCMD_LINE_VAL_STRING, 0 },
	{ wxCMD_LINE_OPTION, wxT("c"), wxT("compress"),
		_("regex pattern for files to be compressed"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE },
	{ wxCMD_LINE_OPTION, wxT("c"), wxT("exclude"),
		_("regex pattern for files to be excluded"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE },
	{ wxCMD_LINE_SWITCH, wxT("p"), wxT("show-progress"),
		_("show progress"), (wxCmdLineParamType)0, 0 },
	{ wxCMD_LINE_SWITCH, wxT("d"), wxT("dry-run"),
		_("do not make output"), (wxCmdLineParamType)0, 0 },
	{ wxCMD_LINE_SWITCH, wxT("n"), wxT("new"),
		_("force making a new archive set (do not make a patch)"), (wxCmdLineParamType)0, 0 },
	{ wxCMD_LINE_SWITCH, wxT("l"), wxT("list"),
		_("list files in archive set (default if no dir given)"), (wxCmdLineParamType)0, 0 },

	{ wxCMD_LINE_PARAM,	 NULL, NULL, _("archive-file"),
		  wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
	{ wxCMD_LINE_PARAM,	 NULL, NULL, _("dir"),
		  wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

	{ wxCMD_LINE_NONE, NULL, NULL, NULL, (wxCmdLineParamType)0, 0 }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		アプリケーションメインルーチン
//! @returns	終了コード
//---------------------------------------------------------------------------
int wxKrkrReleaserConsoleApp::OnRun()
{
	try
	{
		wxCmdLineParser parser(TVPCmdLineDesc, argc, argv);
		parser.SetLogo(_("Kirikiri3 Releaser (utility for XP4 archive file system)"));
		switch(parser.Parse())
		{
		case -1: // help
			return 0;
		case 0: // success
			break;
		default: // syntax error
			return 3;
		}

		tTVPProgressCallback callback;

		// アーカイブからファイルを列挙
		std::map<wxString, tTVPXP4MetadataReaderStorageItem> in_archive_items_map;
		wxString archive_base_name;
		archive_base_name = parser.GetParam();
		wxPrintf(wxT("archive : %s\n"), archive_base_name.c_str());
		if(wxFileName(archive_base_name).FileExists())
		{
			TVPReadXP4Metadata(&callback, archive_base_name, in_archive_items_map);
			std::vector<tTVPXP4WriterInputFile> list;
			TVPXP4MetadataReaderStorageItemToXP4WriterInputFile(in_archive_items_map, list);
			ListArchiveItems(list);
		}

/*
		wxArrayString excludepat;
		excludepat.Add(  wxT("/\\.")    );
		wxArrayString compresspat;
		compresspat.Add( wxT("\\.txt$") );
		compresspat.Add( wxT("\\.ks$")  );


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
*/
	}
	catch(const wxString & e)
	{
		wxLogError(e);
		return 3;
	}
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		アーカイブ内のファイルをリスト表示する
//! @param		files: ファイルリスト
//---------------------------------------------------------------------------
void wxKrkrReleaserConsoleApp::ListArchiveItems(
	const std::vector<tTVPXP4WriterInputFile> & files)
{
	// リストをソート
	std::vector<tTVPXP4WriterInputFile> sorted_files(files);
	std::sort(sorted_files.begin(), sorted_files.end());

	// 順番に表示
	for(std::vector<tTVPXP4WriterInputFile>::iterator i = sorted_files.begin();
		i != sorted_files.end(); i++)
	{
		// 各行の先頭には
		// A 追加
		// D 削除
		// M 変更
		//   そのまま
		// のマークが付く
		switch(i->GetFlags() & TVP_XP4_FILE_STATE_MASK)
		{
		case TVP_XP4_FILE_ADDED:
			wxPrintf(wxT("A ")); break;
		case TVP_XP4_FILE_DELETED:
			wxPrintf(wxT("D ")); break;
		case TVP_XP4_FILE_MODIFIED:
			wxPrintf(wxT("M ")); break;
		default:
			wxPrintf(wxT("  ")); break;
		}

		wxPrintf(wxT("%s\n"), i->GetInArchiveName().c_str());
	}
}
//---------------------------------------------------------------------------
