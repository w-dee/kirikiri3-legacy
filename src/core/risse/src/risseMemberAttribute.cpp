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

	if(rhs.Access != acNone)
	{
		if(Access != acNone) overwritten = true;
		Access = rhs.Access;
	}
	if(rhs.Visibility != vcNone)
	{
		if(Visibility != vcNone) overwritten = true;
		Visibility = rhs.Visibility;
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
	if(Access != acNone)
	{
		if(!str.IsEmpty()) str += RISSE_WC(' ');
		switch(Access)
		{
			case acPublic:		str += RISSE_WS("public");		break;
			case acInternal:	str += RISSE_WS("internal");	break;
			case acPrivate:		str += RISSE_WS("private");		break;
			default: ;
		}
	}
	if(Visibility != vcNone)
	{
		if(!str.IsEmpty()) str += RISSE_WC(' ');
		switch(Visibility)
		{
			case vcEnumerable:	str += RISSE_WS("enumerable");	break;
			case vcHidden:		str += RISSE_WS("hidden");		break;
			default: ;
		}
	}
	if(Override != ocNone)
	{
		if(!str.IsEmpty()) str += RISSE_WC(' ');
		switch(Override)
		{
			case ocStatic:		str += RISSE_WS("static");		break;
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
			case pcVar:			str += RISSE_WS("var");			break;
			case pcConst:		str += RISSE_WS("const");		break;
			case pcProperty:	str += RISSE_WS("property");	break;
			default: ;
		}
	}

	return str;
}
//---------------------------------------------------------------------------














//---------------------------------------------------------------------------
} // namespace Risse
