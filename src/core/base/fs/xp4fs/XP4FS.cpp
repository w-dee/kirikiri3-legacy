//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4ファイルシステムの実装
//---------------------------------------------------------------------------

#include "XP4FS.h"
#include "XP4Archive.h"

#include <zlib.h>
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
			name_noext(b + TJS_W(".")),
			exclude(e),
			path(p + TJS_W("/"))

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
//! @param		dirname: ディレクトリ名
//! @param		callback: コールバックオブジェクト
//! @return		取得できたファイル数
//---------------------------------------------------------------------------
size_t tTVPXP4FS::GetFileListAt(const ttstr & dirname,
	iTVPFileSystemIterationCallback * callback)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	// dir_name の最後に '/' がついていなければ追加
	ttstr dir_name(dirname);
	if(!dir_name.EndsWith(TJS_W('/'))) dir_name += TJS_W('/');

	// dir_name を名前の先頭に持つ最初のインデックスを取得
	tjs_size idx = GetFileItemStartIndex(dir_name);
	if(idx == static_cast<tjs_size>(-1))
	{
		// なかった、つまりディレクトリが存在しない
		TVPThrowExceptionMessage(_("can not open directory"));
	}

	// idx から検索を開始する
	size_t count = 0;
	ttstr lastdir;
	for(; idx < FileItems.size(); idx++)
	{
		if(!FileItems[idx].Name.StartsWith(dir_name)) break; // 終わり

		ttstr name(FileItems[idx].Name.c_str() + dir_name.GetLem());
		const char * slashp;
		if(!(slashp = TJS_strchr(name.c_str(), TJS_W('/'))))
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
			lastdir += TJS_W('/');
		}
	}
	return count;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルが存在するかどうかを得る
//! @param		filename: ファイル名
//! @return		ファイルが存在する場合真
//---------------------------------------------------------------------------
bool tTVPXP4FS::FileExists(const ttstr & filename)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	if(filename.EndsWith(TJS_W('/'))) return false; // ディレクトリは違う

	return GetFileItemIndex(filename) != static_cast<tjs_size>(-1);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリが存在するかどうかを得る
//! @param		dirname: ディレクトリ名
//! @return		ディレクトリが存在する場合真
//---------------------------------------------------------------------------
bool tTVPXP4FS::DirectoryExists(const ttstr & dirname)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	// dir_name の最後に '/' がついていなければ追加
	ttstr dir_name(dirname);
	if(!dir_name.EndsWith(TJS_W('/'))) dir_name += TJS_W('/');

	return GetFileItemStartIndex(dir_name) != static_cast<tjs_size>(-1);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルを削除する
