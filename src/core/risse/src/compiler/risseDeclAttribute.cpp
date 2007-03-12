//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 宣言時の属性
//---------------------------------------------------------------------------
#include "../prec.h"

#include "risseDeclAttribute.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(59144,28247,41947,16395,152,20119,4221,27137);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
bool tRisseDeclAttribute::Overwrite(const tRisseDeclAttribute & rhs)
{
	bool mem_overwritten = tRisseMemberAttribute::Overwrite(rhs);
	bool overwritten = false;

	if(rhs.Context != ccNone)
	{
		if(Context != ccNone) overwritten = true;
		Context = rhs.Context;
	}

	return overwritten || mem_overwritten;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseDeclAttribute::AsString() const
{
	tRisseString str = tRisseMemberAttribute::AsString();
	if(Context != ccNone)
	{
		if(!str.IsEmpty()) str += RISSE_WC(' ');
		switch(Context)
		{
			case ccStatic:		str += RISSE_WS("static");		break;
			default: ;
		}
	}

	return str;
}
//---------------------------------------------------------------------------














//---------------------------------------------------------------------------
} // namespace Risse
