//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ユーティリティ
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseUtils.h"

#if 0
namespace Risse
{
RISSE_DEFINE_SOURCE_ID(3401,9600,47324,18071,37028,228,43889,3977);

//---------------------------------------------------------------------------
iRisseDispatch2 * RisseObjectTraceTarget;
//---------------------------------------------------------------------------
static void RisseTrimStringLength(tRisseString &str, risse_int len)
{
	if(str.GetLen() > len)
	{
		risse_char *p = str.Independ();
		p[len] = 0; // trim length
		if(len >= 3)
		{
			p[len-1] = RISSE_WC('.');
			p[len-2] = RISSE_WC('.');
			p[len-3] = RISSE_WC('.');
		}
		str.FixLen();
	}
}
//---------------------------------------------------------------------------
const risse_char * RisseVariantTypeToTypeString(tRisseVariantType type)
{
	switch(type)
	{
	case tvtVoid: return (const risse_char *)RISSE_WS("void");
	case tvtInteger: return (const risse_char *)RISSE_WS("int");
	case tvtReal: return (const risse_char *)RISSE_WS("real");
	case tvtString: return (const risse_char *)RISSE_WS("string");
	case tvtOctet: return (const risse_char *)RISSE_WS("octet");
	case tvtObject: return (const risse_char *)RISSE_WS("object");
	}
	return (const risse_char *)RISSE_WS("unknown");
}
//---------------------------------------------------------------------------
tRisseString RisseVariantToReadableString(const tRisseVariant &val,
	risse_int maxlen)
{
	// convert given variant to human-readable string
	// ( eg. "(string)\"this is a\\nstring\"" )

	tRisseVariantType type = val.Type();

	switch(type)
	{
	case tvtVoid:
	  {
		tRisseString str(RISSE_WS("(void)"));
		RisseTrimStringLength(str, maxlen);
		return str;
	  }
	case tvtInteger:
	  {
		tRisseString str(RISSE_WS("(int)"));
		str += (tRisseString)val;
		RisseTrimStringLength(str, maxlen);
		return str;
	  }
	case tvtReal:
	  {
		tRisseString str(RISSE_WS("(real)"));
		str += (tRisseString)val;
		RisseTrimStringLength(str, maxlen);
		return str;
	  }
	case tvtString:
	  {
		tRisseString str(RISSE_WS("(string)\""));
		str += ttstr(val).EscapeC();
		str += RISSE_WS("\"");
		RisseTrimStringLength(str, maxlen);
		return str;
	  }
	case tvtOctet:
	  {
		// TODO: octet conversion
		tRisseString str(RISSE_WS("(octet)<% "));
		tRisseVariantString * s = RisseOctetToListString(val.AsOctetNoAddRef());
		try
		{
			str += s;
		}
		catch(...)
		{
			if(s) s->Release();
			throw;
		}
		if(s) s->Release();
		str += RISSE_WS(" %>");
		RisseTrimStringLength(str, maxlen);
		return str;
	  }
	case tvtObject:
	  {
		tRisseString str(RISSE_WS("(object)"));
		try
		{
			str += ttstr(val);
		}
		catch(...)
		{
		}
		RisseTrimStringLength(str, maxlen);
		return str;
	  }
	}
	return RISSE_WS("");
}
//---------------------------------------------------------------------------
tRisseString RisseVariantToExpressionString(const tRisseVariant &val)
{
	// convert given variant to string which can be interpret as an expression.
	// this function does not convert objects ( returns empty string )

	tRisseVariantType type = val.Type();

	switch(type)
	{
	case tvtVoid:
	  {
	  	return RISSE_WS("void");
	  }
	case tvtInteger:
	  {
		return RISSE_WS("int ") + ttstr(val);
	  }
	case tvtReal:
	  {
		tRisseString str(RISSE_WS("real "));
		tRisseVariantString *s = RisseRealToHexString(val.AsReal());
		try
		{
			str += s;
		}
		catch(...)
		{
			if(s) s->Release();
			throw;
		}
		if(s) s->Release();
		return str + RISSE_WS1(" /") + RISSE_WS2("* ") + ttstr(val) + RISSE_WS3(" *") + RISSE_WS4("/");
	  }
	case tvtString:
	  {
		tRisseString str(RISSE_WS("string \""));
		str += ttstr(val).EscapeC();
		str += RISSE_WS("\"");
		return str;
	  }
	case tvtOctet:
	  {
		tRisseString str(RISSE_WS("octet <% "));
		tRisseVariantString * s = RisseOctetToListString(val.AsOctetNoAddRef());
		try
		{
			str += s;
		}
		catch(...)
		{
			if(s) s->Release();
			throw;
		}
		if(s) s->Release();
		str += RISSE_WS(" %>");
		return str;
	  }
	case tvtObject:
	  {
		return RISSE_WS("");
	  }
	}
	return RISSE_WS("");
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// RisseAlignedAlloc : aligned memory allocator
//---------------------------------------------------------------------------
// template classes to determine an integer type which have the same size as void *.
struct tRissePointerSizeEnum { enum tRissePointerSize { size = sizeof(void*) }; };
template <risse_int size> struct tRissePointerSizedIntegerBase { typedef long type; };
template <> struct tRissePointerSizedIntegerBase<8> { typedef risse_uint64 type; };
template <> struct tRissePointerSizedIntegerBase<4> { typedef risse_uint32 type; };
struct tRissePointerSizedInteger : public tRissePointerSizedIntegerBase< tRissePointerSizeEnum::size > {};
//---------------------------------------------------------------------------
void * RisseAlignedAlloc(risse_uint bytes, risse_uint align_bits)
{
	// aligned memory allocation is to be used to gain performance on some processors.
	risse_int align = 1 << align_bits;
	void *ptr = (void *)(new risse_uint8[bytes + align + sizeof(void*)]);
	void *org_ptr = ptr;
	tRissePointerSizedInteger::type *iptr =
		reinterpret_cast<tRissePointerSizedInteger::type *>(&ptr);
	*iptr += align + sizeof(void*);
	*iptr &= ~(tRissePointerSizedInteger::type)(align - 1);
	(reinterpret_cast<void**>(ptr))[-1] = org_ptr;
	return ptr;
}
//---------------------------------------------------------------------------
void RisseAlignedDealloc(void *ptr)
{
	delete [] (risse_uint8*)((reinterpret_cast<void**>(ptr))[-1]);
}
//---------------------------------------------------------------------------




}

#endif
