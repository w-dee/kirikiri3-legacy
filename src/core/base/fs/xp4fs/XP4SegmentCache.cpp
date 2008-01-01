//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
#include "risse/include/risseUtils.h"
#include "base/fs/xp4fs/XP4FS.h"
#include "base/fs/xp4fs/XP4Archive.h"
#include "base/fs/xp4fs/XP4SegmentCache.h"
#include "base/fs/xp4fs/DecompressedHolder.h"
#include <zlib.h>
#include <algorithm>

namespace Risa {
RISSE_DEFINE_SOURCE_ID(49735,29532,60918,17253,28305,59876,22175,55631);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tXP4SegmentCache::tXP4SegmentCache()
{
	TotalBytes = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tXP4SegmentCache::~tXP4SegmentCache()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tXP4SegmentCache::CheckLimit()
{
	volatile tCriticalSection::tLocker cs_holder(CS);

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
void tXP4SegmentCache::Clear()
{
	volatile tCriticalSection::tLocker cs_holder(CS);

	HashTable.Clear();
	TotalBytes = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tXP4SegmentCache::tDataBlock
	tXP4SegmentCache::Find(void * pointer, risse_size storage_index,
		risse_size segment_index,
		tBinaryStream * instream, risse_uint64 dataofs, risse_size insize,
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
	volatile tCriticalSection::tLocker cs_holder(CS);

	// ハッシュテーブルを検索する
	boost::shared_ptr<tDecompressedHolder> * ptr = 
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
		new tDecompressedHolder(
			tDecompressedHolder::dhmZLib,
			instream,
			insize,
			uncomp_size));

	// ハッシュに入れる
	HashTable.AddWithHash(key, hash, block);

	// データブロックを返す
	return block;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


