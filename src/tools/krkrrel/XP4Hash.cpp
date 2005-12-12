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
#include "prec.h"
#include "wxFileWrapper.h"
#include "XP4Hash.h"
#include "ProgressCallback.h"
#include <tomcrypt.h>


//---------------------------------------------------------------------------
//! @brief		ファイルのハッシュ値を計算する
//! @param		callback 進捗コールバックオブジェクト
//! @param		filename ハッシュを計算するファイル名
//---------------------------------------------------------------------------
void tTVPXP4Hash::Make(iTVPProgressCallback * callback, const wxString &filename)
{
	if(find_hash(TVP_XP4_HASH_METHOD_INTERNAL_STRING) == -1)
	{
		int errnum = register_hash(&TVP_XP4_HASH_DESC);
		if(errnum != CRYPT_OK) throw wxString(error_to_string(errnum), wxConvUTF8);
	}

	hash_state st;
	TVP_XP4_HASH_INIT(&st);

	wxFileEx file(filename);

	wxFileOffset size = file.Length();
	wxFileOffset left = size;
	unsigned char buf[8192];
	while(left > 0)
	{
		unsigned long onesize = 
			left > sizeof(buf) ? sizeof(buf) : static_cast<unsigned long>(left);
		file.ReadBuffer(buf, onesize);
		TVP_XP4_HASH_DO_PROCESS(&st, buf, onesize);
		left -= onesize;
		if(callback) callback->OnProgress(static_cast<int>((size - left) * 100 / size));
	}

	TVP_XP4_HASH_DONE(&st, Hash);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ハッシュ値を標準出力に表示する
//---------------------------------------------------------------------------
void tTVPXP4Hash::Print() const
{
	for(size_t i = 0; i < TVP_XP4_HASH_SIZE; i++)
		wxPrintf(wxT("%02x"), static_cast<int>(Hash[i]));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ハッシュのチャンク名を得る
//! @return		ハッシュのチャンク名(constではないが内容は変更しないこと!)
//---------------------------------------------------------------------------
/* const */ unsigned  char *  tTVPXP4Hash::GetHashChunkName()
{
	static unsigned char hash_name[] = TVP_XP4_HASH_METHOD_CHUNK_NAME;
	return hash_name;
}
//---------------------------------------------------------------------------


