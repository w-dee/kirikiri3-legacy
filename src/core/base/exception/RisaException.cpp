//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risaで内部的に用いている例外クラスの管理など
//---------------------------------------------------------------------------
#include "prec.h"
#include "RisaException.h"

RISSE_DEFINE_SOURCE_ID(2100);

//---------------------------------------------------------------------------
//! @brief		eRisaException型の例外を投げる
//---------------------------------------------------------------------------
void eRisaException::Throw(const ttstr &str)
{
	throw eRisaException(str);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		eRisaException型の例外を投げる
//! @param		str  文字列 (中に %1 などの指令を埋め込む)
//! @param		s1   文字列中の %1 と置き換えたい文字列
//---------------------------------------------------------------------------
void eRisaException::Throw(const ttstr &str, const ttstr & s1)
{
	throw eRisaException(ttstr(str, s1));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		eRisaException型の例外を投げる
//! @param		str  文字列 (中に %1 などの指令を埋め込む)
//! @param		s1   文字列中の %1 と置き換えたい文字列
//! @param		s2   文字列中の %2 と置き換えたい文字列
//---------------------------------------------------------------------------
void eRisaException::Throw(const ttstr &str, const ttstr & s1, const ttstr & s2)
{
	throw eRisaException(ttstr(str, s1, s2));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		eRisaException型の例外を投げる
//! @param		str  文字列 (中に %1 などの指令を埋め込む)
//! @param		s1   文字列中の %1 と置き換えたい文字列
//! @param		s2   文字列中の %2 と置き換えたい文字列
//! @param		s3   文字列中の %3 と置き換えたい文字列
//---------------------------------------------------------------------------
void eRisaException::Throw(const ttstr &str, const ttstr & s1, const ttstr & s2, const ttstr & s3)
{
	throw eRisaException(ttstr(str, s1, s2, s3));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		eRisaException型の例外を投げる
//! @param		str  文字列 (中に %1 などの指令を埋め込む)
//! @param		s1   文字列中の %1 と置き換えたい文字列
//! @param		s2   文字列中の %2 と置き換えたい文字列
//! @param		s3   文字列中の %3 と置き換えたい文字列
//! @param		s4   文字列中の %4 と置き換えたい文字列
//---------------------------------------------------------------------------
void eRisaException::Throw(const ttstr &str, const ttstr & s1, const ttstr & s2, const ttstr & s3, const ttstr & s4)
{
	throw eRisaException(ttstr(str, s1, s2, s3, s4));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		内部エラー例外を発生させる
//! @param		line     エラーの起こった行
//! @param		filename エラーの起こったファイル名
//---------------------------------------------------------------------------
void eRisaException::ThrowInternalError(int line, const char * filename)
{
	Throw(RISSE_WS_TR("Internal error at $2 line $1"), ttstr(line), ttstr(wxString(filename, wxConvUTF8)));
}
//---------------------------------------------------------------------------
