//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief CP932(≒SJIS)->UNICODE 変換
//---------------------------------------------------------------------------
// UNICODEとSJIS(cp932)の変換マップについては unicode.org を参照のこと



#ifndef _CP932_UNI_
#define _CP932_UNI_

namespace Risa {
//---------------------------------------------------------------------------

risse_size SJISToUnicodeString(const char * in, risse_char *out);
risse_char SJISToUnicode(risse_uint sjis);

//---------------------------------------------------------------------------
} // namespace Risa
#endif

