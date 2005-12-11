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
class tTVPXP4ArchiveStream : public tTJSBinaryStream
{
	boost::shared_ptr<tTVPXP4Archive> Owner; //!< このアーカイブストリームが属するアーカイブ
	tjs_size FileIndex; //!< アーカイブ中でのインデックス
	tTJSBinaryStream * Stream; //!< 内容にアクセスするためのバイナリストリーム

	tjs_size CurSegmentNum; //!< 現在のファイルポインタのあるセグメント番号(0～)
	tjs_size LastOpenedSegmentNum; //!< 最後に開いていたセグメント番号(0～)
	tjs_uint64 CurPos; //!< current position in absolute file position
	tjs_uint64 SegmentRemain; //!< remain bytes in current segment
	tjs_uint64 SegmentPos; //!< offset from current segment's start
	boost::shared_ptr<tTVPDecompressedHolder> DecompressedData; // decompressed segment data

public:
	tTVPXP4ArchiveStream(
			boost::shared_ptr<tTVPArchive> ptr,
			tjs_size idx, tjs_uint32 flags);
	~tTVPXP4ArchiveStream();

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
