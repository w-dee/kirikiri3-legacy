//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
#include "base/utils/RisaThread.h"



//---------------------------------------------------------------------------
//! @brief		XP4 In-Archive Stream Implmentation
//---------------------------------------------------------------------------
class tRisaXP4ArchiveStream : public tBinaryStream,
	protected depends_on<tRisaXP4StreamCache>,
	protected depends_on<tRisaXP4SegmentCache>
{
	tRisaCriticalSection CS; //!< このファイルシステムを保護するクリティカルセクション

	boost::shared_ptr<tRisaXP4Archive> Owner; //!< このアーカイブストリームが属するアーカイブ
	risse_size FileIndex; //!< アーカイブ中でのインデックス
	const tRisaXP4Archive::tFile & FileInfo; //!< ファイル情報
	const tRisaXP4Archive::tSegment * SegmentInfo; //!< セグメント情報

	tBinaryStream * Stream; //!< 内容にアクセスするためのバイナリストリーム
	risse_size CurSegmentNum; //!< 現在のファイルポインタのあるセグメント番号(0～)
	risse_size LastOpenedSegmentNum; //!< 最後に開いていたセグメント番号(0～)
	bool SegmentOpened; //!< セグメントが開かれているかどうか
	risse_uint64 CurPos; //!< current position in absolute file position
	risse_uint64 SegmentRemain; //!< remain bytes in current segment
	risse_uint64 SegmentPos; //!< offset from current segment's start
	tRisaXP4SegmentCache::tDataBlock DecompressedData; // decompressed segment data

public:
	//! @brief		コンストラクタ
	//! @param		ptr tRisaArchive インスタンスへのスマートポインタ
	//! @param		idx アーカイブ内でのファイルのインデックス
	//! @param		flags アクセスフラグ
	tRisaXP4ArchiveStream(
			boost::shared_ptr<tRisaXP4Archive> ptr,
			risse_size idx, risse_uint32 flags);

	//! @brief		デストラクタ
	~tRisaXP4ArchiveStream();

private:
	//! @brief		現在のセグメントが開かれていることを確実にする
	void EnsureSegment(); // ensure accessing to current segment

	//! @brief		指定位置にシークする
	//! @param		pos シーク先の位置
	//! @note		この関数は内部状態を変えるだけであり、実際にセグメントを開くなどはしない
	void SeekToPosition(risse_uint64 pos); // open segment at 'pos' and seek

	//! @brief		次のセグメントを開く
	bool OpenNextSegment();

public:
	//! @brief		シーク
	//! @param		offset 移動オフセット
	//! @param		whence 移動オフセットの基準 (RISSE_BS_SEEK_* 定数)
	//! @return		移動後のファイルポインタ
	risse_uint64 Seek(risse_int64 offset, risse_int whence);

	//! @brief		読み込み
	//! @param		buffer 読み込み先バッファ
	//! @param		read_size 読み込むバイト数
	//! @return		実際に読み込まれたバイト数
	risse_size Read(void *buffer, risse_size read_size);

	//! @brief		書き込み
	//! @param		buffer 書き込むバッファ
	//! @param		read_size 書き込みたいバイト数
	//! @return		実際に書き込まれたバイト数
	risse_size Write(const void *buffer, risse_size write_size);

	//! @brief		ファイルの終わりを現在のポインタに設定する
	void SetEndOfFile();

	//! @brief		サイズを得る
	//! @return		このストリームのサイズ
	risse_uint64 GetSize();
};
//---------------------------------------------------------------------------





#endif
