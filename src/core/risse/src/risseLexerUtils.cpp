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
bool tRisseLexerUtility::SkipSpaceExceptForNewLine(const risse_char * & ptr)
{
	while(*ptr && Risse_iswspace_nc(*ptr) &&
		!IsNewLineChar(*ptr)) ptr ++;
	return *ptr != 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseLexerUtility::StepNewLineChar(const risse_char * & ptr)
{
	if(ptr[0] == RISSE_WC('\r') && ptr[1] == RISSE_WC('\n'))
		ptr += 2; // CR LF
	else
		ptr ++; // CR のみ あるいは LF のみ

	if(*ptr == 0) return false;

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseLexerUtility::SkipToLineEnd(const risse_char * & ptr)
{
	while(*ptr && !IsNewLineChar(*ptr)) ptr++;
	if(*ptr == 0) return false;
	return StepNewLineChar(ptr);
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

	// TODO: 改行がリテラル中にあった場合はソースの改行コードにかかわらず常に \n とするように

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
		// 空白をスキップ
		if(!SkipSpace(ptr))
			eRisseError::Throw(RISSE_WS_TR("Unclosed octet literal"));

		// コメントをスキップ
		switch(SkipComment(ptr))
		{
		case scrEnded:
			eRisseError::Throw(RISSE_WS_TR("Unclosed octet literal"));
		case scrContinue:
		case scrNotComment:
			;
		}

		// 空白をスキップ
		if(!SkipSpace(ptr))
			eRisseError::Throw(RISSE_WS_TR("Unclosed octet literal"));

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
			val = ret;
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
	// TODO: 改行がリテラル中にあった場合はソースの改行コードにかかわらず常に \n とするように

	bool ok = false;
	bool lastbackslash = false;
	tRisseString str;
	tRisseString flg;

	risse_char delimiter = *ptr; // デリミタを読む
	if(!*(++ptr)) return false;

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
		else if(*ptr == delimiter && !lastbackslash)
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

} // namespace Risse



