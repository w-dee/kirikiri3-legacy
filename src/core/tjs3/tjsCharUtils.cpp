//---------------------------------------------------------------------------
/*
	TJS3 Script Engine
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief tjs_char 操作関数群
//---------------------------------------------------------------------------
#include "tjsCommHead.h"
#include "tjsConfig.h"
#include "tjsCharUtils.h"

TJS_DEFINE_SOURCE_ID(0);

namespace TJS
{

//---------------------------------------------------------------------------
//! @brief		tjs_char 版 atoi
//---------------------------------------------------------------------------
tjs_int TJS_atoi(const tjs_char *s)
{
	int r = 0;
	bool sign = false;
	while(*s && *s <= 0x20) s++; // skip spaces
	if(!*s) return 0;
	if(*s == TJS_WC('-'))
	{
		sign = true;
		s++;
		while(*s && *s <= 0x20) s++; // skip spaces
		if(!*s) return 0;
	}

	while(*s >= TJS_WC('0') && *s <= TJS_WC('9'))
	{
		r *= 10;
		r += *s - TJS_WC('0');
		s++;
	}
	if(sign) r = -r;
	return r;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tjs_int を tjs_char 文字列に変換する
//! @param		value     変換したい値
//! @param		string    出力先文字列バッファ
//! @return		string パラメータで指定した出力先文字列バッファがそのまま帰る
//! @note		string には十分な余裕があることが前提
//---------------------------------------------------------------------------
tjs_char * TJS_int_to_str(tjs_int value, tjs_char *string)
{
	tjs_char *ostring = string;

	if(value<0) *(string++) = TJS_WC('-'), value = -value;

	tjs_char buf[40];

	tjs_char *p = buf;

	do
	{
		*(p++) = (value % 10) + TJS_WC('0');
		value /= 10;
	} while(value);

	p--;
	while(buf <= p) *(string++) = *(p--);
	*string = 0;

	return ostring;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tjs_int64 を tjs_char 文字列に変換する
//! @param		value     変換したい値
//! @param		string    出力先文字列バッファ
//! @return		string パラメータで指定した出力先文字列バッファがそのまま帰る
//! @note		string には十分な余裕があることが前提
//---------------------------------------------------------------------------
tjs_char * TJS_tTVInt_to_str(tjs_int64 value, tjs_char *string)
{
	if((tjs_uint64)value == TJS_UI64_VAL(0x8000000000000000))
	{
		// this is a special number which we must avoid normal conversion
		TJS_strcpy(string, TJS_WS("-9223372036854775808"));
		return string;
	}

	tjs_char *ostring = string;

	if(value<0) *(string++) = TJS_WC('-'), value = -value;

	tjs_char buf[40];

	tjs_char *p = buf;

	do
	{
		*(p++) = (value % 10) + TJS_WC('0');
		value /= 10;
	} while(value);

	p--;
	while(buf <= p) *(string++) = *(p--);
	*string = 0;

	return ostring;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tjs_real を tjs_char 文字列に変換する
//! @param		value     変換したい値
//! @param		string    出力先文字列バッファ
//! @return		string パラメータで指定した出力先文字列バッファがそのまま帰る
//! @note		string には十分な余裕があることが前提
//---------------------------------------------------------------------------
tjs_char * TJS_tTVReal_to_str(tjs_real value, tjs_char *string)
{
	// 実数を文字列に変換する処理は実は結構難しい
	// なので、とりあえず snprintf を使うことにする。
	// snprintf が使えない環境では 一度 sprintf で変換した文字を tjs_char に
	// 変換する。
#ifdef TJS_snprintf
	TJS_snprintf(string, 25, TJS_WS("%.15lg"), value);
	return string;
#else
	char tmp[25];
	snprintf(tmp, sizeof(tmp), "%.15lg", value);
	const char *p = tmp;
	tjs_char *d = string;
	while(*p) *(d++) = static_cast<tjs_char>(*(p++));
	*d = 0;
	return string;
#endif
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ポインタを tjs_char 文字列に変換する
//! @param		value     変換したい値
//! @param		string    出力先文字列バッファ
//! @return		string パラメータで指定した出力先文字列バッファがそのまま帰る
//! @note		string には十分な余裕があることが前提
//---------------------------------------------------------------------------
tjs_char * TJS_pointer_to_str(const void *value, tjs_char *string)
{
#ifdef TJS_snprintf
	TJS_snprintf(string, 25, TJS_WS("%p"), value);
	return string;
#else
	char tmp[25];
	snprintf(tmp, sizeof(tmp), "%p", value);
	const char *p = tmp;
	tjs_char *d = string;
	while(*p) *(d++) = static_cast<tjs_char>(*(p++));
	*d = 0;
	return string;
#endif
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字列のASCII英字の大文字・小文字同一視比較
//! @param		s1     値1
//! @param		s2     値2
//! @return		s1 < s2 ? -1 : s1 > s2 ? 1 : 0
//! @note		この関数は ASCII 英字のみを同一視比較する
//---------------------------------------------------------------------------
tjs_int TJS_straicmp(const tjs_char *s1, const tjs_char *s2)
{
	for(;;)
	{
		tjs_char c1 = *s1, c2 = *s2;
		if(c1 >= TJS_WC('a') && c1 <= TJS_WC('z')) c1 += TJS_WC('Z')-TJS_WC('z');
		if(c2 >= TJS_WC('a') && c2 <= TJS_WC('z')) c2 += TJS_WC('Z')-TJS_WC('z');
		if(c1 == TJS_WC('\0')) return (c2 == TJS_WC('\0')) ? 0 : -1;
		if(c2 == TJS_WC('\0')) return (c1 == TJS_WC('\0')) ? 0 : 1;
		if(c1 < c2) return -1;
		if(c1 > c2) return 1;
		s1++;
		s2++;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		最大で指定コードポイント数分だけ文字列をコピー
//! @param		d    格納先バッファ
//! @param		s    コピー元バッファ
//! @param		len  コピーする最大コードポイント数
//! @note		この関数は strncpy と違い、指定コードポイント数分コピーした後、
//!				最大文字数に達していてもいなくても、d の最後に \0 を追加する。
//!				つまり、d は最低、len + 1 コードポイントの領域が確保されて
//!				いなければならない。また、s が len に達していない場合、
//!				d の残りは null では埋められない。
//---------------------------------------------------------------------------
void TJS_strcpy_maxlen(tjs_char *d, const tjs_char *s, size_t len)
{
	tjs_char ch;
	len++;
	while((ch=*s)!=0 && --len) *(d++) = ch, s++;
	*d = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字列コピー
//! @param		d    格納先バッファ
//! @param		s    コピー元バッファ
//---------------------------------------------------------------------------
void TJS_strcpy(tjs_char *d, const tjs_char *s)
{
	tjs_char ch;
	while((ch=*s)!=0) *(d++) = ch, s++;
	*d = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字列の長さを得る
//! @param		d    文字列
//! @return		文字列長
//---------------------------------------------------------------------------
size_t TJS_strlen(const tjs_char *d)
{
	const tjs_char *p = d;
	while(*d) d++;
	return d-p;
}
//---------------------------------------------------------------------------


#ifdef TJS_WCHAR_T_SIZE_IS_16BIT

//---------------------------------------------------------------------------
//! @brief		文字列を比較する
//! @param		s1   文字列その1
//! @param		s2   文字列その2
//! @return		s1 < s2 ? -1 : s1 > s2 ? 1 : 0
//---------------------------------------------------------------------------
int TJS_strcmp(const tjs_char *s1, const tjs_char *s2)
{
	for(;;)
	{
		if(!*s1) return (!*s2) ? 0 : -1;
		if(*s1 != *s2)
		{
			if(*s1 < *s2) return -1;
			return 1;
		}
		s1++;
		s2++;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字列を最大でnコードポイントまで比較する
//! @param		s1   文字列その1
//! @param		s2   文字列その2
//! @param		n    最大コードポイント数
//! @return		s1 < s2 ? -1 : s1 > s2 ? 1 : 0
//---------------------------------------------------------------------------
int TJS_strncmp(const tjs_char *s1, const tjs_char *s2, size_t n)
{
	for(;;)
	{
		if(!*s1) return (!*s2) ? 0 : -1;
		if(*s1 != *s2)
		{
			if(*s1 < *s2) return -1;
			return 1;
		}
		if(--n) return 0;
		s1++;
		s2++;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		最大で指定コードポイント数分だけ文字列をコピー
//! @param		d    格納先バッファ
//! @param		s    コピー元バッファ
//! @param		len  コピーする最大コードポイント数
//! @return		d が返る
//! @note		動作については strncpy と同じ
//---------------------------------------------------------------------------
tjs_char *TJS_strncpy(tjs_char *d, const tjs_char *s, size_t len)
{
	tjs_char * pd = d;

	len ++;
	while(--len)
	{
		*d = *s;
		d ++;
		if(*s) s++;
	}

	return pd;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字列の連結
//! @param		d    格納先バッファ
//! @param		s    コピー元バッファ
//! @return		d が返る
//! @note		動作については strcat と同じ
//---------------------------------------------------------------------------
tjs_char *TJS_strcat(tjs_char *d, const tjs_char *s)
{
	tjs_char * pd = d;
	while(*d) d++;
	TJS_strcpy(d, s);
	return pd;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字列中の部分文字列の検索
//! @param		s1 検索対象文字列(target)
//! @param		s2 検索する文字列(substring)
//! @return		s1 中の見つかった位置。見つからなかった場合は NULL
//---------------------------------------------------------------------------
tjs_char *TJS_strstr(const tjs_char *s1, const tjs_char *s2)
{
	while(*s1)
	{
		if(*s1 == *s2)
			if(!TJS_strcmp(s1, s2)) return const_cast<tjs_char*>(s1);
		s1 ++;
	}
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字列中のコードポイントの検索
//! @param		s  検索対象文字列(target)
//! @param		c  検索するコードポイント
//! @return		s  中の見つかった位置。見つからなかった場合は NULL
//---------------------------------------------------------------------------
tjs_char *TJS_strchr(const tjs_char *s, int c)
{
	while(*s)
	{
		if(*s == c) return const_cast<tjs_char*>(s);
		s++;
	}
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		数値を表す文字列をdoubleに変換
//! @param		nptr    文字列
//! @param		endptr  数値を表す文字列の次の文字を表すポインタがここに格納される (NULL可)
//! @return		変換された数値
//---------------------------------------------------------------------------
double TJS_strtod(const tjs_char *nptr, tjs_char **endptr)
{
	// ここでは非常に簡易的な実装を採る
	// いったん  buf に内容をコピーしてから それを strtod で変換して帰す。
	char buf[50];
	const tjs_char *p = nptr;
	char *bp = buf;
	while(*p && static_cast<size_t>(bp - buf) < sizeof(buf) - 1 )
		*bp = static_cast<char>(*p), bp++, p++;
	*bp = 0;
	char *ep;
	double val = strtod(buf, &ep);
	*endptr = const_cast<tjs_char *>((ep - buf) + nptr);
	return val;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		UTF16文字列をtjs_char(UTF-32文字列) に変換
//! @param		out 出力文字列 (最低でもinと同じ要素数を持つこと)
//! @param		in  入力文字列
//---------------------------------------------------------------------------
void TJSConvertUTF16ToTJSCharString(tjs_char * out, const tjs_uint16 * in)
{
	while(*in)
	{
		if((*in<0xd800) || (*in>0xdfff))
		{
			*(out++) = *(in++);
		}
		else if ((*in<0xd800) || (*in>0xdfff))
		{
			// 無効領域
			*(out++) = '?';
			in++;
		}
		else
		{
			// サロゲートペア
			*(out++) = ((in[0] - 0xd7c0) << 10) + (in[1] - 0xdc00);
			in += 2;
		}
	}
	*out = 0;
}
//---------------------------------------------------------------------------
#endif  // #ifdef TJS_WCHAR_T_SIZE_IS_16BIT








#ifdef DEBUG
//---------------------------------------------------------------------------
//! @brief		一行中に複数の TJS_WS を使用した場合に例外を送出する関数
//---------------------------------------------------------------------------
void TJSThrowWSAssertionFailure(const wchar_t * source, tjs_int line)
{
	TVPThrowExceptionMessage(
		ttstr(TJS_WS("you can not use multiple TJS_WS in a line; use TJS_WS, TJS_WS2, TJS_WS3 ... at %1 line %2"),
			ttstr(source), ttstr(line)));
}
//---------------------------------------------------------------------------
#endif




} // namespace TJS
