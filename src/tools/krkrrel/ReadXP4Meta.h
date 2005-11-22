//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4 メタデータの読み込み
//---------------------------------------------------------------------------
#ifndef _READXP4META_H_
#define _READXP4META_H_

/*
	ここでは、XP4 アーカイブファイルのうち、アーカイブ内ファイルのファイル名と
	タイムスタンプ、フラグを得るだけを目的とする。
	実際の吉里吉里で使用されるルーチン群はパフォーマンスの最適化などで
	かなり複雑になるため、まったく別の実装とする。
*/

//---------------------------------------------------------------------------
//! @brief		アーカイブ内のストレージアイテムを現すクラス
//---------------------------------------------------------------------------
class tTVPXP4MetadataReaderStorageItem
{
protected:
	wxString InArchiveName; //!< ストレージ名
	wxUint16 Flags; //!< フラグ
	wxDateTime Time; //!< タイムスタンプ

public:
	tTVPXP4MetadataReaderStorageItem(
		const wxString & inarchivename,
		wxUint16 flags = 0,
		const wxDateTime & time = wxDateTime()) :
		InArchiveName(inarchivename),
		Flags(flags),
		Time(time)
		{;} //!< コンストラクタ

	tTVPXP4MetadataReaderStorageItem(const unsigned char * meta, size_t metasize);

	const wxString & GetInArchiveName() const { return InArchiveName; } //!< ストレージ名を得る
	const wxDateTime & GetTime() const { return Time; } //!< タイムスタンプを得る
	wxUint16 GetFlags() const { return Flags; } //!< フラグを得る
	void SetFlags(wxUint16 flags) { Flags = flags; } 
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		一つのアーカイブを現すクラス
//---------------------------------------------------------------------------
class tTVPXP4MetadataReaderArchive
{
	std::vector<tTVPXP4MetadataReaderStorageItem> ItemVector; //!< ストレージの配列

public:
	tTVPXP4MetadataReaderArchive(const wxString & filename);

	const std::vector<tTVPXP4MetadataReaderStorageItem> &
		GetItemVector() const { return ItemVector; } // ストレージの配列を得る
};
//---------------------------------------------------------------------------
#endif
