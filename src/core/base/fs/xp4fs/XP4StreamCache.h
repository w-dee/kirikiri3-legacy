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

#ifndef XP4STREAMCACHEH
#define XP4STREAMCACHEH


#include "StorageIntf.h"

#include <boost/singleton.hpp>


//---------------------------------------------------------------------------
//!@brief		ストリームキャッシュクラス
//---------------------------------------------------------------------------
class tTVPXP4StreamCache : public boost::basic_singleton<tTVPXP4StreamCache>
{
	static const int MAX_ITEM = 8; //!< キャッシュするハンドル数

	tTJSCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション
	tjs_uint Age; //!< キャッシュ世代

	//!@brief キャッシュアイテムの構造体
	struct tItem
	{
		void * Pointer; //!< アーカイブインスタンスへのポインタ
		tTJSBinaryStream * Stream; //!< 入力ストリームオブジェクト
		tjs_uint Age; //!< キャッシュ世代
	} Pool[MAX_ITEM];

protected:
	tTVPXP4StreamCache();
	~tTVPXP4StreamCache();

public:
	tTJSBinaryStream * GetStream(void * pointer, const ttstr & name);
	void ReleaseStream(void * pointer, tTJSBinaryStream * stream);
	void ReleaseStreamByPointer(void * pointer);
	void ReleaseAll();
};
//---------------------------------------------------------------------------



#endif
