//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief アーカイブファイルのストリームのキャッシュ
//---------------------------------------------------------------------------
#include "prec.h"


#include "base/fs/xp4fs/XP4FS.h"
#include "base/fs/xp4fs/XP4StreamCache.h"



namespace Risa {
RISSE_DEFINE_SOURCE_ID(49404,13781,19467,19024,25758,42474,10136,44880);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tXP4StreamCache::tXP4StreamCache()
{
	// 変数を初期化
	Age = 0;
	for(int i = 0; i < MAX_ITEM; i++)
	{
		Pool[i].Pointer = NULL;
		Pool[i].Stream = NULL;
		Pool[i].Age = 0;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tXP4StreamCache::~tXP4StreamCache()
{
	ReleaseAll(); // すべてを解放
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tBinaryStream * tXP4StreamCache::GetStream(void * pointer, const tString & name)
{
	volatile tCriticalSection::tLocker cs_holder(CS);

	// linear search wiil be enough here because the 
	// RISA__MAX_ARCHIVE_Stream_CACHE is relatively small
	for(risse_int i =0; i < MAX_ITEM; i++)
	{
		tItem *item = Pool + i;
		if(item->Stream && item->Pointer == pointer)
		{
			// found in the pool
			tBinaryStream * stream = item->Stream;
			item->Stream = NULL;
			return stream;
		}
	}

	// not found in the pool
	// simply create a stream and return it
	return tFileSystemManager::instance()->CreateStream(name, RISSE_BS_READ);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tXP4StreamCache::ReleaseStream(void * pointer, tBinaryStream * stream)
{
	volatile tCriticalSection::tLocker cs_holder(CS);

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
	// counter overflow in ArchiveStreamCacheAge
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
void tXP4StreamCache::ReleaseStreamByPointer(void * pointer)
{
	volatile tCriticalSection::tLocker cs_holder(CS);

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
void tXP4StreamCache::ReleaseAll()
{
	volatile tCriticalSection::tLocker cs_holder(CS);

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



//---------------------------------------------------------------------------
} // namespace Risa


