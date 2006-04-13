//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse レキシカル・アナライザ(字句解析器)向けのユーティリティ関数群
//---------------------------------------------------------------------------
/*! @note
	ここでは、Risse の lexer に限らず、他の lexer でもおおよそ必要になるであろう、
	文字列の解析、数値の解析などの関数群を実装する。
*/


#include "prec.h"
#include "risseLexerUtils.h"
#include "risseException.h"


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(49805,54699,17434,18495,59306,19776,3233,9707);


//---------------------------------------------------------------------------
bool tRisseLexerUtility::SkipSpace(const risse_char * & ptr)
{
	while(*ptr && Risse_iswspace_nc(*ptr)) ptr ++;
	return *ptr != 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseLexerUtility::SkipToLineEnd(const risse_char * & ptr)
{
	while(*ptr != RISSE_WC('\r') && *ptr != RISSE_WC('\n') && *ptr) ptr++;
	if(*ptr == 0) return false;

	if(ptr[0] == RISSE_WC('\r') && ptr[1] == RISSE_WC('\n'))
		ptr += 2; // CR LF
	else
		ptr ++; // CR のみ あるいは LF のみ

	if(*ptr == 0) return false;

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_int tRisseLexerUtility::HexNum(risse_char ch)
{
	if(ch>=RISSE_WC('a') && ch<=RISSE_WC('f')) return ch-RISSE_WC('a')+10;
	if(ch>=RISSE_WC('A') && ch<=RISSE_WC('F')) return ch-RISSE_WC('A')+10;
	if(ch>=RISSE_WC('0') && ch<=RISSE_WC('9')) return ch-RISSE_WC('0');
	return -1;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_int tRisseLexerUtility::OctNum(risse_char ch)
{
	if(ch>=RISSE_WC('0') && ch<=RISSE_WC('7')) return ch-RISSE_WC('0');
	return -1;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_int tRisseLexerUtility::DecNum(risse_char ch)
{
	if(ch>=RISSE_WC('0') && ch<=RISSE_WC('9')) return ch-RISSE_WC('0');
	return -1;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_int tRisseLexerUtility::BinNum(risse_char ch)
{
	if(ch==RISSE_WC('0')) return 0;
	if(ch==RISSE_WC('1')) return 1;
	return -1;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
risse_int tRisseLexerUtility::UnescapeBackSlash(risse_char ch)
{
	// convert "\?"
	// ch must indicate "?"
	switch(ch)
	{
	case RISSE_WC('a'): return 0x07;
	case RISSE_WC('b'): return 0x08;
	case RISSE_WC('f'): return 0x0c;
	case RISSE_WC('n'): return 0x0a;
	case RISSE_WC('r'): return 0x0d;
	case RISSE_WC('t'): return 0x09;
	case RISSE_WC('v'): return 0x0b;
	default : return ch;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseLexerUtility::tSkipCommentResult
	tRisseLexerUtility::SkipComment(const risse_char * & ptr)
{
	if(ptr[0] != RISSE_WC('/')) return scrNotComment;

	if(ptr[1] == RISSE_WC('/'))
	{
		// line comment; skip to newline
		if(!SkipToLineEnd(ptr)) return scrEnded;
		SkipSpace(ptr);
		if(*ptr ==0) return scrEnded;

		return scrContinue;
	}
	else if(ptr[1] == RISSE_WC('*'))
	{
		// block comment; skip to the next '*' '/'
		// and we must allow nesting of the comment.
		ptr += 2;
		if(*ptr == 0) eRisseError::Throw(RISSE_WS_TR("Unclosed comment found"));
		risse_int level = 0;
		for(;;)
		{
			if(ptr[0] == RISSE_WC('/') && ptr[1] == RISSE_WC('*'))
			{
				// note: we cannot avoid comment processing when the
				// nested comment is in string literals.
				level ++;
			}
			if(ptr[0] == RISSE_WC('*') && ptr[1] == RISSE_WC('/'))
			{
				if(level == 0)
				{
					ptr += 2;
					break;
				}
				level --;
			}
			if(!*(++ptr)) eRisseError::Throw(RISSE_WS_TR("Unclosed comment found"));
		}
		if(*ptr ==0) return scrEnded;
		SkipSpace(ptr);
		if(*ptr ==0) return scrEnded;

		return scrContinue;
	}

	return scrNotComment;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseLexerUtility::StringMatch(const risse_char * & ptr, const risse_char *wrd, bool isword)
{
	// compare string with a script starting from sc and wrd.
	// word matching is processed if isword is true.
	const risse_char *save = ptr;
	while(*wrd && *ptr)
	{
		if(*ptr != *wrd) break;
		++ptr;
		wrd++;
	}

	if(*wrd) { ptr=save; return false; }
	if(isword)
	{
		if(Risse_iswalpha_nc(*ptr) || *ptr == RISSE_WC('_'))
			{ ptr=save; return false; }
	}
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseLexerUtility::tParseStringResult
	tRisseLexerUtility::ParseString(
		const risse_char * & ptr, 
		tRisseString &val,
		risse_char delim, bool embexpmode)
{
	// delim1 must be '\'' or '"'
	// delim2 must be '&' or '\0'

	tRisseString str;

	tParseStringResult status = psrNone;

	while(*ptr)
	{
		if(*ptr == RISSE_WC('\\'))
		{
			// \ 記号によるエスケープ
			if(!*(++ptr)) break;
			if(*ptr == RISSE_WC('x') || *ptr == RISSE_WC('X'))
			{
				// hex
				// starts with a "\x", be parsed while characters are
				// recognized as hex-characters, but limited of size of risse_char.
				// 現状の Risse では risse_char は必ず 32bit なので
				// \x のあとは最大8桁を読み込む。
				if(!*(++ptr)) break;
				risse_int num;
				risse_char code = 0;
				risse_int count = 0;
				while((num = HexNum(*ptr)) != -1 &&
					count < (int)(sizeof(risse_char)*2))
				{
					code <<= 4;
					code += num;
					count ++;
					if(!*(++ptr)) break;
				}
				if(*ptr == 0) break;

				// risse_char は UTF-32 を想定しているが、最上位
				// ビットは使用できない (エラー状態を表す用途に使われる
				// 可能性があるため )
				if(code == 0 || (code & (1 << (sizeof(risse_char)*8 - 1))))
					eRisseError::Throw(RISSE_WS_TR("UTF-32 code out of range"));

				str += (risse_char)code;
			}
			else if(*ptr == RISSE_WC('0'))
			{
				// octal
				if(!*(++ptr)) break;

				risse_int num;
				risse_int code=0;
				while((num = OctNum(*ptr))!=-1)
				{
					code*=8;
					code+=num;
					if(!*(++ptr)) break;
				}
				if(*ptr == 0) break;

				// risse_char は UTF-32 を想定しているが、最上位
				// ビットは使用できない (エラー状態を表す用途に使われる
				// 可能性があるため )
				if(code == 0 || (code & (1 << (sizeof(risse_char)*8 - 1))))
					eRisseError::Throw(RISSE_WS_TR("UTF-32 code out of range"));

				str += (risse_char)code;
			}
			else
			{
				str += (risse_char)UnescapeBackSlash(*ptr);
				if(!*(++ptr)) break;
			}
		}
		else if(*ptr == delim)
		{
			// string delimiters
			if(!*(++ptr))
			{
				status = psrDelimiter;
				break;
			}

			const risse_char *p = ptr;
			SkipSpace(p);
			if(*p == delim)
			{
				// sequence of 'A' 'B' will be combined as 'AB'
				ptr = p;
				ptr++;
			}
			else
			{
				status = psrDelimiter;
				break;
			}
		}
		else if(embexpmode && *ptr == RISSE_WC('&'))
		{
			// '&'
			if(!*(++ptr)) break;
			status = psrAmpersand;
			break;
		}
		else if(embexpmode && *ptr == RISSE_WC('$'))
		{
			// '$'
			// '{' must be placed immediately after '$'
			const risse_char *p = ptr;
			if(!*(++ptr)) break;
			if(*ptr == RISSE_WC('{'))
			{
				if(!*(++ptr)) break;
				status = psrDollar;
				break;
			}
			else
			{
				ptr = p;
				str += *ptr;
				++ptr;
			}
		}
		else
		{
			if(*ptr == RISSE_WC('\r') || *ptr == RISSE_WC('\n'))
			{
				// 入力スクリプトの改行コードは様々な物が考えられるが、
				// ここでは \n に統一する
				if(ptr[0] == RISSE_WC('\r') && ptr[1] == RISSE_WC('\n'))
					ptr += 2; // CR LF 改行
				else
					++ptr;

				str += RISSE_WC('\n');
			}
			else
			{
				str += *ptr;
				++ptr;
			}
		}
	}

	if(status == psrNone)
	{
		// error
		eRisseError::Throw(RISSE_WS_TR("Unclosed string literal"));
	}

	str.Fit();
	val = str;

	return status;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseLexerUtility::ParseString(const risse_char * & ptr, tRisseString &val)
{
	// Ptr は '\'' or '"' を指していないとならない

	risse_char delimiter = *ptr;

	ptr ++;

	return ParseString(ptr, val, delimiter, false) == psrDelimiter;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tRisseString tRisseLexerUtility::ExtractNumber(
	const risse_char * & ptr,
	risse_int (*validdigits)(risse_char ch),
	const risse_char *expmark, bool &isreal)
{
	tRisseString tmp;

	bool point_found = false;
	bool exp_found = false;
	while(true)
	{
		if(validdigits(*ptr) != -1)
		{
			tmp += *ptr;
			if(!*(++ptr)) break;
		}
		else if(*ptr == RISSE_WC('.') && !point_found && !exp_found)
		{
			point_found = true;
			tmp += *ptr;
			if(!*(++ptr)) break;
		}
		else if((*ptr == expmark[0] || *ptr == expmark[1]) && !exp_found)
		{
			exp_found = true;
			tmp += *ptr;
			if(!*(++ptr)) break;
			if(!SkipSpace(ptr)) break;
			if(*ptr == RISSE_WC('+'))
			{
				tmp += *ptr;
				if(!*(++ptr)) break;
				if(!SkipSpace(ptr)) break;
			}
			else if(*ptr == RISSE_WC('-'))
			{
				tmp += *ptr;
				if(!*(++ptr)) break;
				if(!SkipSpace(ptr)) break;
			}
		}
		else
		{
			break;
		}
	}

	isreal = point_found || exp_found;

	return tmp;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseLexerUtility::ParseNonDecimalReal(
	const risse_char * ptr, risse_real &val,
	risse_int (*validdigits)(risse_char ch), risse_int basebits)
{
	// parse non-decimal(hexiadecimal, octal or binary) floating-point number.
	// this routine heavily depends on IEEE double floating-point number expression.

	risse_uint64 main = RISSE_UI64_VAL(0); // significand(有効数字)
	risse_int exp = 0; // 2^n exponental (指数)
	risse_int numsignif = 0; // significand bit count (including leading left-most '1') in "main"
	bool pointpassed = false;


	// scan input
	while(true)
	{
		if(*ptr == RISSE_WC('.'))
		{
			pointpassed = true;
		}
		else if(*ptr == RISSE_WC('p') || *ptr == RISSE_WC('P'))
		{
			if(!*(++ptr)) break;
			if(!SkipSpace(ptr)) break;

			bool biassign = false;
			if(*ptr == RISSE_WC('+'))
			{
				biassign = false;
				if(!*(++ptr)) break;
				if(!SkipSpace(ptr)) break;
			}

			if(*ptr == RISSE_WC('-'))
			{
				biassign = true;
				if(!*(++ptr)) break;
				if(!SkipSpace(ptr)) break;
			}

			risse_int bias = 0;
			while(true)
			{
				bias *= 10;
				bias += DecNum(*ptr);
				if(!*(++ptr)) break;
			}
			if(biassign) bias = -bias;
			exp += bias;
			break;
		}
		else
		{
			risse_int n = validdigits(*ptr);
			if(numsignif == 0)
			{
				// find msb flaged bit
				risse_int b = basebits - 1;
				while(b >= 0)
				{
					if((1<<b) & n) break;
					b--;
				}

				b++;
				if(b)
				{
					// n is not zero
					// place it to the main's msb
					numsignif = b;
					main |= ((risse_uint64)n << (64 - numsignif));
					if(pointpassed)
						exp -= (basebits - b + 1);
					else
						exp = b - 1;
				}
				else
				{
					// n is zero
					if(pointpassed) exp -= basebits;
				}
			}
			else
			{
				// append to main
				if(numsignif + basebits < 64)
				{
					numsignif += basebits;
					main |= ((risse_uint64)n << (64 - numsignif));
				}
				if(!pointpassed) exp += basebits;
			}
		}
		if(!*(++ptr)) break;
	}

	main >>= (64 - 1 - RISSE_IEEE_D_SIGNIFICAND_BITS);

	if(main == 0)
	{
		// zero
		val = (risse_real)0.0;
		return true;
	}

	main &= ((RISSE_UI64_VAL(1) << RISSE_IEEE_D_SIGNIFICAND_BITS) - RISSE_UI64_VAL(1));

	if(exp < RISSE_IEEE_D_EXP_MIN)
	{
		// denormal
		// treat as zero
		val = (risse_real)0.0;
		return true;
	}

	if(exp > RISSE_IEEE_D_EXP_MAX)
	{
		// too large
		// treat as infinity
		risse_real d;
		*(risse_uint64*)&d = RISSE_IEEE_D_P_INF;
		val = d;
		return true;
	}

	// compose IEEE double
	risse_real d;
	*reinterpret_cast<risse_uint64*>(&d) =
		RISSE_IEEE_D_MAKE_SIGN(0) |
		RISSE_IEEE_D_MAKE_EXP(exp) |
		RISSE_IEEE_D_MAKE_SIGNIFICAND(main);
	val = d;
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseLexerUtility::ParseNonDecimalInteger(const risse_char *ptr, risse_int64 &val, 
	risse_int (*validdigits)(risse_char ch), risse_int basebits)
{
	risse_int64 v = 0;
	while(true)
	{
		v <<= basebits;
		v += validdigits(*ptr);
		if(!*(++ptr)) break;
	}
	val = (risse_int64)v;
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseLexerUtility::ParseNonDecimalNumber(const risse_char * & ptr, tRisseVariant &val,
	risse_int (*validdigits)(risse_char ch), risse_int base)
{
	bool isreal = false;
	tRisseString tmp(ExtractNumber(ptr, validdigits, RISSE_WS("Pp"), isreal));

	if(tmp.IsEmpty()) return false;

	const risse_char *p = tmp.c_str();

	if(isreal)
	{
		risse_real r;
		bool ret = ParseNonDecimalReal(p, r, validdigits, base);
		if(ret) val = r;
		return ret;
	}
	else
	{
		risse_int64 r;
		bool ret = ParseNonDecimalInteger(p, r, validdigits, base);
		if(ret) val = r;
		return ret;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseLexerUtility::ParseDecimalReal(const risse_char * ptr, risse_real &val)
{
	val = static_cast<risse_real>(Risse_strtod(ptr, NULL));
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseLexerUtility::ParseDecimalInteger(const risse_char * ptr, risse_int64 &val)
{
	int n;
	risse_int64 num = 0;
	while((n = DecNum(*ptr)) != -1)
	{
		num *= 10;
		num += n;
		if(!*(++ptr)) break;
	}
	val = num;
	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseLexerUtility::ParseNumber2(const risse_char * & ptr, tRisseVariant &val)
{
	// stage 2

	if(StringMatch(ptr, RISSE_WS("true"), true))
	{
		val = (risse_int64)true;
		return true;
	}
	if(StringMatch(ptr, RISSE_WS("false"), true))
	{
		val = (risse_int64)false;
		return true;
	}
	if(StringMatch(ptr, RISSE_WS("NaN"), true))
	{
		// Not a Number
		risse_real d;
		*reinterpret_cast<risse_uint64*>(&d) = RISSE_IEEE_D_P_NaN;
		val = d;
		return true;
	}
	if(StringMatch(ptr, RISSE_WS("Infinity"), true))
	{
		// positive inifinity
		risse_real d;
		*reinterpret_cast<risse_uint64*>(&d) = RISSE_IEEE_D_P_INF;
		val = d;
		return true;
	}

	const risse_char *ptr_save = ptr;

	if(*ptr == RISSE_WC('0'))
	{
		if(!*(++ptr))
		{
			val = static_cast<risse_int64>(0);
			return true;
		}

		risse_char mark = *ptr;

		if(mark == RISSE_WC('X') || mark == RISSE_WC('x'))
		{
			// hexadecimal
			if(!*(++ptr)) return false;
			return ParseNonDecimalNumber(ptr, val, HexNum, 4);
		}

		if(mark == RISSE_WC('B') || mark == RISSE_WC('b'))
		{
			// binary
			if(!*(++ptr)) return false;
			return ParseNonDecimalNumber(ptr, val, BinNum, 1);
		}

		if(mark == RISSE_WC('.'))
		{
			// decimal point
			ptr = ptr_save;
			goto decimal;
		}

		if(mark == RISSE_WC('E') || mark == RISSE_WC('e'))
		{
			// exp
			ptr = ptr_save;
			goto decimal;
		}

		if(mark == RISSE_WC('P') || mark == RISSE_WC('p'))
		{
			// 2^n exp
			return false;
		}

		// octal
		ptr = ptr_save;
		return ParseNonDecimalNumber(ptr, val, OctNum, 3);
	}

	// integer decimal or real decimal
decimal:
	bool isreal = false;
	tRisseString tmp(ExtractNumber(ptr, DecNum, RISSE_WS("Ee"), isreal));

	if(tmp.IsEmpty()) return false;

	const risse_char *p = tmp.c_str();

	if(isreal)
	{
		risse_real r;
		bool ret = ParseDecimalReal(p, r);
		if(ret) val = r;
		return ret;
	}
	else
	{
		risse_int64 r;
		bool ret = ParseDecimalInteger(p, r);
		if(ret) val = r;
		return ret;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseLexerUtility::ParseNumber(const risse_char * & ptr, tRisseVariant &val)
{
	// parse a number pointed by ptr

	bool sign = false; // true if negative

	if(*ptr == RISSE_WC('+'))
	{
		sign = false;
		if(!*(++ptr)) return false;
		if(!SkipSpace(ptr)) return false;
	}
	else if(*ptr == RISSE_WC('-'))
	{
		sign = true;
		if(!*(++ptr)) return false;
		if(!SkipSpace(ptr)) return false;
	}

	if(ParseNumber2(ptr, val))
	{
		if(sign) val = -val;
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseLexerUtility::ParseOctet(const risse_char * & ptr, tRisseOctet &val)
{
	// parse a octet literal;
	// syntax is:
	// <% xx xx xx xx xx xx ... %>
	// where xx is hexadecimal 8bit(octet) binary representation.
	ptr += 2; // skip <%

	bool leading = true;
	risse_uint8 cur = 0;

	tRisseOctet ret;

	for(;*ptr;)
	{
		// コメントをスキップ
		switch(SkipComment(ptr))
		{
		case scrEnded:
			eRisseError::Throw(RISSE_WS_TR("Unclosed octet literal"));
		case scrContinue:
		case scrNotComment:
			;
		}

		// 次の一文字へのポインタを得る
		const risse_char *next = ptr;
		next ++;

		// リテラルの終了？
		if(ptr[0] == RISSE_WC('%') && ptr[1] == RISSE_WC('>'))
		{
			ptr = next;
			ptr ++;

			// オクテット列リテラルが終了した
			if(!leading) ret += cur;
			ret.Fit();
			return true;
		}

		// １６進数を数値に変換
		risse_char ch = *ptr;
		risse_int n = HexNum(ch);
		if(n != -1)
		{
			// 変換に成功
			if(leading)
			{
				cur = (risse_uint8)(n);
				leading = false;
			}
			else
			{
				cur <<= 4;
				cur += n;

				ret += cur;

				leading = true;
			}
		}
		else
		{
			// 変換に失敗した文字は単に無視する
			;
		}

		// カンマまたはスペースの場合は次に進める
		if(!leading && (ch == RISSE_WC(',') || ch == RISSE_WC(' ')))
		{
			ret += cur;

			leading = true;
		}

		ptr = next;
	}

	// error
	eRisseError::Throw(RISSE_WS_TR("Unclosed octet literal"));

	return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
bool tRisseLexerUtility::ParseRegExp(const risse_char * & ptr, tRisseString &pat, tRisseString &flags)
{
	// parse a regular expression pointed by 'ptr'.
	// this is essencially the same as string parsing, except for
	// not to decode escaped characters by '\\'.
	// the regexp must be terminated by the delimiter '/', not succeeded by '\\'.

	bool ok = false;
	bool lastbackslash = false;
	tRisseString str;
	tRisseString flg;

	for(;*ptr;)
	{
		if(*ptr == RISSE_WC('\\'))
		{
			str += *ptr;
			if(lastbackslash)
				lastbackslash = false;
			else
				lastbackslash = true;
		}
		else if(*ptr == RISSE_WC('/') && !lastbackslash)
		{
			// regexp delimiters
			if(!*(++ptr))
			{
				ok = true;
				break;
			}

			// flags can be here
			while(*ptr >= RISSE_WC('a') && *ptr <= RISSE_WC('z'))
			{
				flg += *ptr;
				if(!*(++ptr)) break;
			}
			ok = true;
			break;
		}
		else
		{
			lastbackslash = false;
			str += *ptr;
		}
		++ptr;
	}

	if(!ok)
	{
		// error
		eRisseError::Throw("Unclosed regular expression literal");
	}

	pat = str;
	flags = flg;

	return true;
}

//---------------------------------------------------------------------------

#if 0


//---------------------------------------------------------------------------
// hash table for reserved words
//---------------------------------------------------------------------------
static tRisseCustomObject * RisseReservedWordHash = NULL;
static bool RisseReservedWordHashInit = false;
static risse_int RisseReservedWordHashRefCount;
//---------------------------------------------------------------------------
void RisseReservedWordsHashAddRef()
{
	if(RisseReservedWordHashRefCount == 0)
	{
		RisseReservedWordHashInit = false;
		RisseReservedWordHash = new tRisseCustomObject();
	}
	RisseReservedWordHashRefCount ++;
}
//---------------------------------------------------------------------------
void RisseReservedWordsHashRelease()
{
	RisseReservedWordHashRefCount --;

	if(RisseReservedWordHashRefCount == 0)
	{
		RisseReservedWordHash->Release();
		RisseReservedWordHash = NULL;
	}
}
//---------------------------------------------------------------------------
static void RisseRegisterReservedWordsHash(const risse_char *word, risse_int num)
{
	tRisseVariant val((risse_int64)num);
	RisseReservedWordHash->PropSet(RISSE_MEMBERENSURE, word, NULL, &val,
		RisseReservedWordHash);
}
//---------------------------------------------------------------------------
#define RISSE_REG_RES_WORD(word, value) RisseRegisterReservedWordsHash(RISSE_WS(word), value);
static void RisseInitReservedWordsHashTable()
{
	if(RisseReservedWordHashInit) return;
	RisseReservedWordHashInit = true;

	RISSE_REG_RES_WORD("break", T_BREAK);
	RISSE_REG_RES_WORD("continue", T_CONTINUE);
	RISSE_REG_RES_WORD("const", T_CONST);
	RISSE_REG_RES_WORD("catch", T_CATCH);
	RISSE_REG_RES_WORD("class", T_CLASS);
	RISSE_REG_RES_WORD("case", T_CASE);
	RISSE_REG_RES_WORD("debugger", T_DEBUGGER);
	RISSE_REG_RES_WORD("default", T_DEFAULT);
	RISSE_REG_RES_WORD("delete", T_DELETE);
	RISSE_REG_RES_WORD("do", T_DO);
	RISSE_REG_RES_WORD("extends", T_EXTENDS);
	RISSE_REG_RES_WORD("export", T_EXPORT);
	RISSE_REG_RES_WORD("enum", T_ENUM);
	RISSE_REG_RES_WORD("else", T_ELSE);
	RISSE_REG_RES_WORD("function", T_FUNCTION);
	RISSE_REG_RES_WORD("finally", T_FINALLY);
	RISSE_REG_RES_WORD("false", T_FALSE);
	RISSE_REG_RES_WORD("for", T_FOR);
	RISSE_REG_RES_WORD("global", T_GLOBAL);
	RISSE_REG_RES_WORD("getter", T_GETTER);
	RISSE_REG_RES_WORD("goto", T_GOTO);
	RISSE_REG_RES_WORD("incontextof", T_INCONTEXTOF);
	RISSE_REG_RES_WORD("invalidate", T_INVALIDATE);
	RISSE_REG_RES_WORD("instanceof", T_INSTANCEOF);
	RISSE_REG_RES_WORD("isvalid", T_ISVALID);
	RISSE_REG_RES_WORD("import", T_IMPORT);
	RISSE_REG_RES_WORD("int", T_INT);
	RISSE_REG_RES_WORD("in", T_IN);
	RISSE_REG_RES_WORD("if", T_IF);
	RISSE_REG_RES_WORD("null", T_NULL);
	RISSE_REG_RES_WORD("new", T_NEW);
	RISSE_REG_RES_WORD("octet", T_OCTET);
	RISSE_REG_RES_WORD("protected", T_PROTECTED);
	RISSE_REG_RES_WORD("property", T_PROPERTY);
	RISSE_REG_RES_WORD("private", T_PRIVATE);
	RISSE_REG_RES_WORD("public", T_PUBLIC);
	RISSE_REG_RES_WORD("return", T_RETURN);
	RISSE_REG_RES_WORD("real", T_REAL);
	RISSE_REG_RES_WORD("synchronized", T_SYNCHRONIZED);
	RISSE_REG_RES_WORD("switch", T_SWITCH);
	RISSE_REG_RES_WORD("static", T_STATIC);
	RISSE_REG_RES_WORD("setter", T_SETTER);
	RISSE_REG_RES_WORD("string", T_STRING);
	RISSE_REG_RES_WORD("super", T_SUPER);
	RISSE_REG_RES_WORD("typeof", T_TYPEOF);
	RISSE_REG_RES_WORD("throw", T_THROW);
	RISSE_REG_RES_WORD("this", T_THIS);
	RISSE_REG_RES_WORD("true", T_TRUE);
	RISSE_REG_RES_WORD("try", T_TRY);
	RISSE_REG_RES_WORD("void", T_VOID);
	RISSE_REG_RES_WORD("var", T_VAR);
	RISSE_REG_RES_WORD("while", T_WHILE);
	RISSE_REG_RES_WORD("NaN", T_NAN);
	RISSE_REG_RES_WORD("Infinity", T_INFINITY);
	RISSE_REG_RES_WORD("with", T_WITH);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tRisseLexicalAnalyzer
//---------------------------------------------------------------------------
tRisseLexicalAnalyzer::tRisseLexicalAnalyzer(tRisseScriptBlock *block,
	const risse_char *script, bool exprmode, bool resneeded)
{
	// resneeded is valid only if exprmode is true

	RisseInitReservedWordsHashTable();

	Block = block;
	ExprMode = exprmode;
	ResultNeeded = resneeded;
	PrevToken = -1;
	risse_int len = Risse_strlen(script);
	Script = new risse_char[len+2];
	Risse_strcpy(Script, script);
	if(ExprMode)
	{
		// append ';' on expression analyze mode
		Script[len] = RISSE_WC(';');
		Script[len+1] = 0;
	}
	else
	{
		if(Script[0] == RISSE_WC('#') && Script[1] == RISSE_WC('!'))
		{
			// shell script like file
			Script[0] = RISSE_WC('/');
			Script[1] = RISSE_WC('/');  // convert #! to //
		}
	}

	if(risseEnableDicFuncQuickHack) //----- dicfunc quick-hack
	{
		DicFunc = false;
		if(ExprMode && (Script[0] == RISSE_WC('[') || (Script[0] == RISSE_WC('%') && Script[1] == RISSE_WC('['))))
		{
			DicFunc = true;
		}
	}

	Current = Script;
	IfLevel = 0;
	PrevPos = 0;
	NestLevel = 0;
	First = true;
	RegularExpression = false;
	BareWord = false;
	PutValue(tRisseVariant());
}
//---------------------------------------------------------------------------
tRisseLexicalAnalyzer::~tRisseLexicalAnalyzer()
{
	Free();
}
//---------------------------------------------------------------------------
tRisseSkipCommentResult tRisseLexicalAnalyzer::SkipUntil_endif()
{
	// skip until @endif
	risse_int exl = IfLevel;
	IfLevel ++;
	while(true)
	{
		if(*Current == RISSE_WC('/'))
		{
			switch(RisseSkipComment(&Current))
			{
			case scrEnded:
				Risse_eRisseCompileError(RissePPError, Block, Current-Script);
				break;
			case scrContinue:
				break;
			case scrNotComment:
				if(!RisseNext(&Current))
					Risse_eRisseCompileError(RissePPError, Block, Current-Script);
				break;
			}
		}
		else if(*Current == RISSE_WC('@'))
		{
			Current ++;
			bool skipp = false;

			if(!Risse_strncmp(Current, RISSE_WS("if"), 2))
			{
				IfLevel ++;
				Current += 2;
				skipp = true;
			}
			else if(!Risse_strncmp(Current, RISSE_WS("set"), 3))
			{
				Current += 3;
				skipp = true;
			}
			else if(!Risse_strncmp(Current, RISSE_WS("endif"), 5))
			{
				// endif
				Current += 5;
				IfLevel--;
				if(IfLevel == exl)
				{
					// skip ended
					SkipSpace(&Current);
					if(!*Current) return scrEnded;
					return scrContinue;
				}
			}
			else
			{
				// skipp = false
			}

			if(skipp)
			{
				// skip parenthesises
				if(!SkipSpace(&Current))
					Risse_eRisseCompileError(RissePPError, Block, Current-Script);

				if(*Current!=RISSE_WC('('))
					Risse_eRisseCompileError(RissePPError, Block, Current-Script);

				RisseNext(&Current);
				risse_int plevel = 0;
				while(*Current && (plevel || *Current!=RISSE_WC(')')))
				{
					if(*Current == RISSE_WC('(')) plevel++;
					else if(*Current == RISSE_WC(')')) plevel--;
					RisseNext(&Current);
				}
				if(!*Current)
					Risse_eRisseCompileError(RissePPError, Block, Current-Script);
				RisseNext(&Current);
				if(!*Current)
					Risse_eRisseCompileError(RissePPError, Block, Current-Script);
			}
		}
		else
		{
			if(!RisseNext(&Current))
				Risse_eRisseCompileError(RissePPError, Block, Current-Script);
		}
	}
}
//---------------------------------------------------------------------------
tRisseSkipCommentResult tRisseLexicalAnalyzer::ProcessPPStatement()
{
	// process pre-prosessor statements.
	// here "Current" points '@'.
	const risse_char *org = Current;

	Current ++;

	if(!Risse_strncmp(Current, RISSE_WS("set"), 3))
	{
		// set statement
		Block->NotifyUsingPreProcessor();
		Current+=3;
		if(!SkipSpace(&Current))
			Risse_eRisseCompileError(RissePPError, Block, Current-Script);

		if(*Current!=RISSE_WC('('))
			Risse_eRisseCompileError(RissePPError, Block, Current-Script);

		RisseNext(&Current);
		const risse_char *st = Current;
		risse_int plevel = 0;
		while(*Current && (plevel || *Current!=RISSE_WC(')')))
		{
			if(*Current == RISSE_WC('(')) plevel++;
			else if(*Current == RISSE_WC(')')) plevel--;
			RisseNext(&Current);
		}
		const risse_char *ed = Current;

		RisseNext(&Current);

		try
		{
			ParsePPExpression(st, ed-st); // evaluate exp
		}
		catch(...)
		{
			Risse_eRisseCompileError(RissePPError, Block, Current-Script);
		}

		SkipSpace(&Current);
		if(!*Current) return scrEnded;

		return scrContinue;
	}

	if(!Risse_strncmp(Current, RISSE_WS("if"), 2))
	{
		// if statement

		Block->NotifyUsingPreProcessor();
		Current += 2;
		if(!SkipSpace(&Current))
			Risse_eRisseCompileError(RissePPError, Block, Current-Script);

		if(*Current!=RISSE_WC('('))
			Risse_eRisseCompileError(RissePPError, Block, Current-Script);

		RisseNext(&Current);
		const risse_char *st = Current;
		risse_int plevel = 0;
		while(*Current && (plevel || *Current!=RISSE_WC(')')))
		{
			if(*Current == RISSE_WC('(')) plevel++;
			else if(*Current == RISSE_WC(')')) plevel--;
			RisseNext(&Current);
		}
		const risse_char *ed = Current;

		RisseNext(&Current);

		risse_int32 ret;

		try
		{
			ret = ParsePPExpression(st, ed-st); // evaluate exp
		}
		catch(...)
		{
			Risse_eRisseCompileError(RissePPError, Block, Current-Script);
		}

		if(!ret) return SkipUntil_endif();  // skip to endif

		IfLevel ++;

		SkipSpace(&Current);
		if(!*Current) return scrEnded;
		return scrContinue;
	}


	if(!Risse_strncmp(Current, RISSE_WS("endif"), 5))
	{
		Current += 5;
		IfLevel --;
		if(IfLevel < 0)
			Risse_eRisseCompileError(RissePPError, Block, Current-Script);

		SkipSpace(&Current);
		if(!*Current) return scrEnded;
		return scrContinue;
	}

	Current = org;
	return scrNotComment;
}
//---------------------------------------------------------------------------
#define RISSE_MATCH_W(word, code) \
	if(StringMatch(&Current, RISSE_WS(word), true)) return (code)
#define RISSE_MATCH_S(word, code) \
	if(StringMatch(&Current, RISSE_WS(word), false)) return (code)
#define RISSE_MATCH_W_V(word, code, val) \
	if(StringMatch(&Current, RISSE_WS(word), true)) { n=PutValue(val); return (code); }
#define RISSE_MATCH_S_V(word, code, val) \
	if(StringMatch(&Current, RISSE_WS(word), false)) { n=PutValue(val); return (code); }
#define RISSE_1CHAR(code) \
	RisseNext(&Current); return (code)
risse_int tRisseLexicalAnalyzer::GetToken(risse_int &n)
{
	// returns token, pointed by 'Current'

	if(*Current == 0) return 0;

	if(RegularExpression)
	{
		// the next token was marked as a regular expression by the parser
		RegularExpression = false;

		Current = Script + PrevPos; // draws position of the first '/' back

		RisseNext(&Current);

		tRisseVariant pat;
		RisseParseRegExp(pat, &Current);
		n = PutValue(pat);

		return T_REGEXP;
	}

re_match:

	switch(*Current)
	{
	case RISSE_WC('>'):
		RISSE_MATCH_S(">>>=", T_RBITSHIFTEQUAL);
		RISSE_MATCH_S(">>>", T_RBITSHIFT);
		RISSE_MATCH_S(">>=", T_RARITHSHIFTEQUAL);
		RISSE_MATCH_S(">>", T_RARITHSHIFT);
		RISSE_MATCH_S(">=", T_GTOREQUAL);
		RISSE_1CHAR(T_GT);

	case RISSE_WC('<'):
		RISSE_MATCH_S("<<=", T_LARITHSHIFTEQUAL);
		RISSE_MATCH_S("<->", T_SWAP);
		RISSE_MATCH_S("<=", T_LTOREQUAL);
		RISSE_MATCH_S("<<", T_LARITHSHIFT);
		{
			const risse_char *next = Current;
			RisseNext(&next);
			if(*next == RISSE_WC('%'))
			{
				// '<%'   octet literal
				tRisseVariant v;
				RisseParseOctet(v, &Current);
				n = PutValue(v);
				return T_CONSTVAL;
			}
		}
		RISSE_1CHAR(T_LT);

	case RISSE_WC('='):
		RISSE_MATCH_S("===", T_DISCEQUAL);
		RISSE_MATCH_S("==", T_EQUALEQUAL);
		RISSE_MATCH_S("=>", T_COMMA);
			// just a replacement for comma, like perl
		RISSE_1CHAR(T_EQUAL);

	case RISSE_WC('!'):
		RISSE_MATCH_S("!==", T_DISCNOTEQUAL);
		RISSE_MATCH_S("!=", T_NOTEQUAL);
		RISSE_1CHAR(T_EXCRAMATION);

	case RISSE_WC('&'):
		RISSE_MATCH_S("&&=", T_LOGICALANDEQUAL);
		RISSE_MATCH_S("&&", T_LOGICALAND);
		RISSE_MATCH_S("&=", T_AMPERSANDEQUAL);
		RISSE_1CHAR(T_AMPERSAND);

	case RISSE_WC('|'):
		RISSE_MATCH_S("||=", T_LOGICALOREQUAL);
		RISSE_MATCH_S("||", T_LOGICALOR);
		RISSE_MATCH_S("|=", T_VERTLINEEQUAL);
		RISSE_1CHAR(T_VERTLINE);

	case RISSE_WC('.'):
		if(Current[1] >= RISSE_WC('0') && Current[1] <= RISSE_WC('9'))
		{
			// number
			tRisseVariant v;
			RisseParseNumber(v, &Current);
			n=PutValue(v);
			return T_CONSTVAL;
		}
		RISSE_MATCH_S("...", T_OMIT);
		RISSE_1CHAR(T_DOT);

	case RISSE_WC('+'):
		RISSE_MATCH_S("++", T_INCREMENT);
		RISSE_MATCH_S("+=", T_PLUSEQUAL);
		RISSE_1CHAR(T_PLUS);

	case RISSE_WC('-'):
		RISSE_MATCH_S("-=", T_MINUSEQUAL);
		RISSE_MATCH_S("--", T_DECREMENT);
		RISSE_1CHAR(T_MINUS);

	case RISSE_WC('*'):
		RISSE_MATCH_S("*=", T_ASTERISKEQUAL);
		RISSE_1CHAR(T_ASTERISK);

	case RISSE_WC('/'):
		// check comments
		switch(RisseSkipComment(&Current))
		{
		case scrContinue:
			goto re_match;
		case scrEnded:
			return 0;
		case scrNotComment:
			;
		}

		RISSE_MATCH_S("/=", T_SLASHEQUAL);
		RISSE_1CHAR(T_SLASH);

	case RISSE_WC('\\'):
		RISSE_MATCH_S("\\=", T_BACKSLASHEQUAL);
		RISSE_1CHAR(T_BACKSLASH);

	case RISSE_WC('%'):
		RISSE_MATCH_S("%=", T_PERCENTEQUAL);
		RISSE_1CHAR(T_PERCENT);

	case RISSE_WC('^'):
		RISSE_MATCH_S("^=", T_CHEVRONEQUAL);
		RISSE_1CHAR(T_CHEVRON);

	case RISSE_WC('['):
		NestLevel++;
		RISSE_1CHAR(T_LBRACKET);

	case RISSE_WC(']'):
		NestLevel--;
		RISSE_1CHAR(T_RBRACKET);

	case RISSE_WC('('):
		NestLevel++;
		RISSE_1CHAR(T_LPARENTHESIS);

	case RISSE_WC(')'):
		NestLevel--;
		RISSE_1CHAR(T_RPARENTHESIS);

	case RISSE_WC('~'):
		RISSE_1CHAR(T_TILDE);

	case RISSE_WC('?'):
		RISSE_1CHAR(T_QUESTION);

	case RISSE_WC(':'):
		RISSE_1CHAR(T_COLON);

	case RISSE_WC(','):
		RISSE_1CHAR(T_COMMA);

	case RISSE_WC(';'):
		RISSE_1CHAR(T_SEMICOLON);

	case RISSE_WC('{'):
		NestLevel++;
		RISSE_1CHAR(T_LBRACE);

	case RISSE_WC('}'):
		NestLevel--;
		RISSE_1CHAR(T_RBRACE);

	case RISSE_WC('#'):
		RISSE_1CHAR(T_SHARP);

	case RISSE_WC('$'):
		RISSE_1CHAR(T_DOLLAR);

	case RISSE_WC('\''):
	case RISSE_WC('\"'):
		// literal string
	  {
		tRisseVariant v;
		RisseParseString(v, &Current);
		n=PutValue(v);
		return T_CONSTVAL;
	  }

	case RISSE_WC('@'):
		// embeddable expression in string (such as @"this can be embeddable like &variable;")
	  {
		const risse_char *org = Current;
		if(!RisseNext(&Current)) return 0;
		if(!SkipSpace(&Current)) return 0;
		if(*Current == RISSE_WC('\'') || *Current == RISSE_WC('\"'))
		{
			tEmbeddableExpressionData data;
			data.State = evsStart;
			data.WaitingNestLevel = NestLevel;
			data.Delimiter = *Current;
			data.NeedPlus = false;
			if(!RisseNext(&Current)) return 0;
			EmbeddableExpressionDataStack.push_back(data);
			return -1;
		}
		else
		{
			Current = org;
		}

		// possible pre-prosessor statements
		switch(ProcessPPStatement())
		{
		case scrContinue:
			goto re_match;
		case scrEnded:
			return 0;
		case scrNotComment:
			Current = org;
			break;
		}
		break;
	  }

	case RISSE_WC('0'):
	case RISSE_WC('1'):
	case RISSE_WC('2'):
	case RISSE_WC('3'):
	case RISSE_WC('4'):
	case RISSE_WC('5'):
	case RISSE_WC('6'):
	case RISSE_WC('7'):
	case RISSE_WC('8'):
	case RISSE_WC('9'):
		// number
	  {
		tRisseVariant v;
		bool r = RisseParseNumber(v, &Current);
		if(!r) Risse_eRisseCompileError(RisseNumberError, Block, Current-Script);
		n=PutValue(v);
		return T_CONSTVAL;
	  }
	}

	if(!Risse_iswalpha_nc(*Current) && *Current!=RISSE_WC('_'))
	{
		ttstr str(RisseInvalidChar);
		risse_char ch[2];
		ch[0] = *Current;
		ch[1] = 0;
		str.Replace(RISSE_WS("%1"), ttstr(ch).EscapeC());
		Risse_eRisseError(str);
	}


	const risse_char *ptr = Current;
	risse_int nch = 0;
	while(Risse_iswdigit_nc(*ptr) || Risse_iswalpha_nc(*ptr) || *ptr==RISSE_WC('_') ||
		*ptr>0x0100 || *ptr == RISSE_SKIP_CODE)
		ptr++, nch++;

	if(nch == 0)
	{
		ttstr str(RisseInvalidChar);
		risse_char ch[2];
		ch[0] = *Current;
		ch[1] = 0;
		str.Replace(RISSE_WS("%1"), ttstr(ch).EscapeC());
		Risse_eRisseError(str);
	}

	ttstr str(Current, nch);
	Current += nch;

	risse_char *s, *d;
	s = d = str.Independ();
	while(*s)
	{
		// eliminate RISSE_SKIP_CODE
		if(*s == RISSE_SKIP_CODE)
		{
			s++;
			continue;
		}
		*d = *s;
		d++, s++;
	}
	*d = 0;
	str.FixLen();

	risse_int retnum;

	if(BareWord)
		retnum = -1;
	else
		retnum = RisseReservedWordHash->GetValueInteger(str.c_str(), str.GetHint());

	BareWord = false;

	if(retnum == -1)
	{
		// not a reserved word
		n = PutValue(str);
		return T_SYMBOL;
	}

	switch(retnum)
	{
	case T_FALSE:
		n = PutValue(tRisseVariant(false));
		return T_CONSTVAL;
	case T_NULL:
		n = PutValue(tRisseVariant((iRisseDispatch2*)NULL));
		return T_CONSTVAL;
	case T_TRUE:
		n = PutValue(tRisseVariant(true));
		return T_CONSTVAL;
	case T_NAN:
	  {
		RisseSetFPUE();
		risse_real d;
		*(risse_uint64*)&d = RISSE_IEEE_D_P_NaN;
		n = PutValue(tRisseVariant(d));
		return T_CONSTVAL;
	  }
	case T_INFINITY:
	  {
		RisseSetFPUE();
		risse_real d;
		*(risse_uint64*)&d = RISSE_IEEE_D_P_INF;
		n = PutValue(tRisseVariant(d));
		return T_CONSTVAL;
	  }
	}

	return retnum;
}
//---------------------------------------------------------------------------
risse_int32 tRisseLexicalAnalyzer::ParsePPExpression(const risse_char *start, risse_int n)
{
	// parses a conditional compile experssion starting with "start",
	// character count "n".
	risse_char *buf = new risse_char[n+1];
	risse_char *p;
	const risse_char *lim = start + n;
	p=buf;
	while(start < lim)
	{
		*p = *start;
		RisseNext(&start);
		p++;
	}

	*p = 0;

	tRissePPExprParser *parser = new tRissePPExprParser(Block->GetRisse(), buf);

	risse_int32 result;
	try
	{
		result = parser->Parse();
	}
	catch(...)
	{
		delete parser;
		throw;
	}

	delete parser;

	return result;
}
//---------------------------------------------------------------------------
void tRisseLexicalAnalyzer::PreProcess(void)
{
	// pre-process

	// proceeds newline code unification, conditional compile control.


	/* unify new line codes */
	risse_char *p;
	p=Script;
	while(*p)
	{
		if(*p==RISSE_WC('\r') && *(p+1)==RISSE_WC('\n'))
			*p=RISSE_SKIP_CODE;
		else if(*p==RISSE_WC('\r'))
			*p=RISSE_WC('\n');
		p++;
	}
}

//---------------------------------------------------------------------------
risse_int tRisseLexicalAnalyzer::PutValue(const tRisseVariant &val)
{
	tRisseVariant *v = new tRisseVariant(val);
	Values.push_back(v);
	return Values.size() -1;
}
//---------------------------------------------------------------------------
void tRisseLexicalAnalyzer::Free(void)
{
	if(Script) delete [] Script;
	std::vector<tRisseVariant*>::iterator i;
	for(i = Values.begin(); i != Values.end(); i++)
	{
		delete *i;
	}
	Values.clear();
}
/*
//---------------------------------------------------------------------------
void tRisseLexicalAnalyzer::NextBraceIsBlockBrace()
{
	BlockBrace = true;
}
*/
//---------------------------------------------------------------------------
risse_int tRisseLexicalAnalyzer::GetCurrentPosition()
{
	return Current - Script;
}
//---------------------------------------------------------------------------
risse_int tRisseLexicalAnalyzer::GetNext(risse_int &value)
{

	if(First)
	{
		First = false;
		try
		{
			PreProcess();
		}
		catch(eRisseCompileError &e)
		{
			_yyerror(e.GetMessageString().c_str(), Block, e.GetPosition());
			return 0;
		}
		Current = Script;
		PrevPos = 0;

		if(ExprMode && ResultNeeded)
		{
			value = 0;
			return T_RETURN;
				// return T_RETURN first if 'expression' mode
				// (and ResultNeeded is specified)
		}
	}

	risse_int n;
	value = 0;

	do
	{
		if(RetValDeque.size())
		{
			tTokenPair pair = RetValDeque.front();
			RetValDeque.pop_front();
			value = pair.value;
			PrevToken = pair.token;
			return pair.token;
		}



		try
		{
			if(!EmbeddableExpressionDataStack.size())
			{
				// normal mode
				SkipSpace(&Current);
				const risse_char *org = Current;
				n = GetToken(value);
				PrevPos = org - Script;

				if(risseEnableDicFuncQuickHack) //----- dicfunc quick-hack
				{
					if(DicFunc)
					{
						if(n == T_PERCENT)
						{
							// push "function { return %"
							RetValDeque.push_back(tTokenPair(T_FUNCTION, 0));
							RetValDeque.push_back(tTokenPair(T_LBRACE, 0));
							RetValDeque.push_back(tTokenPair(T_RETURN, 0));
							RetValDeque.push_back(tTokenPair(T_PERCENT, 0));
							n = -1;
						}
						else if(n == T_LBRACKET && PrevToken != T_PERCENT)
						{
							// push "function { return ["
							RetValDeque.push_back(tTokenPair(T_FUNCTION, 0));
							RetValDeque.push_back(tTokenPair(T_LBRACE, 0));
							RetValDeque.push_back(tTokenPair(T_RETURN, 0));
							RetValDeque.push_back(tTokenPair(T_LBRACKET, 0));
							n = -1;
						}
						else if(n == T_RBRACKET)
						{
							// push "] ; }( )"
							RetValDeque.push_back(tTokenPair(T_RBRACKET, 0));
							RetValDeque.push_back(tTokenPair(T_SEMICOLON, 0));
							RetValDeque.push_back(tTokenPair(T_RBRACE, 0));
							RetValDeque.push_back(tTokenPair(T_LPARENTHESIS, 0));
							RetValDeque.push_back(tTokenPair(T_RPARENTHESIS, 0));
							n = -1;
						}
					}
				}
			}
			else
			{
				// embeddable expression mode
				tEmbeddableExpressionData &data = EmbeddableExpressionDataStack.back();
				switch(data.State)
				{
				case evsStart:
					RetValDeque.push_back(tTokenPair(T_LPARENTHESIS, 0));
					n = -1;
					data.State = evsNextIsStringLiteral;
					break;

				case evsNextIsStringLiteral:
				  {
					tRisseVariant v;
					tRisseInternalParseStringResult res =
						RisseInternalParseString(v, &Current,
							data.Delimiter, RISSE_WC('&'));
					if(res == psrDelimiter)
					{
						// embeddable expression mode ended
						ttstr str(v);
						if(!str.IsEmpty())
						{
							if(data.NeedPlus)
								RetValDeque.push_back(tTokenPair(T_PLUS, 0));
						}
						if(!str.IsEmpty() || !data.NeedPlus)
							RetValDeque.push_back(tTokenPair(T_CONSTVAL, PutValue(v)));
						RetValDeque.push_back(tTokenPair(T_RPARENTHESIS, 0));
						EmbeddableExpressionDataStack.pop_back();
						n = -1;
						break;
					}
					else
					{
						// *Current is next to ampersand mark or '${'
						ttstr str(v);
						if(!str.IsEmpty())
						{
							if(data.NeedPlus)
								RetValDeque.push_back(tTokenPair(T_PLUS, 0));
							RetValDeque.push_back(tTokenPair(T_CONSTVAL, PutValue(v)));
							data.NeedPlus = true;
						}
						if(data.NeedPlus)
							RetValDeque.push_back(tTokenPair(T_PLUS, 0));
						RetValDeque.push_back(tTokenPair(T_STRING, 0));
						RetValDeque.push_back(tTokenPair(T_LPARENTHESIS, 0));
						data.State = evsNextIsExpression;
						if(res == psrAmpersand)
							data.WaitingToken = T_SEMICOLON;
						else if(res == psrDollar)
							data.WaitingToken = T_RBRACE, NestLevel++;
						n = -1;
						break;
					}
				  }

				case evsNextIsExpression:
				  {
					SkipSpace(&Current);
					const risse_char *org = Current;
					n = GetToken(value);
					PrevPos = org - Script;

					if(n == data.WaitingToken && NestLevel == data.WaitingNestLevel)
					{
						// end of embeddable expression mode
						RetValDeque.push_back(tTokenPair(T_RPARENTHESIS, 0));
						data.NeedPlus = true;
						data.State = evsNextIsStringLiteral;
						n = -1;
					}

					break;
				  }
				}
			}

			if(n == 0)
			{
				if(IfLevel != 0)
					Risse_eRisseCompileError(RissePPError, Block, Current-Script);
			}
		}
		catch(eRisseCompileError &e)
		{
			_yyerror(e.GetMessageString().c_str(), Block);
			return 0;
		}
		catch(eRisseScriptError &e)
		{
			_yyerror(e.GetMessageString().c_str(), Block);
			return 0;
		}
		catch(eRisse &e)
		{
			_yyerror(e.GetMessageString().c_str(), Block);
			return 0;
		}
	#ifdef RISSE_SUPPORT_VCL
		catch(EAccessViolation &e)
		{
			ttstr msg(e.Message.c_str());
			_yyerror(msg.c_str(), Block);
			return 0;
		}
		catch(Exception &e)
		{
			ttstr msg(e.Message.c_str());
			_yyerror(msg.c_str(), Block);
			return 0;
		}
	#endif
	} while(n < 0);

/*
	if(BlockBrace)
	{
		if(n == T_LBRACE) n = T_BLOCK_LBRACE;
		BlockBrace = false;
	}
*/
	PrevToken = n;
	return n;
}
//---------------------------------------------------------------------------
void tRisseLexicalAnalyzer::SetStartOfRegExp(void)
{
	// notifies that a regular expression ( regexp ) 's
	// first '/' ( that indicates the start of the regexp ) had been detected.
	// this will be called by the parser.

	RegularExpression = true;
}
//---------------------------------------------------------------------------
void tRisseLexicalAnalyzer::SetNextIsBareWord(void)
{
	// notifies that the next word must be treated as a bare word
	// (not a reserved word)
	// this will be called after . (dot) operator by the parser.

	BareWord = true;
}
//---------------------------------------------------------------------------
#endif

} // namespace Risse



