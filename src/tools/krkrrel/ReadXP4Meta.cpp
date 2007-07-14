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
#include "prec.h"
#include "XP4Archive.h"
#include "ReadXP4Meta.h"
#include "wxFileWrapper.h"
#include <zlib.h>

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		指定された位置のメモリから16bit LE整数を読み込む
//---------------------------------------------------------------------------
static inline wxUint16 RisaReadI16LEFromMem(const unsigned char *mem)
{
	return static_cast<wxUint16>(mem[0]) + (static_cast<wxUint16>(mem[1]) << 8);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定された位置のメモリから32bit LE整数を読み込む
//---------------------------------------------------------------------------
static inline wxUint32 RisaReadI32LEFromMem(const unsigned char *mem)
{
	return static_cast<wxUint32>(mem[0]) + (static_cast<wxUint32>(mem[1]) << 8) +
		(static_cast<wxUint32>(mem[2]) << 16) + (static_cast<wxUint32>(mem[3]) << 24);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定された位置のメモリから64bit LE整数を読み込む
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
//! @brief		コンストラクタ
//! @param		meta 入力メタデータ
//! @param		metasize 入力メタデータのサイズ
//---------------------------------------------------------------------------
tRisaXP4MetadataReaderStorageItem::tRisaXP4MetadataReaderStorageItem(
										const unsigned char * meta, size_t metasize)
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
		throw wxString(_("chunk 'info' not found"));
	}

	// info チャンクから情報を読み取る
	Flags = RisaReadI16LEFromMem(chunk + 0);
	Flags &=~ RISA__XP4_FILE_MARKED; // MARKED はクリア
	InArchiveName = wxT("/") + wxString(reinterpret_cast<const char *>(chunk + 2), wxConvUTF8);

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
			throw wxString(_("chunk 'time' not found"));
	}

	// Segm チャンクを探す
	static unsigned char chunkname_Segm[] = { 'S', 'e', 'g', 'm' };
	if(RisaFindChunk(chunkname_Segm, meta, metasize, &chunk, &chunksize))
	{
		Size = 0;
		// すべての segm チャンクを探し、サイズを合計する
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
		}
	}
	else
	{
		if((Flags & RISA__XP4_FILE_STATE_MASK) != RISA__XP4_FILE_STATE_DELETED)
			throw wxString(_("chunk 'Segm' not found"));
	}

	// ハッシュ用 チャンクを探す(現状はsha1固定)
	if(RisaFindChunk(tRisaXP4Hash::GetHashChunkName(), meta,
		metasize, &chunk, &chunksize))
	{
		if(chunksize != static_cast<size_t>(tRisaXP4Hash::GetSize()))
			throw wxString(_("invalid hash chunk"));
		Hash.SetHash(chunk);
	}
	else
	{
		if((Flags & RISA__XP4_FILE_STATE_MASK) != RISA__XP4_FILE_STATE_DELETED)
			throw wxString(_("hash chunk not found"));
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		filename アーカイブファイル名
//---------------------------------------------------------------------------
tRisaXP4MetadataReaderArchive::tRisaXP4MetadataReaderArchive(const wxString & filename)
{
	// アーカイブファイルを読み込む

	// アーカイブファイルを開く
	wxFileEx file(filename);

	// ヘッダのシグニチャをチェック
	static unsigned char XP4Mark1[] = // 8bytes
		{ 0x58/*'X'*/, 0x50/*'P'*/, 0x34/*'4'*/, 0x0d/*'\r'*/,
		  0x0a/*'\n'*/, 0x20/*' '*/, 0x0a/*'\n'*/, 0x1a/*EOF*/,
		  0xff /* sentinel */ };
	static unsigned char XP4Mark2[] = // 3bytes
		{ 0x8b, 0x67, 0x01, 0xff/* sentinel */ };

	unsigned char buf[11];
	file.ReadBuffer(buf, 11);
	if(memcmp(buf, XP4Mark1, 8) ||
		memcmp(buf+8, XP4Mark2, 3))
	{
		// シグニチャが一致しない
		throw wxString::Format(_("'%s' is not an XP4 archive file"),
			filename.c_str());
	}

	// インデックスまでシーク
	wxUint64 index_ofs;
	file.ReadBuffer(&index_ofs, sizeof(index_ofs));
	index_ofs = wxUINT64_SWAP_ON_BE(index_ofs);
	file.SetPosition(index_ofs);

	// インデックスを読み込む
	unsigned char flags;
	file.ReadBuffer(&flags, 1);
	wxUint32 storage_count;
	wxUint32 raw_index_size;
	wxUint32 compressed_index_size;
	file.ReadBuffer(&storage_count, sizeof(storage_count));
	file.ReadBuffer(&raw_index_size, sizeof(raw_index_size));
	file.ReadBuffer(&compressed_index_size, sizeof(compressed_index_size));
	storage_count = wxUINT32_SWAP_ON_BE(storage_count);
	raw_index_size = wxUINT32_SWAP_ON_BE(raw_index_size);
	compressed_index_size = wxUINT32_SWAP_ON_BE(compressed_index_size);

	ItemVector.reserve(storage_count);

	unsigned char * raw_index = NULL;
	unsigned char * compressed_index = NULL;
	try
	{
		raw_index = new unsigned char [raw_index_size];
		compressed_index = new unsigned char [compressed_index_size];
		file.ReadBuffer(compressed_index, compressed_index_size);
		file.Close(); // 必要な情報は読み終わったのでファイルを閉じる

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
				throw wxString::Format(
					_("decompression of archive index of '%s' failed"),
					filename.c_str());
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
			throw wxString::Format(
				_("chunk 'Item' not found in file '%s'"),
				filename.c_str());
		}

		// Item チャンクの内容を読み込む
		// Item チャンクの中には複数の File チャンクがある
		const unsigned char *mem = chunk;
		const unsigned char *mem_limit = mem + chunksize;
		size_t left = mem_limit - mem;
		static unsigned char chunkname_File[] = { 'F', 'i', 'l', 'e' };
		while(RisaFindChunk(chunkname_File, mem, left, &chunk, &chunksize))
		{
			// File チャンクが見つかった
			ItemVector.push_back(tRisaXP4MetadataReaderStorageItem(chunk, chunksize));
			mem = chunk + chunksize;
			left = mem_limit - mem;
		}

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
	}
	catch(...)
	{
		if(raw_index) delete [] raw_index;
		if(compressed_index) delete [] compressed_index;
		throw;
	}
	delete [] raw_index;
	delete [] compressed_index;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa

