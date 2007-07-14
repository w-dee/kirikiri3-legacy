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
#include "risse/include/risseStaticStrings.h"
#include "base/exception/RisaException.h"



namespace Risa {
RISSE_DEFINE_SOURCE_ID(51314,64088,22515,17558,46471,30589,44752,43930);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void eRisaException::Throw(const tString &str)
{
	throw new tTemporaryException(ss_RuntimeException, str);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void eRisaException::Throw(const tString &str, const tString & s1)
{
	throw new tTemporaryException(ss_RuntimeException, tString(str, s1));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void eRisaException::Throw(const tString &str, const tString & s1, const tString & s2)
{
	throw new tTemporaryException(ss_RuntimeException, tString(str, s1, s2));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void eRisaException::Throw(const tString &str, const tString & s1, const tString & s2, const tString & s3)
{
	throw new tTemporaryException(ss_RuntimeException, tString(str, s1, s2, s3));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void eRisaException::Throw(const tString &str, const tString & s1, const tString & s2, const tString & s3, const tString & s4)
{
	throw new tTemporaryException(ss_RuntimeException, tString(str, s1, s2, s3, s4));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void eRisaException::ThrowInternalError(int line, const char * filename)
{
	Throw(RISSE_WS_TR("Internal error at %2 line %1"), tString::AsString(line), tString(wxString(filename, wxConvUTF8)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa

