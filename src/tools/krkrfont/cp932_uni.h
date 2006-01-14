//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief CP932(≒SJIS)->UNICODE 変換
//---------------------------------------------------------------------------
// UNICODEとSJIS(cp932)の変換マップについては unicode.org を参照のこと


#ifndef _CP932_UNI_
#define _CP932_UNI_

risse_size TVPSJISToUnicodeString(const char * in, risse_char *out);
risse_char TVPSJISToUnicode(risse_uint sjis);

#endif

