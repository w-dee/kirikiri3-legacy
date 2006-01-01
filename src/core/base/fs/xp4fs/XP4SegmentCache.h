//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief アーカイブファイルのセグメントのキャッシュ
//---------------------------------------------------------------------------


#ifndef XP4SEGMENTCACHEH
#define XP4SEGMENTCACHEH


#include <boost/singleton.hpp>
#include <boost/shared_ptr.hpp>
#include "DecompressedHolder.h"



//---------------------------------------------------------------------------
//!@brief		セグメントキャッシュクラス
//---------------------------------------------------------------------------
class tTVPXP4SegmentCache : public boost::basic_singleton<tTVPXP4SegmentCache>
{
	static const tjs_size ONE_LIMIT = 1024*1024; //!< これを超えるセグメントはキャッシュしない
	static const tjs_size TOTAL_LIMIT = 1024*1024; //!< トータルでこれ以上はキャッシュしない

	tTJSCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション
	tjs_size TotalBytes; //!< このクラスが保持しているトータルのバイト数

	//! @brief キャッシュアイテムのkeyとなる構造体
	struct tKey
	{
		void * Pointer; //!< アーカイブインスタンスへのポインタ
		tjs_size StorageIndex; //!< storage index in archive
		tjs_size SegmentIndex; //!< segment index in storage
	};

	//! @brief tKey のハッシュを作成するクラス
	class tKeyHasher
	{
	public:
		static tjs_uint32 Make(const tKey &val)
		{
			v  = reinterpret_cast<tjs_uint32>(val.Pointer);
			v ^= v >> 4;
			v ^= static_cast<tjs_uint32>(val.StorageIndex);
			v ^= static_cast<tjs_uint32>(val.SegmentIndex<<2);
			return v;
		}
	};

public:
	typedef boost::shared_ptr<tTVPDecompressedHolder> tDataBlock; //!< キャッシュアイテムのvalueのtypedef

private:
	typedef tTJSHashTable<tKey, tDataBlock, tKeyHasher> tHashTable; //!< ハッシュテーブルのtypedef

	tHashTable HashTable; //!< ハッシュテーブル

protected:
	tTVPXP4SegmentCache();
	~tTVPXP4SegmentCache();

public:
	void CheckLimit();
	void Clear();
	tDataBlock
		Find(void * pointer, tjs_size storage_index, tjs_size segment_index,
			tTJSBinaryStream * instream, tjs_uint64 dataofs, tjs_size insize,
			tjs_size uncomp_size);
};
//---------------------------------------------------------------------------



#endif
