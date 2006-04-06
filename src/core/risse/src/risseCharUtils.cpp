//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief risse_char 操作関数群
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseConfig.h"
#include "risseCharUtils.h"
#include "risseCxxString.h"


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(1411,29360,55440,18029,27323,38271,59690,56185);

//---------------------------------------------------------------------------
//! @brief		risse_char 版 atoi
//---------------------------------------------------------------------------
risse_int Risse_atoi(const risse_char *s)
{
	int r = 0;
	bool sign = false;
	while(*s && *s <= 0x20) s++; // skip spaces
	if(!*s) return 0;
	if(*s == RISSE_WC('-'))
	{
		sign = true;
		s++;
		while(*s && *s <= 0x20) s++; // skip spaces
		if(!*s) return 0;
	}

	while(*s >= RISSE_WC('0') && *s <= RISSE_WC('9'))
	{
		r *= 10;
		r += *s - RISSE_WC('0');
		s++;
	}
	if(sign) r = -r;
	return r;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		risse_int を risse_char 文字列に変換する
//! @param		value     変換したい値
//! @param		string    出力先文字列バッファ
//! @return		string パラメータで指定した出力先文字列バッファがそのまま帰る
//! @note		string には十分な余裕があることが前提
//---------------------------------------------------------------------------
risse_char * Risse_int_to_str(risse_int value, risse_char *string)
{
	risse_char *ostring = string;

	if(value<0) *(string++) = RISSE_WC('-'), value = -value;

	risse_char buf[40];

	risse_char *p = buf;

	do
	{
		*(p++) = (value % 10) + RISSE_WC('0');
		value /= 10;
	} while(value);

	p--;
	while(buf <= p) *(string++) = *(p--);
	*string = 0;

	return ostring;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		risse_int64 を risse_char 文字列に変換する
//! @param		value     変換したい値
//! @param		string    出力先文字列バッファ
//! @return		string パラメータで指定した出力先文字列バッファがそのまま帰る
//! @note		string には十分な余裕があることが前提
//---------------------------------------------------------------------------
risse_char * Risse_tTVInt_to_str(risse_int64 value, risse_char *string)
{
	if((risse_uint64)value == RISSE_UI64_VAL(0x8000000000000000))
	{
		// this is a special number which we must avoid normal conversion
		Risse_strcpy(string, RISSE_WS("-9223372036854775808"));
		return string;
	}

	risse_char *ostring = string;

	if(value<0) *(string++) = RISSE_WC('-'), value = -value;

	risse_char buf[40];

	risse_char *p = buf;

	do
	{
		*(p++) = (value % 10) + RISSE_WC('0');
		value /= 10;
	} while(value);

	p--;
	while(buf <= p) *(string++) = *(p--);
	*string = 0;

	return ostring;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		risse_real を risse_char 文字列に変換する
//! @param		value     変換したい値
//! @param		string    出力先文字列バッファ
//! @return		string パラメータで指定した出力先文字列バッファがそのまま帰る
//! @note		string には十分な余裕があることが前提
//---------------------------------------------------------------------------
risse_char * Risse_tTVReal_to_str(risse_real value, risse_char *string)
{
	// 実数を文字列に変換する処理は実は結構難しい
	// なので、とりあえず snprintf を使うことにする。
	// snprintf が使えない環境では 一度 sprintf で変換した文字を risse_char に
	// 変換する。
#ifdef Risse_snprintf
	Risse_snprintf(string, 25, RISSE_WS("%.15lg"), value);
	return string;
#else
	char tmp[25];
#ifdef _MSC_VER
	_snprintf(tmp, sizeof(tmp), "%.15lg", value);
#else
	snprintf(tmp, sizeof(tmp), "%.15lg", value);
#endif
	const char *p = tmp;
	risse_char *d = string;
	while(*p) *(d++) = static_cast<risse_char>(*(p++));
	*d = 0;
	return string;
#endif
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ポインタを risse_char 文字列に変換する
//! @param		value     変換したい値
//! @param		string    出力先文字列バッファ
//! @return		string パラメータで指定した出力先文字列バッファがそのまま帰る
//! @note		string には十分な余裕があることが前提
//---------------------------------------------------------------------------
risse_char * Risse_pointer_to_str(const void *value, risse_char *string)
{
#ifdef Risse_snprintf
	Risse_snprintf(string, 25, RISSE_WS("%p"), value);
	return string;
#else
	char tmp[25];
#ifdef _MSC_VER
	_snprintf(tmp, sizeof(tmp), "%p", value);
#else
	snprintf(tmp, sizeof(tmp), "%p", value);
#endif
	const char *p = tmp;
	risse_char *d = string;
	while(*p) *(d++) = static_cast<risse_char>(*(p++));
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
risse_int Risse_straicmp(const risse_char *s1, const risse_char *s2)
{
	for(;;)
	{
		risse_char c1 = *s1, c2 = *s2;
		if(c1 >= RISSE_WC('a') && c1 <= RISSE_WC('z')) c1 += RISSE_WC('Z')-RISSE_WC('z');
		if(c2 >= RISSE_WC('a') && c2 <= RISSE_WC('z')) c2 += RISSE_WC('Z')-RISSE_WC('z');
		if(c1 == RISSE_WC('\0')) return (c2 == RISSE_WC('\0')) ? 0 : -1;
		if(c2 == RISSE_WC('\0')) return (c1 == RISSE_WC('\0')) ? 0 : 1;
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
void Risse_strcpy_maxlen(risse_char *d, const risse_char *s, risse_size len)
{
	risse_char ch;
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
void Risse_strcpy(risse_char *d, const risse_char *s)
{
	while((*(d++) = *(s++)) != 0)   ;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字列の長さを得る
//! @param		d    文字列
//! @return		文字列長
//---------------------------------------------------------------------------
risse_size Risse_strlen(const risse_char *d)
{
	const risse_char *p = d;
	if(!*d) return 0;
	while(*(++d) != 0)   ;
	return d-p;
}
//---------------------------------------------------------------------------


#ifdef RISSE_WCHAR_T_SIZE_IS_16BIT

//---------------------------------------------------------------------------
//! @brief		文字列を比較する
//! @param		s1   文字列その1
//! @param		s2   文字列その2
//! @return		s1 < s2 ? -1 : s1 > s2 ? 1 : 0
//---------------------------------------------------------------------------
int Risse_strcmp(const risse_char *s1, const risse_char *s2)
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
int Risse_strncmp(const risse_char *s1, const risse_char *s2, risse_size n)
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
risse_char *Risse_strncpy(risse_char *d, const risse_char *s, risse_size len)
{
	risse_char * pd = d;

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
risse_char *Risse_strcat(risse_char *d, const risse_char *s)
{
	risse_char * pd = d;
	while(*d) d++;
	Risse_strcpy(d, s);
	return pd;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字列中の部分文字列の検索
//! @param		s1 検索対象文字列(target)
//! @param		s2 検索する文字列(substring)
//! @return		s1 中の見つかった位置。見つからなかった場合は NULL
//---------------------------------------------------------------------------
risse_char *Risse_strstr(const risse_char *s1, const risse_char *s2)
{
	while(*s1)
	{
		if(*s1 == *s2)
		{
			const risse_char *p1 = s1 + 1;
			const risse_char *p2 = s2 + 1;
			while(*p2 && *p1 == *p2) p1++, p2++;
			if(*p2 == 0) return const_cast<risse_char*>(s1);
		}
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
risse_char *Risse_strchr(const risse_char *s, int c)
{
	while(*s)
	{
		if(*s == c) return const_cast<risse_char*>(s);
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
double Risse_strtod(const risse_char *nptr, risse_char **endptr)
{
	// ここでは非常に簡易的な実装を採る
	// いったん  buf に内容をコピーしてから それを strtod で変換して帰す。
	char buf[50];
	const risse_char *p = nptr;
	char *bp = buf;
	while(*p && static_cast<risse_size>(bp - buf) < sizeof(buf) - 1 )
		*bp = static_cast<char>(*p), bp++, p++;
	*bp = 0;
	char *ep;
	double val = strtod(buf, &ep);
	if(endptr) *endptr = const_cast<risse_char *>((ep - buf) + nptr);
	return val;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		UTF16文字列をrisse_char(UTF-32文字列) に変換
//! @param		out 出力文字列 (最低でもinと同じ要素数を持つこと) null終端は書き込まれる
//! @param		in  入力文字列
//! @return		出力バッファのサイズ (null終端を含まず, risse_char単位)
//---------------------------------------------------------------------------
risse_size RisseConvertUTF16ToRisseCharString(risse_char * out, const risse_uint16 * in)
{
	risse_char *org_out = out;
	while(*in)
	{
		if((*in<0xd800) || (*in>0xdfff))
		{
			*(out++) = *(in++);
		}
		else if((*in<0xd800) || (*in>0xdfff))
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
	return out - org_out;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		risse_char(UTF-32文字列) をUTF16文字列に変換
//! @param		out 出力文字列(NULL可)  null終端は書き込まれる
//! @param		in  入力文字列
//! @param		in_len 入力文字列のサイズ (-1L の場合は自動判定)
//! @return		出力バッファのサイズ (null終端を含まず, risse_uint16単位)
//---------------------------------------------------------------------------
risse_size RisseConvertRisseCharToUTF16String(risse_uint16 * out,
	const risse_char * in, risse_size in_len)
{
	// in_len が -1 の場合はすでにそれが risse_size で表すことのできる
	// 最大値であるとみなし、ここでは in_len の補正は行わない

	risse_size out_size = 0;
	while(in_len -- && *in)
	{
		if(*in<=0xffff)
		{
			if (out) *(out++) = (risse_uint16)*in;
			out_size ++;
		}
		else if(*in>=0x110000)
		{
			if (out) *(out++) = '?';
			out_size ++;
		}
		else
		{
			// サロゲートペア
			if(out)
			{
				out[0] = (risse_uint16) ((*in >> 10)+0xd7c0);
				out[1] = (risse_uint16) ((*in&0x3ff)+0xdc00);
				out += 2;
			}
			out_size += 2;
		}
		in ++;
	}
	if(out) *out = 0;
	return out_size;
}
//---------------------------------------------------------------------------

#endif  // #ifdef RISSE_WCHAR_T_SIZE_IS_16BIT




//---------------------------------------------------------------------------
static risse_int inline RisseRisseCharToUtf8(risse_char in, char * out)
{
	// convert a wide character 'in' to utf-8 character 'out'
	if     (in < (1<< 7))
	{
		if(out)
		{
			out[0] = (char)in;
		}
		return 1;
	}
	else if(in < (1<<11))
	{
		if(out)
		{
			out[0] = (char)(0xc0 | (in >> 6));
			out[1] = (char)(0x80 | (in & 0x3f));
		}
		return 2;
	}
	else if(in < (1<<16))
	{
		if(out)
		{
			out[0] = (char)(0xe0 | (in >> 12));
			out[1] = (char)(0x80 | ((in >> 6) & 0x3f));
			out[2] = (char)(0x80 | (in & 0x3f));
		}
		return 3;
	}
	else if(in < (1<<21))
	{
		if(out)
		{
			out[0] = (char)(0xf0 | (in >> 18));
			out[1] = (char)(0x80 | ((in >> 12) & 0x3f));
			out[2] = (char)(0x80 | ((in >> 6 ) & 0x3f));
			out[3] = (char)(0x80 | (in & 0x3f));
		}
		return 4;
	}
	else if(in < (1<<26))
	{
		if(out)
		{
			out[0] = (char)(0xf8 | (in >> 24));
			out[1] = (char)(0x80 | ((in >> 16) & 0x3f));
			out[2] = (char)(0x80 | ((in >> 12) & 0x3f));
			out[3] = (char)(0x80 | ((in >> 6 ) & 0x3f));
			out[4] = (char)(0x80 | (in & 0x3f));
		}
		return 5;
	}
	else if(in < (1<<31))
	{
		if(out)
		{
			out[0] = (char)(0xfc | (in >> 30));
			out[1] = (char)(0x80 | ((in >> 24) & 0x3f));
			out[2] = (char)(0x80 | ((in >> 18) & 0x3f));
			out[3] = (char)(0x80 | ((in >> 12) & 0x3f));
			out[4] = (char)(0x80 | ((in >> 6 ) & 0x3f));
			out[5] = (char)(0x80 | (in & 0x3f));
		}
		return 6;
	}

	return -1;
}
//---------------------------------------------------------------------------
static bool inline RisseUtf8ToRisseChar(const char * & in, risse_char *out)
{
	// convert a utf-8 charater from 'in' to wide charater 'out'
	const unsigned char * & p = (const unsigned char * &)in;
	if(p[0] < 0x80)
	{
		if(out) *out = (risse_char)in[0];
		in++;
		return true;
	}
	else if(p[0] < 0xc2)
	{
		// invalid character
		return false;
	}
	else if(p[0] < 0xe0)
	{
		// two bytes (11bits)
		if((p[1] & 0xc0) != 0x80) return false;
		if(out) *out = ((p[0] & 0x1f) << 6) + (p[1] & 0x3f);
		in += 2;
		return true;
	}
	else if(p[0] < 0xf0)
	{
		// three bytes (16bits)
		if((p[1] & 0xc0) != 0x80) return false;
		if((p[2] & 0xc0) != 0x80) return false;
		if(out) *out = ((p[0] & 0x1f) << 12) + ((p[1] & 0x3f) << 6) + (p[2] & 0x3f);
		in += 3;
		return true;
	}
	else if(p[0] < 0xf8)
	{
		// four bytes (21bits)
		if((p[1] & 0xc0) != 0x80) return false;
		if((p[2] & 0xc0) != 0x80) return false;
		if((p[3] & 0xc0) != 0x80) return false;
		if(out) *out = ((p[0] & 0x07) << 18) + ((p[1] & 0x3f) << 12) +
			((p[2] & 0x3f) << 6) + (p[3] & 0x3f);
		in += 4;
		return true;
	}
	else if(p[0] < 0xfc)
	{
		// five bytes (26bits)
		if((p[1] & 0xc0) != 0x80) return false;
		if((p[2] & 0xc0) != 0x80) return false;
		if((p[3] & 0xc0) != 0x80) return false;
		if((p[4] & 0xc0) != 0x80) return false;
		if(out) *out = ((p[0] & 0x03) << 24) + ((p[1] & 0x3f) << 18) +
			((p[2] & 0x3f) << 12) + ((p[3] & 0x3f) << 6) + (p[4] & 0x3f);
		in += 5;
		return true;
	}
	else if(p[0] < 0xfe)
	{
		// six bytes (31bits)
		if((p[1] & 0xc0) != 0x80) return false;
		if((p[2] & 0xc0) != 0x80) return false;
		if((p[3] & 0xc0) != 0x80) return false;
		if((p[4] & 0xc0) != 0x80) return false;
		if((p[5] & 0xc0) != 0x80) return false;
		if(out) *out = ((p[0] & 0x01) << 30) + ((p[1] & 0x3f) << 24) +
			((p[2] & 0x3f) << 18) + ((p[3] & 0x3f) << 12) +
			((p[4] & 0x3f) << 6) + (p[5] & 0x3f);
		in += 6;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		UTF-8 文字列を risse_char 文字列に変換する
//! @param		in   入力 UTF-8 文字列
//! @param		out  出力 risse_char 文字列 (NULL可)
//! @return		変換後の risse_char のコードポイント数 (null-terminatorを含まず)
//---------------------------------------------------------------------------
risse_size RisseUtf8ToRisseCharString(const char * in, risse_char *out)
{
	// convert input utf-8 string to output wide string
	risse_size count = 0;
	while(*in)
	{
		risse_char c;
		if(out)
		{
			if(!RisseUtf8ToRisseChar(in, &c))
				return static_cast<risse_size>(-1L); // invalid character found
			*out++ = c;
		}
		else
		{
			if(!RisseUtf8ToRisseChar(in, NULL))
				return static_cast<risse_size>(-1L); // invalid character found
		}
		count ++;
	}
	return count;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		risse_char 文字列を UTF-8 文字列に変換する
//! @param		in   入力 risse_char 文字列
//! @param		out  出力 UTF-8 文字列 (NULL可)
//! @return		変換後の UTF-8 のバイト数 (null-terminatorを含まず)
//---------------------------------------------------------------------------
risse_size RisseRisseCharToUtf8String(const risse_char * in, char * out)
{
	// convert input wide string to output utf-8 string
	risse_size count = 0;
	while(*in)
	{
		risse_int n;
		if(out)
		{
			n = RisseRisseCharToUtf8(*in, out);
			out += n;
		}
		else
		{
			n = RisseRisseCharToUtf8(*in, NULL);
				/*
					in this situation, the compiler's inliner
					will collapse all null check parts in
					RisseRisseCharToUtf8.
				*/
		}
		if(n == -1) return static_cast<risse_size>(-1L); // invalid character found
		count += n;
		in++;
	}
	return count;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		risse_char 型の文字列を wxString に変換する
//! @param		str  risse_char*型の文字列
//! @param		len  文字列の長さ(コードポイント単位) -1 = 自動判別
//! @return		wxString型の文字列
//---------------------------------------------------------------------------
wxString RisseCharToWxString(const risse_char * str, risse_size len)
{
	if(!str) return wxString();
	if(str[0] == 0) return wxString();
#ifdef RISSE_WCHAR_T_SIZE_IS_16BIT
	// UTF-32 から UTF-16 への変換が必要

	// 変換後の文字列長を取得
	risse_size converted_size =
		RisseConvertRisseCharToUTF16String(NULL, str, len); // lenは-1になりうるので注意

	if(converted_size == static_cast<risse_size>(-1L))
		return wxString(); // failed to convert

	// 変換後の文字列を一時的に格納するバッファを確保
	wchar_t *buf = new (PointerFreeGC) wchar_t[converted_size + 1];
	if(RisseConvertRisseCharToUTF16String(
			reinterpret_cast<risse_uint16*>(buf), str, len)
					== static_cast<risse_size>(-1))
		return wxString();

	return wxString(buf);
#else
	return wxString(str, (len == static_cast<risse_size>(-1L)) ? wxSTRING_MAXLEN:len);
#endif
}
//---------------------------------------------------------------------------






#ifdef RISSE_CHARUTILS_DEBUG
#include <stdio.h>
//---------------------------------------------------------------------------
//! @brief		一行中に複数の RISSE_WS を使用した場合に例外を送出する関数
//---------------------------------------------------------------------------
void RisseThrowWSAssertionFailure(const wchar_t * source, risse_int line)
{
	fprintf(stderr, "assertion failure at %ls line %d\n", source, line);
	abort();
#if 0
	Risse_eRisseError(
		ttstr(RISSE_WS("you can not use multiple RISSE_WS in a line; use RISSE_WS, RISSE_WS2, RISSE_WS3 ... at %1 line %2"),
			ttstr(source), ttstr(line)));
#endif
}
//---------------------------------------------------------------------------
#endif




} // namespace Risse
