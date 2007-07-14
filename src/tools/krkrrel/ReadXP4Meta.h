//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4 メタデータの読み込み
//---------------------------------------------------------------------------
#ifndef _READXP4META_H_
#define _READXP4META_H_

#include "ProgressCallback.h"
#include "XP4Hash.h"
/*
	ここでは、XP4 アーカイブファイルのうち、アーカイブ内ファイルのファイル名と
	タイムスタンプ、フラグを得るだけを目的とする。
	実際のRisaで使用されるルーチン群はパフォーマンスの最適化などで
	かなり複雑になるため、まったく別の実装とする。
*/

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		アーカイブ内のストレージアイテムを現すクラス
//---------------------------------------------------------------------------
class tXP4MetadataReaderStorageItem
{
protected:
	wxString InArchiveName; //!< ストレージ名
	wxUint16 Flags; //!< フラグ
	wxFileOffset Size; //!< (無圧縮時の)ファイルサイズ
	wxDateTime Time; //!< タイムスタンプ
	tXP4Hash Hash; //!< ハッシュ

public:
	tXP4MetadataReaderStorageItem(
		const wxString & inarchivename,
		wxUint16 flags = 0,
		wxFileOffset size = -1,
		const wxDateTime & time = wxDateTime()) :
		InArchiveName(inarchivename),
		Flags(flags),
		Size(size),
		Time(time)
		{;} //!< コンストラクタ

	tXP4MetadataReaderStorageItem(const unsigned char * meta, size_t metasize);

	const wxString & GetInArchiveName() const { return InArchiveName; } //!< ストレージ名を得る
	wxFileOffset GetSize() const { return Size; } //!< サイズを得る
	wxUint16 GetFlags() const { return Flags; } //!< フラグを得る
	const wxDateTime & GetTime() const { return Time; } //!< タイムスタンプを得る
	void SetFlags(wxUint16 flags) { Flags = flags; } 
	const tXP4Hash & GetHash() const { return Hash; } //!< ハッシュを得る
	tXP4Hash & GetHash() { return Hash; } //!< ハッシュを得る
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		一つのアーカイブを現すクラス
//---------------------------------------------------------------------------
class tXP4MetadataReaderArchive
{
	wxString TargetDir;
	std::vector<tXP4MetadataReaderStorageItem> ItemVector; //!< ストレージの配列

public:
	tXP4MetadataReaderArchive(const wxString & filename);

	const std::vector<tXP4MetadataReaderStorageItem> &
		GetItemVector() const { return ItemVector; } //!< ストレージの配列を得る

	const wxString & GetTargetDir() const
		{ return TargetDir; } //!< このアーカイブの元となった対象ディレクトリを得る
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa

#endif
