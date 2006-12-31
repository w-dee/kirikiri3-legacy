//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief Risse レキシカル・アナライザ(字句解析機)の実装
//---------------------------------------------------------------------------
#ifndef risseLexH
#define risseLexH

#include "risseConfig.h"
#include "risseVariant.h"
#include <vector>
#include <deque>


extern bool risseEnableDicFuncQuickHack;
// Defining this enables quick-hack, avoiding the dictionary/array parser
// memory overflow.
// This is done with replacing %[ ... ] to function { return %[ ... ]; }()
// and replacing [ ... ] to function { return [ ... ]; }().
// These replacing is applied for expression which starts with "%[" or "[", 
// may cause some sideeffects....

namespace Risse
{
//---------------------------------------------------------------------------
extern risse_int RisseHexNum(risse_char ch) throw();
extern risse_int RisseOctNum(risse_char ch) throw();
extern risse_int RisseDecNum(risse_char ch) throw();
extern risse_int RisseBinNum(risse_char ch) throw();

bool RisseParseString(tRisseVariant &val, const risse_char **ptr);
bool RisseParseNumber(tRisseVariant &val, const risse_char **ptr);
void RisseReservedWordsHashAddRef();
void RisseReservedWordsHashRelease();
enum tRisseSkipCommentResult
{ scrContinue, scrEnded, scrNotComment };
//---------------------------------------------------------------------------
class tRisseScriptBlock;
class tRisseLexicalAnalyzer
{
public:
	tRisseLexicalAnalyzer(tRisseScriptBlock *block, const risse_char *script,
		bool exprmode, bool resneeded);
	~tRisseLexicalAnalyzer();

private:
	const risse_char *Current;
	risse_int PrevPos;
	risse_int PrevToken;
	bool First;
	bool ExprMode;
	bool ResultNeeded;
	risse_int NestLevel;

	bool DicFunc; //----- dicfunc quick-hack

	struct tTokenPair
	{
		risse_int token;
		risse_int value;

		tTokenPair(risse_int token, risse_int value)
		{
			this->token = token;
			this->value = value;
		}
	};

	std::deque<tTokenPair> RetValDeque;

//	bool BlockBrace;

	bool RegularExpression;
	bool BareWord;

	enum tEmbeddableExpressionState
	{	evsStart, evsNextIsStringLiteral, evsNextIsExpression };

	struct tEmbeddableExpressionData
	{
		tEmbeddableExpressionState State;
		risse_int WaitingNestLevel;
		risse_int WaitingToken;
		risse_char Delimiter;
		bool NeedPlus;
	};

	std::vector<tEmbeddableExpressionData> EmbeddableExpressionDataStack;


	tRisseScriptBlock *Block;

	risse_char *Script;

	tRisseSkipCommentResult SkipUntil_endif();
	tRisseSkipCommentResult ProcessPPStatement();

	risse_int GetToken(risse_int &value);

	risse_int32 ParsePPExpression(const risse_char *start,
		risse_int n);

	void PreProcess(void);

	std::vector<tRisseVariant *> Values;

	risse_int PutValue(const tRisseVariant &val);


	risse_int IfLevel; // @if nesting level

public:
	const tRisseVariant & GetValue(risse_int idx) const
	{
		return *Values[idx];
	}
	const risse_char * GetString(risse_int idx) const
	{
		return Values[idx]->GetString();
	}

	void Free(void);

//	void NextBraceIsBlockBrace();

	risse_int GetCurrentPosition();

	risse_int GetNext(risse_int &value);

	void SetStartOfRegExp(void);
	void SetNextIsBareWord();

};
//---------------------------------------------------------------------------

} // namespace Risse

#endif
