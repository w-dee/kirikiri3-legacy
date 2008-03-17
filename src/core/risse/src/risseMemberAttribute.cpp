//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
bool tMemberAttribute::Overwrite(tMemberAttribute rhs)
{
	bool overwritten = false;

	if(rhs.Mutability != mcNone)
	{
		if(Mutability != mcNone) overwritten = true;
		Mutability = rhs.Mutability;
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
	if(rhs.Access != acNone)
	{
		if(Access != acNone) overwritten = true;
		Access = rhs.Access;
	}

	return overwritten;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tMemberAttribute::AsString() const
{
	tString str;
	if(Mutability != mcNone)
	{
		if(!str.IsEmpty()) str += RISSE_WC(' ');
		switch(Mutability)
		{
			case mcVar:			str += RISSE_WS("var");			break;
			case mcConst:		str += RISSE_WS("const");		break;
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
	if(Access != acNone)
	{
		if(!str.IsEmpty()) str += RISSE_WC(' ');
		switch(Access)
		{
			case acPublic:		str += RISSE_WS("public");		break;
			case acInternal:	str += RISSE_WS("internal");	break;
			case acProtected:	str += RISSE_WS("protected");	break;
			case acPrivate:		str += RISSE_WS("private");		break;
			default: ;
		}
	}

	return str;
}
//---------------------------------------------------------------------------














//---------------------------------------------------------------------------
} // namespace Risse
