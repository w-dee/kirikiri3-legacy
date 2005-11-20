//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ファイルリストに関する処理を行う
//---------------------------------------------------------------------------
#include "prec.h"
#include "ProgressCallback.h"
#include "WriteXP4.h"
#include "XP4Archive.h"
#include "FileList.h"

//---------------------------------------------------------------------------
//! @brief		指定ディレクトリにあるファイルを再帰的に検索し、リストを取得する
//! @param		callback: 進捗コールバックオブジェクト
//! @param		dir: 対象ディレクトリ
//! @param		basedir: ベースディレクトリ
//! @param		dest: 格納先配列
//---------------------------------------------------------------------------
static void TVPInternalGetFileListAt(iTVPProgressCallback * callback,
	const wxString & dir, const wxString & basedir,
	std::vector<tTVPXP4WriterInputFile> & dest)
{
	wxArrayString tmp;

	// 対象ディレクトリのファイルを全て列挙
	class Trav : public wxDirTraverser
	{
	public:
		Trav(wxArrayString & files, wxArrayString & dirs) :
			Files(files), Dirs(dirs)
		{
		}

		virtual wxDirTraverseResult OnFile(const wxString & filename)
		{
			Files.Add(filename);
			return wxDIR_CONTINUE;
		}

		virtual wxDirTraverseResult OnDir(const wxString & dirname)
		{
			if(dirname != wxT(".") && dirname != wxT(".."))
				Dirs.Add(dirname);
			return wxDIR_CONTINUE;
		}

	private:
		wxArrayString& Files;
		wxArrayString& Dirs;
	};

	wxArrayString files;
	wxArrayString dirs;
	Trav trav(files, dirs);
	wxDir dir_obj;
	if(!dir_obj.Open(dir)) // wxmsw の実装を見ているとこの関数は絶対に失敗しないみたいなんだけど
	{
		// ディレクトリのオープンに失敗したとき
		throw wxString::Format(_("Cannot open directory %s"), dir.c_str());
	}
	if(dir_obj.Traverse(trav) == (size_t)-1)
	{
		throw wxString::Format(_("Cannot read directory %s"), dir.c_str());
	}

	// ファイルを追加
	for(size_t i = 0; i < files.GetCount(); i++)
	{
		wxString item_name = files[i].c_str() + basedir.Length();
		dest.push_back(tTVPXP4WriterInputFile(
			item_name,
			basedir,
			TVPNormalizeXP4ArchiveStorageName(item_name)));
	}

	// ディレクトリを再帰する
	for(size_t i = 0; i < dirs.GetCount(); i++)
	{
		tTVPProgressCallbackAggregator agg(callback,
			i     * 100 / dirs.GetCount(),
			(i+1) * 100 / dirs.GetCount());
		TVPInternalGetFileListAt(&agg,
			dir + wxFileName::GetPathSeparator() + dirs[i], basedir, dest);
	}

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定ディレクトリにあるファイルを再帰的に検索し、リストを取得する
//! @param		callback: 進捗コールバックオブジェクト
//! @param		dir: 対象ディレクトリ
//! @param		dest: 格納先配列(内容はクリアされる)
//---------------------------------------------------------------------------
void TVPGetFileListAt(iTVPProgressCallback * callback,
	const wxString & dir, std::vector<tTVPXP4WriterInputFile> & dest)
{
	// 格納先 vector のクリア
	dest.clear();

	// ディレクトリ名のチェック
	wxString target_dir = dir;
	// ディレクトリ名の最後に パス・デリミタがついている場合は除去
	if(target_dir.Length() >= 1 && wxFileName::IsPathSeparator(target_dir.Last()))
		target_dir.RemoveLast();

	// 対象ディレクトリを再帰的に探る
	// callback は全てのディレクトリに一様にファイルが入っているわけではないので
	// ほとんど意味をなさない(正確な進捗報告をするのは無理)
	TVPInternalGetFileListAt(callback, target_dir, target_dir, dest);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルをパターンに従って分類する
//! @param		callback: 進捗コールバックオブジェクト
//! @param		excludepat: 除外正規表現パターン
//! @param		compresspat: 圧縮正規表現パターン
//! @param		dest: 格納先配列
//---------------------------------------------------------------------------
void TVPClassifyFiles(iTVPProgressCallback * callback,
	const wxArrayString & excludepat,
	const wxArrayString & compresspat,
	std::vector<tTVPXP4WriterInputFile> &  dest
	)
{
	// 正規表現パターンをコンパイルする
	wxRegEx * excludes = NULL;
	wxRegEx * compresses = NULL;
	try
	{
		excludes = new wxRegEx[excludepat.GetCount()];
		compresses = new wxRegEx[compresspat.GetCount()];

#ifdef wxHAS_REGEX_ADVANCED
	#define TVP_XP4_RULE_RE wxRE_ADVANCED
#else
	#define TVP_XP4_RULE_RE wxRE_DEFAULT
#endif

		for(size_t i = 0; i < excludepat.GetCount(); i++)
		{
			if(!excludes[i].Compile(excludepat[i], TVP_XP4_RULE_RE))
				throw wxString::Format(_("Error while compiling regular expression %s"),
					excludepat[i].c_str());
		}

		for(size_t i = 0; i < compresspat.GetCount(); i++)
		{
			if(!compresses[i].Compile(compresspat[i], TVP_XP4_RULE_RE))
				throw wxString::Format(_("Error while compiling regular expression %s"),
					compresspat[i].c_str());
		}

		// リストを元に分類を行う
		for(std::vector<tTVPXP4WriterInputFile>::iterator i = dest.begin();
			i != dest.end(); )
		{
			// 進捗を報告
			callback->OnProgress((i - dest.begin()) * 100 / dest.size());

			// 除外されるファイルをチェック
			unsigned char flags = 0;
			for(size_t j = 0; j < excludepat.GetCount(); j++)
			{
				if(excludes[j].Matches(i->InArchiveName))
					flags |= TVP_XP4_FILE_EXCLUDED;
			}

			// 圧縮されるファイルをチェック
			for(size_t j = 0; j < compresspat.GetCount(); j++)
			{
				if(compresses[j].Matches(i->InArchiveName))
					flags |= TVP_XP4_FILE_COMPRESSED;
			}

			// 情報を格納
			if(flags & TVP_XP4_FILE_EXCLUDED)
			{
				// 削除の場合
				i = dest.erase(i);
			}
			else
			{
				i->Flags = flags; // フラグを設定
				i++;
			}
		}
	}
	catch(...)
	{
		if(excludes) delete [] excludes;
		if(compresses) delete [] compresses;
		throw;
	}

	if(excludes) delete [] excludes;
	if(compresses) delete [] compresses;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		アーカイブ内ストレージの名前を正規化する
//! @param		name: 正規化したいストレージ名
//! @returns	正規化したストレージ名
//---------------------------------------------------------------------------
wxString TVPNormalizeXP4ArchiveStorageName(const wxString & name)
{
	// TODO: UNICODE 正規化

	// パス区切り文字の修正
	// パス区切り文字は '/' に統一する
	wxString ret(name);
	if(ret.Length() > 0)
	{
		wxChar pathsep = (wxChar)(wxFileName::GetPathSeparator());
		for(size_t n = 0; ret.GetChar(n); n++)
		{
			if(ret.GetChar(n) == pathsep)
				ret.GetWritableChar(n) = wxT('/');
		}
	}

	return ret;
}
//---------------------------------------------------------------------------
