//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4ファイルシステムで用いられるストリームクラス
//---------------------------------------------------------------------------

#ifndef XP3ArchiveH
#define XP3ArchiveH

#include <boost/shared_ptr.hpp>
#include "XP4FS.h"
#include "XP4SegmentCache.h"



//---------------------------------------------------------------------------
//! @brief		XP4 In-Archive Stream Implmentation
//---------------------------------------------------------------------------
class tTVPXP3ArchiveStream : public tTJSBinaryStream
{
	boost::weak_ptr<tTVPXP4FS> Owner; //!< このアーカイブストリームが属するアーカイブ
	tjs_size FileIndex; //!< アーカイブ中でのインデックス
	tTJSBinaryStream * Stream; //!< 内容にアクセスするためのバイナリストリーム
	tjs_uint64 OrgSize; //!< (無圧縮状態での) ストリーム全体のサイズ
	tjs_size CurSegmentNum; //!< 現在のファイルポインタのあるセグメント番号
	tTVPXP3ArchiveSegment *CurSegment; //!< 現在開いているセグメント (NULLの場合はまだ開いていない)
	tjs_int LastOpenedSegmentNum; //!< 最後に開いていたセグメント番号
	tjs_uint64 CurPos; //!< current position in absolute file position
	tjs_uint64 SegmentRemain; //!< remain bytes in current segment
	tjs_uint64 SegmentPos; //!< offset from current segment's start
	boost::shared_ptr<tTVPDecompressedHolder> DecompressedData; // decompressed segment data

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
