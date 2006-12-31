//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief 文字列クラス tRisseString ( ttstr ) の実装
//---------------------------------------------------------------------------
#include "risseCommHead.h"

#include <stdarg.h>
#include <stdio.h>


#include "risseString.h"
#include "risseVariant.h"


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(35412,4231,23251,16964,5557,10004,55378,56854);

const risse_char *RisseNullStrPtr = RISSE_WS("");
//---------------------------------------------------------------------------
tRisseString::tRisseString(const tRisseVariant & val)
{
	Ptr = val.AsString();
}
//---------------------------------------------------------------------------
tRisseString::tRisseString(risse_int n) // from int
{
	Ptr = RisseIntegerToString(n);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		コンストラクタ(文字列中の%1などを置き換えたいとき)
//! @param		str  文字列 (中に %1 などの指令を埋め込む)
//! @param		s1   文字列中の %1 と置き換えたい文字列
//---------------------------------------------------------------------------
tRisseString::tRisseString(
	const tRisseString &str,
	const tRisseString &s1)
{
	risse_offset allocsize = str.GetLen() + s1.GetLen();
	if(allocsize <= 0)
	{
		// str が2CP以下
		Ptr = str.Ptr; if(Ptr) Ptr->AddRef();
	}
	else
	{
		bool s1_emit = false;
		Ptr = RisseAllocVariantStringBuffer(allocsize);
		risse_char *dest = *Ptr;
		const risse_char *strp = str.c_str();

		while(*strp)
		{
			if     (*strp == RISSE_WC('%') && strp[1] == RISSE_WC('1') && !s1_emit)
			{
				strp += 2;
				Risse_strcpy(dest, s1.c_str());
				dest += s1.GetLen();
				s1_emit = true;
			}
			else if(*strp == RISSE_WC('%') && strp[1] == RISSE_WC('%'))
			{
				strp += 2;
				*(dest++) = RISSE_WC('%');
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
//! @param		str  文字列 (中に %1 などの指令を埋め込む)
//! @param		s1   文字列中の %1 と置き換えたい文字列
//! @param		s2   文字列中の %2 と置き換えたい文字列
//---------------------------------------------------------------------------
tRisseString::tRisseString(
	const tRisseString &str,
	const tRisseString &s1,
	const tRisseString &s2)
{
	risse_offset allocsize = str.GetLen() + s1.GetLen() + s2.GetLen();
	if(allocsize <= 0)
	{
		Ptr = str.Ptr; if(Ptr) Ptr->AddRef();
	}
	else
	{
		bool s1_emit = false;
		bool s2_emit = false;
		Ptr = RisseAllocVariantStringBuffer(allocsize);
		risse_char *dest = *Ptr;
		const risse_char *strp = str.c_str();

		while(*strp)
		{
			if     (*strp == RISSE_WC('%') && strp[1] == RISSE_WC('1') && !s1_emit)
			{
				strp += 2;
				Risse_strcpy(dest, s1.c_str());
				dest += s1.GetLen();
				s1_emit = true;
			}
			else if(*strp == RISSE_WC('%') && strp[1] == RISSE_WC('2') && !s2_emit)
			{
				strp += 2;
				Risse_strcpy(dest, s2.c_str());
				dest += s2.GetLen();
				s2_emit = true;
			}
			else if(*strp == RISSE_WC('%') && strp[1] == RISSE_WC('%'))
			{
				strp += 2;
				*(dest++) = RISSE_WC('%');
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
//! @param		str  文字列 (中に %1 などの指令を埋め込む)
//! @param		s1   文字列中の %1 と置き換えたい文字列
//! @param		s2   文字列中の %2 と置き換えたい文字列
//! @param		s3   文字列中の %3 と置き換えたい文字列
//---------------------------------------------------------------------------
tRisseString::tRisseString(
	const tRisseString &str,
	const tRisseString &s1,
	const tRisseString &s2,
	const tRisseString &s3)
{
	risse_offset allocsize = str.GetLen() + s1.GetLen() + s2.GetLen() + s3.GetLen();
	if(allocsize <= 0)
	{
		Ptr = str.Ptr; if(Ptr) Ptr->AddRef();
	}
	else
	{
		bool s1_emit = false;
		bool s2_emit = false;
		bool s3_emit = false;
		Ptr = RisseAllocVariantStringBuffer(allocsize);
		risse_char *dest = *Ptr;
		const risse_char *strp = str.c_str();

		while(*strp)
		{
			if     (*strp == RISSE_WC('%') && strp[1] == RISSE_WC('1') && !s1_emit)
			{
				strp += 2;
				Risse_strcpy(dest, s1.c_str());
				dest += s1.GetLen();
				s1_emit = true;
			}
			else if(*strp == RISSE_WC('%') && strp[1] == RISSE_WC('2') && !s2_emit)
			{
				strp += 2;
				Risse_strcpy(dest, s2.c_str());
				dest += s2.GetLen();
				s2_emit = true;
			}
			else if(*strp == RISSE_WC('%') && strp[1] == RISSE_WC('3') && !s3_emit)
			{
				strp += 2;
				Risse_strcpy(dest, s3.c_str());
				dest += s3.GetLen();
				s3_emit = true;
			}
			else if(*strp == RISSE_WC('%') && strp[1] == RISSE_WC('%'))
			{
				strp += 2;
				*(dest++) = RISSE_WC('%');
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
//! @param		str  文字列 (中に %1 などの指令を埋め込む)
//! @param		s1   文字列中の %1 と置き換えたい文字列
//! @param		s2   文字列中の %2 と置き換えたい文字列
//! @param		s3   文字列中の %3 と置き換えたい文字列
//! @param		s4   文字列中の %4 と置き換えたい文字列
//---------------------------------------------------------------------------
tRisseString::tRisseString(
	const tRisseString &str,
	const tRisseString &s1,
	const tRisseString &s2,
	const tRisseString &s3,
	const tRisseString &s4)
{
	risse_offset allocsize = str.GetLen() +
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
		Ptr = RisseAllocVariantStringBuffer(allocsize);
		risse_char *dest = *Ptr;
		const risse_char *strp = str.c_str();

		while(*strp)
		{
			if     (*strp == RISSE_WC('%') && strp[1] == RISSE_WC('1') && !s1_emit)
			{
				strp += 2;
				Risse_strcpy(dest, s1.c_str());
				dest += s1.GetLen();
				s1_emit = true;
			}
			else if(*strp == RISSE_WC('%') && strp[1] == RISSE_WC('2') && !s2_emit)
			{
				strp += 2;
				Risse_strcpy(dest, s2.c_str());
				dest += s2.GetLen();
				s2_emit = true;
			}
			else if(*strp == RISSE_WC('%') && strp[1] == RISSE_WC('3') && !s3_emit)
			{
				strp += 2;
				Risse_strcpy(dest, s3.c_str());
				dest += s3.GetLen();
				s3_emit = true;
			}
			else if(*strp == RISSE_WC('%') && strp[1] == RISSE_WC('%'))
			{
				strp += 2;
				*(dest++) = RISSE_WC('%');
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
risse_char * tRisseString::InternalIndepend()
{
	// severs sharing of the string instance
	// and returns independent internal buffer

	tRisseVariantString *newstr =
		RisseAllocVariantString(Ptr->operator const risse_char*());

	Ptr->Release();
	Ptr = newstr;

	return const_cast<risse_char *>(newstr->operator const risse_char*());
}
//---------------------------------------------------------------------------
risse_int64 tRisseString::AsInteger() const
{
	return Ptr->ToInteger();
}
//---------------------------------------------------------------------------
void tRisseString::Replace(const tRisseString &from, const tRisseString &to, bool forall)
{
	// replaces the string partial "from", to "to".
	// all "from" are replaced when "forall" is true.
	if(IsEmpty()) return;
	if(from.IsEmpty()) return;

	risse_int fromlen = from.GetLen();

	for(;;)
	{
		const risse_char *st;
		const risse_char *p;
		st = c_str();
		p = Risse_strstr(st, from.c_str());
		if(p)
		{
			tRisseString name(*this, p-st);
			tRisseString n2(p + fromlen);
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
tRisseString tRisseString::AsLowerCase() const
{
	risse_int len = GetLen();

	if(len == 0) return tRisseString();

	tRisseString ret((tRisseStringBufferLength)(len));

	const risse_char *s = c_str();
	risse_char *d = ret.Independ();
	while(*s)
	{
		if(*s >= RISSE_WC('A') && *s <= RISSE_WC('Z'))
			*d = *s +(RISSE_WC('a')-RISSE_WC('A'));
		else
			*d = *s;
		d++;
		s++;
	}

	return ret;
}
//---------------------------------------------------------------------------
tRisseString tRisseString::AsUpperCase() const
{
	risse_int len = GetLen();

	if(len == 0) return tRisseString();

	tRisseString ret((tRisseStringBufferLength)(len));

	const risse_char *s = c_str();
	risse_char *d = ret.Independ();
	while(*s)
	{
		if(*s >= RISSE_WC('a') && *s <= RISSE_WC('z'))
			*d = *s +(RISSE_WC('A')-RISSE_WC('a'));
		else
			*d = *s;
		d++;
		s++;
	}

	return ret;
}
//---------------------------------------------------------------------------
void tRisseString::ToLowerCase()
{
	risse_char *p = Independ();
	if(p)
	{
		while(*p)
		{
			if(*p >= RISSE_WC('A') && *p <= RISSE_WC('Z'))
				*p += (RISSE_WC('a')-RISSE_WC('A'));
			p++;
		}
	}
}
//---------------------------------------------------------------------------
void tRisseString::ToUppserCase()
{
	risse_char *p = Independ();
	if(p)
	{
		while(*p)
		{
			if(*p >= RISSE_WC('a') && *p <= RISSE_WC('z'))
				*p += (RISSE_WC('A')-RISSE_WC('a'));
			p++;
		}
	}
}
//---------------------------------------------------------------------------
tRisseString tRisseString::EscapeC() const
{
	const risse_char * hexchars = RISSE_WS("0123456789");
	ttstr ret;
	const risse_char * p = c_str();
	bool hexflag = false;
	for(;*p;p++)
	{
		switch(*p)
		{
		case 0x07: ret += RISSE_WS("\\a"); hexflag = false; continue;
		case 0x08: ret += RISSE_WS("\\b"); hexflag = false; continue;
		case 0x0c: ret += RISSE_WS("\\f"); hexflag = false; continue;
		case 0x0a: ret += RISSE_WS("\\n"); hexflag = false; continue;
		case 0x0d: ret += RISSE_WS("\\r"); hexflag = false; continue;
		case 0x09: ret += RISSE_WS("\\t"); hexflag = false; continue;
		case 0x0b: ret += RISSE_WS("\\v"); hexflag = false; continue;
		case RISSE_WC('\\'): ret += RISSE_WS("\\\\"); hexflag = false; continue;
		case RISSE_WC('\''): ret += RISSE_WS("\\\'"); hexflag = false; continue;
		case RISSE_WC('\"'): ret += RISSE_WS("\\\""); hexflag = false; continue;
		default:
			if(hexflag)
			{
				if(*p >= RISSE_WC('a') && *p <= RISSE_WC('f') ||
					*p >= RISSE_WC('A') && *p <= RISSE_WC('F') ||
						*p >= RISSE_WC('0') && *p <= RISSE_WC('9') )
				{
					risse_char buf[5];
					buf[0] = RISSE_WC('\\');
					buf[1] = RISSE_WC('x');
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
				risse_char buf[5];
				buf[0] = RISSE_WC('\\');
				buf[1] = RISSE_WC('x');
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
tRisseString tRisseString::UnescapeC() const
{
	// TODO: UnescapeC
	return RISSE_WS("");
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字列が指定された文字列で始まっているかどうかをチェック
//! @param		string 文字列
//! @return		文字列が指定された文字列で始まっていれば真、そうでなければ偽
//---------------------------------------------------------------------------
bool tRisseString::StartsWith(const risse_char *string) const
{
	// return true if this starts with "string"
	if(!Ptr)
	{
		if(!*string) return true; // empty string starts with empty string
		return false;
	}
	const risse_char *this_p = *Ptr;
	while(*string && *this_p)
	{
		if(*string != *this_p) return false;
		string++, this_p++;
	}
	if(!*string) return true;
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字列が指定された文字列で終わっているかどうかをチェック
//! @param		string 文字列
//! @return		文字列が指定された文字列で終わっていれば真、そうでなければ偽
//---------------------------------------------------------------------------
bool tRisseString::EndsWith(const risse_char *string) const
{
	if(!Ptr)
	{
		if(!*string) return true; // empty string ends with empty string
		return false;
	}

	risse_size string_len = Risse_strlen(string);
	risse_size this_len = Ptr->GetLength();

	if(string_len > this_len) return false; // string が この文字列よりも長いので偽

	return !Risse_strcmp(string, *Ptr + this_len - string_len);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		UTF-8文字列に変換を行った場合の長さを返す
//! @return		UTF-8文字列の長さ(null terminatorを含まず) (size_t)-1L = 変換に失敗した場合
//---------------------------------------------------------------------------
size_t tRisseString::GetUtf8Length() const
{
	return RisseRisseCharToUtf8String(c_str(), NULL);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		UTF-8文字列に変換する
//! @param		dest 書き込み先 (GetUtf8Length() +1 の長さが最低でも必要)
//---------------------------------------------------------------------------
void tRisseString::GetUtf8String(char * dest) const
{
	size_t written = RisseRisseCharToUtf8String(c_str(), dest);
	if(written != static_cast<size_t>(-1L))
		dest[written] = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString operator + (const risse_char *lhs, const tRisseString &rhs)
{
	tRisseString ret(lhs);
	ret += rhs;
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString RisseInt32ToHex(risse_uint32 num, int zeropad)
{
	// convert given number to HEX string.
	// zeros are padded when the output string's length is smaller than "zeropad".
	// "zeropad" cannot be larger than 8.
	if(zeropad > 8) zeropad = 8;

	risse_char buf[12];
	risse_char buf2[12];

	risse_char *p = buf;
	risse_char *d = buf2;

	do
	{
		*(p++) = (RISSE_WS("0123456789ABCDEF"))[num % 16];
		num /= 16;
		zeropad --;
	} while(zeropad || num);

	p--;
	while(buf <= p) *(d++) = *(p--);
	*d = 0;

	return ttstr(buf2); 
}
//---------------------------------------------------------------------------

tRisseString RisseEmptyString; // holds an empty string

} // namespace Risse

