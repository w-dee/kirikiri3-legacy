//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

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
bool tDeclAttribute::Overwrite(const tDeclAttribute & rhs)
{
	bool mem_overwritten = tMemberAttribute::Overwrite(rhs);
	bool overwritten = false;

	if(rhs.Context != ccNone)
	{
		if(Context != ccNone) overwritten = true;
		Context = rhs.Context;
	}

	if(rhs.Sync != scNone)
	{
		if(Sync != scNone) overwritten = true;
		Sync = rhs.Sync;
	}

	return overwritten || mem_overwritten;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tDeclAttribute::AsString() const
{
	tString str = tMemberAttribute::AsString();
	if(Context != ccNone)
	{
		if(!str.IsEmpty()) str += RISSE_WC(' ');
		switch(Context)
		{
			case ccStatic:			str += RISSE_WS("static");		break;
			default: ;
		}
	}
	if(Sync != scNone)
	{
		if(!str.IsEmpty()) str += RISSE_WC(' ');
		switch(Sync)
		{
			case scSynchronized:	str += RISSE_WS("synchronized");		break;
			default: ;
		}
	}

	return str;
}
//---------------------------------------------------------------------------














//---------------------------------------------------------------------------
} // namespace Risse
