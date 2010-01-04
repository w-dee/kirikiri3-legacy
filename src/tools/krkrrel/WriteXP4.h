//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4 アーカイブファイルの書き込み
//---------------------------------------------------------------------------

#ifndef _WRITEXP4_H_
#define _WRITEXP4_H_

#include "ProgressCallback.h"
#include "wxFileWrapper.h"
#include "ReadXP4Meta.h"
#include "XP4Archive.h"

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * ストレージ内の各セグメントを表すクラス
 */
class tXP4WriterStorage;
class tXP4WriterSegment
{
	wxFileOffset Offset; //!< (非圧縮時の)ストレージ先頭からのオフセット
	wxFileOffset Size; //!< (非圧縮時の)サイズ
	bool IsCompressed; //!< ZLIB 圧縮の際に真

	wxFileOffset StoreOffset; //!< (実際に格納されている)オフセット
	wxFileOffset StoreSize; //!< (実際に格納されている)サイズ  無圧縮の場合は Size と同じ

public:
	tXP4WriterSegment(
		wxFileOffset offset,
		wxFileOffset size,
		bool iscompressed);

	~tXP4WriterSegment();

	wxFileOffset GetStoreOffset() const //!< (実際に格納された)オフセットを得る
		{ return StoreOffset; }
	wxFileOffset GetStoreSize() const //!< (実際に格納された)サイズを得る
		{ return StoreSize; }

	void WriteBody(iRisaProgressCallback * callback,
		wxFileEx & input, wxFileEx & file);

	void WriteMetaData(wxMemoryBuffer & buf);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * XP4アーカイブへ格納するファイルのアイテム
 */
class tXP4WriterInputFile : public tXP4MetadataReaderStorageItem
{
protected:
	wxString InputName; //!< 入力ファイル名(ベースディレクトリ名部分をのぞく)
	wxString BaseDirName; //!< 入力ファイルのベースディレクトリ名

public:
	tXP4WriterInputFile(
		const wxString & inarchivename,
		wxUint16 flags = 0,
		const wxDateTime & time = wxDateTime(),
		const wxString & inputname = wxEmptyString,
		const wxString & basedirname = wxEmptyString) :
				tXP4MetadataReaderStorageItem(
						inarchivename,
						flags,
						(flags & RISA_XP4_FILE_STATE_MASK) ==
								RISA_XP4_FILE_STATE_DELETED ?
							-1 : wxFileEx(basedirname + inputname).Length(),
						time),
				InputName(inputname),
				BaseDirName(basedirname)
				 {;}  //!< コンストラクタ
	tXP4WriterInputFile(const tXP4MetadataReaderStorageItem & ref) :
		tXP4MetadataReaderStorageItem(ref)
				 {;} //!< コンストラクタ

	bool operator < (const tXP4WriterInputFile & rhs) const
	{
		// 比較用演算子
		return InArchiveName < rhs.InArchiveName;
	}

	const wxString & GetInputName() const { return InputName; } //!< 入力ファイル名を得る
	const wxString & GetBaseDirName() const { return BaseDirName; } //!< 入力ファイルのベースディレクトリ名を得る

	wxString GetPath() const { return BaseDirName + InputName; } //!< 入力ファイルのフルパスを得る
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * アーカイブ内のストレージアイテムを表すクラス
 */
class tXP4WriterStorage : public tXP4WriterInputFile
{
	std::vector<tXP4WriterSegment> SegmentVector; //!< セグメントの配列
	bool IsReference; //!< 他のストレージアイテムを参照している場合は 真

public:
	tXP4WriterStorage(
		const tXP4WriterInputFile & inputfile);

	tXP4WriterStorage(
		const tXP4WriterInputFile & inputfile,
		const tXP4WriterStorage & ref);

	void MakeHash(iRisaProgressCallback * callback);

	void WriteBody(iRisaProgressCallback * callback, wxFileEx & archive);

	void WriteMetaData(wxMemoryBuffer & buf);

	/**
	 * ソート用比較関数
	 */
	bool operator < (const tXP4WriterStorage &rhs) const
	{
		// サイズにおいて降順でソートするための関数
		// サイズは 2MB 単位で比較する
		wxFileOffset thissize =     Size / (2*1024*1024l);
		wxFileOffset rhssize  = rhs.Size / (2*1024*1024l);
		return thissize > rhssize;
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * アーカイブを表すクラス
 */
class tXP4WriterArchive
{
	std::vector<tXP4WriterStorage> StorageVector; //!< このアーカイブ内に含まれるtXP4WriterStorage の配列
	wxFileOffset Size; //!< アーカイブのサイズ
	wxString FileName; //!< アーカイブファイル名
	wxString TargetDir; //!< アーカイブに格納されるファイルの元となったディレクトリ名
	bool ArchiveOk; //!< アーカイブが正常に作成されたかどうか

public:
	tXP4WriterArchive(const wxString & filename, const wxString & targetdir);
	~tXP4WriterArchive();
	wxFileOffset GetSize() const { return Size; } //!< アーカイブのファイルサイズを得る
	const tXP4WriterStorage & GetStorageItem(size_t idx)
		{ return StorageVector[idx]; } //!< 指定されたインデックスにあるストレージオブジェクトを得る
	size_t AddAndWriteBody(iRisaProgressCallback * callback,
		const tXP4WriterStorage & storage);
	void WriteMetaData(iRisaProgressCallback * callback, bool compress);
	void SetArchiveOk() { ArchiveOk = true; } //!< アーカイブを確定する
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * XP4アーカイブへの書き込みを管理するクラス
 */
class tXP4Writer
{
	iRisaProgressCallback * ProgressCallback; //!< コールバック用オブジェクト
	wxString BaseFileName;	//!< ベースファイル名(パス付きだが拡張子をのぞく)
	wxFileOffset  SplitLimit;	//!< 分割する際のファイルサイズの上限(バイト単位) 0=分割なし
	std::vector<tXP4WriterInputFile> List; //!< 入力ファイルのリスト
	std::vector<tXP4WriterArchive *> ArchiveVector; //!< 各ボリュームファイルを表す配列
	wxString TargetDir; //!< アーカイブに格納されるファイルの元となったディレクトリ名

	unsigned int NewArchive();

public:
	tXP4Writer(
		iRisaProgressCallback * callback,
		const wxString & basefilename,
		wxFileOffset splitlimit,
		const std::vector<tXP4WriterInputFile> & list,
		const wxString & targetdir);
	~tXP4Writer();

	void MakeArchive();
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif

