//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief アーカイブファイルのストリームのキャッシュ
//---------------------------------------------------------------------------
#include "prec.h"
RISSE_DEFINE_SOURCE_ID(2010);


#include "XP4FS.h"
#include "XP4StreamCache.h"





//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaXP4StreamCache::tRisaXP4StreamCache()
{
	// 変数を初期化
	Age = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaXP4StreamCache::~tRisaXP4StreamCache()
{
	ReleaseAll(); // すべてを解放
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ストリームを取得する
//! @param		pointer アーカイブインスタンスへのポインタ (識別に用いられる)
//! @param		name アーカイブファイル名
//! @return		ストリーム
//---------------------------------------------------------------------------
tRisseBinaryStream * tRisaXP4StreamCache::GetStream(void * pointer, const ttstr & name)
{
	volatile tRisseCriticalSectionHolder cs_holder(CS);

	// linear search wiil be enough here because the 
	// RISA__MAX_ARCHIVE_Stream_CACHE is relatively small
	for(risse_int i =0; i < MAX_ITEM; i++)
	{
		tItem *item = Pool + i;
		if(item->Stream && item->Pointer == pointer)
		{
			// found in the pool
			tRisseBinaryStream * stream = item->Stream;
			item->Stream = NULL;
			return stream;
		}
	}

	// not found in the pool
	// simply create a stream and return it
	return tRisaFileSystemManager::instance()->CreateStream(name, RISSE_BS_READ);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ストリームを解放する
//! @param		pointer アーカイブインスタンスへのポインタ
//! @param		stream ストリーム
//---------------------------------------------------------------------------
void tRisaXP4StreamCache::ReleaseStream(void * pointer, tRisseBinaryStream * stream)
{
	volatile tRisseCriticalSectionHolder cs_holder(CS);

	// search empty cell in the pool
	risse_uint oldest_age = 0;
	risse_int oldest = 0;
	for(risse_int i = 0; i < MAX_ITEM; i++)
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
	// counter overflow in RisaArchiveStreamCacheAge
	// is not so a big problem.
	// counter overflow can worsen the cache performance,
	// but it occurs only when the counter is overflowed
	// (it's too far less than usual)
	tItem *oldest_item = Pool + oldest;
	delete oldest_item->Stream, oldest_item->Stream = NULL;
	oldest_item->Pointer = pointer;
	oldest_item->Stream = stream;
	oldest_item->Age = ++Age;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたポインタを持つストリームをすべて解放する
//! @param		pointer アーカイブインスタンスへのポインタ
//---------------------------------------------------------------------------
void tRisaXP4StreamCache::ReleaseStreamByPointer(void * pointer)
{
	volatile tRisseCriticalSectionHolder cs_holder(CS);

	for(risse_int i = 0; i < MAX_ITEM; i++)
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
void tRisaXP4StreamCache::ReleaseAll()
{
	volatile tRisseCriticalSectionHolder cs_holder(CS);

	for(risse_int i = 0; i < MAX_ITEM; i++)
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



