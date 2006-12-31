//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4ファイルシステムの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/fs/xp4fs/XP4FS.h"
#include "base/fs/xp4fs/XP4Archive.h"
#include "base/exception/RisaException.h"
#include <algorithm>
#include <map>

RISSE_DEFINE_SOURCE_ID(9133,5164,36031,18883,4749,40441,40379,56790);



//---------------------------------------------------------------------------
tRisaXP4FS::tRisaXP4FS(const ttstr & name)
{
	// まず、nameで示されたディレクトリにあるすべてのパッチアーカイブを列挙する
	std::vector<ttstr> archive_names;
	ttstr path; // アーカイブのあるディレクトリ
	ttstr name_noext; // name_base から拡張子を取り除いた物
	ttstr name_nopath; // name からパスを取り除いた物

	//- 入力ファイル名を分解
	tRisaFileSystemManager::SplitPathAndName(name, &path, &name_nopath);
	tRisaFileSystemManager::SplitExtension(name_nopath, &name_noext, NULL);

	//- archive_names の先頭に name を追加
	archive_names.push_back(name);

	//- ディレクトリ中にあるファイルを列挙し、name_noext に合致する
	//- ファイルを archive_names に追加する
	class tLister : public tRisaFileSystemIterationCallback
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
			name_noext(b + RISSE_WS(".")),
			exclude(e),
			path(p + RISSE_WS("/"))

			{;}

		bool OnFile(const ttstr & filename)
		{
			// ファイル名が name_noext に合致するかを調べる
			if(filename.StartsWith(name_noext) && filename != exclude)
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

	tRisaFileSystemManager::instance()->GetFileListAt(path, &lister);

	//- archive_names の先頭の要素以外を名前順に並べ替え
	std::sort(archive_names.begin() + 1, archive_names.end());

	// パッチリビジョンを追うためのマップを作成
	std::map<ttstr, tFileItemBasicInfo> map;

	class tMapper : public tRisaXP4Archive::iMapCallback
	{
		std::map<ttstr, tFileItemBasicInfo> & Map;
		risse_size CurrentArchiveIndex;
	public:
		tMapper(std::map<ttstr, tFileItemBasicInfo> & map) :
			Map(map), CurrentArchiveIndex(0)
			{;}

		void operator () (
			const ttstr & name,
			risse_size file_index)
		{
			// 追加/置き換えの場合
			std::map<ttstr, tFileItemBasicInfo>::iterator i;
			i = Map.find(name);

			if(i != Map.end()) Map.erase(i);

			tFileItemBasicInfo item;
			item.ArchiveIndex = CurrentArchiveIndex;
			item.FileIndex = file_index;
			Map.insert(std::pair<ttstr, tFileItemBasicInfo>(name,item));
		}
		void operator () (
			const ttstr & name )
		{
			// 削除の場合
			std::map<ttstr, tFileItemBasicInfo>::iterator i;
			i = Map.find(name);
			if(i != Map.end()) Map.erase(i);
		}
		void SetArchiveIndex(risse_size n) { CurrentArchiveIndex = n; }
	} mapper(map);

	// アーカイブそれぞれを順に読み込む
	Archives.reserve(archive_names.size());
	for(std::vector<ttstr>::iterator i = archive_names.begin();
		i != archive_names.end(); i++)
	{
		mapper.SetArchiveIndex(i - archive_names.begin());
		boost::shared_ptr<tRisaXP4Archive> arc(new tRisaXP4Archive(*i, mapper));
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
tRisaXP4FS::~tRisaXP4FS()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tRisaXP4FS::GetFileListAt(const ttstr & dirname,
	tRisaFileSystemIterationCallback * callback)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	// dir_name の最後に '/' がついていなければ追加
	ttstr dir_name(dirname);
	if(!dir_name.EndsWith(RISSE_WC('/'))) dir_name += RISSE_WC('/');

	// dir_name を名前の先頭に持つ最初のインデックスを取得
	risse_size idx = GetFileItemStartIndex(dir_name);
	if(idx == static_cast<risse_size>(-1))
	{
		// なかった、つまりディレクトリが存在しない
		eRisaException::Throw(RISSE_WS_TR("can not open directory"));
	}

	// idx から検索を開始する
	size_t count = 0;
	ttstr lastdir;
	for(; idx < FileItems.size(); idx++)
	{
		if(!FileItems[idx].Name.StartsWith(dir_name)) break; // 終わり

		ttstr name(FileItems[idx].Name.c_str() + dir_name.GetLen());
		const risse_char * slashp;
		if(!(slashp = Risse_strchr(name.c_str(), RISSE_WC('/'))))
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
			lastdir = ttstr(name.c_str() + (slashp - name.c_str())); // ディレクトリ名
			count ++;
			if(callback) if(!callback->OnDirectory(lastdir)) return count;
			lastdir += RISSE_WC('/');
		}
	}
	return count;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisaXP4FS::FileExists(const ttstr & filename)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	if(filename.EndsWith(RISSE_WC('/'))) return false; // ディレクトリは違う

	return GetFileItemIndex(filename) != static_cast<risse_size>(-1);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisaXP4FS::DirectoryExists(const ttstr & dirname)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	// dir_name の最後に '/' がついていなければ追加
	ttstr dir_name(dirname);
	if(!dir_name.EndsWith(RISSE_WC('/'))) dir_name += RISSE_WC('/');

	return GetFileItemStartIndex(dir_name) != static_cast<risse_size>(-1);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaXP4FS::RemoveFile(const ttstr & filename)
{
	eRisaException::Throw(RISSE_WS_TR("can not delete file (filesystem is read-only)"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaXP4FS::RemoveDirectory(const ttstr & dirname, bool recursive)
{
	eRisaException::Throw(RISSE_WS_TR("can not delete directory (filesystem is read-only)"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaXP4FS::CreateDirectory(const ttstr & dirname, bool recursive)
{
	eRisaException::Throw(RISSE_WS_TR("can not make directory (filesystem is read-only)"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaXP4FS::Stat(const ttstr & filename, tRisaStatStruc & struc)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	risse_size idx = GetFileItemIndex(filename);
	if(idx == static_cast<risse_size>(-1))
		tRisaFileSystemManager::RaiseNoSuchFileOrDirectoryError();

	Archives[FileItems[idx].ArchiveIndex]->Stat(FileItems[idx].FileIndex, struc);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseBinaryStream * tRisaXP4FS::CreateStream(const ttstr & filename, risse_uint32 flags)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	risse_size idx = GetFileItemIndex(filename);
	if(idx == static_cast<risse_size>(-1))
		tRisaFileSystemManager::RaiseNoSuchFileOrDirectoryError();

	// 書き込みを伴う動作はできない
	if(flags & RISSE_BS_ACCESS_WRITE_BIT)
		eRisaException::Throw(RISSE_WS_TR("access denied (filesystem is read-only)"));

	return Archives[FileItems[idx].ArchiveIndex]->
		CreateStream(Archives[FileItems[idx].ArchiveIndex],
		FileItems[idx].FileIndex, flags);
			// boost::shared_ptr<tRisaArchive> を持ってるのはこのクラスだけなので
			// これ経由でこのスマートポインタを渡してやらなければならない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tRisaXP4FS::GetFileItemStartIndex(const ttstr & name)
{
	// returns first index which have 'name' at start of the name.
	// returns -1 if the target is not found.
	// the item must be sorted by ttstr::operator < , otherwise this function
	// will not work propertly.
	risse_size total_count = FileItems.size();
	risse_size s = 0, e = total_count;
	while(e - s > 1)
	{
		risse_int m = (e + s) / 2;
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
	if(s >= total_count) return static_cast<risse_size>(-1); // out of the index
	if(FileItems[s].Name.StartsWith(name)) return s;
	s++;
	if(s >= total_count) return static_cast<risse_size>(-1); // out of the index
	if(FileItems[s].Name.StartsWith(name)) return s;
	return static_cast<risse_size>(-1);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tRisaXP4FS::GetFileItemIndex(const ttstr & name)
{
	risse_size total_count = FileItems.size();
	risse_size s = 0, e = total_count;
	while(e - s > 1)
	{
		risse_int m = (e + s) / 2;
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
	if(s >= total_count) return static_cast<risse_size>(-1); // out of the index
	if(FileItems[s].Name == name) return s;
	s++;
	if(s >= total_count) return static_cast<risse_size>(-1); // out of the index
	if(FileItems[s].Name == name) return s;
	return static_cast<risse_size>(-1);
}
//---------------------------------------------------------------------------














