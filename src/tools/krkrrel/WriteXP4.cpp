//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4 アーカイブファイルの書き込み
//---------------------------------------------------------------------------
#include "prec.h"
#include "WriteXP4.h"
#include "ProgressCallback.h"
#include "XP4Archive.h"
#include "FileList.h"
#include <zlib.h>

/*
	XP4 アーカイブファイルは、実のところ XP3 アーカイブと大差はない。

	ただし、以下の相違点がある。

	・ファイルマジック定数が違う(当たり前といえば当たり前)
	・ファイル名が UTF-8 として記録されている
	・PEXP3 のような PE との結合はサポートしない
	・ステート TVP_XP4_FILE_STATE_DELETED を持つ
	  (パッチによりファイルが削除されたことを示す)
	・データ保護フラグ (TVP_XP3_FILE_PROTECTED) を持たない
	  (非常に初期の、良識のある展開ツール作者は、このフラグの存在の
	  意図をくみ、それなりの対応をしてくれたのだが………あまり意味がない)

	メタデータ構造

	File *
		Segm ?
			segm *
		time ?
		sha1 ?
		info
*/

//---------------------------------------------------------------------------
//! @brief		wxFile 同士のコピー
//! @param		callback: 進捗コールバックオブジェクト
//! @param		src: コピー元
//! @param		dest: コピー先
//! @param		size: コピーするバイト数 (wxFileOffset)
//! @return		コピーされたバイト数
//! @note		コピー元もコピー先も、現在のファイルポインタからコピーされる。
//!				また、コピー後に入力ファイルのファイルポインタが正確にコピー
//!				されたバイト数ぶんだけ進んでいる保証はない (出力ファイル側は
//!				コピーされた分だけ進んでいる)
//---------------------------------------------------------------------------
#define TVP_COPY_BUF_SIZE 65536
static wxFileOffset TVPCopyFile(iTVPProgressCallback * callback,
	wxFileEx & src, wxFileEx & dest, wxFileOffset size)
{
	wxFileOffset copied = 0;
	unsigned char *buf = new unsigned char [TVP_COPY_BUF_SIZE];
	if(callback) callback->OnProgress(0);
	try
	{
		wxFileOffset left = size;
		while(left > 0)
		{
			unsigned long one_size =
				left > TVP_COPY_BUF_SIZE ? TVP_COPY_BUF_SIZE:left;
			one_size = src.Read(buf, one_size);
			if(one_size == 0) break;
			one_size = dest.wxFile::Write(buf, one_size);
			if(one_size == 0) break;
			copied += one_size;
			left -= one_size;

			if(callback) callback->OnProgress((int)(copied * 100 / size));
		}
	}
	catch(...)
	{
		delete [] buf;
		throw;
	}
	delete [] buf;
	if(callback) callback->OnProgress(100);
	return copied;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		offset: (非圧縮時の)ストレージ先頭からのオフセット
//! @param		size:  (非圧縮時の)サイズ
//! @param		ZLIB 圧縮の際に真
//---------------------------------------------------------------------------
tTVPXP4WriterSegment::tTVPXP4WriterSegment(
		wxFileOffset offset,
		wxFileOffset size,
		bool iscompressed)
			: Offset(offset), Size(size),
			 IsCompressed(iscompressed)
{
	StoreOffset = 0;
	StoreSize = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPXP4WriterSegment::~tTVPXP4WriterSegment()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルをアーカイブに書き込む
//! @param		callback: 進捗コールバックオブジェクト
//! @param		input: 入力ファイル
//! @param		file: アーカイブファイル
//---------------------------------------------------------------------------
void tTVPXP4WriterSegment::WriteBody(iTVPProgressCallback * callback,
		wxFileEx & input, wxFileEx & file)
{
	// コールバックを呼ぶ
	if(callback) callback->OnProgress(0);

	// アーカイブファイルを 8 bytes 境界にアラインメントする
	file.Align(8);

	// 圧縮する？
	if(IsCompressed)
	{
		// 圧縮する
		// 入力ファイルをオフセットまでシーク
		input.SetPosition(Offset);

		// 入力ファイルをメモリに読み込む
		// 出力用のメモリを確保する
		unsigned char *srcbuf  = NULL;
		unsigned char *outbuf  = NULL;
		try
		{
			srcbuf = new unsigned char[(size_t)Size];
			unsigned long compsize = 0;
			outbuf = new BYTE[compsize =
					(unsigned long)(Size + Size /100 + 1024)];
			input.ReadBuffer(srcbuf, (unsigned int)Size);

			int res;
			unsigned long srclen = (unsigned long)Size;

			// compress with zlib deflate
			res = compress2(outbuf, &compsize, srcbuf, srclen,
				Z_DEFAULT_COMPRESSION);
			if(res != Z_OK) IsCompressed  = false; // 圧縮に失敗

			if(compsize >= srclen) IsCompressed = false; // 圧縮に失敗

			// ファイルに書き込む
			StoreSize = compsize;
			StoreOffset = file.Tell();
			file.WriteBuffer(outbuf, compsize);
		}
		catch(...)
		{
			if(srcbuf) delete [] srcbuf;
			if(outbuf) delete [] outbuf;
			throw;
		}
		if(srcbuf) delete [] srcbuf;
		if(outbuf) delete [] outbuf;
	}

	// 圧縮しない？
	if(!IsCompressed)
	{
		// 圧縮しない

		// ここには、上記の 圧縮してるところで圧縮に失敗した場合でも
		// 到達するので注意

		// 入力ファイルをオフセットまでシーク
		input.SetPosition(Offset);

		// 入力から出力へコピー
		StoreOffset = file.Tell();
		TVPCopyFile(callback, input, file, Size);
		StoreSize = Size;
	}

	// コールバックを呼ぶ
	if(callback) callback->OnProgress(100); // 100%
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		メタデータを書き込む
//! @param		buf: 出力メモリバッファ
//---------------------------------------------------------------------------
void tTVPXP4WriterSegment::WriteMetaData(wxMemoryBuffer & buf)
{
	// このセグメントに対応するメタデータをbufに書き込む
	// データ構造(全てLE)
	// uint32		'segm' subchunk
	// uint32		chunksize = 33
	// +0  uint8	Flags
	// +1  uint64	Offset
	// +9  uint64	Size
	// +17 uint64	StoreOffset
	// +25 uint64	StoreSize
	wxUint32 i32;
	wxUint64 i64;

	static char segm_chunk[4] = { 's', 'e', 'g', 'm' }; // segm chunk name

	buf.AppendData(segm_chunk, 4);
		// うーん、AppendDataの第１引数は const void * じゃなくて void * なのだ
	// chunksize
	i32 = wxUINT32_SWAP_ON_BE(33);
	buf.AppendData(&i32, sizeof(i32));
	// Flags
	buf.AppendByte(
		IsCompressed ? TVP_XP4_SEGM_ENCODE_ZLIB : TVP_XP4_SEGM_ENCODE_RAW);
	// Offset
	i64 = wxUINT64_SWAP_ON_BE(Offset);
	buf.AppendData(&i64, sizeof(i64));
	// Size
	i64 = wxUINT64_SWAP_ON_BE(Size);
	buf.AppendData(&i64, sizeof(i64));
	// StoreOffset
	i64 = wxUINT64_SWAP_ON_BE(StoreOffset);
	buf.AppendData(&i64, sizeof(i64));
	// StoreSize
	i64 = wxUINT64_SWAP_ON_BE(StoreSize);
	buf.AppendData(&i64, sizeof(i64));
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		コンストラクタ(他のアイテムを参照しない場合)
//! @param		inputfile: 入力ファイルの情報
//! @param		basedirlength: アーカイブ対象ディレクトリのベース部分の長さ
//---------------------------------------------------------------------------
#define COMPRESS_SPLIT_UNIT 256*1024 //!< 圧縮時の分割バイト数
tTVPXP4WriterStorage::tTVPXP4WriterStorage(
		const tTVPXP4WriterInputFile & inputfile)
			: tTVPXP4WriterInputFile(inputfile)
{
	IsReference = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ(他のストレージアイテムを参照する場合)
//! @param		inputfile: 入力ファイルの情報
//! @param		basedirlength: アーカイブ対象ディレクトリのベース部分の長さ
//! @param		ref: 参照アイテム
//---------------------------------------------------------------------------
tTVPXP4WriterStorage::tTVPXP4WriterStorage(
	const tTVPXP4WriterInputFile & inputfile,
	const tTVPXP4WriterStorage & ref)
		: tTVPXP4WriterInputFile(inputfile)
{
	IsReference = true; // 参照

	// 参照アイテムからデータをコピー
	Size = ref.Size;
	SegmentVector = ref.SegmentVector;
	Hash = ref.Hash;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルのハッシュを得る
//---------------------------------------------------------------------------
void tTVPXP4WriterStorage::MakeHash(iTVPProgressCallback * callback)
{
	if((Flags & TVP_XP4_FILE_STATE_MASK) != TVP_XP4_FILE_STATE_DELETED)
	{
		if(!Hash.GetHasHash())
		{
			// ハッシュがまだ計算されていない場合は計算を行う
			Hash.MakeHash(callback, BaseDirName + InputName);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		XP4アーカイブへファイルを格納する
//! @param		callback: 進捗コールバックオブジェクト
//! @param		archive: アーカイブファイルオブジェクト
//---------------------------------------------------------------------------
void tTVPXP4WriterStorage::WriteBody(
	iTVPProgressCallback * callback, wxFileEx & archive)
{
	if(IsReference) return;
		// このストレージアイテムが他のストレージアイテムを参照している場合は
		// body として書き込む物は何もない

	if((Flags & TVP_XP4_FILE_STATE_MASK) == TVP_XP4_FILE_STATE_DELETED) return;
		// このストレージが「削除」を表す場合は何もしない

	// セグメントを作成する
	if(Flags & TVP_XP4_FILE_COMPRESSED)
	{
		// ファイルは圧縮されている
		// ZLIB はシークを苦手とするので、COMPRESS_SPLIT_UNIT ごとに
		// 圧縮元ファイルを分割し、それごとに圧縮を行うこととする
		wxFileOffset left = Size;
		wxFileOffset offset = 0;
		while(left > 0)
		{
			wxFileOffset onesize =
				left > COMPRESS_SPLIT_UNIT ? COMPRESS_SPLIT_UNIT : left;
			SegmentVector.push_back(
				tTVPXP4WriterSegment(offset, onesize, true));
			offset += onesize;
			left -= onesize;
		}
	}
	else
	{
		// ファイルは圧縮されていない
		// ファイル一個まるごとを表すセグメントを一個追加する
		SegmentVector.push_back(
			tTVPXP4WriterSegment(0, Size, false));
	}

	// 入力ファイルを開く
	wxFileEx file(BaseDirName + InputName);

	// すべてのセグメントに対して body 書き込みを行わせる
	size_t idx = 0;
	size_t segment_count = SegmentVector.size();
	for(std::vector<tTVPXP4WriterSegment>::iterator i = SegmentVector.begin();
		i != SegmentVector.end(); i++, idx++)
	{
		tTVPProgressCallbackAggregator agg(
			callback,
			(idx    ) * 100 / segment_count,
			(idx + 1) * 100 / segment_count);
		i->WriteBody(&agg, file, archive);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		メタデータを書き込む
//! @param		buf: 出力メモリバッファ
//---------------------------------------------------------------------------
void tTVPXP4WriterStorage::WriteMetaData(wxMemoryBuffer & buf)
{
	// メタデータを buf に書き込む
	wxMemoryBuffer newbuf;
	wxUint16 i16;
	wxUint32 i32;

	if((Flags & TVP_XP4_FILE_STATE_MASK) != TVP_XP4_FILE_STATE_DELETED)
	{
		// 「削除」されたファイルではない場合
		wxMemoryBuffer segmentsbuf;
		for(std::vector<tTVPXP4WriterSegment>::iterator i = SegmentVector.begin();
			i != SegmentVector.end(); i++)
		{
			i->WriteMetaData(segmentsbuf);
		}
		// "Segm" チャンクの書き込み
		static char Segm_chunk[4] = { 'S', 'e', 'g', 'm' }; // Segm chunk name
		newbuf.AppendData(Segm_chunk, 4);
		i32 = wxUINT32_SWAP_ON_BE(segmentsbuf.GetDataLen());
		newbuf.AppendData(&i32, sizeof(i32));
		newbuf.AppendData(segmentsbuf.GetData(), segmentsbuf.GetDataLen());

		// sha1 チャンクの書き込み
		static char sha1_chunk[4] = { 's', 'h', 'a', '1' }; // sha1 chunk name
		newbuf.AppendData(sha1_chunk, 4);
		i32 = wxUINT32_SWAP_ON_BE(Hash.GetSize());
		newbuf.AppendData(&i32, sizeof(i32));
		newbuf.AppendData(const_cast<unsigned char *>((const unsigned char *)Hash),
			Hash.GetSize());

		// time チャンクの書き込み
		// time チャンクの構造は以下の通り(全てLE)
		// uint32		'time' subchunk
		// uint32		chunksize = 9
		// +0 int16		Year
		// +2 uint8		Month(0～)
		// +3 uint8		Day
		// +4 uint8		Hour
		// +5 uint8		Minute
		// +6 uint8		Second
		// +7 uint16	Millisecond
		static char time_chunk[4] = { 't', 'i', 'm', 'e' }; // time chunk name
		newbuf.AppendData(time_chunk, 4);
		i32 = wxUINT32_SWAP_ON_BE(9); // chunksize
		newbuf.AppendData(&i32, sizeof(i32));
		i16 = wxINT16_SWAP_ON_BE(Time.GetYear());
		newbuf.AppendData(&i16, sizeof(i16)); // Year
		newbuf.AppendByte(Time.GetMonth()); // Month
		newbuf.AppendByte(Time.GetDay()); // Day
		newbuf.AppendByte(Time.GetHour()); // Hour
		newbuf.AppendByte(Time.GetMinute()); // Minute
		newbuf.AppendByte(Time.GetSecond()); // Second
		i16 = wxUINT16_SWAP_ON_BE(Time.GetMillisecond());
		newbuf.AppendData(&i16, sizeof(i16)); // Millisecond
	}

	// info チャンクの書き込み
	// info チャンクの構造
	// uint32		'info' subchunk
	// uint32		chunksize = 2 + (strlen(FileName) + 1)
	// uint16		Flags
	// char[]		FileName (in utf-8 encoding, *with* null-terminator)
	static char info_chunk[4] = { 'i', 'n', 'f', 'o' }; // info chunk name
	newbuf.AppendData(info_chunk, 4);
	wxCharBuffer utf8name = InArchiveName.mb_str(wxConvUTF8);
	size_t utf8name_len = strlen(utf8name);
	i32 = wxUINT32_SWAP_ON_BE(2 + (utf8name_len + 1));
	newbuf.AppendData(&i32, sizeof(i32)); // chunksize
	i16 = wxUINT16_SWAP_ON_BE(Flags);
	newbuf.AppendData(&i16, sizeof(i16)); // Flags
	newbuf.AppendData(const_cast<char *>((const char *)utf8name) + 1,
		utf8name_len - 1 + 1); // FileName

	// File チャンクにまとめ、bufに書き込む
	static char File_chunk[4] = { 'F', 'i', 'l', 'e' }; // info chunk name
	buf.AppendData(File_chunk, 4); // 'File' majorchunk
	i32 = wxUINT32_SWAP_ON_BE(newbuf.GetDataLen());
	buf.AppendData(&i32, sizeof(i32)); // chunksize
	buf.AppendData(newbuf.GetData(), newbuf.GetDataLen());
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTVPXP4WriterArchive::tTVPXP4WriterArchive(const wxString & filename) :
		FileName(filename)
{
	// 早速ファイルを開き、ヘッダを書き込む
	wxFileEx file(FileName, wxFile::write);

	static unsigned char XP4Mark1[] = // 8bytes
		{ 0x58/*'\x00'*/, 0x50/*'P'*/, 0x34/*'4'*/, 0x0d/*'\r'*/,
		  0x0a/*'\n'*/, 0x20/*' '*/, 0x0a/*'\n'*/, 0x1a/*EOF*/,
		  0xff /* sentinel */ };
	static unsigned char XP4Mark2[] = // 3bytes
		{ 0x8b, 0x67, 0x01, 0xff/* sentinel */ };

	file.WriteBuffer(XP4Mark1, 8);
	file.WriteBuffer(XP4Mark2, 3);

	// この時点で、ファイルの先頭のバイトは本来 'X' であるはずだが、\0 が
	// 書き込まれている (アーカイブファイル作成の最終段階で 'X' に書き戻す)

	// この時点で ファイルポインタは 11 バイト目!

	file.WriteBuffer("\0\0\0\0\0\0\0\0", 8); // インデックスへのオフセットとなる

	Size = file.Tell();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		アーカイブにファイルを追加し、書き込む
//! @param		callback: 進捗コールバックオブジェクト
//! @param		storage: 書き込むファイル
//! @return		格納されたアーカイブ内のストレージのインデックス
//---------------------------------------------------------------------------
size_t tTVPXP4WriterArchive::AddAndWriteBody(iTVPProgressCallback * callback,
	const tTVPXP4WriterStorage & storage)
{
	StorageVector.push_back(storage);
	wxFileEx file(FileName, wxFile::write_append);
	(StorageVector.end() - 1)->WriteBody(callback, file);

	Size = file.Tell();

	return StorageVector.size() - 1;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		アーカイブにメタデータを書き込む
//! @param		callback: 進捗コールバックオブジェクト
//! @param		compress: インデックスを圧縮するかどうか
//---------------------------------------------------------------------------
void tTVPXP4WriterArchive::WriteMetaData(iTVPProgressCallback * callback,
	bool compress)
{
	// メタデータをメモリ上に作成する
	tTVPProgressCallbackAggregator agg(callback, 0, 50);

	wxMemoryBuffer buf;
	size_t idx = 0;
	size_t storage_count = StorageVector.size();
	for(std::vector<tTVPXP4WriterStorage>::iterator i = StorageVector.begin();
		i != StorageVector.end(); i++, idx++)
	{
		agg.OnProgress(idx * 100 / storage_count);
		i->WriteMetaData(buf);
	}

	wxFileEx file(FileName, wxFile::write_append);
	wxFileOffset index_ofs;
	unsigned char *outbuf  = NULL;
	try
	{
		unsigned long inputsize = buf.GetDataLen();
		unsigned long compsize = 0;
		outbuf = new BYTE[compsize =
				(unsigned long)(inputsize + inputsize /100 + 1024)];

		int res;

		// compress with zlib deflate
		if(compress)
		{
			res = compress2(outbuf, &compsize, (const unsigned char *)buf.GetData(),
				inputsize, Z_DEFAULT_COMPRESSION);
			if(res != Z_OK)
			{
				// 圧縮に失敗
				throw wxString::Format(_("failed to compress the index data of file '%s'"),
						FileName.c_str());
			}
		}
		else
		{
			memcpy(outbuf, buf.GetData(), inputsize);
			compsize = inputsize;
		}

		if(callback) callback->OnProgress(75); // 75% ぐらい？

		// インデックスを書き込む
		file.Align(8); // 8バイト境界にアライン
		index_ofs = file.Tell();
		wxUint8 i8;
		i8 = compress ? 1:0; // 1 = compressed ?
		file.WriteBuffer(&i8, 1); // flags
		wxUint32 i32;
		i32 = wxUINT32_SWAP_ON_BE((wxUint32)storage_count);
		file.WriteBuffer(&i32, sizeof(i32)); // storage count
											// (これは目安。この数値を信じないこと)
		i32 = wxUINT32_SWAP_ON_BE((wxUint32)inputsize);
		file.WriteBuffer(&i32, sizeof(i32)); // raw index size
		i32 = wxUINT32_SWAP_ON_BE((wxUint32)compsize);
		file.WriteBuffer(&i32, sizeof(i32)); // compressed index size
		file.WriteBuffer(outbuf, compsize); // index
		file.Align(8); // ファイルの最後も8バイト境界にアライン
	}
	catch(...)
	{
		if(outbuf) delete [] outbuf;
		throw;
	}
	if(outbuf) delete [] outbuf;

	if(callback) callback->OnProgress(90); // 90% ぐらい？

	// インデックスへのオフセットを書き込む
	file.SetPosition(11); // オフセットが書き込まれる位置
	wxUint64 i64;
	i64 = wxUINT64_SWAP_ON_BE(index_ofs);
	file.WriteBuffer(&i64, sizeof(i64)); // index offset

	// アーカイブファイルの先頭を 'X' にする
	file.SetPosition(0);
	file.WriteBuffer("X", 1);

	if(callback) callback->OnProgress(100);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		callback: 進捗コールバックオブジェクト
//! @param		basefilename: ベースファイル名(パス付きだが拡張子をのぞく)
//! @param		splitlimit: 分割する際のファイルサイズの上限(バイト単位) 0=分割なし
//! @param		list: 入力ファイルのリスト
//! @param		basedirname: 入力ファイルのベースディレクトリ
//---------------------------------------------------------------------------
tTVPXP4Writer::tTVPXP4Writer(
	iTVPProgressCallback * callback,
	const wxString & basefilename,
	wxFileOffset splitlimit,
	const std::vector<tTVPXP4WriterInputFile> & list
		) :
		ProgressCallback(callback),
		BaseFileName(basefilename),
		SplitLimit(splitlimit),
		List(list)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPXP4Writer::~tTVPXP4Writer()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		新しいアーカイブボリュームファイルを作成する
//! @return		新しいアーカイブボリューム番号
//---------------------------------------------------------------------------
unsigned int tTVPXP4Writer::NewArchive()
{
	// アーカイブボリュームファイル名は
	// volnum = 0 の場合は BaseFileName + ".xp4"
	// volnum = 1 の場合は BaseFileName + ".001.xp4"
	// volnum = 2 の場合は BaseFileName + ".002.xp4"
	//    :
	//    :
	// となる。

	unsigned int volnum = ArchiveVector.size();

	wxString filename;
	if(volnum == 0)
		filename = BaseFileName + wxT(".xp4");
	else
		filename = BaseFileName + wxString::Format(wxT("%03u.xp4"), volnum);

	ArchiveVector.push_back(tTVPXP4WriterArchive(filename));

	return volnum;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		アーカイブファイルを作成する
//---------------------------------------------------------------------------
void tTVPXP4Writer::MakeArchive()
{
	std::vector<tTVPXP4WriterStorage> sorted_list;

	// TODO: ファイルが無かった場合の処理は？

	// どのファイルがどのアーカイブに入っているかをハッシュから求めるmap
	std::map<tTVPXP4Hash, std::pair<size_t, size_t> > storage_archive_map;

	// 進捗コールバックアグリゲータ
	tTVPProgressCallbackAggregator agg(ProgressCallback, 0, 33);

	// ファイルサイズ・ハッシュを得ながら sorted_list に追加する
	// 0 ～ 33% 区間
	sorted_list.reserve(List.size());
	size_t file_count = List.size();
	size_t idx = 0;
	for(std::vector<tTVPXP4WriterInputFile>::iterator i = List.begin();
		i != List.end(); i++, idx++)
	{
		tTVPProgressCallbackAggregator agg2(
			&agg,
			(idx    ) * 100 / file_count,
			(idx + 1) * 100 / file_count);
		sorted_list.push_back(tTVPXP4WriterStorage(*i));
		sorted_list[idx].MakeHash(&agg2);
	}

	// 分割が有効にされた場合、XP4 アーカイブ内ではファイルは「大きい順」に配置される
	if(SplitLimit != 0)
	{
		// サイズは 2MB の粒度で比較し、
		// サイズの似通ったファイル ≒ 種類の同じファイル
		// 間では順位が変わらないように stable_sort を用いる
		std::stable_sort(sorted_list.begin(), sorted_list.end());
	}

	// ファイルを一つ一つ格納していく
	agg.SetRange(33, 98); // 33 ～ 98% 区間

	file_count = sorted_list.size();
	idx = 0;
	for(std::vector<tTVPXP4WriterStorage>::iterator i = sorted_list.begin();
		i != sorted_list.end(); i++, idx++)
	{
		tTVPProgressCallbackAggregator agg2(
			&agg,
			(idx    ) * 100 / file_count,
			(idx + 1) * 100 / file_count);

		// deleted か？
		if((i->GetFlags() & TVP_XP4_FILE_STATE_MASK) == TVP_XP4_FILE_STATE_DELETED)
		{
			// 削除されたファイル
			// 削除ファイルは無条件に 一番最初のボリュームに追加する
			if(ArchiveVector.size() == 0) NewArchive(); // 初回でアーカイブを作成
			ArchiveVector[0].AddAndWriteBody(&agg2, *i);
			continue; //------------------------------------- ↑ continue
		}

		size_t volnum = 0;
		size_t storage_idx = 0;

		// map を参照する
		std::map<tTVPXP4Hash, std::pair<size_t, size_t> >::iterator ih;
		ih = storage_archive_map.find(i->GetHash());
		if(ih != storage_archive_map.end())
		{
			// すでに同じファイルが挿入されている
			//  同じファイルの場合は、前に挿入されたファイルを参照するだけの
			// 情報を生成する
			volnum      = ih->second.first;
			storage_idx = ih->second.second;
			ArchiveVector[volnum].AddAndWriteBody(
				&agg2,
				tTVPXP4WriterStorage(
					*(tTVPXP4WriterInputFile*)(&(*i)),
					ArchiveVector[volnum].GetStorageItem(storage_idx)));
			continue; //------------------------------------- ↑ continue
		}

		if(SplitLimit != 0)
		{
			// 分割が有効な場合
			if(i->GetSize() > SplitLimit)
			{
				// かつ そのファイルがすでに SplitLimit をオーバーしている場合
				// 新規にアーカイブを作り、そこに書き込む
				volnum = NewArchive();
				storage_idx = ArchiveVector[volnum].AddAndWriteBody(&agg2, *i);
			}
			else
			{
				// 容量があいてそうなアーカイブを探す
				volnum = 0;
				std::vector<tTVPXP4WriterArchive>::iterator a;
				for(a = ArchiveVector.begin(); a != ArchiveVector.end(); a++, volnum++)
				{
					if(SplitLimit - a->GetSize() > i->GetSize())
					{
						// ここのアーカイブならば収まりそう
						storage_idx = ArchiveVector[volnum].AddAndWriteBody(&agg2, *i);
						break;
					}
				}
				if(a == ArchiveVector.end())
				{
					// 容量があいているアーカイブが無かった
					// 新規にアーカイブを作り、そこに書き込む
					volnum = NewArchive();
					storage_idx = ArchiveVector[volnum].AddAndWriteBody(&agg2, *i);
				}
			}
		}
		else
		{
			// 分割を行わない場合
			if(ArchiveVector.size() == 0) NewArchive(); // 初回でアーカイブを作成
			volnum = 0;
			storage_idx = ArchiveVector[0].AddAndWriteBody(&agg2, *i);
		}

		// map に追加
		storage_archive_map.insert(
			std::pair<tTVPXP4Hash, std::pair<size_t, size_t> >
					(sorted_list[idx].GetHash(),
					std::pair<size_t, size_t>(volnum, storage_idx)));
	}

	// 各アーカイブファイルのメタデータを格納する
	agg.SetRange(98, 100); // 98 ～ 100% 区間
	size_t archive_count = ArchiveVector.size();
	idx = 0;
	for(std::vector<tTVPXP4WriterArchive>::iterator i = ArchiveVector.begin();
		i != ArchiveVector.end(); i++)
	{
		tTVPProgressCallbackAggregator agg2(
			&agg,
			(idx    ) * 100 / archive_count,
			(idx + 1) * 100 / archive_count);

		i->WriteMetaData(&agg2, false);
	}
}
//---------------------------------------------------------------------------

