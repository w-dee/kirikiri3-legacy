#include "prec.h"
#include "signal.h"
#include "ProgressCallback.h"
#include "WriteXP4.h"
#include "FileList.h"
#include "XP4Archive.h"
#include "ReadXP4Meta.h"



//---------------------------------------------------------------------------
//! @brief		コマンドラインオプションの定義を返す
//---------------------------------------------------------------------------
static wxCmdLineEntryDesc * TVPGetCmdLineEntryDesc()
{
	static wxCmdLineEntryDesc cmdlinedesc[] =
	{
		{ wxCMD_LINE_SWITCH, wxT("h"), wxT("help"),
			_("show help message"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
		{ wxCMD_LINE_SWITCH, wxT("u"), wxT("use-archive-target"),
			_("get target dir from the archive set"), (wxCmdLineParamType)0, 0 },
		{ wxCMD_LINE_OPTION, wxT("s"), wxT("split"),
			_("split archive in given size (MB)"), wxCMD_LINE_VAL_NUMBER, 0 },
		{ wxCMD_LINE_OPTION, wxT("r"), wxT("revision"),
			_("patch revision"), wxCMD_LINE_VAL_STRING, 0 },
		{ wxCMD_LINE_SWITCH, wxT("p"), wxT("show-progress"),
			_("show progress"), (wxCmdLineParamType)0, 0 },
		{ wxCMD_LINE_SWITCH, wxT("d"), wxT("dry-run"),
			_("do not make output"), (wxCmdLineParamType)0, 0 },
		{ wxCMD_LINE_OPTION, wxT("c"), wxT("class-list"),
			_("regex list file for file classification"), wxCMD_LINE_VAL_STRING, 0 },
		{ wxCMD_LINE_SWITCH, wxT("n"), wxT("new"),
			_("force making a new archive set (not a patch)"), (wxCmdLineParamType)0, 0 },
		{ wxCMD_LINE_SWITCH, wxT("g"), wxT("show-generation"),
			_("show archive patch generation history"), (wxCmdLineParamType)0, 0 },
		{ wxCMD_LINE_SWITCH, NULL, wxT("show-default-class"),
			_("show default classification regex list"), (wxCmdLineParamType)0, 0 },

		{ wxCMD_LINE_PARAM,	 NULL, NULL, _("archive-file"),
			  wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
		{ wxCMD_LINE_PARAM,	 NULL, NULL, _("dir"),
			  wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

		{ wxCMD_LINE_NONE, NULL, NULL, NULL, (wxCmdLineParamType)0, 0 }
	};
	return cmdlinedesc;
}
//---------------------------------------------------------------------------


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
	static void LoadClassList(const wxString & filename, wxArrayString &pattern);
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
//! @brief		シグナルハンドラ
//---------------------------------------------------------------------------
static bool TVPInterrupted = false;
static void TVPSigInt(int sig)
{
	TVPInterrupted = true;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief		進捗を表示するクラス
//---------------------------------------------------------------------------
class tTVPProgressCallback : public iTVPProgressCallback
{
	bool Show;
	int PrevPercent;
public:
	tTVPProgressCallback(bool show = true) : Show(show), PrevPercent(-1) {;}
	void OnProgress(int percent); // パーセント単位での達成率
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// 進捗を表示する
//---------------------------------------------------------------------------
void tTVPProgressCallback::OnProgress(int percent)
{
	if(TVPInterrupted) throw wxString(_("operation was interrupted"));
	if(Show)
	{
		if(PrevPercent != percent)
		{
			wxFprintf(stderr, _("%2d percent done"), percent);
			wxFprintf(stderr, wxT("\r"));
			PrevPercent = percent;
		}
	}
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		アプリケーションが開始するとき
//! @returns	成功すれば真
//---------------------------------------------------------------------------
bool wxKrkrReleaserConsoleApp::OnInit()
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
//! @returns	終了コード
//---------------------------------------------------------------------------
int wxKrkrReleaserConsoleApp::OnRun()
{
	try
	{
		// シグナルハンドラの設定
		signal(SIGINT, TVPSigInt);

		// コマンドラインパーサーオブジェクトを作成
		wxCmdLineParser parser(TVPGetCmdLineEntryDesc(), argc, argv);

		// --show-default-class を検索
		for(int i = 1; i < argc; i++)
		{
			if(!wxStrcmp(argv[i], wxT("--show-default-class")))
			{
				wxArrayString patterns;
				TVPXP4GetDefaultClassList(patterns);
				for(size_t i = 0; i < patterns.GetCount(); i++)
					wxPrintf(wxT("%s\n"), patterns[i].c_str());
				return 0;
			}
		}

		// コマンドライン引数を解析
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

		// フラグなど
		bool show_progress = parser.Found(wxT("p"));
		tTVPProgressCallback callback(show_progress);
		tTVPProgressCallbackAggregator agg(&callback, 0, 20);

		std::vector<tTVPXP4WriterInputFile> filelist;
		std::map<wxString, tTVPXP4MetadataReaderStorageItem> in_archive_items_map;
		wxString archive_base_name;
		archive_base_name = parser.GetParam();

		bool make_new_archive = false; // 新しいアーカイブファイルを作成するかどうか
		bool target_dir_specified = parser.GetParamCount() >= 2;
		wxString target_dir;
		if(target_dir_specified) target_dir = parser.GetParam(1);

		// -n, --new をチェック
		if(parser.Found(wxT("n")))
		{
			// 強制的に新規アーカイブファイルを作成する
			if(wxFileName(archive_base_name).FileExists())
			{
				// 既存のアーカイブファイルを削除する
				TVPDeleteArchiveSet(archive_base_name);
			}
			make_new_archive = true;
		}

		// コマンドラインパラメータで指定されたアーカイブファイルが存在するかどうか
		if(wxFileName(archive_base_name).FileExists())
		{
			// 存在する
		}
		else
		{
			// アーカイブファイルは存在しない
			// アーカイブファイルの残渣が残ってないことを確実にするために
			// 関連するファイルをすべて削除する
			TVPDeleteArchiveSet(archive_base_name);
			make_new_archive = true;
		}

		// 既存のアーカイブファイルの内容を読み込む
		if(!make_new_archive)
		{
			if(!target_dir_specified && !parser.Found(wxT("u")))
			{
				// ターゲットディレクトリが指定されていない場合
				// アーカイブファイル内のリストを表示して終了
				if(!parser.Found(wxT("g")))
				{
					// パッチ履歴を表示しない場合
					// 最新のデータのみを表示して終了
					TVPReadXP4Metadata(NULL, archive_base_name, in_archive_items_map);
					TVPXP4MetadataReaderStorageItemToXP4WriterInputFile(
											in_archive_items_map, filelist);
					ListArchiveItems(filelist);
					return 0;
				}
				else
				{
					// パッチ履歴を表示する場合
					// 個々のアーカイブファイルのパッチを表示
					std::vector<wxString> archive_list;
					TVPEnumerateArchiveFiles(archive_base_name, archive_list);
					for(std::vector<wxString>::iterator i = archive_list.begin();
						i != archive_list.end(); i++)
					{
						tTVPXP4MetadataReaderArchive arc(*i);
						std::vector<tTVPXP4WriterInputFile> filelist;
						for(std::vector<tTVPXP4MetadataReaderStorageItem>::const_iterator 
								j = arc.GetItemVector().begin(); j != arc.GetItemVector().end(); j++)
							filelist.push_back(tTVPXP4WriterInputFile(*j));
						wxPrintf(wxT("archive %s\n"), i->c_str());
						wxPrintf(wxT("from    %s\n"), arc.GetTargetDir().c_str());
						ListArchiveItems(filelist);
					}
					return 0;
				}
			}
			else
			{
				wxString target_dir_from_archive;
				TVPReadXP4Metadata(NULL, archive_base_name, in_archive_items_map,
					&target_dir_from_archive);

				if(parser.Found(wxT("u")))
				{
					// 対象ディレクトリをアーカイブのデータから取得する場合
					target_dir = target_dir_from_archive;
					target_dir_specified = true;
					if(target_dir.IsEmpty())
					{
						throw wxString::Format(
							_("archive set '%s' does not have target directory information"),
							archive_base_name.c_str());
					}
				}
			}
		}

		// 対象ディレクトリのリストを読み込む
		if(!target_dir_specified)
			throw wxString(_("please specify target 'dir'"));
						// 対象ディレクトリが指定されていない

		TVPGetFileListAt(&agg, target_dir, filelist);

		if(filelist.size() == 0)
		{
			throw wxString::Format(_("there are no files in target directory '%s'"),
				target_dir.c_str());
		}

		// パターンを用意
		//- デフォルトのパターン
		wxArrayString patterns;
		TVPXP4GetDefaultClassList(patterns);

		//- パターンをさらにファイルから読むように指定されていた場合
		wxString filename;
		if(parser.Found(wxT("c"), &filename))
		{
			// ファイルから分類を読み込む
			LoadClassList(filename, patterns);
		}

		// パターンに従ってリストを分類
		agg.SetRange(20, 25);
		TVPXP4ClassifyFiles(&agg, patterns, filelist);

		// アーカイブファイルの分割情報を得る
		wxFileOffset split_limit = 0;
		long split_mb = 0;
		if(parser.Found(wxT("s"), &split_mb))
			split_limit = (wxFileOffset)split_mb * (1024l*1024l);

		// ファイル名を決定
		wxFileName output_filename(archive_base_name);
		output_filename.ClearExt(); // 拡張子を取り去る

		if(make_new_archive)
		{
			// 新規にアーカイブを作成する場合
			archive_base_name = output_filename.GetFullPath();
		}
		else
		{
			// パッチを作成する場合
			wxString rev;
			if(!parser.Found(wxT("r"), &rev))
			{
				// リビジョンが指定されていない場合
				// リビジョンを現在時刻から作成する
				// リビジョンは YYYYMMDDHHMMSS (local time)
				rev = wxDateTime::Now().Format(wxT("p%Y%m%d%H%M%S"));
			}
			archive_base_name = output_filename.GetFullPath() +
				wxT(".") + rev;
		}

		// 既存のアーカイブと ディレクトリから読み取った情報を比較
		agg.SetRange(14, 15);
		TVPCompareXP4StorageNameMap(&agg, in_archive_items_map, filelist);

		if(filelist.size() == 0)
		{
			throw wxString::Format(_("the archive is up to date; nothing is to be added/modified/deleted"),
				target_dir.c_str());
		}

		// アーカイブを作成
		if(!parser.Found(wxT("d"))) // dry-run で無ければ
		{
			agg.SetRange(25, 100);
			tTVPXP4Writer writer(
				&agg,
				archive_base_name,
				split_limit,
				filelist,
				target_dir);
			writer.MakeArchive();
		}

		//  リストを表示
		if(show_progress) wxFprintf(stderr, wxT("\n"));
		ListArchiveItems(filelist);
	}
	catch(const wxString & e)
	{
		wxFprintf(stderr, wxT("\n"));
		wxLogError(e);
		return 3;
	}
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		分類用ルールの書かれたファイルを読み込む
//! @param		filename: ルールの書かれたファイルのファイル名
//! @param		pattern: 格納先配列(内容はクリアされない)
//---------------------------------------------------------------------------
void wxKrkrReleaserConsoleApp::LoadClassList(const wxString & filename,
	wxArrayString &pattern)
{

	// ファイルは (wxWidgets のデフォルトである) UTF-8 で記述されるべき。
	wxTextFile file(filename);

	if(!file.Open())
		throw wxString::Format(_("can not load class file '%s'"), filename.c_str());

	wxString str;
	for ( str = file.GetFirstLine(); !file.Eof(); str = file.GetNextLine() )
	{
		pattern.Add(str);
	}
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
		case TVP_XP4_FILE_STATE_ADDED:
			wxPrintf(wxT(" A ")); break;
		case TVP_XP4_FILE_STATE_DELETED:
			wxPrintf(wxT(" D ")); break;
		case TVP_XP4_FILE_STATE_MODIFIED:
			wxPrintf(wxT(" M ")); break;
		default:
			wxPrintf(wxT("   ")); break;
		}

		wxPrintf(wxT("%s\n"), i->GetInArchiveName().c_str());
	}
}
//---------------------------------------------------------------------------
