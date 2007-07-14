//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief UNICODE->CP932(≒SJIS) 変換
//---------------------------------------------------------------------------
// UNICODEとSJIS(cp932)の変換マップについては unicode.org を参照のこと



#ifndef _UNI_CP932_H_
#define _UNI_CP932_H_

namespace Risa {
//---------------------------------------------------------------------------
risse_uint RisaUnicodeToSJIS(risse_char in);
//---------------------------------------------------------------------------
} // namespace Risa

#endif
