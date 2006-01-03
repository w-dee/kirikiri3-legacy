//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4ファイルシステムの実装
//---------------------------------------------------------------------------
#include "prec.h"
TJS_DEFINE_SOURCE_ID(2007);

#include "XP4FS.h"
#include "XP4Archive.h"

#include <algorithm>



//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTVPXP4FS::tTVPXP4FS(const ttstr & name)
{
	// まず、nameで示されたディレクトリにあるすべてのパッチアーカイブを列挙する
	std::vector<ttstr> archive_names;
	ttstr path; // アーカイブのあるディレクトリ
	ttstr name_noext; // name_base から拡張子を取り除いた物
	ttstr name_nopath; // name からパスを取り除いた物

	//- 入力ファイル名を分解
	tTVPFileSystemManager::SplitPathAndName(name, &path, &name_nopath);
	tTVPFileSystemManager::SplitExtension(name_nopath, &name_noext, NULL);

	//- archive_names の先頭に name を追加
	archive_names.push_back(name);

	//- ディレクトリ中にあるファイルを列挙し、name_noext に合致する
	//- ファイルを archive_names に追加する
	class tLister : public iTVPFileSystemIterationCallback
	{
		std::vector<ttstr>& archive_names;
		const ttstr & name_noext;
		const ttstr & exclude;
		const ttstr & path;
	public:
		tLister(
			std::vector<ttstr>& n,
			const ttstr & b, const ttstr & e, const ttstr & p) :

			archive_names(n),
			name_noext(b + TJS_WS(".")),
			exclude(e),
			path(p + TJS_WS("/"))

			{;}

		bool OnFile(const ttstr & filename)
		{
			// ファイル名が name_noext に合致するかを調べる
			if(filename.StartWith(name_noext) && filename != exclude)
			{
				// 名前が name_noext で始まり、exclude ではない場合
				archive_names.push_back(path + filename); // ファイル名に追加
			}
			return true;
		}

		bool OnDirectory(const ttstr & dirname)
		{
			// ディレクトリにはここでは興味がない
			return true;
		}
	} lister(archive_names, name_noext, name_nopath, path);

	tTVPFileSystemManager::pointer filesystem;
	filesystem->GetFileListAt(path, &lister);

	//- archive_names の先頭の要素以外を名前順に並べ替え
	std::sort(archive_names.begin() + 1, archive_names.end());

	// パッチリビジョンを追うためのマップを作成
	std::map<ttstr, tFileItemBasicInfo> map;

	class tMapper : public iMapCallback
	{
		std::map<ttstr, tFileItemBasicInfo> & Map;
		tjs_size CurrentArchiveIndex;
	public:
		tMapper(std::map<ttstr, tFileItemBasicInfo> & map) :
			Map(map), CurrentArchiveIndex(0)
			{;}

		void operator () (
			const ttstr & name,
			tjs_size file_index)
		{
			// 追加/置き換えの場合
			std::map<ttstr, tFileItemBasicInfo>::iterator i;
			i = Map.find(name);

			if(i != Map.end()) map.erase(i);

			tFileItem item;
			item.ArchiveIndex = CurrentArchiveIndex;
			item.FileIndex = file_index;
			Map.insert(std::pair<ttstr, tFileItem>(name,item));
		}
		void operator () (
			const ttstr & name )
		{
			// 削除の場合
			std::map<ttstr, tFileItemBasicInfo>::iterator i;
			i = Map.find(name);
			if(i != Map.end()) Map.erase(i);
		}
		void SetArchiveIndex(tjs_size n) { CurrentArchiveIndex = n; }
	} mapper(map);

	// アーカイブそれぞれを順に読み込む
	Archives.reserve(archive_names.size());
	for(std::vector<ttstr>::iterator i = archive_names.begin();
		i != archive_names.end(); i++)
	{
		mapper.SetArchiveIndex(i - archive_names.begin());
		boost::shared_ptr<tTVPArchive> arc(new tTVPArchive(*i, mapper));
		Archives.push_back(arc);
	}

	// FileItems を作成
	FileItems.reserve(map.size());
	for(
		std::map<ttstr, tFileItemBasicInfo>::iterator i = map.begin();
		i != map.end(); i++)
	{
		FileItems.push_back(tFileItemInfo(i->second, i->first));
	}

	// FileItems をソート (後に二分検索を行うため)
	std::sort(FileItems.begin(), FileItems.end());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPXP4FS::~tTVPXP4FS()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイル一覧を取得する
//! @param		dirname ディレクトリ名
//! @param		callback コールバックオブジェクト
//! @return		取得できたファイル数
//---------------------------------------------------------------------------
size_t tTVPXP4FS::GetFileListAt(const ttstr & dirname,
	iTVPFileSystemIterationCallback * callback)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	// dir_name の最後に '/' がついていなければ追加
	ttstr dir_name(dirname);
	if(!dir_name.EndsWith(TJS_WC('/'))) dir_name += TJS_WC('/');

	// dir_name を名前の先頭に持つ最初のインデックスを取得
	tjs_size idx = GetFileItemStartIndex(dir_name);
	if(idx == static_cast<tjs_size>(-1))
	{
		// なかった、つまりディレクトリが存在しない
		eTVPException::Throw(TJS_WS_TR("can not open directory"));
	}

	// idx から検索を開始する
	size_t count = 0;
	ttstr lastdir;
	for(; idx < FileItems.size(); idx++)
	{
		if(!FileItems[idx].Name.StartsWith(dir_name)) break; // 終わり

		ttstr name(FileItems[idx].Name.c_str() + dir_name.GetLem());
		const char * slashp;
		if(!(slashp = TJS_strchr(name.c_str(), TJS_WC('/'))))
		{
			// 名前の部分に / を含んでいない; つまりファイル名
			count ++;
			if(callback) if(!callback->OnFile(name)) return count;
		}
		else
		{
			// 名前の部分に / を含んでいる; つまりディレクトリ名
			if(!lastdir.IsEmpty() && name.StartsWith(lastdir))
				continue; // すでに列挙した
			lastdir = ttstr(name.c_str() + (p - name.c_str())); // ディレクトリ名
			count ++;
			if(callback) if(!callback->OnDirectory(lastdir)) return count;
			lastdir += TJS_WC('/');
		}
	}
	return count;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルが存在するかどうかを得る
//! @param		filename ファイル名
//! @return		ファイルが存在する場合真
//---------------------------------------------------------------------------
bool tTVPXP4FS::FileExists(const ttstr & filename)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	if(filename.EndsWith(TJS_WC('/'))) return false; // ディレクトリは違う

	return GetFileItemIndex(filename) != static_cast<tjs_size>(-1);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリが存在するかどうかを得る
//! @param		dirname ディレクトリ名
//! @return		ディレクトリが存在する場合真
//---------------------------------------------------------------------------
bool tTVPXP4FS::DirectoryExists(const ttstr & dirname)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	// dir_name の最後に '/' がついていなければ追加
	ttstr dir_name(dirname);
	if(!dir_name.EndsWith(TJS_WC('/'))) dir_name += TJS_WC('/');

	return GetFileItemStartIndex(dir_name) != static_cast<tjs_size>(-1);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルを削除する
//! @param		filename ファイル名
//---------------------------------------------------------------------------
void tTVPXP4FS::RemoveFile(const ttstr & filename)
{
	eTVPException::Throw(TJS_WS_TR("can not delete file (filesystem is read-only)"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリを削除する
//! @param		dirname ディレクトリ名
//! @param		recursive 再帰的にディレクトリを削除するかどうか
//---------------------------------------------------------------------------
void tTVPXP4FS::RemoveDirectory(const ttstr & dirname, bool recursive)
{
	eTVPException::Throw(TJS_WS_TR("can not delete directory (filesystem is read-only)"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリを作成する
//! @param		dirname ディレクトリ名
//! @param		recursive 再帰的にディレクトリを作成するかどうか
//---------------------------------------------------------------------------
void tTVPXP4FS::CreateDirectory(const ttstr & dirname, bool recursive)
{
	eTVPException::Throw(TJS_WS_TR("can not make directory (filesystem is read-only)"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたファイルの stat を得る
//! @param		filename ファイル名
//! @param		struc stat 結果の出力先
//---------------------------------------------------------------------------
void tTVPXP4FS::Stat(const ttstr & filename, tTVPStatStruc & struc)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	tjs_size idx = GetFileItemIndex(filename);
	if(idx == static_cast<tjs_size>(-1))
		tTVPFileSystemManager::RaiseNoSuchFileOrDirectoryError();

	Archives[FileItems[idx].ArchiveIndex]->Stat(FileItems[idx].FileIndex, struc);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたファイルのストリームを得る
//! @param		filename ファイル名
//! @param		flags フラグ
//! @return		ストリームオブジェクト
//---------------------------------------------------------------------------
tTVPBinaryStream * tTVPXP4FS::CreateStream(const ttstr & filename, tjs_uint32 flags)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	tjs_size idx = GetFileItemIndex(filename);
	if(idx == static_cast<tjs_size>(-1))
		tTVPFileSystemManager::RaiseNoSuchFileOrDirectoryError();

	// 書き込みを伴う動作はできない
	if(Flags & TJS_BS_ACCESS_WRITE_BIT)
		eTVPException::Throw(TJS_WS_TR("access denied (filesystem is read-only)"));

	return Archives[FileItems[idx].ArchiveIndex]->
		CreateStream(Archives[FileItems[idx].ArchiveIndex],
		FileItems[idx].FileIndex, flags);
			// boost::shared_ptr<tTVPArchive> を持ってるのはこのクラスだけなので
			// これ経由でこのスマートポインタを渡してやらなければならない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		name ファイル名で始まる最初の FileItems内のインデックスを得る
//! @param		name 名前
//! @return		FileItems内のインデックス (見つからなかった場合は (tjs_size)-1 が返る)
//---------------------------------------------------------------------------
tjs_size tTVPXP4FS::GetFileItemStartIndex(const ttstr & name)
{
	// returns first index which have 'name' at start of the name.
	// returns -1 if the target is not found.
	// the item must be sorted by ttstr::operator < , otherwise this function
	// will not work propertly.
	tjs_size total_count = FileItems.size();
	tjs_size s = 0, e = total_count;
	while(e - s > 1)
	{
		tjs_int m = (e + s) / 2;
		if(!(FileItems[m].Name < name))
		{
			// m is after or at the target
			e = m;
		}
		else
		{
			// m is before the target
			s = m;
		}
	}

	// at this point, s or s+1 should point the target.
	// be certain.
	if(s >= (tjs_int)total_count) return static_cast<tjs_size>(-1); // out of the index
	if(FileItems[s].Name.StartsWith(name)) return s;
	s++;
	if(s >= (tjs_int)total_count) return static_cast<tjs_size>(-1); // out of the index
	if(FileItems[s].Name.StartsWith(name)) return s;
	return static_cast<tjs_size>(-1);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		name に対応する FileItems内のインデックスを得る
//! @param		name 名前
//! @return		FileItems内のインデックス (見つからなかった場合は (tjs_size)-1 が返る)
//! @note		GetFileItemStartIndex と違い、その名前とぴったり一致しない限りは見つからないとみなす
//---------------------------------------------------------------------------
tjs_size tTVPXP4FS::GetFileItemIndex(const ttstr & name)
{
	tjs_size total_count = FileItems.size();
	tjs_size s = 0, e = total_count;
	while(e - s > 1)
	{
		tjs_int m = (e + s) / 2;
		if(!(FileItems[m].Name < name))
		{
			// m is after or at the target
			e = m;
		}
		else
		{
			// m is before the target
			s = m;
		}
	}

	// at this point, s or s+1 should point the target.
	// be certain.
	if(s >= (tjs_int)total_count) return static_cast<tjs_size>(-1); // out of the index
	if(FileItems[s] == name) return s;
	s++;
	if(s >= (tjs_int)total_count) return static_cast<tjs_size>(-1); // out of the index
	if(FileItems[s] == name) return s;
	return static_cast<tjs_size>(-1);
}
//---------------------------------------------------------------------------














