//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4 メタデータの読み込み
//---------------------------------------------------------------------------
#ifndef _XP4HASH_H_
#define _XP4HASH_H_

#include "ProgressCallback.h"
/*
	ここでは、XP4 アーカイブファイルのうち、アーカイブ内ファイルのファイル名と
	タイムスタンプ、フラグを得るだけを目的とする。
	実際の吉里吉里で使用されるルーチン群はパフォーマンスの最適化などで
	かなり複雑になるため、まったく別の実装とする。
*/

//---------------------------------------------------------------------------
// 使用する hash アルゴリズムに関する定義
//---------------------------------------------------------------------------
#define TVP_XP4_HASH_INIT						sha1_init
#define TVP_XP4_HASH_DO_PROCESS					sha1_process
#define TVP_XP4_HASH_DONE						sha1_done
#define TVP_XP4_HASH_DESC						sha1_desc
#define TVP_XP4_HASH_METHOD_STRING				"SHA1"
#define TVP_XP4_HASH_METHOD_INTERNAL_STRING		"sha1"
#define TVP_XP4_HASH_METHOD_CHUNK_NAME			{ 's', 'h', 'a', '1' }
#define TVP_XP4_HASH_SIZE						20
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// ハッシュを表すクラス
//---------------------------------------------------------------------------
class tTVPXP4Hash
{
	unsigned char Hash[TVP_XP4_HASH_SIZE];
	bool HasHash;
public:
	tTVPXP4Hash() { HasHash = false; memset(Hash, 0, TVP_XP4_HASH_SIZE); }

	operator unsigned char *() { return Hash; }
	bool operator < (const tTVPXP4Hash & rhs) const
	{
		return memcmp(Hash, rhs.Hash, sizeof(Hash)) < 0;
	}
	bool operator == (const tTVPXP4Hash & rhs) const
	{
		return HasHash == rhs.HasHash && 
			!memcmp(Hash, rhs.Hash, sizeof(Hash));
	}
	bool operator != (const tTVPXP4Hash & rhs) const
	{
		return !(operator ==(rhs));
	}
	void SetHash(const unsigned char hash[TVP_XP4_HASH_SIZE])
		{ memcpy(Hash, hash, TVP_XP4_HASH_SIZE); HasHash = true; }
	bool GetHasHash() const { return HasHash; }
	void SetHasHash(bool has = true) { HasHash = has; if(!has) memset(Hash, 0, TVP_XP4_HASH_SIZE); }
	static size_t GetSize() { return TVP_XP4_HASH_SIZE; }
	void Make(iTVPProgressCallback * callback, const wxString &filename);
	void Print() const;
	static /* const */ unsigned  char * GetHashChunkName();
};
//---------------------------------------------------------------------------


#endif
