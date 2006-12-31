//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risaで内部的に用いている例外クラスの管理など
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/exception/RisaException.h"

RISSE_DEFINE_SOURCE_ID(51314,64088,22515,17558,46471,30589,44752,43930);

//---------------------------------------------------------------------------
void eRisaException::Throw(const ttstr &str)
{
	throw eRisaException(str);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void eRisaException::Throw(const ttstr &str, const ttstr & s1)
{
	throw eRisaException(ttstr(str, s1));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void eRisaException::Throw(const ttstr &str, const ttstr & s1, const ttstr & s2)
{
	throw eRisaException(ttstr(str, s1, s2));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void eRisaException::Throw(const ttstr &str, const ttstr & s1, const ttstr & s2, const ttstr & s3)
{
	throw eRisaException(ttstr(str, s1, s2, s3));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void eRisaException::Throw(const ttstr &str, const ttstr & s1, const ttstr & s2, const ttstr & s3, const ttstr & s4)
{
	throw eRisaException(ttstr(str, s1, s2, s3, s4));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void eRisaException::ThrowInternalError(int line, const char * filename)
{
	Throw(RISSE_WS_TR("Internal error at %2 line %1"), ttstr(line), ttstr(wxString(filename, wxConvUTF8)));
}
//---------------------------------------------------------------------------

