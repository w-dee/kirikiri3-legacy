//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Assertion 定義
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "risseExceptionClass.h"

namespace Risse
{
//---------------------------------------------------------------------------
RISSE_DEFINE_SOURCE_ID(10710,4962,63524,19088,28325,47284,39725,62590);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void Assert(const char * message, const char * filename, int lineno)
{
	fflush(stderr);
	fflush(stdout);
	fprintf(stderr, "\n\nRisse: Assertion failed: '%s' failed at %s line %d\n",
			message, filename, lineno);

	fflush(stderr);
	fflush(stdout);

	tAssertionErrorClass::Throw(
		tString(RISSE_WS("assertion failed: '%1' failed at source %2 line %3"),
						tString(message),
						tString(filename),
						tString::AsString((risse_int)lineno)));
}
//---------------------------------------------------------------------------
}

