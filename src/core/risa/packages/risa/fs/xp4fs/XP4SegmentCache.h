//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief アーカイブファイルのセグメントのキャッシュ
//---------------------------------------------------------------------------


#ifndef XP4SEGMENTCACHEH
#define XP4SEGMENTCACHEH


#include <boost/smart_ptr.hpp>
#include "base/fs/xp4fs/DecompressedHolder.h"
#include "base/utils/Singleton.h"
#include "base/utils/RisaThread.h"

namespace Risa {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * セグメントキャッシュクラス
 */
class tXP4SegmentCache : public singleton_base<tXP4SegmentCache>
{
	static const risse_size ONE_LIMIT = 1024*1024; //!< これを超えるセグメントはキャッシュしない
	static const risse_size TOTAL_LIMIT = 1024*1024; //!< トータルでこれ以上はキャッシュしない

	tCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション
	risse_size TotalBytes; //!< このクラスが保持しているトータルのバイト数

	/**
	 * キャッシュアイテムのkeyとなる構造体
	 */
	struct tKey
	{
		void * Pointer; //!< アーカイブインスタンスへのポインタ
		risse_size StorageIndex; //!< storage index in archive
		risse_size SegmentIndex; //!< segment index in storage
		bool operator == (const tKey & rhs) const
			{ return Pointer == rhs.Pointer &&
				StorageIndex == rhs.StorageIndex &&
				SegmentIndex == rhs.SegmentIndex; }
	};

	/**
	 * tKey のハッシュを作成するクラス
	 */
	class tKeyHasher
	{
	public:
		static risse_uint32 Make(const tKey &val)
		{
			risse_uint32 v;
			v  = reinterpret_cast<risse_uint32>(val.Pointer);
			v ^= v >> 4;
			v ^= static_cast<risse_uint32>(val.StorageIndex);
			v ^= static_cast<risse_uint32>(val.SegmentIndex<<2);
			return v;
		}
	};

public:
	typedef boost::shared_ptr<tDecompressedHolder> tDataBlock; //!< キャッシュアイテムのvalueのtypedef

private:
	typedef tHashTable<tKey, tDataBlock, tKeyHasher> tHashTable; //!< ハッシュテーブルのtypedef

	tHashTable HashTable; //!< ハッシュテーブル

public:
	/**
	 * コンストラクタ
	 */
	tXP4SegmentCache();

	/**
	 * デストラクタ
	 */
	~tXP4SegmentCache();

public:
	/**
	 * キャッシュの上限に達していないかどうかをチェックし、はみ出た分を削除
	 */
	void CheckLimit();

	/**
	 * キャッシュをすべてクリアする
	 */
	void Clear();

	/**
	 * キャッシュを検索する(無ければアイテムを作成して返す)
	 * @param pointer		アーカイブインスタンスへのポインタ
	 * @param storage_index	storage index in archive
	 * @param segment_index	segment index in storage
	 * @param instream		キャッシュ中に無かった場合に読みに行くストリーム
	 * @param dataofs		キャッシュ中に無かった場合に読みに行くストリーム中のデータブロックのオフセット
	 * @param insize		キャッシュ中に無かった場合に読みに行くバイト数
	 * @param uncomp_size	キャッシュ中に無かった場合に読みに行ったデータを展開したら何バイトになるか
	 * @return	展開されたデータブロック
	 */
	tDataBlock
		Find(void * pointer, risse_size storage_index, risse_size segment_index,
			tBinaryStream * instream, risse_uint64 dataofs, risse_size insize,
			risse_size uncomp_size);
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa


#endif
