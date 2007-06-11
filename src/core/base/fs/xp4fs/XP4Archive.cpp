//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4 アーカイブの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include <zlib.h>
#include "base/exception/RisaException.h"
#include "base/fs/common/FSManager.h"
#include "base/fs/xp4fs/XP4Archive.h"
#include "base/fs/xp4fs/XP4SegmentCache.h"
#include "base/fs/xp4fs/XP4StreamCache.h"
#include "base/fs/xp4fs/XP4Stream.h"

RISSE_DEFINE_SOURCE_ID(63449,40639,29098,17674,37258,38306,59594,20366);

//---------------------------------------------------------------------------
//! @brief		指定された位置のメモリから16bit LE整数を読み込む
//! @param		mem メモリ
//---------------------------------------------------------------------------
static inline wxUint16 RisaReadI16LEFromMem(const unsigned char *mem)
{
	return static_cast<wxUint16>(mem[0]) + (static_cast<wxUint16>(mem[1]) << 8);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定された位置のメモリから32bit LE整数を読み込む
//! @param		mem メモリ
//---------------------------------------------------------------------------
static inline wxUint32 RisaReadI32LEFromMem(const unsigned char *mem)
{
	return static_cast<wxUint32>(mem[0]) + (static_cast<wxUint32>(mem[1]) << 8) +
		(static_cast<wxUint32>(mem[2]) << 16) + (static_cast<wxUint32>(mem[3]) << 24);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定された位置のメモリから64bit LE整数を読み込む
//! @param		mem メモリ
//---------------------------------------------------------------------------
static inline wxUint64 RisaReadI64LEFromMem(const unsigned char *mem)
{
	wxUint32 low  = RisaReadI32LEFromMem(mem);
	wxUint32 high = RisaReadI32LEFromMem(mem + 4);
	return (static_cast<wxUint64>(high) << 32) | (static_cast<wxUint64>(low));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		チャンクを探す
//! @param		chunkname チャンク名
//! @param		mem 検索を行うメモリ領域の先頭位置
//! @param		limit 検索を行うメモリ領域のサイズ
//! @param		chunkcontent チャンクが見つかった場合、そのチャンクの内容の先頭を指すポインタが入る
//! @param		chunksize チャンクが見つかった場合、そのチャンクのサイズが入る
//! @return		チャンクが見つかった場合に 真、見つからなかった場合は偽
//---------------------------------------------------------------------------
static bool RisaFindChunk(const unsigned char * chunkname,
	const unsigned char *mem, size_t memsize,
	const unsigned char ** chunkcontent, size_t * chunksize)
{
	size_t i = 0;
	while(i < memsize)
	{
		if(
			mem[i+0] == chunkname[0] &&
			mem[i+1] == chunkname[1] &&
			mem[i+2] == chunkname[2] &&
			mem[i+3] == chunkname[3])
		{
			// チャンクが見つかった
			if(chunksize) *chunksize = RisaReadI32LEFromMem(mem + i + 4);
			if(chunkcontent) *chunkcontent = mem + i + 8;
			return true;
		}
		i += RisaReadI32LEFromMem(mem + i + 4) + 8;
	}
	return false; // チャンクが見つからなかった
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tRisaXP4Archive::tFile::tFile(tRisaXP4Archive *owner, const unsigned char * meta,
	size_t metasize, tRisseString & inarchivename, bool &deleted)
{
	// この時点で meta, metasize は File チャンクの先頭 'File' の
	// 内容を示していなければならない
	const unsigned char * chunk;
	size_t chunksize;

	// info チャンクを探す
	static unsigned char chunkname_info[] = { 'i', 'n', 'f', 'o' };
	if(!RisaFindChunk(chunkname_info, meta, metasize, &chunk, &chunksize))
	{
		// info チャンクが見つからなかった
		eRisaException::Throw(RISSE_WS_TR("chunk 'info' not found"));
	}

	// info チャンクから情報を読み取る
	Flags = RisaReadI16LEFromMem(chunk + 0);
	Flags &=~ RISA__XP4_FILE_MARKED; // MARKED はクリア
	inarchivename = tRisseString(wxString(reinterpret_cast<const char *>(chunk + 2), wxConvUTF8));
	deleted = (Flags & RISA__XP4_FILE_STATE_MASK) == RISA__XP4_FILE_STATE_DELETED ;

	// time チャンクを探す
	static unsigned char chunkname_time[] = { 't', 'i', 'm', 'e' };
	if(RisaFindChunk(chunkname_time, meta, metasize, &chunk, &chunksize))
	{
		// time チャンクから情報を読み取る
		// time チャンクは ファイルが '削除' とマークされている場合は
		// 存在しない
		static wxDateTime::Month monthes[] =
			{wxDateTime::Jan, wxDateTime::Feb, wxDateTime::Mar, wxDateTime::Apr,
			 wxDateTime::May, wxDateTime::Jun, wxDateTime::Jul, wxDateTime::Aug,
			 wxDateTime::Sep, wxDateTime::Oct, wxDateTime::Nov, wxDateTime::Dec};
		Time.Set(
			chunk[3], // day
			monthes[static_cast<int>(chunk[2]) >= 12 ? 0 : static_cast<int>(chunk[2])], // month
			RisaReadI16LEFromMem(chunk + 0), // year
			chunk[4], // hour
			chunk[5], // minute
			chunk[6], // second
			RisaReadI16LEFromMem(chunk + 7) // millisecond
			 );
	}
	else
	{
		if((Flags & RISA__XP4_FILE_STATE_MASK) != RISA__XP4_FILE_STATE_DELETED)
			eRisaException::Throw(RISSE_WS_TR("chunk 'time' not found"));
	}

	// Segm チャンクを探す
	static unsigned char chunkname_Segm[] = { 'S', 'e', 'g', 'm' };
	if(RisaFindChunk(chunkname_Segm, meta, metasize, &chunk, &chunksize))
	{
		Size = 0;
		SegmentStart = owner->GetSegments().size();
		SegmentCount = 0;
		// すべての segm チャンクを探し、owner に push する
		static unsigned char chunkname_segm[] = { 's', 'e', 'g', 'm' };
		const unsigned char * subchunk;
		size_t subchunksize;
		const unsigned char *chunk_limit = chunk + chunksize;
		size_t left = chunk_limit - chunk;
		while(RisaFindChunk(chunkname_segm, chunk, left, &subchunk, &subchunksize))
		{
			// Segm チャンクが見つかった
			Size += RisaReadI64LEFromMem(subchunk + 9);
			chunk = subchunk + subchunksize;
			left = chunk_limit - chunk;
			SegmentCount ++;

			// owner にセグメントの情報をpush
			tRisaXP4Archive::tSegment segment;
			segment.Flags       = static_cast<wxUint8>(subchunk[0]);
			segment.Offset      = RisaReadI64LEFromMem(subchunk + 1);
			segment.Size        = RisaReadI64LEFromMem(subchunk + 9);
			segment.StoreOffset = RisaReadI64LEFromMem(subchunk + 17);
			segment.StoreSize   = RisaReadI64LEFromMem(subchunk + 25);
			owner->GetSegments().push_back(segment);
		}
	}
	else
	{
		if((Flags & RISA__XP4_FILE_STATE_MASK) != RISA__XP4_FILE_STATE_DELETED)
			eRisaException::Throw(RISSE_WS_TR("chunk 'Segm' not found"));
	}

	// ハッシュ用 チャンクを探す(現状はsha1固定)
	static unsigned char chunkname_sha1[] = { 's', 'h', 'a', '1' };
	if(RisaFindChunk(chunkname_sha1, meta,
		metasize, &chunk, &chunksize))
	{
		if(chunksize != sizeof(Hash))
			eRisaException::Throw(RISSE_WS_TR("invalid hash chunk"));
		memcpy(Hash, chunk, sizeof(Hash));
	}
	else
	{
		if((Flags & RISA__XP4_FILE_STATE_MASK) != RISA__XP4_FILE_STATE_DELETED)
			eRisaException::Throw(RISSE_WS_TR("hash chunk not found"));
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaXP4Archive::tRisaXP4Archive(const tRisseString & filename, iMapCallback & callback)
{
	// アーカイブファイルを読み込む
	FileName = filename;

	// アーカイブファイルを開く
	std::auto_ptr<tRisseBinaryStream>
		stream(tRisaFileSystemManager::instance()->CreateStream(filename, RISSE_BS_READ));

	// ヘッダのシグニチャをチェック
	static unsigned char XP4Mark1[] = // 8bytes
		{ 0x58/*'X'*/, 0x50/*'P'*/, 0x34/*'4'*/, 0x0d/*'\r'*/,
		  0x0a/*'\n'*/, 0x20/*' '*/, 0x0a/*'\n'*/, 0x1a/*EOF*/,
		  0xff /* sentinel */ };
	static unsigned char XP4Mark2[] = // 3bytes
		{ 0x8b, 0x67, 0x01, 0xff/* sentinel */ };

	unsigned char buf[11];
	stream->ReadBuffer(buf, 11);
	if(memcmp(buf, XP4Mark1, 8) ||
		memcmp(buf+8, XP4Mark2, 3))
	{
		// シグニチャが一致しない
		eRisaException::Throw(tRisseString(wxString::Format(RISSE_WS_TR("'%s' is not an XP4 archive file"),
			filename.AsWxString().c_str())));
	}

	// インデックスまでシーク
	wxUint64 index_ofs;
	stream->ReadBuffer(&index_ofs, sizeof(index_ofs));
	index_ofs = wxUINT64_SWAP_ON_BE(index_ofs);
	stream->SetPosition(index_ofs);

	// インデックスを読み込む
	unsigned char flags;
	stream->ReadBuffer(&flags, 1);
	wxUint32 storage_count;
	wxUint32 raw_index_size;
	wxUint32 compressed_index_size;
	stream->ReadBuffer(&storage_count, sizeof(storage_count));
	stream->ReadBuffer(&raw_index_size, sizeof(raw_index_size));
	stream->ReadBuffer(&compressed_index_size, sizeof(compressed_index_size));
	storage_count = wxUINT32_SWAP_ON_BE(storage_count);
	raw_index_size = wxUINT32_SWAP_ON_BE(raw_index_size);
	compressed_index_size = wxUINT32_SWAP_ON_BE(compressed_index_size);

	Files.reserve(storage_count);
	Segments.reserve(storage_count);
		// segmentの個数はstorage_countと同一にはならないが、
		// 最低でもstorage_countはあるので目安としてこのサイズを確保しておく

	unsigned char * raw_index = NULL;
	unsigned char * compressed_index = NULL;

	raw_index = new (PointerFreeGC) unsigned char [raw_index_size];
	compressed_index = new (PointerFreeGC) unsigned char [compressed_index_size];
	stream->ReadBuffer(compressed_index, compressed_index_size);

	// 圧縮が行われている場合は展開を行う
	if((flags & RISA__XP4_INDEX_ENCODE_METHOD_MASK) ==
		RISA__XP4_INDEX_ENCODE_ZLIB)
	{
		// 圧縮が行われている
		unsigned long input_size = compressed_index_size;
		unsigned long output_size = raw_index_size;
		int res = uncompress(raw_index, &output_size,
			compressed_index, input_size);
		if(res != Z_OK || output_size != raw_index_size)
		{
			// 圧縮インデックスの展開に失敗した
			eRisaException::Throw(tRisseString(wxString::Format(
				RISSE_WS_TR("decompression of archive index of '%s' failed"),
				filename.AsWxString().c_str())));
		}
	}
	else
	{
		// 圧縮は行われていない
		memcpy(raw_index, compressed_index, compressed_index_size);
	}

	// Item チャンクを探す
	const unsigned char *chunk;
	size_t chunksize;
	static unsigned char chunkname_Item[] = { 'I', 't', 'e', 'm' };
	if(!RisaFindChunk(chunkname_Item, raw_index, raw_index_size, &chunk, &chunksize))
	{
		eRisaException::Throw(tRisseString(wxString::Format(
			RISSE_WS_TR("chunk 'Item' not found in file '%s'"),
			filename.AsWxString().c_str())));
	}

	// Item チャンクの内容を読み込む
	// Item チャンクの中には複数の File チャンクがある
	const unsigned char *mem = chunk;
	const unsigned char *mem_limit = mem + chunksize;
	size_t left = mem_limit - mem;
	static unsigned char chunkname_File[] = { 'F', 'i', 'l', 'e' };
	tRisseString inarchivename;
	bool deleted;
	while(RisaFindChunk(chunkname_File, mem, left, &chunk, &chunksize))
	{
		// File チャンクが見つかった
		risse_size idx = Files.size();
		Files.push_back(tFile(this, chunk, chunksize, inarchivename, deleted));
		mem = chunk + chunksize;
		left = mem_limit - mem;

		// callback を呼ぶ
		if(deleted)
			callback(inarchivename); // 削除
		else
			callback(inarchivename, idx); // 追加または更新
	}

	/*
		// Meta チャンクを探す
		static unsigned char chunkname_Meta[] = { 'M', 'e', 't', 'a' };
		if(RisaFindChunk(chunkname_Meta, raw_index, raw_index_size, &chunk, &chunksize))
		{
			// targ サブチャンクを探す
			static unsigned char chunkname_targ[] = { 't', 'a', 'r', 'g' };
			const unsigned char *targ_chunk;
			size_t targ_chunksize;
			if(RisaFindChunk(chunkname_targ, chunk, chunksize, &targ_chunk, &targ_chunksize))
			{
				// これはアーカイブの元となったファイル名
				TargetDir = wxString(reinterpret_cast<const char *>(targ_chunk), wxConvUTF8);
			}
		}
	*/

	delete (PointerFreeGC) [] raw_index;
	delete (PointerFreeGC) [] compressed_index;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaXP4Archive::~tRisaXP4Archive()
{
	// キャッシュをクリアする
	// このアーカイブに関連したキャッシュのみを解放できるように実装することも
	// できるが、処理の単純化のためにすべてキャッシュをクリアしてしまうことにする
	depends_on<tRisaXP4SegmentCache>::locked_instance()->Clear();
	depends_on<tRisaXP4StreamCache>::locked_instance()->Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaXP4Archive::Stat(risse_size idx, tRisaStatStruc & struc)
{
	struc.Clear();

	struc.Size  = Files[idx].Size; // サイズ
	struc.MTime = Files[idx].Time; // 更新時刻

	// 他の情報はサッパリ
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseBinaryStream * tRisaXP4Archive::CreateStream(
			boost::shared_ptr<tRisaXP4Archive> ptr,
			risse_size idx, risse_uint32 flags)
{
	return new tRisaXP4ArchiveStream(ptr, idx, flags);
}
//---------------------------------------------------------------------------
