//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

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

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * ファイルのハッシュ値を計算する
 * @param callback	進捗コールバックオブジェクト
 * @param filename	ハッシュを計算するファイル名
 */
void tXP4Hash::Make(iRisaProgressCallback * callback, const wxString &filename)
{
	if(find_hash(RISA__XP4_HASH_METHOD_INTERNAL_STRING) == -1)
	{
		int errnum = register_hash(&RISA__XP4_HASH_DESC);
		if(errnum != CRYPT_OK) throw wxString(error_to_string(errnum), wxConvUTF8);
	}

	hash_state st;
	RISA__XP4_HASH_INIT(&st);

	wxFileEx file(filename);

	wxFileOffset size = file.Length();
	wxFileOffset left = size;
	unsigned char buf[8192];
	while(left > 0)
	{
		unsigned long onesize = 
			left > sizeof(buf) ? sizeof(buf) : static_cast<unsigned long>(left);
		file.ReadBuffer(buf, onesize);
		RISA__XP4_HASH_DO_PROCESS(&st, buf, onesize);
		left -= onesize;
		if(callback) callback->OnProgress(static_cast<int>((size - left) * 100 / size));
	}

	RISA__XP4_HASH_DONE(&st, Hash);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * ハッシュ値を標準出力に表示する
 */
void tXP4Hash::Print() const
{
	for(size_t i = 0; i < RISA__XP4_HASH_SIZE; i++)
		wxPrintf(wxT("%02x"), static_cast<int>(Hash[i]));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * ハッシュのチャンク名を得る
 * @return	ハッシュのチャンク名(constではないが内容は変更しないこと!)
 */
/* const */ unsigned  char *  tXP4Hash::GetHashChunkName()
{
	static unsigned char hash_name[] = RISA__XP4_HASH_METHOD_CHUNK_NAME;
	return hash_name;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa

