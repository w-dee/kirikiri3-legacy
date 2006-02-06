//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4ファイルシステムの実装
//---------------------------------------------------------------------------
/*
	XP4 アーカイブ中の圧縮されたセグメントは、このソースファイルに実装された
	機構によりメモリ上にキャッシュされる
*/

#include "prec.h"
#include "risseUtils.h"
#include "XP4FS.h"
#include "XP4Archive.h"
#include "XP4SegmentCache.h"
#include "DecompressedHolder.h"
#include <zlib.h>
#include <algorithm>

RISSE_DEFINE_SOURCE_ID(49735,29532,60918,17253,28305,59876,22175,55631);



//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaXP4SegmentCache::tRisaXP4SegmentCache()
{
	TotalBytes = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaXP4SegmentCache::~tRisaXP4SegmentCache()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		キャッシュの上限に達していないかどうかをチェックし、はみ出た分を削除
//---------------------------------------------------------------------------
void tRisaXP4SegmentCache::CheckLimit()
{
	volatile tRisseCriticalSection::tLocker cs_holder(CS);

	// TotalBytes が TOTAL_LIMIT 以下になるまでキャッシュの最後
	// を削除する
	while(TotalBytes > TOTAL_LIMIT)
	{
		// chop last segment
		tHashTable::tIterator i;
		i = HashTable.GetLast();
		if(!i.IsNull())
		{
			risse_size size = i.GetValue()->GetSize();
			TotalBytes -= size;
			HashTable.ChopLast(1);
		}
		else
		{
			break;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		キャッシュをすべてクリアする
//---------------------------------------------------------------------------
void tRisaXP4SegmentCache::Clear()
{
	volatile tRisseCriticalSection::tLocker cs_holder(CS);

	HashTable.Clear();
	TotalBytes = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		キャッシュを検索する(無ければアイテムを作成して返す)
//! @param		pointer アーカイブインスタンスへのポインタ
//! @param		storage_index storage index in archive
//! @param		segment_index segment index in storage
//! @param		instream キャッシュ中に無かった場合に読みに行くストリーム
//! @param		dataofs キャッシュ中に無かった場合に読みに行くストリーム中のデータブロックのオフセット
//! @param		insize キャッシュ中に無かった場合に読みに行くバイト数
//! @param		uncomp_size キャッシュ中に無かった場合に読みに行ったデータを展開したら何バイトになるか
//! @return		展開されたデータブロック
//---------------------------------------------------------------------------
tRisaXP4SegmentCache::tDataBlock
	tRisaXP4SegmentCache::Find(void * pointer, risse_size storage_index,
		risse_size segment_index,
		tRisseBinaryStream * instream, risse_uint64 dataofs, risse_size insize,
		risse_size uncomp_size)
{
	// 検索キーを作る
	tKey key;
	key.Pointer = pointer;
	key.StorageIndex = storage_index;
	key.SegmentIndex = segment_index;

	// ハッシュを作成
	risse_uint32 hash = tKeyHasher::Make(key);

	// これ以降をスレッド保護
	volatile tRisseCriticalSection::tLocker cs_holder(CS);

	// ハッシュテーブルを検索する
	boost::shared_ptr<tRisaDecompressedHolder> * ptr = 
		HashTable.FindAndTouchWithHash(key, hash);
	if(ptr)
	{
		// キャッシュ中にあった
		return *ptr; // データブロックを返す
	}

	// キャッシュ中にない
	// 入力ストリームをシーク
	instream->SetPosition(dataofs);

	// データブロックを新たに作成
	tDataBlock block(
		new tRisaDecompressedHolder(
			tRisaDecompressedHolder::dhmZLib,
			instream,
			insize,
			uncomp_size));

	// ハッシュに入れる
	HashTable.AddWithHash(key, hash, block);

	// データブロックを返す
	return block;
}
//---------------------------------------------------------------------------

