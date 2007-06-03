//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オブジェクトのメンバの属性
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseObject.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(8931,16672,6481,19338,53901,49923,23094,28080);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
bool tRisseMemberAttribute::Overwrite(tRisseMemberAttribute rhs)
{
	bool overwritten = false;

	if(rhs.Variable != vcNone)
	{
		if(Variable != vcNone) overwritten = true;
		Variable = rhs.Variable;
	}
	if(rhs.Override != ocNone)
	{
		if(Override != ocNone) overwritten = true;
		Override = rhs.Override;
	}
	if(rhs.Property != pcNone)
	{
		if(Property != pcNone) overwritten = true;
		Property = rhs.Property;
	}

	return overwritten;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseMemberAttribute::AsString() const
{
	tRisseString str;
	if(Variable != vcNone)
	{
		if(!str.IsEmpty()) str += RISSE_WC(' ');
		switch(Variable)
		{
			case vcVar:			str += RISSE_WS("var");			break;
			case vcConst:		str += RISSE_WS("const");		break;
			default: ;
		}
	}
	if(Override != ocNone)
	{
		if(!str.IsEmpty()) str += RISSE_WC(' ');
		switch(Override)
		{
			case ocVirtual:		str += RISSE_WS("virtual");		break;
			case ocFinal:		str += RISSE_WS("final");		break;
			default: ;
		}
	}
	if(Property != pcNone)
	{
		if(!str.IsEmpty()) str += RISSE_WC(' ');
		switch(Property)
		{
			case pcField:		str += RISSE_WS("field");		break;
			case pcProperty:	str += RISSE_WS("property");	break;
			default: ;
		}
	}

	return str;
}
//---------------------------------------------------------------------------














//---------------------------------------------------------------------------
} // namespace Risse
