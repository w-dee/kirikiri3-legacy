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
#include "prec.h"
#include "base/fs/xp4fs/XP4FS.h"
#include "base/fs/xp4fs/XP4Archive.h"
#include "base/fs/xp4fs/XP4Stream.h"
#include "base/fs/xp4fs/XP4StreamCache.h"
#include "base/exception/RisaException.h"
#include <zlib.h>
#include <algorithm>


RISSE_DEFINE_SOURCE_ID(9282,1527,60023,18507,43687,60046,2141,35691);


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		ptr tRisaArchive インスタンスへのスマートポインタ
//! @param		idx アーカイブ内でのファイルのインデックス
//! @param		flags アクセスフラグ
//---------------------------------------------------------------------------
tRisaXP4ArchiveStream::tRisaXP4ArchiveStream(
			boost::shared_ptr<tRisaXP4Archive> ptr,
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

	Stream = tRisaXP4StreamCache::instance()->GetStream(Owner.get(), ptr->GetFileName());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaXP4ArchiveStream::~tRisaXP4ArchiveStream()
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	tRisaXP4StreamCache::instance()->ReleaseStream(Owner.get(), Stream);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		現在のセグメントが開かれていることを確実にする
//---------------------------------------------------------------------------
void tRisaXP4ArchiveStream::EnsureSegment()
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
		DecompressedData = tRisaXP4SegmentCache::instance()->Find(
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
//! @brief		指定位置にシークする
//! @param		pos シーク先の位置
//! @note		この関数は内部状態を変えるだけであり、実際にセグメントを開くなどはしない
//---------------------------------------------------------------------------
void tRisaXP4ArchiveStream::SeekToPosition(risse_uint64 pos)
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
//! @brief		次のセグメントを開く
//---------------------------------------------------------------------------
bool tRisaXP4ArchiveStream::OpenNextSegment()
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
//! @param		offset 移動オフセット
//! @param		whence 移動オフセットの基準 (RISSE_BS_SEEK_* 定数)
//! @return		移動後のファイルポインタ
//---------------------------------------------------------------------------
risse_uint64 tRisaXP4ArchiveStream::Seek(risse_int64 offset, risse_int whence)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

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
//! @brief		読み込み
//! @param		buffer 読み込み先バッファ
//! @param		read_size 読み込むバイト数
//! @return		実際に読み込まれたバイト数
//---------------------------------------------------------------------------
risse_uint tRisaXP4ArchiveStream::Read(void *buffer, risse_size read_size)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

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
			Stream->ReadBuffer(reinterpret_cast<risse_uint8*>(buffer) + write_size, one_size);
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
//! @param		buffer 書き込むバッファ
//! @param		read_size 書き込みたいバイト数
//! @return		実際に書き込まれたバイト数
//---------------------------------------------------------------------------
risse_uint tRisaXP4ArchiveStream::Write(const void *buffer, risse_size write_size)
{
	eRisaException::Throw(RISSE_WS_TR("access denied (filesystem is read-only)"));
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルの終わりを現在のポインタに設定する
//---------------------------------------------------------------------------
void tRisaXP4ArchiveStream::SetEndOfFile()
{
	eRisaException::Throw(RISSE_WS_TR("access denied (filesystem is read-only)"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		サイズを得る
//! @return		このストリームのサイズ
//---------------------------------------------------------------------------
risse_uint64 tRisaXP4ArchiveStream::GetSize()
{
	return FileInfo.Size;
}
//---------------------------------------------------------------------------

