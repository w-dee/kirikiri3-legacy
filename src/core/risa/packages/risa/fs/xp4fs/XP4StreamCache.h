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

#ifndef XP4STREAMCACHEH
#define XP4STREAMCACHEH

#include "base/utils/Singleton.h"
#include "base/utils/RisaThread.h"


namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//!@brief		ストリームキャッシュクラス
//---------------------------------------------------------------------------
class tXP4StreamCache : public singleton_base<tXP4StreamCache>
{
	static const int MAX_ITEM = 8; //!< キャッシュするハンドル数

	tCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション
	risse_uint Age; //!< キャッシュ世代

	//!@brief キャッシュアイテムの構造体
	struct tItem
	{
		void * Pointer; //!< アーカイブインスタンスへのポインタ
		tBinaryStream * Stream; //!< 入力ストリームオブジェクト
		risse_uint Age; //!< キャッシュ世代
	} Pool[MAX_ITEM];

public:
	//! @brief		コンストラクタ
	tXP4StreamCache();

	//! @brief		デストラクタ
	~tXP4StreamCache();

public:
	//! @brief		ストリームを取得する
	//! @param		pointer アーカイブインスタンスへのポインタ (識別に用いられる)
	//! @param		name アーカイブファイル名
	//! @return		ストリーム
	tBinaryStream * GetStream(void * pointer, const tString & name);

	//! @brief		ストリームを解放する
	//! @param		pointer アーカイブインスタンスへのポインタ
	//! @param		stream ストリーム
	void ReleaseStream(void * pointer, tBinaryStream * stream);

	//! @brief		指定されたポインタを持つストリームをすべて解放する
	//! @param		pointer アーカイブインスタンスへのポインタ
	void ReleaseStreamByPointer(void * pointer);

	//! @brief		すべてのストリームを解放する
	void ReleaseAll();

	//! @brief		内容をクリアする
	void Clear() { ReleaseAll(); }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa



#endif
