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
#include "ReadXP4Meta.h"


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
	if(dir_obj.Traverse(trav,
					wxEmptyString, wxDIR_DEFAULT & (~wxDIR_HIDDEN)) == (size_t)-1)
	{
		throw wxString::Format(_("Cannot read directory %s"), dir.c_str());
	}

	// ファイルを追加
	for(size_t i = 0; i < files.GetCount(); i++)
	{
		wxString item_name = files[i].c_str() + basedir.Length();
		dest.push_back(tTVPXP4WriterInputFile(
			TVPNormalizeXP4ArchiveStorageName(item_name),
			0,
			wxFileName(files[i]).GetModificationTime(),
			item_name,
			basedir
			));
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
				if(excludes[j].Matches(i->GetInArchiveName()))
					flags |= TVP_XP4_FILE_EXCLUDED;
			}

			// 圧縮されるファイルをチェック
			for(size_t j = 0; j < compresspat.GetCount(); j++)
			{
				if(compresses[j].Matches(i->GetInArchiveName()))
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
				i->SetFlags(flags); // フラグを設定
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


//---------------------------------------------------------------------------
//! @brief		tTVPXP4MetadataReaderArchive から得た input 内の項目を map に追加する
//! @param		callback: 進捗コールバックオブジェクト
//! @param		map: 追加先map
//! @param		input: 入力配列
//! @note		すでに追加先に存在していた場合は追加先を書き換える。
//!				削除すべき場合は削除する。
//---------------------------------------------------------------------------
void TVPApplyXP4StorageNameMap(
	iTVPProgressCallback * callback,
	std::map<wxString, tTVPXP4MetadataReaderStorageItem> &map,
	const std::vector<tTVPXP4MetadataReaderStorageItem> &input)
{
	for(std::vector<tTVPXP4MetadataReaderStorageItem>::const_iterator i =
		input.begin(); i != input.end(); i++)
	{
		callback->OnProgress((i - input.begin()) * 100 / input.size());
		std::map<wxString, tTVPXP4MetadataReaderStorageItem>::iterator mi;
		mi = map.find(i->GetInArchiveName());
		if(i->GetFlags() & TVP_XP4_FILE_DELETED)
		{
			// 削除フラグがたっている
			map.erase(mi); // アイテムを削除する
		}
		else
		{
			// 追加または置き換え
			if(mi != map.end()) map.erase(mi); // 旧データは削除する

			// 追加する
			map.insert(std::pair<wxString, tTVPXP4MetadataReaderStorageItem>
				(i->GetInArchiveName(), *i));
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたアーカイブファイル名をベースとするすべてのアーカイブファイルを列挙する
//! @param		archivename: アーカイブファイル名
//! @param		archives: 格納先配列(内容はクリアされる)
//! @note		この関数を呼ぶ時点では archivename に対応するファイルは存在している
//!				ことが確認できていなければならない
//---------------------------------------------------------------------------
static void TVPEnumerateArchiveFiles(const wxString & archivename,
	std::vector<wxString> & archives)
{
	// archives の内容をクリア
	archives.clear();

	// アーカイブファイルを列挙
	// 列挙すべきは: そのアーカイブファイルそのもの
	//               (そのアーカイブファイルから拡張子をのぞいたもの).*.xp4
	archives.push_back(archivename); // アーカイブファイルそのものを push

	wxFileName basename(archivename);
	basename.ClearExt(); // 拡張子を取り去る

	wxArrayString files;
	wxDir::GetAllFiles(basename.GetPath(), &files, basename.GetName() + wxT(".*.xp4"),
		 wxDIR_FILES);

	for(size_t i = 0; i < files.Count(); i++)
		archives.push_back(files[i]);

	for(std::vector<wxString>::iterator i = archives.begin(); i != archives.end(); i++)
		wxPrintf(wxT("found archive %s\n"), i->c_str());

	// 先頭以外をファイル名順にソート
	std::sort(archives.begin() + 1, archives.end());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたアーカイブファイルのメタデータを読み込む
//! @param		callback: 進捗コールバックオブジェクト
//! @param		archivename: アーカイブファイル名
//! @param		dest: 格納先マップ(内容はクリアされる)
//---------------------------------------------------------------------------
void TVPReadXP4Metadata(
	iTVPProgressCallback * callback,
	const wxString & archivename,
	std::map<wxString, tTVPXP4MetadataReaderStorageItem> &dest)
{
	// dest の内容をクリア
	dest.clear();

	// ファイルを列挙
	std::vector<wxString> archives;
	TVPEnumerateArchiveFiles(archivename, archives);

	// ファイル名順にアーカイブを読み込み、map に追加
	for(std::vector<wxString>::iterator i = archives.begin(); i != archives.end(); i++)
	{
		tTVPProgressCallbackAggregator agg(callback,
				(i - archives.begin()    )* 100 / archives.size(),
				(i - archives.begin() + 1)* 100 / archives.size());
		tTVPXP4MetadataReaderArchive archive(*i);
		TVPApplyXP4StorageNameMap(&agg, dest, archive.GetItemVector());
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		arc(アーカイブ内既存ファイル) と ref(ターゲットディレクトリ)を比較し、ref を更新する
//! @param		callback: 進捗コールバックオブジェクト
//! @param		arc: アーカイブ内の既存ファイルを現すmap
//! @param		ref: ターゲットディレクトリから取得したファイル一覧
//---------------------------------------------------------------------------
void TVPCompareXP4StorageNameMap(
	iTVPProgressCallback * callback,
	std::map<wxString, tTVPXP4MetadataReaderStorageItem> &arc,
	std::vector<tTVPXP4WriterInputFile> & ref)
{
	tTVPProgressCallbackAggregator agg(callback, 0, 50); // 0～50% 区間

	// ref のアイテムごとに処理をする
	for(std::vector<tTVPXP4WriterInputFile>::iterator i = ref.begin();
		i != ref.end(); i++)
	{
		agg.OnProgress((i - ref.begin()) * 100 / ref.size());
		// arc 内に i が存在するか？
		std::map<wxString, tTVPXP4MetadataReaderStorageItem>::iterator mi;
		mi = arc.find(i->GetInArchiveName());
		if(mi != arc.end())
		{
			// i が存在した
			// タイプスタンプを比較
			if(mi->second.GetTime() != i->GetTime())
			{
				// タイムスタンプが変わっている
				// 置き換えるべきファイルとしてマークする
				i->SetFlags((i->GetFlags() & ~ TVP_XP4_FILE_STATE_MASK) | TVP_XP4_FILE_MODIFIED);
			}
			// mi にもフラグを設定する
			mi->second.SetFlags(mi->second.GetFlags() | TVP_XP4_FILE_MARKED);
		}
		else
		{
			// i が存在しない
			// 追加すべきファイルとしてマークする
			i->SetFlags((i->GetFlags() & ~ TVP_XP4_FILE_STATE_MASK) | TVP_XP4_FILE_ADDED);
		}
	}

	// 今度は arc のアイテムごとに処理をする
	// arc のうち、TVP_XP4_FILE_MARKED のフラグがついていないファイルは
	// ターゲットディレクトリに存在せず、削除されたファイルである
	agg.SetRange(50, 100); // 50～100% 区間
	size_t arc_count = arc.size();
	size_t arc_index = 0;
	for(std::map<wxString, tTVPXP4MetadataReaderStorageItem>::iterator i = arc.begin();
		i != arc.end(); i++, arc_index++)
	{
		agg.OnProgress(arc_index * 100 / arc_count);
		if(!(i->second.GetFlags() & TVP_XP4_FILE_MARKED))
		{
			// マークされていない
			// ref に「削除」として追加する
			ref.push_back(tTVPXP4WriterInputFile(i->first, TVP_XP4_FILE_DELETED));
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tTVPXP4MetadataReaderStorageItem の配列を tTVPXP4WriterInputFile に変換する
//! @param		input: 入力 tTVPXP4MetadataReaderStorageItem の map
//! @param		output: 出力 tTVPXP4WriterInputFile の配列(内容はクリアされる)
//---------------------------------------------------------------------------
void TVPXP4MetadataReaderStorageItemToXP4WriterInputFile(
	const std::map<wxString, tTVPXP4MetadataReaderStorageItem> & input,
	std::vector<tTVPXP4WriterInputFile> & output)
{
	// output の内容をクリア
	output.clear();
	output.reserve(input.size());

	// input の内容を変換しながら output に追加
	for(std::map<wxString, tTVPXP4MetadataReaderStorageItem>::const_iterator i = input.begin();
		i != input.end(); i++)
	{
		output.push_back(tTVPXP4WriterInputFile((i->second)));
	}
}
//---------------------------------------------------------------------------

