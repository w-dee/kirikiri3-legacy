//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse レキシカル・アナライザ(字句解析器)の実装
//---------------------------------------------------------------------------
#include "prec.h"

namespace Risse
{
//---------------------------------------------------------------------------
class tRisseLexer
{
public: // public static members
	//! @brief		16進数文字1桁を数値に
	//! @param		ch	文字
	//! @return		文字の表す数値 (-1:解析不能)
	static risse_int HexNum(risse_char ch);

	//! @brief		8進数文字1桁を数値に
	//! @param		ch	文字
	//! @return		文字の表す数値 (-1:解析不能)
	static risse_int OctNum(risse_char ch);

	//! @brief		10進数文字1桁を数値に
	//! @param		ch	文字
	//! @return		文字の表す数値 (-1:解析不能)
	static risse_int DecNum(risse_char ch);

	//! @brief		2進数文字1桁を数値に
	//! @param		ch	文字
	//! @return		文字の表す数値 (-1:解析不能)
	static risse_int BinNum(risse_char ch);

	//! @brief		バックスラッシュエスケープの値を得る
	//! @param		ch	文字
	//! @return		文字の表すコード
	static risse_int UnescapeBackSlash(risse_char ch);

	//! @brief	コメントをスキップしようとした際の状態
	enum tSkipCommentResult
	{
		scrContinue,	//!< スクリプトはまだ続く
		scrEnded,		//!< スクリプトは終わった
		scrNotComment	//!< 実はコメントではなかった
	};

	//! @brief		コメントのスキップ
	//! @param		ptr		解析ポインタ
	//! @return		スキップした結果どうなったか
	static tRisseSkipCommentResult SkipComment(const risse_char * & ptr);

	//! @brief		ホワイトスペースのスキップ
	//! @param		ptr		解析ポインタ
	//! @return		スクリプトが継続するかどうか
	static bool SkipSpace(const risse_char * & ptr);


private:
	//! @brief		Ptr にある文字列を別の文字列と比較する
	//! @param		wrd		比較する文字列
	//! @param		isword	単語単位の比較を行う場合に真
	//! @return		単語が一致したかどうか
	bool StringMatch(const risse_char *wrd, bool isword);

	//! @brief InternalParseString の戻り値
	enum tInternalParseStringResult
	{
		psrNone, //!< (エラー)
		psrDelimiter, //!< デリミタにぶつかった
		psrAmpersand, //!< & にぶつかった ( &式; 形式の埋め込み )
		psrDollar
	};

	//! @brief		文字列を解析する(内部関数)
	//! @param		val		解析した文字列を格納する先
	//! @param		delim	デリミタ ( '\'' か '"' )
	//! @param		embexpmode	埋め込み式モードかどうか (@つき文字列リテラルかどうか)
	//! @return		内部ステータス
	tInternalParseStringResult
		InternalParseString(tRisseString &val,
			risse_char delim, bool embexpmode);
		
	bool RisseParseString(tRisseString &val);

public:
	const risse_char * Ptr; //!< 解析ポインタ

};

//---------------------------------------------------------------------------
} // namespace Risse





























//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

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

