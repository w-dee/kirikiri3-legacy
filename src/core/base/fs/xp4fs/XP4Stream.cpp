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

#include "XP4FS.h"
#include "XP4Archive.h"

#include <zlib.h>
#include <algorithm>




//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTVPXP4ArchiveStream::tTVPXP4ArchiveStream(
			boost::shared_ptr<tTVPArchive> ptr,
			tjs_size idx, tjs_uint32 flags)
				: Owner(ptr) , FileIndex(idx),
				FileInfo(ptr->GetFileInfo()),
				SegmentInfo(ptr->GetSegmentInfo())
{
	CurSegmentNum = 0;
	LastOpenedSegmentNum = -1;
	SegmentOpened = false;
	CurPos = 0;
	SegmentRemain = 0;
	SegmentPos = 0;

	tTVPXP4StreamCache::pointer streamcache;
	Stream = streamcache->GetStream(Owner.get(), ptr->GetFileName());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPXP4ArchiveStream::~tTVPXP4ArchiveStream()
{
	volatile tTJSCriticalSectionHolder holder(CS);

	tTVPXP4StreamCache::pointer streamcache;
	streamcache->Release(Owner.get(), Stream);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		現在のセグメントが開かれていることを確実にする
//---------------------------------------------------------------------------
void tTVPXP4ArchiveStream::EnsureSegment()
{
	// ensure accessing to current segment
	if(SegmentOpened) return; // すでにセグメントが開かれている場合は何もしない

	// 最後に開かれたセグメントが現在のセグメントと同じ?
	if(LastOpenedSegmentNum == CurSegmentNum)
	{
		// セグメントが圧縮されていない場合はストリームをシークして返る
		if(!Segments[CurSegmentNum].IsCompressed())
			Stream->SetPosition(Segments[CurSegmentNum].StoreSize + SegmentPos);
		return;
	}

	// is compressed segment ?
	if(Segments[CurSegmentNum].IsCompressed)
	{
		// a compressed segment
		tTVPXP4SegmentCache::pointer segmentcache;

		// セグメントキャッシュの中から探す
		DecompressedData = segmentcache->Find(
			Owner.get(), FileIndex, CurSegmentNum,
			Stream, Segments[CurSegmentNum].StoreOffset,
			Segments[CurSegmentNum].StoreSize,
			Segments[CurSegmentNum].Size);
	}
	else
	{
		// not a compressed segment
		Stream->SetPosition(Segments[CurSegmentNum].StoreSize + SegmentPos);
	}

	SegmentOpened = true;
	LastOpenedSegmentNum = CurSegmentNum;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定位置にシークする
//! @param		pos: シーク先の位置
//! @note		この関数は内部状態を変えるだけであり、実際にセグメントを開くなどはしない
//---------------------------------------------------------------------------
void tTVPXP4ArchiveStream::SeekToPosition(tjs_uint64 pos)
{
	// open segment at 'pos' and seek
	// pos must between zero thru OrgSize
	if(CurPos == pos) return;

	// do binary search to determine current segment number
	tjs_size st = 0;
	tjs_size et = FileInfo.SegmentCount;
	tjs_size seg_num;

	while(true)
	{
		if(et-st <= 1) { seg_num = st; break; }
		tjs_int m = st + (et-st)/2;
		if(SegmentInfo[m].Offset > pos)
			et = m;
		else
			st = m;
	}

	CurSegmentNum = seg_num;
	SegmentOpened = false;

	SegmentPos = pos - SegmentInfo[CurSegmentNum].Offset;
	SegmentRemain = SegmentInfo[CurSegmentNum].OrgSize - SegmentPos;
	CurPos = pos;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		次のセグメントを開く
//---------------------------------------------------------------------------
bool tTVPXP4ArchiveStream::OpenNextSegment()
{
	// open next segment
	if(CurSegmentNum == FileInfo.SegmentCount)
		return false; // no more segments
	CurSegmentNum ++;
	SegmentOpened = false;
	SegmentPos = 0;
	SegmentRemain = SegmentInfo[CurSegmentNum].Size;
	CurPos = SegmentInfo[CurSegmentNum].Offset;
	EnsureSegment();
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		シーク
//---------------------------------------------------------------------------
tjs_uint64 tTVPXP4ArchiveStream::Seek(tjs_int64 offset, tjs_int whence)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	tjs_int64 newpos;
	switch(whence)
	{
	case TJS_BS_SEEK_SET:
		newpos = offset;
		if(offset >= 0 && offset <= FileInfo.Size)
		{
			SeekToPosition(newpos);
		}
		return CurPos;

	case TJS_BS_SEEK_CUR:
		newpos = offset + CurPos;
		if(offset >= 0 && offset <= FileInfo.Size)
		{
			SeekToPosition(newpos);
		}
		return CurPos;

	case TJS_BS_SEEK_END:
		newpos = offset + FileInfo.Size;
		if(offset >= 0 && offset <= FileInfo.Size)
		{
			SeekToPosition(newpos);
		}
		return CurPos;
	}
	return CurPos;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		読み込み
//---------------------------------------------------------------------------
tjs_uint tTVPXP4ArchiveStream::Read(void *buffer, tjs_size read_size)
{
	volatile tTJSCriticalSectionHolder holder(CS);

	EnsureSegment();

	tjs_size write_size = 0;
	while(read_size)
	{
		while(SegmentRemain == 0)
		{
			// must go next segment
			if(!OpenNextSegment()) // open next segment
				return write_size; // could not read more
		}

		tjs_size one_size =
			read_size > SegmentRemain ? static_cast<tjs_uint>(SegmentRemain) : read_size;

		if(Segments[CurSegmentNum].IsCompressed())
		{
			// compressed segment; read from uncompressed data in memory
			memcpy((tjs_uint8*)buffer + write_size,
				DecompressedData->GetData() + static_cast<tjs_size>(SegmentPos), one_size);
		}
		else
		{
			// read directly from stream
			Stream->ReadBuffer(reinterpret_cast<tjs_uint8*>(buffer) + write_size, one_size);
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


//---------------------------------------------------------------------------
//! @brief		書き込み
//---------------------------------------------------------------------------
tjs_uint tTVPXP4ArchiveStream::Write(const void *buffer, tjs_size write_size)
{
	TVPThrowExceptionMessage(_("access denied (filesystem is read-only)"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルの終わりを現在のポインタに設定する
//---------------------------------------------------------------------------
void tTVPMemoryStream::SetEndOfFile()
{
	TVPThrowExceptionMessage(_("access denied (filesystem is read-only)"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		サイズを得る
//---------------------------------------------------------------------------
tjs_uint64 tTVPXP4ArchiveStream::GetSize()
{
	return FileInfo.Size;
}
//---------------------------------------------------------------------------

