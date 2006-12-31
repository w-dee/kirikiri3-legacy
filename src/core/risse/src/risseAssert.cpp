//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Assertion 定義
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>


namespace Risse
{
//---------------------------------------------------------------------------
void RisseAssert(const char * message, const char * filename, int lineno)
{
	fflush(stderr);
	fflush(stdout);
	fprintf(stderr, "Risse: Assertion failed: '%s' failed at %s line %d\n",
			message, filename, lineno);
	abort();
}
//---------------------------------------------------------------------------
}

