//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4 アーカイブの実装
//---------------------------------------------------------------------------
#ifndef _XP4ARCHIVE_
#define _XP4ARCHIVE_

#include "XP4FS.h"


//---------------------------------------------------------------------------
//! @brief		アーカイブ内のストレージアイテムを現すクラス
//---------------------------------------------------------------------------
class tTVPXP4MetadataReaderStorageItem
{

public:
	tTVPXP4MetadataReaderStorageItem(
		const wxString & inarchivename,
		wxUint16 flags = 0,
		wxFileOffset size = -1,
		const wxDateTime & time = wxDateTime()) :
		InArchiveName(inarchivename),
		Flags(flags),
		Size(size),
		Time(time)
		{;} //!< コンストラクタ

	tTVPXP4MetadataReaderStorageItem(const unsigned char * meta, size_t metasize);

	const wxString & GetInArchiveName() const { return InArchiveName; } //!< ストレージ名を得る
	wxFileOffset GetSize() const { return Size; } //!< サイズを得る
	wxUint16 GetFlags() const { return Flags; } //!< フラグを得る
	const wxDateTime & GetTime() const { return Time; } //!< タイムスタンプを得る
	void SetFlags(wxUint16 flags) { Flags = flags; } 
	const tTVPXP4Hash & GetHash() const { return Hash; } //!< ハッシュを得る
	tTVPXP4Hash & GetHash() { return Hash; } //!< ハッシュを得る
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		一つのアーカイブを現すクラス
//---------------------------------------------------------------------------
class tTVPXP4Archive
{
public:
	struct tFile
	{
		tjs_uint64 Flags; //!< フラグ
		tjs_uint64 Size; //!< (無圧縮時の)ファイルサイズ
		tjs_size SegmentStart; //!< tTVPXP4Archiveインスタンス内のセグメント配列の開始インデックス
		tjs_size SegmentCount; //!< tTVPXP4Archiveインスタンス内のセグメント配列の個数
		wxDateTime Time; //!< タイムスタンプ
		tjs_uint8 Hash[20]; //!< SHA1 ハッシュ

		tFile(tTVPXP4Archive *owner, const unsigned char * meta,
							size_t metasize, ttstr & inarchivename, bool &delete);
	};

	struct tSement
	{
		tjs_uint8  Flags; //!< フラグ
		tjs_uint64 Offset; //!< (非圧縮時の)ストレージ先頭からのオフセット
		tjs_uint64 Size; //!< (非圧縮時の)サイズ
		tjs_uint64 StoreOffset; //!< (実際に格納されている)オフセット
		tjs_uint64 StoreSize; //!< (実際に格納されている)サイズ  無圧縮の場合は Size と同じ
	};

private:
	std::vector<tFile> Files; //!< ファイルの配列
	std::vector<tSegment> Segments; //!< セグメントの配列

public:
	tTVPXP4Archive(const ttstr & filename, tTVPXP4FS::iMapCallback & callback);

	void Stat(tjs_size idx, tTVPStatStruc & struc);
	tTVPBinaryStream * CreateStream(
				boost::shared_ptr<tTVPArchive> ptr,
				tjs_size idx, tjs_uint32 flags);


	std::vector<tSegment> & GetSegments() { return Segments; } // Segments を返す
};
//---------------------------------------------------------------------------
#endif
