//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4ファイルシステムで用いられるストリームクラス
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/fs/xp4fs/XP4FS.h"
#include "base/fs/xp4fs/XP4Archive.h"
#include "base/fs/xp4fs/XP4Stream.h"
#include "base/fs/xp4fs/XP4StreamCache.h"
#include "base/exception/RisaException.h"
#include <zlib.h>
#include <algorithm>

namespace Risa {
RISSE_DEFINE_SOURCE_ID(9282,1527,60023,18507,43687,60046,2141,35691);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tXP4ArchiveStream::tXP4ArchiveStream(
			boost::shared_ptr<tXP4Archive> ptr,
			risse_size idx, risse_uint32 flags)
				: Owner(ptr) , FileIndex(idx),
				FileInfo(ptr->GetFileInfo(idx)),
				SegmentInfo(ptr->GetSegmentInfo(idx))
{
	CurSegmentNum = 0;
	LastOpenedSegmentNum = static_cast<risse_size>(-1);
	SegmentOpened = false;
	CurPos = 0;
	SegmentRemain = 0;
	SegmentPos = 0;

	Stream = depends_on<tXP4StreamCache>::locked_instance()->GetStream(Owner.get(), ptr->GetFileName());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tXP4ArchiveStream::~tXP4ArchiveStream()
{
	volatile tCriticalSection::tLocker holder(CS);

	depends_on<tXP4StreamCache>::locked_instance()->ReleaseStream(Owner.get(), Stream);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tXP4ArchiveStream::EnsureSegment()
{
	// ensure accessing to current segment
	if(SegmentOpened) return; // すでにセグメントが開かれている場合は何もしない

	// 最後に開かれたセグメントが現在のセグメントと同じ?
	if(LastOpenedSegmentNum == CurSegmentNum)
	{
		// セグメントが圧縮されていない場合はストリームをシークして返る
		if(!SegmentInfo[CurSegmentNum].IsCompressed())
			Stream->SetPosition(SegmentInfo[CurSegmentNum].StoreSize + SegmentPos);
		return;
	}

	// is compressed segment ?
	if(SegmentInfo[CurSegmentNum].IsCompressed())
	{
		// a compressed segment
		// セグメントキャッシュの中から探す
		DecompressedData = depends_on<tXP4SegmentCache>::locked_instance()->Find(
			Owner.get(), FileIndex, CurSegmentNum,
			Stream, SegmentInfo[CurSegmentNum].StoreOffset,
			SegmentInfo[CurSegmentNum].StoreSize,
			SegmentInfo[CurSegmentNum].Size);
	}
	else
	{
		// not a compressed segment
		Stream->SetPosition(SegmentInfo[CurSegmentNum].StoreSize + SegmentPos);
	}

	SegmentOpened = true;
	LastOpenedSegmentNum = CurSegmentNum;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tXP4ArchiveStream::SeekToPosition(risse_uint64 pos)
{
	// open segment at 'pos' and seek
	// pos must between zero thru OrgSize
	if(CurPos == pos) return;

	// do binary search to determine current segment number
	risse_size st = 0;
	risse_size et = FileInfo.SegmentCount;
	risse_size seg_num;

	while(true)
	{
		if(et-st <= 1) { seg_num = st; break; }
		risse_int m = st + (et-st)/2;
		if(SegmentInfo[m].Offset > pos)
			et = m;
		else
			st = m;
	}

	CurSegmentNum = seg_num;
	SegmentOpened = false;

	SegmentPos = pos - SegmentInfo[CurSegmentNum].Offset;
	SegmentRemain = SegmentInfo[CurSegmentNum].Size - SegmentPos;
	CurPos = pos;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tXP4ArchiveStream::OpenNextSegment()
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
risse_uint64 tXP4ArchiveStream::Seek(risse_int64 offset, risse_int whence)
{
	volatile tCriticalSection::tLocker holder(CS);

	risse_int64 newpos;
	switch(whence)
	{
	case RISSE_BS_SEEK_SET:
		newpos = offset;
		if(offset >= 0 && offset <= static_cast<risse_int64>(FileInfo.Size))
		{
			SeekToPosition(newpos);
		}
		return CurPos;

	case RISSE_BS_SEEK_CUR:
		newpos = offset + CurPos;
		if(offset >= 0 && offset <= static_cast<risse_int64>(FileInfo.Size))
		{
			SeekToPosition(newpos);
		}
		return CurPos;

	case RISSE_BS_SEEK_END:
		newpos = offset + FileInfo.Size;
		if(offset >= 0 && offset <= static_cast<risse_int64>(FileInfo.Size))
		{
			SeekToPosition(newpos);
		}
		return CurPos;
	}
	return CurPos;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint tXP4ArchiveStream::Read(void *buffer, risse_size read_size)
{
	volatile tCriticalSection::tLocker holder(CS);

	EnsureSegment();

	risse_size write_size = 0;
	while(read_size)
	{
		while(SegmentRemain == 0)
		{
			// must go next segment
			if(!OpenNextSegment()) // open next segment
				return write_size; // could not read more
		}

		risse_size one_size =
			read_size > SegmentRemain ? static_cast<risse_uint>(SegmentRemain) : read_size;

		if(SegmentInfo[CurSegmentNum].IsCompressed())
		{
			// compressed segment; read from uncompressed data in memory
			memcpy((risse_uint8*)buffer + write_size,
				DecompressedData->GetData() + static_cast<risse_size>(SegmentPos), one_size);
		}
		else
		{
			// read directly from stream
			Stream->ReadBuffer(static_cast<risse_uint8*>(buffer) + write_size, one_size);
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
risse_uint tXP4ArchiveStream::Write(const void *buffer, risse_size write_size)
{
	eRisaException::Throw(RISSE_WS_TR("access denied (filesystem is read-only)"));
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tXP4ArchiveStream::SetEndOfFile()
{
	eRisaException::Throw(RISSE_WS_TR("access denied (filesystem is read-only)"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tXP4ArchiveStream::GetSize()
{
	return FileInfo.Size;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa

