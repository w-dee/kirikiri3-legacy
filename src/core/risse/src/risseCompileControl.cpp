//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief 条件コンパイル式処理
//---------------------------------------------------------------------------

#include "risseCommHead.h"

#include "risseCompileControl.h"
#include "risseLex.h"
#include "risseVariant.h"
#include "rissepp.tab.h"
#include "risseError.h"



namespace Risse
{
RISSE_DEFINE_SOURCE_ID(1006);
//---------------------------------------------------------------------------

int ppparse(void*);
//---------------------------------------------------------------------------
// Risse_iswspace
static bool inline Risse_iswspace(risse_char ch)
{
	// the standard iswspace misses when non-zero page code

	if(ch&0xff00)
	{
		return false;
	}
	else
	{
		return isspace(ch);
	}
}
//---------------------------------------------------------------------------
static bool inline Risse_iswdigit(risse_char ch)
{
	// the standard iswdigit misses when non-zero page code

	if(ch&0xff00)
	{
		return false;
	}
	else
	{
		return isdigit(ch);
	}
}
//---------------------------------------------------------------------------
static bool inline Risse_iswalpha(risse_char ch)
{
	// the standard iswalpha misses when non-zero page code

	if(ch&0xff00)
	{
		return true;
	}
	else
	{
		return isalpha(ch);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// tRissePPExprParser
//---------------------------------------------------------------------------
tRissePPExprParser::tRissePPExprParser(tRisse * risse, const risse_char *script)
{
	// script pointed by "script" argument will be released by this class
	// via delete[]
	Risse = risse;
	Script = script;
}
//---------------------------------------------------------------------------
tRissePPExprParser::~tRissePPExprParser()
{
	delete [] Script;
}
//---------------------------------------------------------------------------
risse_int32 tRissePPExprParser::Parse()
{
	Current = Script;
	Result = 0;
	if(ppparse(this))
	{
		Risse_eRisseError(RissePPError);
	}
	return Result;
}
//---------------------------------------------------------------------------
risse_int tRissePPExprParser::GetNext(risse_int32 &value)
{
	// get next token

	while(Risse_iswspace(*Current) && *Current) Current++;
	if(!*Current) return 0;

	switch(*Current)
	{
	case RISSE_WC('('):
		Current++;
		return PT_LPARENTHESIS;

	case RISSE_WC(')'):
		Current++;
		return PT_RPARENTHESIS;

	case RISSE_WC(','):
		Current++;
		return PT_COMMA;

	case RISSE_WC('='):
		if(*(Current+1) == RISSE_WC('=')) { Current+=2; return PT_EQUALEQUAL; }
		Current++;
		return PT_EQUAL;

	case RISSE_WC('!'):
		if(*(Current+1) == RISSE_WC('=')) { Current+=2; return PT_NOTEQUAL; }
		Current++;
		return PT_EXCLAMATION;

	case RISSE_WC('|'):
		if(*(Current+1) == RISSE_WC('|')) { Current+=2; return PT_LOGICALOR; }
		Current++;
		return PT_VERTLINE;

	case RISSE_WC('&'):
		if(*(Current+1) == RISSE_WC('&')) { Current+=2; return PT_LOGICALAND; }
		Current++;
		return PT_AMPERSAND;

	case RISSE_WC('^'):
		Current++;
		return PT_CHEVRON;

	case RISSE_WC('+'):
		Current++;
		return PT_PLUS;

	case RISSE_WC('-'):
		Current++;
		return PT_MINUS;

	case RISSE_WC('*'):
		Current++;
		return PT_ASTERISK;

	case RISSE_WC('/'):
		Current++;
		return PT_SLASH;

	case RISSE_WC('%'):
		Current++;
		return PT_PERCENT;

	case RISSE_WC('<'):
		if(*(Current+1) == RISSE_WC('=')) { Current+=2; return PT_LTOREQUAL; }
		Current++;
		return PT_LT;

	case RISSE_WC('>'):
		if(*(Current+1) == RISSE_WC('=')) { Current+=2; return PT_GTOREQUAL; }
		Current++;
		return PT_GT;

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
	  {
		// number
		tRisseVariant val;
		try
		{
			if(!RisseParseNumber(val, &Current)) return PT_ERROR;
		}
		catch(...)
		{
			return PT_ERROR;
		}
		value = (risse_int32)(tTVInteger)val;
		return PT_NUM;
	  }

	}

	if(!Risse_iswalpha(*Current) && *Current!=RISSE_WC('_'))
	{
		return PT_ERROR;
	}

	const risse_char *st = Current;
	while((Risse_iswalpha(*Current) || Risse_iswdigit(*Current) ||
		*Current==RISSE_WC('_')) && *Current)
		Current++;

	ttstr str(st, Current-st);

	IDs.push_back(str);
    value = IDs.size() -1;

	return PT_SYMBOL;
}
//---------------------------------------------------------------------------
const risse_char * tRissePPExprParser::GetString(risse_int idx) const
{
	return IDs[idx].c_str();
}
//---------------------------------------------------------------------------
int pplex(YYSTYPE *yylex, void *pm)
{
	risse_int32 val;
	risse_int n;
	n = ((tRissePPExprParser*)pm)->GetNext(val);
	if(n == PT_NUM) yylex->val = val;
	if(n == PT_SYMBOL) yylex->nv = val;
	return n;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
} // namespace Risse




