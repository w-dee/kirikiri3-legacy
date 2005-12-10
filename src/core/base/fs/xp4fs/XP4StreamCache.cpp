//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief アーカイブファイルのストリームのキャッシュ
//---------------------------------------------------------------------------

#include "XP4FS.h"
#include "XP4StreamCache.h"





//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTVPXP4StreamCache::tTVPXP4StreamCache()
{
	// 変数を初期化
	Age = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPXP4StreamCache::~tTVPXP4StreamCache()
{
	ReleaseAll(); // すべてを解放
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ストリームを取得する
//! @param		pointer: アーカイブインスタンスへのポインタ (識別に用いられる)
//! @param		name: アーカイブファイル名
//! @return		ストリーム
//---------------------------------------------------------------------------
tTJSBinaryStream * tTVPXP4StreamCache::GetStream(void * pointer, const ttstr & name)
{
	volatile tTJSCriticalSectionHolder cs_holder(CS);

	// linear search wiil be enough here because the 
	// TVP_MAX_ARCHIVE_Stream_CACHE is relatively small
	for(tjs_int i =0; i < MAX_ITEM; i++)
	{
		tItem *item = Pool + i;
		if(item->Stream && item->Pointer == pointer)
		{
			// found in the pool
			tTJSBinaryStream * stream = item->Stream;
			item->Stream = NULL;
			return stream;
		}
	}

	// not found in the pool
	// simply create a stream and return it
	return TVPCreateStream(name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ストリームを解放する
//! @param		pointer: アーカイブインスタンスへのポインタ
//! @param		stream: ストリーム
//---------------------------------------------------------------------------
void tTVPXP4StreamCache::ReleaseStream(void * pointer, tTJSBinaryStream * stream)
{
	volatile tTJSCriticalSectionHolder cs_holder(CS);

	// search empty cell in the pool
	tjs_uint oldest_age = 0;
	tjs_int oldest = 0;
	for(tjs_int i = 0; i < MAX_ITEM; i++)
	{
		tItem *item = Pool + i;
		if(item->Stream == NULL)
		{
			// found the empty cell; fill it
			oldest = i;
			break;
		}

		if(i == 0 || oldest_age > item->Age)
		{
			oldest_age = item->Age;
			oldest = i;
		}
	}

	// empty cell not found
	// free oldest cell and fill it.
	// counter overflow in TVPArchiveStreamCacheAge
	// is not so a big problem.
	// counter overflow can worsen the cache performance,
	// but it occurs only when the counter is overflowed
	// (it's too far less than usual)
	tItem *item = Pool + oldest;
	delete oldest_item->Stream, oldest_item->Stream = NULL;
	oldest_item->Pointer = pointer;
	oldest_item->Stream = stream;
	oldest_item->Age = ++Age;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたポインタを持つストリームをすべて解放する
//! @param		pointer: アーカイブインスタンスへのポインタ
//---------------------------------------------------------------------------
void tTVPXP4StreamCache::ReleaseStreamByPointer(void * pointer)
{
	volatile tTJSCriticalSectionHolder cs_holder(CS);

	for(tjs_int i = 0; i < MAX_ITEM; i++)
	{
		tItem *item = Pool + i;
		if(item->Stream && item->Pointer == pointer)
		{
			delete item->Stream, item->Stream = NULL;
			item->Pointer = NULL;
			item->Age = 0;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		すべてのストリームを解放する
//---------------------------------------------------------------------------
void tTVPXP4StreamCache::ReleaseAll()
{
	volatile tTJSCriticalSectionHolder cs_holder(CS);

	for(tjs_int i = 0; i < MAX_ITEM; i++)
	{
		tItem *item = Pool + i;
		if(item->Stream)
		{
			delete item->Stream, item->Stream = NULL;
			item->Pointer = NULL;
			item->Age = 0;
		}
	}
}
//---------------------------------------------------------------------------



