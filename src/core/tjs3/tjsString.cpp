//---------------------------------------------------------------------------
/*
	TJS3 Script Engine
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// tTJSVariant friendly string class implementation
//---------------------------------------------------------------------------
#include "tjsCommHead.h"

#include <stdarg.h>
#include <stdio.h>


#include "tjsString.h"
#include "tjsVariant.h"


namespace TJS
{
TJS_DEFINE_SOURCE_ID(1030);

const tjs_char *TJSNullStrPtr = TJS_WS("");
//---------------------------------------------------------------------------
tTJSString::tTJSString(const tTJSVariant & val)
{
	Ptr = val.AsString();
}
//---------------------------------------------------------------------------
tTJSString::tTJSString(tjs_int n) // from int
{
	Ptr = TJSIntegerToString(n);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		コンストラクタ(文字列中の%1などを置き換えたいとき)
//! @param		文字列 (中に %1 などの指令を埋め込む)
//! @param		文字列中の %1 と置き換えたい文字列
//---------------------------------------------------------------------------
tTJSString::tTJSString(
	const tTJSString &str,
	const tTJSString &s1)
{
	tjs_offset allocsize = str.GetLen() + s1.GetLen();
	if(allocsize <= 0)
	{
		// str が2CP以下
		Ptr = str.Ptr; if(Ptr) Ptr->AddRef();
	}
	else
	{
		bool s1_emit = false;
		Ptr = TJSAllocVariantStringBuffer(allocsize);
		tjs_char *dest = *Ptr;
		const tjs_char *strp = str.c_str();

		while(*strp)
		{
			if     (*strp == TJS_WC('%') && strp[1] == TJS_WC('1') && !s1_emit)
			{
				strp += 2;
				TJS_strcpy(dest, s1.c_str());
				dest += s1.GetLen();
				s1_emit = true;
			}
			else if(*strp == TJS_WC('%') && strp[1] == TJS_WC('%'))
			{
				strp += 2;
				*(dest++) = TJS_WC('%');
			}
			else
			{
				*(dest++) = *(strp++);
			}
		}
		*dest = 0;

		Ptr->FixLength();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ(文字列中の%1などを置き換えたいとき)
//! @param		文字列 (中に %1 などの指令を埋め込む)
//! @param		文字列中の %1 と置き換えたい文字列
//! @param		文字列中の %2 と置き換えたい文字列
//---------------------------------------------------------------------------
tTJSString::tTJSString(
	const tTJSString &str,
	const tTJSString &s1,
	const tTJSString &s2)
{
	tjs_offset allocsize = str.GetLen() + s1.GetLen() + s2.GetLen();
	if(allocsize <= 0)
	{
		Ptr = str.Ptr; if(Ptr) Ptr->AddRef();
	}
	else
	{
		bool s1_emit = false;
		bool s2_emit = false;
		Ptr = TJSAllocVariantStringBuffer(allocsize);
		tjs_char *dest = *Ptr;
		const tjs_char *strp = str.c_str();

		while(*strp)
		{
			if     (*strp == TJS_WC('%') && strp[1] == TJS_WC('1') && !s1_emit)
			{
				strp += 2;
				TJS_strcpy(dest, s1.c_str());
				dest += s1.GetLen();
				s1_emit = true;
			}
			else if(*strp == TJS_WC('%') && strp[1] == TJS_WC('2') && !s2_emit)
			{
				strp += 2;
				TJS_strcpy(dest, s2.c_str());
				dest += s2.GetLen();
				s2_emit = true;
			}
			else if(*strp == TJS_WC('%') && strp[1] == TJS_WC('%'))
			{
				strp += 2;
				*(dest++) = TJS_WC('%');
			}
			else
			{
				*(dest++) = *(strp++);
			}
		}
		*dest = 0;

		Ptr->FixLength();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ(文字列中の%1などを置き換えたいとき)
//! @param		文字列 (中に %1 などの指令を埋め込む)
//! @param		文字列中の %1 と置き換えたい文字列
//! @param		文字列中の %2 と置き換えたい文字列
//! @param		文字列中の %3 と置き換えたい文字列
//---------------------------------------------------------------------------
tTJSString::tTJSString(
	const tTJSString &str,
	const tTJSString &s1,
	const tTJSString &s2,
	const tTJSString &s3)
{
	tjs_offset allocsize = str.GetLen() + s1.GetLen() + s2.GetLen() + s3.GetLen();
	if(allocsize <= 0)
	{
		Ptr = str.Ptr; if(Ptr) Ptr->AddRef();
	}
	else
	{
		bool s1_emit = false;
		bool s2_emit = false;
		bool s3_emit = false;
		Ptr = TJSAllocVariantStringBuffer(allocsize);
		tjs_char *dest = *Ptr;
		const tjs_char *strp = str.c_str();

		while(*strp)
		{
			if     (*strp == TJS_WC('%') && strp[1] == TJS_WC('1') && !s1_emit)
			{
				strp += 2;
				TJS_strcpy(dest, s1.c_str());
				dest += s1.GetLen();
				s1_emit = true;
			}
			else if(*strp == TJS_WC('%') && strp[1] == TJS_WC('2') && !s2_emit)
			{
				strp += 2;
				TJS_strcpy(dest, s2.c_str());
				dest += s2.GetLen();
				s2_emit = true;
			}
			else if(*strp == TJS_WC('%') && strp[1] == TJS_WC('3') && !s3_emit)
			{
				strp += 2;
				TJS_strcpy(dest, s3.c_str());
				dest += s3.GetLen();
				s3_emit = true;
			}
			else if(*strp == TJS_WC('%') && strp[1] == TJS_WC('%'))
			{
				strp += 2;
				*(dest++) = TJS_WC('%');
			}
			else
			{
				*(dest++) = *(strp++);
			}
		}
		*dest = 0;

		Ptr->FixLength();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ(文字列中の%1などを置き換えたいとき)
//! @param		文字列 (中に %1 などの指令を埋め込む)
//! @param		文字列中の %1 と置き換えたい文字列
//! @param		文字列中の %2 と置き換えたい文字列
//! @param		文字列中の %3 と置き換えたい文字列
//! @param		文字列中の %4 と置き換えたい文字列
//---------------------------------------------------------------------------
tTJSString::tTJSString(
	const tTJSString &str,
	const tTJSString &s1,
	const tTJSString &s2,
	const tTJSString &s3,
	const tTJSString &s4)
{
	tjs_offset allocsize = str.GetLen() +
		s1.GetLen() + s2.GetLen() + s3.GetLen() + s4.GetLen();
	if(allocsize <= 0)
	{
		Ptr = str.Ptr; if(Ptr) Ptr->AddRef();
	}
	else
	{
		bool s1_emit = false;
		bool s2_emit = false;
		bool s3_emit = false;
		Ptr = TJSAllocVariantStringBuffer(allocsize);
		tjs_char *dest = *Ptr;
		const tjs_char *strp = str.c_str();

		while(*strp)
		{
			if     (*strp == TJS_WC('%') && strp[1] == TJS_WC('1') && !s1_emit)
			{
				strp += 2;
				TJS_strcpy(dest, s1.c_str());
				dest += s1.GetLen();
				s1_emit = true;
			}
			else if(*strp == TJS_WC('%') && strp[1] == TJS_WC('2') && !s2_emit)
			{
				strp += 2;
				TJS_strcpy(dest, s2.c_str());
				dest += s2.GetLen();
				s2_emit = true;
			}
			else if(*strp == TJS_WC('%') && strp[1] == TJS_WC('3') && !s3_emit)
			{
				strp += 2;
				TJS_strcpy(dest, s3.c_str());
				dest += s3.GetLen();
				s3_emit = true;
			}
			else if(*strp == TJS_WC('%') && strp[1] == TJS_WC('%'))
			{
				strp += 2;
				*(dest++) = TJS_WC('%');
			}
			else
			{
				*(dest++) = *(strp++);
			}
		}
		*dest = 0;

		Ptr->FixLength();
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tjs_char * tTJSString::InternalIndepend()
{
	// severs sharing of the string instance
	// and returns independent internal buffer

	tTJSVariantString *newstr =
		TJSAllocVariantString(Ptr->operator const tjs_char*());

	Ptr->Release();
	Ptr = newstr;

	return const_cast<tjs_char *>(newstr->operator const tjs_char*());
}
//---------------------------------------------------------------------------
tjs_int64 tTJSString::AsInteger() const
{
	return Ptr->ToInteger();
}
//---------------------------------------------------------------------------
void tTJSString::Replace(const tTJSString &from, const tTJSString &to, bool forall)
{
	// replaces the string partial "from", to "to".
	// all "from" are replaced when "forall" is true.
	if(IsEmpty()) return;
	if(from.IsEmpty()) return;

	tjs_int fromlen = from.GetLen();

	for(;;)
	{
		const tjs_char *st;
		const tjs_char *p;
		st = c_str();
		p = TJS_strstr(st, from.c_str());
		if(p)
		{
			tTJSString name(*this, p-st);
			tTJSString n2(p + fromlen);
			*this = name + to + n2;
			if(!forall) break;
		}
		else
		{
			break;
		}
	}
}
//---------------------------------------------------------------------------
tTJSString tTJSString::AsLowerCase() const
{
	tjs_int len = GetLen();

	if(len == 0) return tTJSString();

	tTJSString ret((tTJSStringBufferLength)(len));

	const tjs_char *s = c_str();
	tjs_char *d = ret.Independ();
	while(*s)
	{
		if(*s >= TJS_WC('A') && *s <= TJS_WC('Z'))
			*d = *s +(TJS_WC('a')-TJS_WC('A'));
		else
			*d = *s;
		d++;
		s++;
	}

	return ret;
}
//---------------------------------------------------------------------------
tTJSString tTJSString::AsUpperCase() const
{
	tjs_int len = GetLen();

	if(len == 0) return tTJSString();

	tTJSString ret((tTJSStringBufferLength)(len));

	const tjs_char *s = c_str();
	tjs_char *d = ret.Independ();
	while(*s)
	{
		if(*s >= TJS_WC('a') && *s <= TJS_WC('z'))
			*d = *s +(TJS_WC('A')-TJS_WC('a'));
		else
			*d = *s;
		d++;
		s++;
	}

	return ret;
}
//---------------------------------------------------------------------------
void tTJSString::ToLowerCase()
{
	tjs_char *p = Independ();
	if(p)
	{
		while(*p)
		{
			if(*p >= TJS_WC('A') && *p <= TJS_WC('Z'))
				*p += (TJS_WC('a')-TJS_WC('A'));
			p++;
		}
	}
}
//---------------------------------------------------------------------------
void tTJSString::ToUppserCase()
{
	tjs_char *p = Independ();
	if(p)
	{
		while(*p)
		{
			if(*p >= TJS_WC('a') && *p <= TJS_WC('z'))
				*p += (TJS_WC('A')-TJS_WC('a'));
			p++;
		}
	}
}
//---------------------------------------------------------------------------
tTJSString tTJSString::EscapeC() const
{
	const tjs_char * hexchars = TJS_WS("0123456789");
	ttstr ret;
	const tjs_char * p = c_str();
	bool hexflag = false;
	for(;*p;p++)
	{
		switch(*p)
		{
		case 0x07: ret += TJS_WS("\\a"); hexflag = false; continue;
		case 0x08: ret += TJS_WS("\\b"); hexflag = false; continue;
		case 0x0c: ret += TJS_WS("\\f"); hexflag = false; continue;
		case 0x0a: ret += TJS_WS("\\n"); hexflag = false; continue;
		case 0x0d: ret += TJS_WS("\\r"); hexflag = false; continue;
		case 0x09: ret += TJS_WS("\\t"); hexflag = false; continue;
		case 0x0b: ret += TJS_WS("\\v"); hexflag = false; continue;
		case TJS_WC('\\'): ret += TJS_WS("\\\\"); hexflag = false; continue;
		case TJS_WC('\''): ret += TJS_WS("\\\'"); hexflag = false; continue;
		case TJS_WC('\"'): ret += TJS_WS("\\\""); hexflag = false; continue;
		default:
			if(hexflag)
			{
				if(*p >= TJS_WC('a') && *p <= TJS_WC('f') ||
					*p >= TJS_WC('A') && *p <= TJS_WC('F') ||
						*p >= TJS_WC('0') && *p <= TJS_WC('9') )
				{
					tjs_char buf[5];
					buf[0] = TJS_WC('\\');
					buf[1] = TJS_WC('x');
					buf[2] = hexchars[ (*p >> 4)  & 0x0f];
					buf[3] = hexchars[ (*p     )  & 0x0f];
					buf[4] = 0;
					hexflag = true;
					ret += buf;
					continue;
				}
			}

			if(*p < 0x20)
			{
				tjs_char buf[5];
				buf[0] = TJS_WC('\\');
				buf[1] = TJS_WC('x');
				buf[2] = hexchars[ (*p >> 4)  & 0x0f];
				buf[3] = hexchars[ (*p     )  & 0x0f];
				buf[4] = 0;
				hexflag = true;
				ret += buf;
			}
			else
			{
				ret += *p;
				hexflag = false;
			}
		}
	}
	return ret;
}
//---------------------------------------------------------------------------
tTJSString tTJSString::UnescapeC() const
{
	// TODO: UnescapeC
	return TJS_WS("");
}
//---------------------------------------------------------------------------
bool tTJSString::StartsWith(const tjs_char *string) const
{
	// return true if this starts with "string"
	if(!Ptr)
	{
		if(!*string) return true; // empty string starts with empty string
		return false;
	}
	const tjs_char *this_p = *Ptr;
	while(*string && *this_p)
	{
		if(*string != *this_p) return false;
		string++, this_p++;
	}
	if(!*string) return true;
	return false;
}
//---------------------------------------------------------------------------
tTJSString operator + (const tjs_char *lhs, const tTJSString &rhs)
{
	tTJSString ret(lhs);
	ret += rhs;
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTJSString TJSInt32ToHex(tjs_uint32 num, int zeropad)
{
	// convert given number to HEX string.
	// zeros are padded when the output string's length is smaller than "zeropad".
	// "zeropad" cannot be larger than 8.
	if(zeropad > 8) zeropad = 8;

	tjs_char buf[12];
	tjs_char buf2[12];

	tjs_char *p = buf;
	tjs_char *d = buf2;

	do
	{
		*(p++) = (TJS_WS("0123456789ABCDEF"))[num % 16];
		num /= 16;
		zeropad --;
	} while(zeropad || num);

	p--;
	while(buf <= p) *(d++) = *(p--);
	*d = 0;

	return ttstr(buf2); 
}
//---------------------------------------------------------------------------
} // namespace TJS

