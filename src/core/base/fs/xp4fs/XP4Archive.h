//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4 アーカイブの実装
//---------------------------------------------------------------------------
#ifndef _XP4ARCHIVE_
#define _XP4ARCHIVE_

#include <wx/datetime.h>
#include "FSManager.h"
#include "XP4ArchiveDefs.h"
#include "XP4SegmentCache.h"
#include "XP4StreamCache.h"

//---------------------------------------------------------------------------
//! @brief		一つのアーカイブを現すクラス
//---------------------------------------------------------------------------
class tTVPXP4Archive
{
	tTVPSingleton<tTVPXP4SegmentCache> ref_tTVPXP4SegmentCache; // tTVPXP4SegmentCache に依存
	tTVPSingleton<tTVPXP4StreamCache> ref_tTVPXP4StreamCache; // tTVPXP4StreamCache に依存
public:
	struct tFile
	{
		risse_uint64 Flags; //!< フラグ
		risse_uint64 Size; //!< (無圧縮時の)ファイルサイズ
		risse_size SegmentStart; //!< tTVPXP4Archiveインスタンス内のセグメント配列の開始インデックス
		risse_size SegmentCount; //!< tTVPXP4Archiveインスタンス内のセグメント配列の個数
		wxDateTime Time; //!< タイムスタンプ
		risse_uint8 Hash[20]; //!< SHA1 ハッシュ

		tFile(tTVPXP4Archive *owner, const unsigned char * meta,
							size_t metasize, ttstr & inarchivename, bool &deleted);
	};

	struct tSegment
	{
		risse_uint8  Flags; //!< フラグ
		risse_uint64 Offset; //!< (非圧縮時の)ストレージ先頭からのオフセット
		risse_uint64 Size; //!< (非圧縮時の)サイズ
		risse_uint64 StoreOffset; //!< (実際に格納されている)オフセット
		risse_uint64 StoreSize; //!< (実際に格納されている)サイズ  無圧縮の場合は Size と同じ
		bool IsCompressed() const
			{ return (Flags & TVP_XP4_SEGM_ENCODE_METHOD_MASK) !=
				TVP_XP4_SEGM_ENCODE_RAW; } //!< セグメントが圧縮されている場合に真
	};

	//! @brief アーカイブを読み込む際に各アーカイブ内のファイルごとに呼ばれるコールバック
	class iMapCallback
	{
	public:
		virtual void operator () (const ttstr & name, risse_size file_index) = 0; //!< 追加/置き換えの場合
		virtual void operator () (const ttstr & name) = 0; //!< 削除の場合
	};

private:
	std::vector<tFile> Files; //!< ファイルの配列
	std::vector<tSegment> Segments; //!< セグメントの配列

	ttstr FileName;

public:
	tTVPXP4Archive(const ttstr & filename, iMapCallback & callback);
	~tTVPXP4Archive();

	void Stat(risse_size idx, tTVPStatStruc & struc);
	tRisseBinaryStream * CreateStream(
				boost::shared_ptr<tTVPXP4Archive> ptr,
				risse_size idx, risse_uint32 flags);


	std::vector<tSegment> & GetSegments()
		{ return Segments; } //!< Segments を返す
	const tFile & GetFileInfo(risse_size idx) const
		{ return Files[idx]; } //!< idx に対応する tFile 構造体を返す
	const tSegment * GetSegmentInfo(risse_size idx) const
		{ return &(Segments[Files[idx].SegmentStart]); } //!< idx に対応するセグメント情報を返す
	const ttstr & GetFileName() const
		{ return FileName; } //!< ファイル名を返す
};
//---------------------------------------------------------------------------
#endif
