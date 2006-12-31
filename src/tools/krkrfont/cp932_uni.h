//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief CP932(≒SJIS)->UNICODE 変換
//---------------------------------------------------------------------------
// UNICODEとSJIS(cp932)の変換マップについては unicode.org を参照のこと


#ifndef _CP932_UNI_
#define _CP932_UNI_

risse_size RisaSJISToUnicodeString(const char * in, risse_char *out);
risse_char RisaSJISToUnicode(risse_uint sjis);

#endif

