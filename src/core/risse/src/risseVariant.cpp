//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief バリアント型の実装
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseVariant.h"


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(8265,43737,22162,17503,41631,46790,57901,27164);





//---------------------------------------------------------------------------
const risse_char * tRisseVariantBlock::GetTypeString(tType type)
{
	switch(type)
	{
	case vtVoid:		return RISSE_WS("void");
	case vtInteger:		return RISSE_WS("integer");
	case vtReal:		return RISSE_WS("real");
	case vtBoolean:		return RISSE_WS("boolean");
	case vtString:		return RISSE_WS("string");
	case vtObject:		return RISSE_WS("object");
	case vtOctet:		return RISSE_WS("octet");
	}
	return NULL;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tRisseString tRisseVariantBlock::AsHumanReadable_Void     (risse_size maxlen) const
{
	return RISSE_WS("void");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseVariantBlock::CastToString_Integer  () const
{
	risse_char buf[40];
	Risse_int64_to_str(AsInteger(), buf);
	return buf;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseVariantBlock::CastToString_Real     () const
{
	risse_int32 cls = RisseGetFPClass(AsReal());

	if(RISSE_FC_IS_NAN(cls))
	{
		return RISSE_WS("NaN");
	}
	if(RISSE_FC_IS_INF(cls))
	{
		if(RISSE_FC_IS_NEGATIVE(cls))
			return RISSE_WS("-Infinity");
		else
			return RISSE_WS("+Infinity");
	}
	if(AsReal() == 0.0)
	{
		if(RISSE_FC_IS_NEGATIVE(cls))
			return RISSE_WS("-0.0");
		else
			return RISSE_WS("+0.0");
	}

	risse_char buf[25];
	Risse_real_to_str(AsReal(), buf);
	return buf;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseVariantBlock::CastToString_Boolean  () const
{
	return AsBoolean()?
		RISSE_WS("true"):
		RISSE_WS("false");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
