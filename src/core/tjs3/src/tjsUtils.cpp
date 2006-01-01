//---------------------------------------------------------------------------
/*
	TJS3 Script Engine
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief ユーティリティ
//---------------------------------------------------------------------------
#include "tjsCommHead.h"

#include "tjsUtils.h"

namespace TJS
{
TJS_DEFINE_SOURCE_ID(1031);

//---------------------------------------------------------------------------
iTJSDispatch2 * TJSObjectTraceTarget;
//---------------------------------------------------------------------------
static void TJSTrimStringLength(tTJSString &str, tjs_int len)
{
	if(str.GetLen() > len)
	{
		tjs_char *p = str.Independ();
		p[len] = 0; // trim length
		if(len >= 3)
		{
			p[len-1] = TJS_WC('.');
			p[len-2] = TJS_WC('.');
			p[len-3] = TJS_WC('.');
		}
		str.FixLen();
	}
}
//---------------------------------------------------------------------------
const tjs_char * TJSVariantTypeToTypeString(tTJSVariantType type)
{
	switch(type)
	{
	case tvtVoid: return (const tjs_char *)TJS_WS("void");
	case tvtInteger: return (const tjs_char *)TJS_WS("int");
	case tvtReal: return (const tjs_char *)TJS_WS("real");
	case tvtString: return (const tjs_char *)TJS_WS("string");
	case tvtOctet: return (const tjs_char *)TJS_WS("octet");
	case tvtObject: return (const tjs_char *)TJS_WS("object");
	}
	return (const tjs_char *)TJS_WS("unknown");
}
//---------------------------------------------------------------------------
tTJSString TJSVariantToReadableString(const tTJSVariant &val,
	tjs_int maxlen)
{
	// convert given variant to human-readable string
	// ( eg. "(string)\"this is a\\nstring\"" )

	tTJSVariantType type = val.Type();

	switch(type)
	{
	case tvtVoid:
	  {
		tTJSString str(TJS_WS("(void)"));
		TJSTrimStringLength(str, maxlen);
		return str;
	  }
	case tvtInteger:
	  {
		tTJSString str(TJS_WS("(int)"));
		str += (tTJSString)val;
		TJSTrimStringLength(str, maxlen);
		return str;
	  }
	case tvtReal:
	  {
		tTJSString str(TJS_WS("(real)"));
		str += (tTJSString)val;
		TJSTrimStringLength(str, maxlen);
		return str;
	  }
	case tvtString:
	  {
		tTJSString str(TJS_WS("(string)\""));
		str += ttstr(val).EscapeC();
		str += TJS_WS("\"");
		TJSTrimStringLength(str, maxlen);
		return str;
	  }
	case tvtOctet:
	  {
		// TODO: octet conversion
		tTJSString str(TJS_WS("(octet)<% "));
		tTJSVariantString * s = TJSOctetToListString(val.AsOctetNoAddRef());
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
		str += TJS_WS(" %>");
		TJSTrimStringLength(str, maxlen);
		return str;
	  }
	case tvtObject:
	  {
		tTJSString str(TJS_WS("(object)"));
		try
		{
			str += ttstr(val);
		}
		catch(...)
		{
		}
		TJSTrimStringLength(str, maxlen);
		return str;
	  }
	}
	return TJS_WS("");
}
//---------------------------------------------------------------------------
tTJSString TJSVariantToExpressionString(const tTJSVariant &val)
{
	// convert given variant to string which can be interpret as an expression.
	// this function does not convert objects ( returns empty string )

	tTJSVariantType type = val.Type();

	switch(type)
	{
	case tvtVoid:
	  {
	  	return TJS_WS("void");
	  }
	case tvtInteger:
	  {
		return TJS_WS("int ") + ttstr(val);
	  }
	case tvtReal:
	  {
		tTJSString str(TJS_WS("real "));
		tTJSVariantString *s = TJSRealToHexString(val.AsReal());
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
		return str + TJS_WS(" /" "* ") + ttstr(val) + TJS_WS(" *" "/");
	  }
	case tvtString:
	  {
		tTJSString str(TJS_WS("string \""));
		str += ttstr(val).EscapeC();
		str += TJS_WS("\"");
		return str;
	  }
	case tvtOctet:
	  {
		tTJSString str(TJS_WS("octet <% "));
		tTJSVariantString * s = TJSOctetToListString(val.AsOctetNoAddRef());
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
		str += TJS_WS(" %>");
		return str;
	  }
	case tvtObject:
	  {
		return TJS_WS("");
	  }
	}
	return TJS_WS("");
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// TJSAlignedAlloc : aligned memory allocator
//---------------------------------------------------------------------------
// template classes to determine an integer type which have the same size as void *.
struct tTJSPointerSizeEnum { enum tTJSPointerSize { size = sizeof(void*) }; };
template <tjs_int size> struct tTJSPointerSizedIntegerBase { typedef long type; };
template <> struct tTJSPointerSizedIntegerBase<8> { typedef tjs_uint64 type; };
template <> struct tTJSPointerSizedIntegerBase<4> { typedef tjs_uint32 type; };
struct tTJSPointerSizedInteger : public tTJSPointerSizedIntegerBase< tTJSPointerSizeEnum::size > {};
//---------------------------------------------------------------------------
void * TJSAlignedAlloc(tjs_uint bytes, tjs_uint align_bits)
{
	// aligned memory allocation is to be used to gain performance on some processors.
	tjs_int align = 1 << align_bits;
	void *ptr = (void *)(new tjs_uint8[bytes + align + sizeof(void*)]);
	void *org_ptr = ptr;
	tTJSPointerSizedInteger::type *iptr =
		reinterpret_cast<tTJSPointerSizedInteger::type *>(&ptr);
	*iptr += align + sizeof(void*);
	*iptr &= ~(tTJSPointerSizedInteger::type)(align - 1);
	(reinterpret_cast<void**>(ptr))[-1] = org_ptr;
	return ptr;
}
//---------------------------------------------------------------------------
void TJSAlignedDealloc(void *ptr)
{
	delete [] (tjs_uint8*)((reinterpret_cast<void**>(ptr))[-1]);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// floating-point class checker
//---------------------------------------------------------------------------
tjs_uint32 TJSGetFPClass(tjs_real r)
{
	tjs_uint64 *ui64 = (tjs_uint64*)&r;

	if(TJS_IEEE_D_IS_NaN(*ui64))
	{
		if(TJS_IEEE_D_SIGN_MASK & *ui64)
			return TJS_FC_CLASS_NAN | TJS_FC_SIGN_MASK;
		else
			return TJS_FC_CLASS_NAN;
	}
	if(TJS_IEEE_D_IS_INF(*ui64))
	{
		if(TJS_IEEE_D_SIGN_MASK & *ui64)
			return TJS_FC_CLASS_INF | TJS_FC_SIGN_MASK;
		else
			return TJS_FC_CLASS_INF;
	}
	if(TJS_IEEE_D_SIGN_MASK & *ui64)
		return TJS_FC_CLASS_NORMAL | TJS_FC_SIGN_MASK;
	else
		return TJS_FC_CLASS_NORMAL;
}
//---------------------------------------------------------------------------

}