//! @param		filename: ファイル名
//---------------------------------------------------------------------------
void tTVPXP4FS::RemoveFile(const ttstr & filename)
{
	TVPThrowExceptionMessage(_("can not delete file; filesystem is read-only"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリを削除する
//! @param		dirname: ディレクトリ名
//! @param		recursive: 再帰的にディレクトリを削除するかどうか
//---------------------------------------------------------------------------
void tTVPXP4FS::RemoveDirectory(const ttstr & dirname, bool recursive)
{
	TVPThrowExceptionMessage(_("can not delete directory; filesystem is read-only"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ディレクトリを作成する
//! @param		dirname: ディレクトリ名
//! @param		recursive: 再帰的にディレクトリを作成するかどうか
//---------------------------------------------------------------------------
void tTVPXP4FS::CreateDirectory(const ttstr & dirname, bool recursive)
{
	TVPThrowExceptionMessage(_("can not make directory; filesystem is read-only"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたファイルの stat を得る
//! @param		filename: ファイル名
//! @param		struc: stat 結果の出力先
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
//! @param		filename: ファイル名
//! @param		flags: フラグ
//! @return		ストリームオブジェクト
//---------------------------------------------------------------------------
tTVPBinaryStream * tTVPXP4FS::CreateStream(const ttstr & filename, tjs_uint32 flags)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	tjs_size idx = GetFileItemIndex(filename);
	if(idx == static_cast<tjs_size>(-1))
		tTVPFileSystemManager::RaiseNoSuchFileOrDirectoryError();

	return Archives[FileItems[idx].ArchiveIndex]->
		CreateStream(Archives[FileItems[idx].ArchiveIndex],
		FileItems[idx].FileIndex, flags);
			// boost::shared_ptr<tTVPArchive> を持ってるのはこのクラスだけなので
			// これ経由でこのスマートポインタを渡してやらなければならない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		name ファイル名で始まる最初の FileItems内のインデックスを得る
//! @param		name: 名前
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
//! @param		name: 名前
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

























//---------------------------------------------------------------------------
// tTVPXP3Archive
//---------------------------------------------------------------------------
/*
	TVP XPK3 virtual file system support. (in short : XP3)
	TVP supports no longer archive type of "XPK1/XPK2"
	( XPK1/XPK2 is used by TVP ver under 0.9x ).

	here word "in-archive" is used for the storages which are contained in
	archive.
*/
//---------------------------------------------------------------------------
static bool TVPGetXP3ArchiveOffset(tTJSBinaryStream *st, const ttstr name,
	tjs_uint64 & offset, bool raise)
{
	st->SetPosition(0);
	tjs_uint8 mark[11+1];
	static tjs_uint8 XP3Mark1[] =
		{ 0x58/*'X'*/, 0x50/*'P'*/, 0x33/*'3'*/, 0x0d/*'\r'*/,
		  0x0a/*'\n'*/, 0x20/*' '*/, 0x0a/*'\n'*/, 0x1a/*EOF*/,
		  0xff /* sentinel */ };
	static tjs_uint8 XP3Mark2[] =
		{ 0x8b, 0x67, 0x01, 0xff/* sentinel */ };

	// XP3 header mark contains:
	// 1. line feed and carriage return to detect corruption by unnecessary
	//    line-feeds convertion
	// 2. 1A EOF mark which indicates file's text readable header ending.
	// 3. 8B67 KANJI-CODE to detect curruption by unnecessary code convertion
	// 4. 01 file structure version and character coding
	//    higher 4 bits are file structure version, currently 0.
	//    lower 4 bits are character coding, currently 1, is BMP 16bit Unicode.

	static tjs_uint8 XP3Mark[11+1];
		// +1: I was warned by CodeGuard that the code will do
		// access overrun... because a number of 11 is not aligned by DWORD, 
		// and the processor may read the value of DWORD at last of this array
		// from offset 8. Then the last 1 byte would cause a fail.
	static bool DoInit = true;
	if(DoInit)
	{
		// the XP3 header above is splitted into two part; to avoid
		// mis-finding of the header in the program's initialized data area.
		DoInit = false;
		memcpy(XP3Mark, XP3Mark1, 8);
		memcpy(XP3Mark + 8, XP3Mark2, 3);
		// here joins it.
	}

	mark[0] = 0; // sentinel
	st->ReadBuffer(mark, 11);
	if(mark[0] == 0x4d/*'M'*/ && mark[1] == 0x5a/*'Z'*/)
	{
		// "MZ" is a mark of Win32/DOS executables,
		// TVP searches the first mark of XP3 archive
		// in the executeble file.
		bool found = false;

		offset = 16;
		st->SetPosition(16);

		// XP3 mark must be aligned by a paragraph ( 16 bytes )
		const tjs_uint one_read_size = 256*1024;
		tjs_uint read;
		tjs_uint8 buffer[one_read_size]; // read 256kbytes at once

		while(0!=(read = st->Read(buffer, one_read_size)))
		{
			tjs_uint p = 0;
			while(p<read)
			{
				if(!memcmp(XP3Mark, buffer + p, 11))
				{
					// found the mark
					offset += p;
					found = true;
					break;
				}
				p+=16;
			}
			if(found) break;
			offset += one_read_size;
		}

		if(!found)
		{
			if(raise)
				TVPThrowExceptionMessage(TVPCannotUnbindXP3EXE, name);
			else
				return false;
		}
	}
	else if(!memcmp(XP3Mark, mark, 11))
	{
		// XP3 mark found
		offset = 0;
	}
	else
	{
		if(raise)
			TVPThrowExceptionMessage(TVPCannotFindXP3Mark, name);
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------
bool TVPIsXP3Archive(const ttstr &name)
{
	tTVPStreamHolder holder(name);
	try
	{
		tjs_uint64 offset;
		return TVPGetXP3ArchiveOffset(holder.Get(), name, offset, false);
	}
	catch(...)
	{
		return false;
	}
}
//---------------------------------------------------------------------------
tTVPXP3Archive::tTVPXP3Archive(const ttstr & name) : tTVPArchive(name)
{
	Name = name;
	Count = 0;

	tjs_uint64 offset;

	tTJSBinaryStream *st = TVPCreateStream(name);

	tjs_uint8 *indexdata = NULL;

	static tjs_uint8 cn_File[] =
		{ 0x46/*'F'*/, 0x69/*'i'*/, 0x6c/*'l'*/, 0x65/*'e'*/ };
	static tjs_uint8 cn_info[] =
		{ 0x69/*'i'*/, 0x6e/*'n'*/, 0x66/*'f'*/, 0x6f/*'o'*/ };
	static tjs_uint8 cn_segm[] =
		{ 0x73/*'s'*/, 0x65/*'e'*/, 0x67/*'g'*/, 0x6d/*'m'*/ };
	static tjs_uint8 cn_adlr[] =
		{ 0x61/*'a'*/, 0x64/*'d'*/, 0x6c/*'l'*/, 0x72/*'r'*/ };

	TVPAddLog(TJS_W("(info) Trying to read XP3 virtual file system "
		"information from : ") +
		name);

	int segmentcount = 0;
	try
	{
		// retrieve archive offset
		TVPGetXP3ArchiveOffset(st, name, offset, true);

		// read index position and seek
		st->SetPosition(11 + offset);

		// read all XP3 indices
		while(true)
		{
			tjs_uint64 index_ofs = st->ReadI64LE();
			st->SetPosition(index_ofs + offset);

			// read index to memory
			tjs_uint8 index_flag;
			st->ReadBuffer(&index_flag, 1);
			tjs_uint index_size;

			if((index_flag & TVP_XP3_INDEX_ENCODE_METHOD_MASK) ==
				TVP_XP3_INDEX_ENCODE_ZLIB)
			{
				// compressed index
				tjs_uint64 compressed_size = st->ReadI64LE();
				tjs_uint64 r_index_size = st->ReadI64LE();

				if((tjs_uint)compressed_size != compressed_size ||
					(tjs_uint)r_index_size != r_index_size)
						TVPThrowExceptionMessage(TVPReadError);
						// too large to handle, or corrupted
				index_size = (tjs_int)r_index_size;
				indexdata = new tjs_uint8[index_size];
				tjs_uint8 *compressed = new tjs_uint8[(tjs_uint)compressed_size];
				try
				{
					st->ReadBuffer(compressed, (tjs_uint)compressed_size);

					unsigned long destlen = (unsigned long)index_size;

					int result = uncompress(  /* uncompress from zlib */
						(unsigned char *)indexdata,
						&destlen, (unsigned char*)compressed,
							(unsigned long)compressed_size);
					if(result != Z_OK ||
						destlen != (unsigned long)index_size)
							TVPThrowExceptionMessage(TVPUncompressionFailed);
				}
				catch(...)
				{
					delete [] compressed;
					throw;
				}
				delete [] compressed;
			}
			else if((index_flag & TVP_XP3_INDEX_ENCODE_METHOD_MASK) ==
				TVP_XP3_INDEX_ENCODE_RAW)
			{
				// uncompressed index
				tjs_uint64 r_index_size = st->ReadI64LE();
				if((tjs_uint)r_index_size != r_index_size)
					TVPThrowExceptionMessage(TVPReadError);
						// too large to handle or corrupted
				index_size = (tjs_uint)r_index_size;
				indexdata = new tjs_uint8[index_size];
				st->ReadBuffer(indexdata, index_size);
			}
			else
			{
				// unknown encode method
				TVPThrowExceptionMessage(TVPReadError);
			}


			// read index information from memory
			tjs_uint ch_file_start = 0;
			tjs_uint ch_file_size = index_size;
			Count = 0;
			for(;;)
			{
				// find 'File' chunk
				if(!FindChunk(indexdata, cn_File, ch_file_start, ch_file_size))
					break; // not found

				// find 'info' sub-chunk
				tjs_uint ch_info_start = ch_file_start;
				tjs_uint ch_info_size = ch_file_size;
				if(!FindChunk(indexdata, cn_info, ch_info_start, ch_info_size))
					TVPThrowExceptionMessage(TVPReadError);

				// read info sub-chunk
				tArchiveItem item;
				tjs_uint32 flags = ReadI32FromMem(indexdata + ch_info_start + 0);
				if(!TVPAllowExtractProtectedStorage && (flags & TVP_XP3_FILE_PROTECTED))
					TVPThrowExceptionMessage(TJS_W("Specified storage had been protected!"));
				item.OrgSize = ReadI64FromMem(indexdata + ch_info_start + 4);
				item.ArcSize = ReadI64FromMem(indexdata + ch_info_start + 12);

				tjs_int len = ReadI16FromMem(indexdata + ch_info_start + 20);
				ttstr name = TVPStringFromBMPUnicode(
						(const tjs_uint16 *)(indexdata + ch_info_start + 22), len);
				item.Name = name;
				NormalizeInArchiveStorageName(item.Name);

				// find 'segm' sub-chunk
				// Each of in-archive storages can be splitted into some segments.
				// Each segment can be compressed or uncompressed independently.
				// segments can share partial area of archive storage. ( this is used for
				// OggVorbis' VQ code book sharing )
				tjs_uint ch_segm_start = ch_file_start;
				tjs_uint ch_segm_size = ch_file_size;
				if(!FindChunk(indexdata, cn_segm, ch_segm_start, ch_segm_size))
					TVPThrowExceptionMessage(TVPReadError);

				// read segm sub-chunk
				tjs_int segment_count = ch_segm_size / 28;
				tjs_uint64 offset_in_archive = 0;
				for(tjs_int i = 0; i<segment_count; i++)
				{
					tjs_uint pos_base = i * 28 + ch_segm_start;
					tTVPXP3ArchiveSegment seg;
					tjs_uint32 flags = ReadI32FromMem(indexdata + pos_base);

					if((flags & TVP_XP3_SEGM_ENCODE_METHOD_MASK) ==
							TVP_XP3_SEGM_ENCODE_RAW)
						seg.IsCompressed = false;
					else if((flags & TVP_XP3_SEGM_ENCODE_METHOD_MASK) ==
							TVP_XP3_SEGM_ENCODE_ZLIB)
						seg.IsCompressed = true;
					else
						TVPThrowExceptionMessage(TVPReadError); // unknown encode method
						
					seg.IsCompressed = flags & 1;
					seg.Start = ReadI64FromMem(indexdata + pos_base + 4) + offset;
						// data offset in archive
					seg.Offset = offset_in_archive; // offset in in-archive storage
					seg.OrgSize = ReadI64FromMem(indexdata + pos_base + 12); // original size
					seg.ArcSize = ReadI64FromMem(indexdata + pos_base + 20); // archived size
					item.Segments.push_back(seg);
					offset_in_archive += seg.OrgSize;
					segmentcount ++;
				}

				// find 'aldr' sub-chunk
				tjs_uint ch_adlr_start = ch_file_start;
				tjs_uint ch_adlr_size = ch_file_size;
				if(!FindChunk(indexdata, cn_adlr, ch_adlr_start, ch_adlr_size))
					TVPThrowExceptionMessage(TVPReadError);

				// read 'aldr' sub-chunk
				item.FileHash = ReadI32FromMem(indexdata + ch_adlr_start);

				// push information
				ItemVector.push_back(item);

				// to next file
				ch_file_start += ch_file_size;
				ch_file_size = index_size - ch_file_start;
				Count++;
			}

			if(!(index_flag & TVP_XP3_INDEX_CONTINUE))
				break; // continue reading index when the bit sets
		}

		// sort item vector by its name (required for tTVPArchive specification)
		std::stable_sort(ItemVector.begin(), ItemVector.end());
	}
	catch(...)
	{
		if(indexdata) delete [] indexdata;
		delete st;
		TVPAddLog(TJS_W("(info) Falied."));
		throw;
	}
	if(indexdata) delete [] indexdata;
	delete st;

	TVPAddLog(TJS_W("(info) Done. (contains ") + ttstr(Count) +
		TJS_W(" file(s), ") + ttstr(segmentcount) + TJS_W(" segment(s))"));
}
//---------------------------------------------------------------------------
tTVPXP3Archive::~tTVPXP3Archive()
{
	TVPFreeArchiveHandlePoolByPointer(this);
}
//---------------------------------------------------------------------------
tTJSBinaryStream * tTVPXP3Archive::CreateStreamByIndex(tjs_uint idx)
{
	if(idx >= ItemVector.size()) TVPThrowExceptionMessage(TVPReadError);

	tArchiveItem &item = ItemVector[idx];

	tTJSBinaryStream *stream = TVPGetCachedArchiveHandle(this, Name);

	tTJSBinaryStream *out;
	try
	{
		out = new tTVPXP3ArchiveStream(this, idx, &(item.Segments), stream,
			item.OrgSize);
	}
	catch(...)
	{
		TVPReleaseCachedArchiveHandle(this, stream);
		throw;
	}

	return out;
}
//---------------------------------------------------------------------------
bool tTVPXP3Archive::FindChunk(const tjs_uint8 *data, const tjs_uint8 * name,
		tjs_uint &start, tjs_uint &size)
{
	tjs_uint start_save = start;
	tjs_uint size_save = size;

	tjs_uint pos = 0;
	while(pos < size)
	{
		bool found = !memcmp(data + start, name, 4);
		start += 4;
		tjs_uint64 r_size = ReadI64FromMem(data + start);
		start += 8;
		tjs_uint size_chunk = (tjs_uint)r_size;
		if(size_chunk != r_size)
			TVPThrowExceptionMessage(TVPReadError);
		if(found)
		{
			// found
			size = size_chunk;
			return true;
		}
		start += size_chunk;
		pos += size_chunk;
	}

	start = start_save;
	size = size_save;
	return false;
}
//---------------------------------------------------------------------------
tjs_int16 tTVPXP3Archive::ReadI16FromMem(const tjs_uint8 *mem)
{
	return (tjs_int16)mem[0] + ((tjs_int16)mem[1] << 8);
}
//---------------------------------------------------------------------------
tjs_int32 tTVPXP3Archive::ReadI32FromMem(const tjs_uint8 *mem)
{
	return (tjs_int32)mem[0] + ((tjs_int32)mem[1] << 8) +
		((tjs_int32)mem[2] << 16) + ((tjs_int32)mem[3] << 24);
}
//---------------------------------------------------------------------------
tjs_int64 tTVPXP3Archive::ReadI64FromMem(const tjs_uint8 *mem)
{
	tjs_int32 low = ReadI32FromMem(mem);
	tjs_int32 high = ReadI32FromMem(mem + 4);
	return ((tjs_int64)high << 32) | ((tjs_int64)low);
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tTVPXP3ArchiveStream : stream class for in-archive storage
//---------------------------------------------------------------------------
tTVPXP3ArchiveStream::tTVPXP3ArchiveStream(tTVPXP3Archive *owner,
	tjs_int storageindex,
	std::vector<tTVPXP3ArchiveSegment> *segments, tTJSBinaryStream * stream,
		tjs_uint64 orgsize)
{
	StorageIndex = storageindex;
	Segments = segments;
	SegmentData = NULL;
	CurSegmentNum = 0;
	CurSegment = &(Segments->operator [](0));
	SegmentPos = 0;
	SegmentRemain = CurSegment->OrgSize;
	SegmentOpened = false;
	CurPos = 0;

	LastOpenedSegmentNum = -1;

	Owner = owner;
	Owner->AddRef(); // hook
	Stream = stream;
	OrgSize = orgsize;
}
//---------------------------------------------------------------------------
tTVPXP3ArchiveStream::~tTVPXP3ArchiveStream()
{
	TVPReleaseCachedArchiveHandle(Owner, Stream);
	Owner->Release(); // unhook
	if(SegmentData) SegmentData->Release();
}
//---------------------------------------------------------------------------
void tTVPXP3ArchiveStream::EnsureSegment()
{
	// ensure accessing to current segment
	if(SegmentOpened) return;

	if(LastOpenedSegmentNum == CurSegmentNum)
	{
		if(!CurSegment->IsCompressed)
			Stream->SetPosition(CurSegment->Start + SegmentPos);
		return;
	}

	// erase buffer
	if(SegmentData) SegmentData->Release(), SegmentData = NULL;

	// is compressed segment ?
	if(CurSegment->IsCompressed)
	{
		// a compressed segment

		if(CurSegment->OrgSize >= TVP_SEGCACHE_ONE_LIMIT)
		{
			// too large to cache
			Stream->SetPosition(CurSegment->Start);
			SegmentData = new tTVPSegmentData;
			SegmentData->SetData((tjs_uint)CurSegment->OrgSize,
				Stream, (tjs_uint)CurSegment->ArcSize);
		}
		else
		{
			// search thru segment cache
			tTVPSegmentCacheSearchData sdata;
			sdata.Name = Owner->GetName();
			sdata.StorageIndex = StorageIndex;
			sdata.SegmentIndex = CurSegmentNum;

			tjs_uint32 hash;
			hash = tTVPSegmentCacheSearchHashFunc::Make(sdata);

			SegmentData = TVPSearchFromSegmentCache(sdata, hash);
			if(!SegmentData)
			{
				// not found in cache
				Stream->SetPosition(CurSegment->Start);
				SegmentData = new tTVPSegmentData;
				SegmentData->SetData((tjs_uint)CurSegment->OrgSize,
					Stream, (tjs_uint)CurSegment->ArcSize);

				// add to cache
				TVPPushToSegmentCache(sdata, hash, SegmentData);
			}
		}
	}
	else
	{
		// not a compressed segment

		Stream->SetPosition(CurSegment->Start + SegmentPos);
	}

	SegmentOpened = true;
	LastOpenedSegmentNum = CurSegmentNum;
}
//---------------------------------------------------------------------------
void tTVPXP3ArchiveStream::SeekToPosition(tjs_uint64 pos)
{
	// open segment at 'pos' and seek
	// pos must between zero thru OrgSize
	if(CurPos == pos) return;

	// do binary search to determine current segment number
	tjs_int st = 0;
	tjs_int et = Segments->size();
	tjs_int seg_num;

	while(true)
	{
		if(et-st <= 1) { seg_num = st; break; }
		tjs_int m = st + (et-st)/2;
		if(Segments->operator[](m).Offset > pos)
			et = m;
		else
			st = m;
	}

	CurSegmentNum = seg_num;
	CurSegment = &(Segments->operator [](CurSegmentNum));
	SegmentOpened = false;

	SegmentPos = pos - CurSegment->Offset;
	SegmentRemain = CurSegment->OrgSize - SegmentPos;
	CurPos = pos;
}
//---------------------------------------------------------------------------
bool tTVPXP3ArchiveStream::OpenNextSegment()
{
	// open next segment
	if(CurSegmentNum == (tjs_int)(Segments->size() -1))
		return false; // no more segments
	CurSegmentNum ++;
	CurSegment = &(Segments->operator [](CurSegmentNum));
	SegmentOpened = false;
	SegmentPos = 0;
	SegmentRemain = CurSegment->OrgSize;
	CurPos = CurSegment->Offset;
	EnsureSegment();
	return true;
}
//---------------------------------------------------------------------------
tjs_uint64 TJS_INTF_METHOD tTVPXP3ArchiveStream::Seek(tjs_int64 offset, tjs_int whence)
{
	tjs_int64 newpos;
	switch(whence)
	{
	case TJS_BS_SEEK_SET:
		newpos = offset;
		if(offset >= 0 && offset <= OrgSize)
		{
			SeekToPosition(newpos);
		}
		return CurPos;

	case TJS_BS_SEEK_CUR:
		newpos = offset + CurPos;
		if(offset >= 0 && offset <= OrgSize)
		{
			SeekToPosition(newpos);
		}
		return CurPos;

	case TJS_BS_SEEK_END:
		newpos = offset + OrgSize;
		if(offset >= 0 && offset <= OrgSize)
		{
			SeekToPosition(newpos);
		}
		return CurPos;
	}
	return CurPos;
}
//---------------------------------------------------------------------------
tjs_uint TJS_INTF_METHOD tTVPXP3ArchiveStream::Read(void *buffer, tjs_uint read_size)
{
	EnsureSegment();

	tjs_uint write_size = 0;
	while(read_size)
	{
		while(SegmentRemain == 0)
		{
			// must go next segment
			if(!OpenNextSegment()) // open next segment
				return write_size; // could not read more
		}

		tjs_uint one_size =
			read_size > SegmentRemain ? (tjs_uint)SegmentRemain : read_size;

		if(CurSegment->IsCompressed)
		{
			// compressed segment; read from uncompressed data in memory
			memcpy((tjs_uint8*)buffer + write_size,
				SegmentData->GetData() + (tjs_uint)SegmentPos, one_size);
		}
		else
		{
			// read directly from stream
			Stream->ReadBuffer((tjs_uint8*)buffer + write_size, one_size);
		}

		// execute filter (for encryption method)
		if(TVPXP3ArchiveExtractionFilter)
		{
			tTVPXP3ExtractionFilterInfo info(CurPos, (tjs_uint8*)buffer + write_size,
				one_size, Owner->GetFileHash(StorageIndex));
			TVPXP3ArchiveExtractionFilter
				( (tTVPXP3ExtractionFilterInfo*) &info );
		}

		// adjust members
		SegmentPos += one_size;
		CurPos += one_size;
		SegmentRemain -= one_size;
		read_size -= one_size;
		write_size += one_size;
	}

	return write_size;
}
//---------------------------------------------------------------------------
tjs_uint TJS_INTF_METHOD tTVPXP3ArchiveStream::Write(const void *buffer, tjs_uint write_size)
{
	return 0;
}
//---------------------------------------------------------------------------
tjs_uint64 TJS_INTF_METHOD tTVPXP3ArchiveStream::GetSize()
{
	return OrgSize;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// Archive extraction utility
//---------------------------------------------------------------------------
#define TVP_LOCAL_TEMP_COPY_BLOCK_SIZE 65536*2
#if 0
// this routine is obsoleted because the Releaser includes over 256-characters
// file name if the user specifies "protect over the archive" option.
// Windows cannot handle such too long filename.
void TVPExtractArchive(const ttstr & name, const ttstr & _destdir, bool allowextractprotected)
{
	// extract file to
	bool TVPAllowExtractProtectedStorage_save =
		TVPAllowExtractProtectedStorage;
	TVPAllowExtractProtectedStorage = allowextractprotected;
	try
	{

		ttstr destdir(_destdir);
		tjs_char last = _destdir.GetLastChar();
		if(_destdir.GetLen() >= 1 && (last != TJS_W('/') && last != TJS_W('\\')))
			destdir += TJS_W('/');

		tTVPArchive *arc = TVPOpenArchive(name);
		try
		{
			tjs_int count = arc->GetCount();
			for(tjs_int i = 0; i < count; i++)
			{
				ttstr name = arc->GetName(i);

				tTJSBinaryStream *src = arc->CreateStreamByIndex(i);
				try
				{
					tTVPStreamHolder dest(destdir + name, TJS_BS_WRITE);
					tjs_uint8 * buffer = new tjs_uint8[TVP_LOCAL_TEMP_COPY_BLOCK_SIZE];
					try
					{
						tjs_uint read;
						while(true)
						{
							read = src->Read(buffer, TVP_LOCAL_TEMP_COPY_BLOCK_SIZE);
							if(read == 0) break;
							dest->WriteBuffer(buffer, read);
						}
					}
					catch(...)
					{
						delete [] buffer;
						throw;
					}
					delete [] buffer;
				}
				catch(...)
				{
//					delete src;
//					throw;
				}
				delete src;
			}

		}
		catch(...)
		{
			arc->Release();
			throw;
		}

		arc->Release();
	}
	catch(...)
	{
		TVPAllowExtractProtectedStorage =
			TVPAllowExtractProtectedStorage_save;
		throw;
	}
	TVPAllowExtractProtectedStorage =
		TVPAllowExtractProtectedStorage_save;
}
#endif
//---------------------------------------------------------------------------

