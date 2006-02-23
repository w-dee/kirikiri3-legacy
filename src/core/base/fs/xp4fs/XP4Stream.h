//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4ファイルシステムで用いられるストリームクラス
//---------------------------------------------------------------------------

#ifndef XP4STREAMH
#define XP4STREAMH

#include <boost/shared_ptr.hpp>
#include "base/fs/xp4fs/XP4FS.h"
#include "base/fs/xp4fs/XP4SegmentCache.h"
#include "base/utils/Thread.h"



//---------------------------------------------------------------------------
//! @brief		XP4 In-Archive Stream Implmentation
//---------------------------------------------------------------------------
class tRisaXP4ArchiveStream : public tRisseBinaryStream
{
	tRisaCriticalSection CS; //!< このファイルシステムを保護するクリティカルセクション

	boost::shared_ptr<tRisaXP4Archive> Owner; //!< このアーカイブストリームが属するアーカイブ
	risse_size FileIndex; //!< アーカイブ中でのインデックス
	const tRisaXP4Archive::tFile & FileInfo; //!< ファイル情報
	const tRisaXP4Archive::tSegment * SegmentInfo; //!< セグメント情報

	tRisseBinaryStream * Stream; //!< 内容にアクセスするためのバイナリストリーム
	risse_size CurSegmentNum; //!< 現在のファイルポインタのあるセグメント番号(0～)
	risse_size LastOpenedSegmentNum; //!< 最後に開いていたセグメント番号(0～)
	bool SegmentOpened; //!< セグメントが開かれているかどうか
	risse_uint64 CurPos; //!< current position in absolute file position
	risse_uint64 SegmentRemain; //!< remain bytes in current segment
	risse_uint64 SegmentPos; //!< offset from current segment's start
	tRisaXP4SegmentCache::tDataBlock DecompressedData; // decompressed segment data

public:
	tRisaXP4ArchiveStream(
			boost::shared_ptr<tRisaXP4Archive> ptr,
			risse_size idx, risse_uint32 flags);
	~tRisaXP4ArchiveStream();

private:
	void EnsureSegment(); // ensure accessing to current segment
	void SeekToPosition(risse_uint64 pos); // open segment at 'pos' and seek
	bool OpenNextSegment();

public:
	risse_uint64 Seek(risse_int64 offset, risse_int whence);
	risse_size Read(void *buffer, risse_size read_size);
	risse_size Write(const void *buffer, risse_size write_size);
	void SetEndOfFile();
	risse_uint64 GetSize();

};
//---------------------------------------------------------------------------





#endif
