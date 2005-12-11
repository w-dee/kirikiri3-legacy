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

#ifndef XP4FSH
#define XP4FSH

#include <boost/shared_ptr.hpp>
#include "XP4Archive.h"



//---------------------------------------------------------------------------
//! @brief		XP4FS の実装
//---------------------------------------------------------------------------
class tTVPXP4FS : public iTVPFileSystem
{
	tTJSCriticalSection CS; //!< このファイルシステムを保護するクリティカルセクション

	//! @brief ファイル一個一個の情報
	struct tFileItemBasicInfo
	{
		tjs_size ArchiveIndex; //!< どのアーカイブに含まれるか
		tjs_size FileIndex; //!< そのアーカイブ内でのファイルのインデックス
	};

	//! @brief 
	struct tFileItemInfo : public tFileItemBasicInfo
	{
		ttstr Name; //!< 名前
		tFileItemInfo(const tFileItemBasicInfo & info, const ttstr & name) :
			tFileItemBasicInfo(info), Name(name) {;} //!< コンストラクタ
		bool operator < (const tFileItemInfo & rhs) const 
			{ return Name < rhs.Name; } //!< 比較関数
	};

	std::vector<boost::shared_ptr<tTVPArchive> > Archives; //!< アーカイブの配列
	std::vector<tFileItemInfo> FileItems; //! ファイルの情報の配列

public:
	//! @brief アーカイブを読み込む際に各アーカイブ内のファイルごとに呼ばれるコールバック
	class iMapCallback
	{
		virtual void operator () (const ttstr & name, tjs_size file_index) = 0; //!< 追加/置き換えの場合
		virtual void operator () (const ttstr & name) = 0; //!< 削除の場合
	};


public:
	tTVPXP4FS(const ttstr & name);

	//-- iTVPFileSystem メンバ
	~tTVPXP4FS();

	size_t GetFileListAt(const ttstr & dirname,
		iTVPFileSystemIterationCallback * callback);
	bool FileExists(const ttstr & filename);
	bool DirectoryExists(const ttstr & dirname);
	void RemoveFile(const ttstr & filename);
	void RemoveDirectory(const ttstr & dirname, bool recursive = false);
	void CreateDirectory(const ttstr & dirname, bool recursive = false);
	void Stat(const ttstr & filename, tTVPStatStruc & struc);
	tTVPBinaryStream * CreateStream(const ttstr & filename, tjs_uint32 flags);

	//-- iTVPFileSystem メンバ ここまで
private:
	tjs_size GetFileItemStartIndex(const ttstr & name);
	tjs_size GetFileItemIndex(const ttstr & name);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// tTVPXP3Archive  : XP3 ( TVP's native archive format ) Implmentation
//---------------------------------------------------------------------------
#define TVP_XP3_INDEX_ENCODE_METHOD_MASK 0x07
#define TVP_XP3_INDEX_ENCODE_RAW      0
#define TVP_XP3_INDEX_ENCODE_ZLIB     1

#define TVP_XP3_INDEX_CONTINUE   0x80

#define TVP_XP3_FILE_PROTECTED (1<<31)

#define TVP_XP3_SEGM_ENCODE_METHOD_MASK  0x07
#define TVP_XP3_SEGM_ENCODE_RAW       0
#define TVP_XP3_SEGM_ENCODE_ZLIB      1

//---------------------------------------------------------------------------
extern bool TVPIsXP3Archive(const ttstr &name); // check XP3 archive
extern void TVPClearXP3SegmentCache(); // clear XP3 segment cache
//---------------------------------------------------------------------------
struct tTVPXP3ArchiveSegment
{
	tjs_uint64 Start;  // start position in archive storage
	tjs_uint64 Offset; // offset in in-archive storage (in uncompressed offset)
	tjs_uint64 OrgSize; // original segment (uncompressed) size
	tjs_uint64 ArcSize; // in-archive segment (compressed) size
	bool IsCompressed; // is compressed ?
};
//---------------------------------------------------------------------------
class tTVPXP3Archive : public tTVPArchive
{
	ttstr Name;

	struct tArchiveItem
	{
		ttstr Name;
		tjs_uint32 FileHash;
		tjs_uint64 OrgSize; // original ( uncompressed ) size
		tjs_uint64 ArcSize; // in-archive size
		std::vector<tTVPXP3ArchiveSegment> Segments;
		bool operator < (const tArchiveItem &rhs) const
		{
			return this->Name < rhs.Name;
		}
	};

	tjs_int Count;

	std::vector<tArchiveItem> ItemVector;
public:
	tTVPXP3Archive(const ttstr & name);
	~tTVPXP3Archive();

	tjs_uint GetCount() { return Count; }
	const ttstr & GetName(tjs_uint idx) const { return ItemVector[idx].Name; }
	tjs_uint32 GetFileHash(tjs_uint idx) const { return ItemVector[idx].FileHash; }
	ttstr GetName(tjs_uint idx) { return ItemVector[idx].Name; }

	const ttstr & GetName() const { return Name; }

	tTJSBinaryStream * CreateStreamByIndex(tjs_uint idx);

private:
	static bool FindChunk(const tjs_uint8 *data, const tjs_uint8 * name,
		tjs_uint &start, tjs_uint &size);
	static tjs_int16 ReadI16FromMem(const tjs_uint8 *mem);
	static tjs_int32 ReadI32FromMem(const tjs_uint8 *mem);
	static tjs_int64 ReadI64FromMem(const tjs_uint8 *mem);
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tTVPXP3ArchiveStream  : XP3 In-Archive Stream Implmentation
//---------------------------------------------------------------------------
class tTVPSegmentData;
class tTVPXP3ArchiveStream : public tTJSBinaryStream
{
	tTVPXP3Archive * Owner;

	tjs_int StorageIndex; // index in archive

	std::vector<tTVPXP3ArchiveSegment> * Segments;
	tTJSBinaryStream * Stream;
	tjs_uint64 OrgSize; // original storage size

	tjs_int CurSegmentNum;
	tTVPXP3ArchiveSegment *CurSegment;
		// currently opened segment ( NULL for not opened )

	tjs_int LastOpenedSegmentNum;

	tjs_uint64 CurPos; // current position in absolute file position

	tjs_uint64 SegmentRemain; // remain bytes in current segment
	tjs_uint64 SegmentPos; // offset from current segment's start

	tTVPSegmentData *SegmentData; // uncompressed segment data

	bool SegmentOpened;

public:
	tTVPXP3ArchiveStream(tTVPXP3Archive *owner, tjs_int storageindex,
		std::vector<tTVPXP3ArchiveSegment> *segments, tTJSBinaryStream *stream,
			tjs_uint64 orgsize);
	~tTVPXP3ArchiveStream();

private:
	void EnsureSegment(); // ensure accessing to current segment
	void SeekToPosition(tjs_uint64 pos); // open segment at 'pos' and seek
	bool OpenNextSegment();


public:
	tjs_uint64 Seek(tjs_int64 offset, tjs_int whence);
	tjs_size Read(void *buffer, tjs_size read_size);
	tjs_size Write(const void *buffer, tjs_size write_size);
	void SetEndOfFile();
	tjs_uint64 GetSize():
};
//---------------------------------------------------------------------------





#endif
