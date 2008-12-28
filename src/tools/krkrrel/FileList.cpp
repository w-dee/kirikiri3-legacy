//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * 分類を行うためのデフォルトのリスト
 */
// クラスリストのファイルは以下の形式
// e:正規表現パターン
// i:正規表現パターン
// c:正規表現パターン
// a:正規表現パターン
// e: が先頭につく行は exclude (除外) 用パターン
// i: が先頭につく行は include (含める) 用パターン
// c: が先頭につく行は compress (圧縮) 用パターン
// a: が先頭につく行は asis  (圧縮しない) 用パターン
// e i c a の代わりに E I C A を使用すると、大文字・小文字を区別しなくなる

// パターンは先頭から順番に処理される
// パターンをクリアしたい場合は、相反する側のパターンに . を指定する
// たとえば、c のリストをクリアしたい場合は a:. を指定する
// たとえば、e のリストをクリアしたい場合は i:. を指定する

static wxChar const  * const XP4DefaultClassList[] = {
	//-- 圧縮を行うファイルのリスト
	wxT("C:\\.wav$"),
	wxT("C:\\.dll$"),
	wxT("C:\\.tpi$"),
	wxT("C:\\.spi$"),
	wxT("C:\\.txt$"),
	wxT("C:\\.mid$"),
	wxT("C:\\.smf$"),
	wxT("C:\\.swf$"),
	wxT("C:\\.ks$" ),
	wxT("C:\\.risse$"),
	wxT("C:\\.ma$" ),
	wxT("C:\\.asq$"),
	wxT("C:\\.asd$"),
	wxT("C:\\.ttf$"),
	wxT("C:\\.ttc$"),
	wxT("C:\\.bff$"),
	wxT("C:\\.bmp$"),
	wxT("C:\\.tft$"),
	wxT("C:\\.cks$"),
	//-- 除外されるファイルのリスト
	wxT("e:/CVS/"  ),   // CVS メタデータディレクトリ
	wxT("E:/\\."     ), // . で始まる名前を持つファイル/ディレクトリ
	wxT("E:\\.xpk$"),
	wxT("E:\\.xp3$"),
	wxT("E:\\.xp4$"),
	wxT("E:\\.exe$"),
	wxT("E:\\.bat$"),
	wxT("E:\\.tmp$"),
	wxT("E:\\.db$" ),
	wxT("E:\\.sue$"),
	wxT("E:\\.vix$"),
	wxT("E:\\.ico$"),
	wxT("E:\\.aul$"),
	wxT("E:\\.aue$"),
	wxT("E:\\.rpf$"),
	wxT("E:\\.bak$"),
	wxT("E:\\.kep$"),
	wxT("E:\\.~[^./]*$"), // ~ で始まる拡張子



	NULL // ターミネータ
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * デフォルトの分類リストを配列にして返す
 * @param dest	格納先配列(内容はクリアされる)
 */
void XP4GetDefaultClassList(wxArrayString & dest)
{
	dest.clear();
	wxChar const * const * p = XP4DefaultClassList;
	while(*p)
	{
		dest.Add(*p);
		p++;
	}
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
/**
 * 指定ディレクトリにあるファイルを再帰的に検索し、リストを取得する
 * @param callback	進捗コールバックオブジェクト
 * @param dir		対象ディレクトリ
 * @param basedir	ベースディレクトリ
 * @param dest		格納先配列
 */
static void InternalGetFileListAt(iRisaProgressCallback * callback,
	const wxString & dir, const wxString & basedir,
	std::vector<tXP4WriterInputFile> & dest)
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
	if(!wxFileName::DirExists(dir)
		|| !dir_obj.Open(dir)) // wxmsw の実装を見ているとwxDir::Openは絶対に失敗しないみたいなんだけど
	{
		// ディレクトリのオープンに失敗したとき
		throw wxString::Format(_("cannot open directory '%s'"), dir.c_str());
	}
	if(dir_obj.Traverse(trav,
					wxEmptyString, wxDIR_DEFAULT & (~wxDIR_HIDDEN)) == static_cast<size_t>(-1))
	{
		throw wxString::Format(_("cannot read directory '%s'"), dir.c_str());
	}

	// ファイルを追加
	for(size_t i = 0; i < files.GetCount(); i++)
	{
		callback->OnProgress(i * 100 / files.GetCount());
		wxString item_name = files[i].c_str() + basedir.Length();
		wxFileName itemfile(files[i]);
		dest.push_back(tXP4WriterInputFile(
			NormalizeXP4ArchiveStorageName(item_name),
			0,
			itemfile.GetModificationTime(),
			item_name,
			basedir
			));
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 指定ディレクトリにあるファイルを再帰的に検索し、リストを取得する
 * @param callback	進捗コールバックオブジェクト
 * @param dir		対象ディレクトリ
 * @param dest		格納先配列(内容はクリアされる)
 */
void GetFileListAt(iRisaProgressCallback * callback,
	const wxString & dir, std::vector<tXP4WriterInputFile> & dest)
{
	// 格納先 vector のクリア
	dest.clear();

	// ディレクトリ名のチェック
	wxString target_dir = dir;
	// ディレクトリ名の最後に パス・デリミタがついている場合は除去
	while(target_dir.Length() >= 1 && wxFileName::IsPathSeparator(target_dir.Last()))
		target_dir.RemoveLast();

	// 対象ディレクトリを再帰的に探る
	// callback は全てのディレクトリに一様にファイルが入っているわけではないので
	// ほとんど意味をなさない(正確な進捗報告をするのは無理)
	InternalGetFileListAt(callback, target_dir, target_dir, dest);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * ファイルをパターンに従って分類する
 * @param callback	進捗コールバックオブジェクト
 * @param pattern	パターン
 * @param dest		格納先配列
 */
void XP4ClassifyFiles(iRisaProgressCallback * callback,
	const wxArrayString & pattern,
	std::vector<tXP4WriterInputFile> &  dest
	)
{
	// 正規表現パターンをコンパイルする
	struct tPattern
	{
		enum tType {
			include, exclude, compress, asis
		} type;

		wxRegEx regex;
	};

	tPattern * patterns = NULL;
	size_t pattern_count = 0;
	try
	{
		patterns = new tPattern[pattern.GetCount()];

#ifdef wxHAS_REGEX_ADVANCED
	#define RISA__XP4_RULE_RE wxRE_ADVANCED
#else
	#define RISA__XP4_RULE_RE wxRE_DEFAULT
#endif

		for(size_t i = 0; i < pattern.GetCount(); i++)
		{
			const wxString & pat = pattern[i];
			int flags = RISA__XP4_RULE_RE;
			if     (pat.StartsWith(wxT("e:"))) // exlucde, case sens
				patterns[pattern_count].type = tPattern::exclude;
			else if(pat.StartsWith(wxT("E:"))) // exlucde, case ignore
				patterns[pattern_count].type = tPattern::exclude,    flags |= wxRE_ICASE;
			else if(pat.StartsWith(wxT("i:"))) // include, case sens
				patterns[pattern_count].type = tPattern::include;
			else if(pat.StartsWith(wxT("I:"))) // include, case ignore
				patterns[pattern_count].type = tPattern::include,    flags |= wxRE_ICASE;
			else if(pat.StartsWith(wxT("c:"))) // compress, case sens
				patterns[pattern_count].type = tPattern::compress;
			else if(pat.StartsWith(wxT("C:"))) // compress, case ignore
				patterns[pattern_count].type = tPattern::compress,   flags |= wxRE_ICASE;
			else if(pat.StartsWith(wxT("a:"))) // asis, case sens
				patterns[pattern_count].type = tPattern::asis;
			else if(pat.StartsWith(wxT("A:"))) // asis, case ignore
				patterns[pattern_count].type = tPattern::asis,       flags |= wxRE_ICASE;
			else
				continue; // unknown; skip

			if(!patterns[pattern_count].regex.Compile(pat.c_str() + 2, flags))
				throw wxString::Format(_("error in regular expression '%s'"), pat.c_str() );
			pattern_count ++;
		}

		// リストを元に分類を行う
		for(std::vector<tXP4WriterInputFile>::iterator i = dest.begin();
			i != dest.end(); )
		{
			// 進捗を報告
			if(callback) callback->OnProgress((i - dest.begin()) * 100 / dest.size());

			// パターンごとに処理
			wxUint16 flags = 0;
			for(size_t j = 0; j < pattern_count; j++)
			{
				if(patterns[j].regex.Matches(i->GetInArchiveName()))
				{
					// パターンに合致した
					switch(patterns[j].type)
					{
					case tPattern::exclude:
						flags |=   RISA__XP4_FILE_EXCLUDED;
						break;
					case tPattern::include:
						flags &= ~ RISA__XP4_FILE_EXCLUDED;
						break;
					case tPattern::compress:
						flags |=   RISA__XP4_FILE_COMPRESSED;
						break;
					case tPattern::asis:
						flags &= ~ RISA__XP4_FILE_COMPRESSED;
						break;
					}
				}
			}

			// 情報を格納
			if(flags & RISA__XP4_FILE_EXCLUDED)
			{
				// 削除の場合
				i = dest.erase(i);
			}
			else
			{
				i->SetFlags(
					(i->GetFlags() &
						~(RISA__XP4_FILE_EXCLUDED|RISA__XP4_FILE_COMPRESSED)
					) | flags); // フラグを設定
				i++;
			}
		}
	}
	catch(...)
	{
		delete [] patterns;
		throw;
	}

	delete [] patterns;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * アーカイブ内ストレージの名前を正規化する
 * @param name	正規化したいストレージ名
 * @return	正規化したストレージ名
 */
wxString NormalizeXP4ArchiveStorageName(const wxString & name)
{
	// TODO: UNICODE 正規化

	// パス区切り文字の修正
	// パス区切り文字は '/' に統一する
	wxString ret(name);
	if(ret.Length() > 0)
	{
		wxChar pathsep = static_cast<wxChar>(wxFileName::GetPathSeparator());
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
/**
 * tXP4MetadataReaderArchive から得た input 内の項目を map に追加する
 * @param callback	進捗コールバックオブジェクト
 * @param map		追加先map
 * @param input		入力配列
 * @note	すでに追加先に存在していた場合は追加先を書き換える。
 *			削除すべき場合は削除する。
 */
void ApplyXP4StorageNameMap(
	iRisaProgressCallback * callback,
	std::map<wxString, tXP4MetadataReaderStorageItem> &map,
	const std::vector<tXP4MetadataReaderStorageItem> &input)
{
	for(std::vector<tXP4MetadataReaderStorageItem>::const_iterator i =
		input.begin(); i != input.end(); i++)
	{
		if(callback) callback->OnProgress((i - input.begin()) * 100 / input.size());
		std::map<wxString, tXP4MetadataReaderStorageItem>::iterator mi;
		mi = map.find(i->GetInArchiveName());
		if((i->GetFlags() & RISA__XP4_FILE_STATE_MASK) == RISA__XP4_FILE_STATE_DELETED)
		{
			// 削除フラグがたっている
			if(mi != map.end()) map.erase(mi); // アイテムを削除する
		}
		else
		{
			// 追加または置き換え
			if(mi != map.end()) map.erase(mi); // 旧データは削除する

			// 追加する
			tXP4MetadataReaderStorageItem item(*i);
			item.SetFlags((item.GetFlags() & ~ RISA__XP4_FILE_STATE_MASK) |
									RISA__XP4_FILE_STATE_NONE);
											// 状態をクリア
			map.insert(std::pair<wxString, tXP4MetadataReaderStorageItem>
				(i->GetInArchiveName(), item));
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 指定されたアーカイブファイル名をベースとするすべてのアーカイブファイルを列挙する
 * @param archivename	アーカイブファイル名
 * @param archives		格納先配列(内容はクリアされる)
 * @note	この関数を呼ぶ時点では archivename に対応するファイルは存在している
 *			ことが確認できていなければならない
 */
void EnumerateArchiveFiles(const wxString & archivename,
	std::vector<wxString> & archives)
{
	// archives の内容をクリア
	archives.clear();

	// アーカイブファイルを列挙
	// 列挙すべきは: そのアーカイブファイルそのもの
	//               (そのアーカイブファイルから拡張子をのぞいたもの).*.xp4
	archives.push_back(wxFileName(archivename).GetFullPath()); // アーカイブファイルそのものを push

	wxFileName basename(archivename);
	basename.ClearExt(); // 拡張子を取り去る

	wxArrayString files;
	wxString path(basename.GetPath());
	bool skip_path = false;
	if(path.IsEmpty())
	{
		// パスが存在しない場合は現在の作業ディレクトリを使用
		path = wxGetCwd();
		skip_path = true;// ディレクトリ部分は後でスキップする
	}
	wxString filespec(basename.GetName() + wxT(".*.xp4"));
	wxDir::GetAllFiles(path, &files, filespec, wxDIR_FILES);

	for(size_t i = 0; i < files.Count(); i++)
	{
		if(skip_path)
			archives.push_back(wxFileName(files[i]).GetFullName());
		else
			archives.push_back(files[i]);
	}

	// 先頭以外をファイル名順にソート
	std::sort(archives.begin() + 1, archives.end());
/*
	for(std::vector<wxString>::iterator i = archives.begin(); i != archives.end(); i++)
		wxPrintf(wxT("found archive %s\n"), i->c_str());
*/
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 指定されたアーカイブファイル名をベースとするすべてのアーカイブファイルを削除する
 * @param archivename	アーカイブファイル名
 */
void DeleteArchiveSet(const wxString & archivename)
{
	// アーカイブファイルを列挙
	std::vector<wxString> archives;
	EnumerateArchiveFiles(archivename, archives);

	// アーカイブファイルを削除
	for(std::vector<wxString>::iterator i = archives.begin();
		i != archives.end(); i++)
	{
		if(wxFileName(*i).FileExists())
		{
			if(!wxRemoveFile(*i))
				throw wxString(_("cannot remove file '%s'"), i->c_str());
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 指定されたアーカイブファイルのメタデータを読み込む
 * @param callback		進捗コールバックオブジェクト
 * @param archivename	アーカイブファイル名
 * @param dest			格納先マップ(内容はクリアされる)
 * @param targetdir		このアーカイブセットが元にした対象ディレクトリを格納するポインタ(null可)
 */
void ReadXP4Metadata(
	iRisaProgressCallback * callback,
	const wxString & archivename,
	std::map<wxString, tXP4MetadataReaderStorageItem> &dest,
	wxString * targetdir)
{
	// dest の内容をクリア
	dest.clear();

	// targetdir の内容をクリア
	if(targetdir) targetdir->Empty();

	// ファイルを列挙
	std::vector<wxString> archives;
	EnumerateArchiveFiles(archivename, archives);

	// ファイル名順にアーカイブを読み込み、map に追加
	for(std::vector<wxString>::iterator i = archives.begin(); i != archives.end(); i++)
	{
		tProgressCallbackAggregator agg(callback,
				(i - archives.begin()    )* 100 / archives.size(),
				(i - archives.begin() + 1)* 100 / archives.size());
		tXP4MetadataReaderArchive archive(*i);
		ApplyXP4StorageNameMap(&agg, dest, archive.GetItemVector());

		// targetdir を取得
		if(targetdir)
		{
			if(!archive.GetTargetDir().IsEmpty())
			{
				*targetdir = archive.GetTargetDir();
					// 一番最後に読み込んだアーカイブファイルのTargetDirが有効になる
			}
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * arc(アーカイブ内既存ファイル) と ref(ターゲットディレクトリ)を比較し、ref を更新する
 * @param callback	進捗コールバックオブジェクト
 * @param arc		アーカイブ内の既存ファイルを現すmap
 * @param ref		ターゲットディレクトリから取得したファイル一覧
 */
void CompareXP4StorageNameMap(
	iRisaProgressCallback * callback,
	std::map<wxString, tXP4MetadataReaderStorageItem> &arc,
	std::vector<tXP4WriterInputFile> & ref)
{
	size_t ref_size = ref.size();
	size_t arc_size = arc.size();

	// ref のアイテムごとに処理をする
	callback->OnProgress(0);
	size_t ref_idx = 0;
	for(std::vector<tXP4WriterInputFile>::iterator i = ref.begin();
		i != ref.end(); i++, ref_idx ++)
	{
		if(callback) callback->OnProgress(ref_idx * 100 / (ref_size + arc_size));
		tProgressCallbackAggregator agg(callback,
				(ref_idx    ) * 100 / (ref_size + arc_size),
				(ref_idx + 1) * 100 / (ref_size + arc_size));

		// arc 内に i が存在するか？
		std::map<wxString, tXP4MetadataReaderStorageItem>::iterator mi;
		mi = arc.find(i->GetInArchiveName());
		if(mi != arc.end())
		{
			// i が存在した
			wxDateTime arc_time = mi->second.GetTime();
			wxDateTime item_time = i->GetTime();

			// タイプスタンプやサイズを比較
			bool modified = false;
			if(arc_time != item_time)
			{

				// タイムスタンプが変わっている
				if(mi->second.GetSize() == i->GetSize())
				{
					// タイムスタンプが変わっているが
					// サイズが同じ場合は、ハッシュが異なるかどうかを
					// チェックする
					if(!i->GetHash().GetHasHash())
					{
						// ハッシュをまだ計算していない場合は計算を行う
						i->GetHash().Make(&agg, i->GetPath());
					}
					if(i->GetHash() != mi->second.GetHash())
					{
						// ハッシュが違う
						modified = true;
					}
					else
					{
						// つまり、タイムスタンプが違っていても
						// サイズとハッシュが同じならば、更新されたとはみなさない
					}
				}
				else
				{
					// ファイルサイズが違う場合はどっちみち更新されている
					modified = true;
				}
			}
			else if(mi->second.GetSize() != i->GetSize())
			{
				// サイズが変わっている
				modified = true;
			}

			if(modified)
			{
				// 置き換えるべきファイルとしてマークする
				i->SetFlags((i->GetFlags() & ~ RISA__XP4_FILE_STATE_MASK) | RISA__XP4_FILE_STATE_MODIFIED);
			}
			// mi にもフラグを設定する
			mi->second.SetFlags(mi->second.GetFlags() | RISA__XP4_FILE_MARKED);
		}
		else
		{
			// i が存在しない
			// 追加すべきファイルとしてマークする
			i->SetFlags((i->GetFlags() & ~ RISA__XP4_FILE_STATE_MASK) | RISA__XP4_FILE_STATE_ADDED);
		}
	}

	// 今度は arc のアイテムごとに処理をする
	// arc のうち、RISA__XP4_FILE_MARKED のフラグがついていないファイルは
	// ターゲットディレクトリに存在せず、削除されたファイルである
	size_t arc_index = 0;
	for(std::map<wxString, tXP4MetadataReaderStorageItem>::iterator i = arc.begin();
		i != arc.end(); i++, arc_index++)
	{
		if(callback) callback->OnProgress((arc_index + ref_size) * 100 / (ref_size + arc_size));
		if(!(i->second.GetFlags() & RISA__XP4_FILE_MARKED))
		{
			// マークされていない
			// ref に「削除」として追加する
			ref.push_back(tXP4WriterInputFile(i->first, RISA__XP4_FILE_STATE_DELETED));
		}
	}

	// マークの付いていないファイルを ref から削除
	for(std::vector<tXP4WriterInputFile>::iterator i = ref.begin();
		i != ref.end(); )
	{
		if((i->GetFlags() & RISA__XP4_FILE_STATE_MASK) == RISA__XP4_FILE_STATE_NONE)
			i = ref.erase(i);
		else
			i++;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * tXP4MetadataReaderStorageItem の配列を tXP4WriterInputFile に変換する
 * @param input		入力 tXP4MetadataReaderStorageItem の map
 * @param output	出力 tXP4WriterInputFile の配列(内容はクリアされる)
 */
void XP4MetadataReaderStorageItemToXP4WriterInputFile(
	const std::map<wxString, tXP4MetadataReaderStorageItem> & input,
	std::vector<tXP4WriterInputFile> & output)
{
	// output の内容をクリア
	output.clear();
	output.reserve(input.size());

	// input の内容を変換しながら output に追加
	for(std::map<wxString, tXP4MetadataReaderStorageItem>::const_iterator i = input.begin();
		i != input.end(); i++)
	{
		output.push_back(tXP4WriterInputFile((i->second)));
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa

