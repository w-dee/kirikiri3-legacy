//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief スクリプトブロック管理
//---------------------------------------------------------------------------
#ifndef risseScriptBlockH
#define risseScriptBlockH

#include "risseInterface.h"
#include "risseInterCodeGen.h"
#include "risseLex.h"
#include "risse.h"

#include <list>

namespace Risse
{
//---------------------------------------------------------------------------
// tRisseScriptBlock - a class for managing the script block
//---------------------------------------------------------------------------
class tRisse;
class tRisseInterCodeContext;
class tRisseScriptBlock
{
public:
	tRisseScriptBlock(tRisse * owner);
	virtual ~tRisseScriptBlock();

private:
	tRisse * Owner;
	risse_int RefCount;
	risse_char *Script;
	risse_char *Name;
	risse_int LineOffset;

	std::list<tRisseInterCodeContext *> InterCodeContextList;

	std::stack<tRisseInterCodeContext *> ContextStack;

	tRisseLexicalAnalyzer *LexicalAnalyzer;

	tRisseInterCodeContext *InterCodeContext;

	std::vector<risse_int> LineVector;
	std::vector<risse_int> LineLengthVector;

	tRisseInterCodeContext * TopLevelContext;

	tRisseString FirstError;
	risse_int FirstErrorPos;

	bool UsingPreProcessor;

public:
	risse_int CompileErrorCount;

	tRisse * GetRisse() { return Owner; }

	void AddRef();
	void Release();

	void Add(tRisseInterCodeContext * cntx);
	void Remove(tRisseInterCodeContext * cntx);

	risse_uint GetContextCount() const { return InterCodeContextList.size(); }
	risse_uint GetTotalVMCodeSize() const;  // returns in VM word size ( 1 word = 32bit )
	risse_uint GetTotalVMDataSize() const;  // returns in tRisseVariant count

	bool IsReusable() const { return GetContextCount() == 1 &&
		TopLevelContext != NULL && !UsingPreProcessor; }

	risse_char * GetLine(risse_int line, risse_int *linelength) const;
	risse_int SrcPosToLine(risse_int pos) const;
	risse_int LineToSrcPos(risse_int line) const;

	ttstr GetLineDescriptionString(risse_int pos) const;

	const risse_char *GetScript() const { return Script; }

	void PushContextStack(const risse_char *name, tRisseContextType type);
	void PopContextStack(void);
	void Parse(const risse_char *script, bool isexpr, bool resultneeded);

	void SetFirstError(const risse_char *error, risse_int pos);

	tRisseLexicalAnalyzer * GetLexicalAnalyzer() { return LexicalAnalyzer; }
	tRisseInterCodeContext * GetCurrentContext() { return InterCodeContext; }

	const risse_char *GetName() const { return Name; }
	void SetName(const risse_char *name, risse_int lineofs);
	ttstr GetNameInfo() const;

	risse_int GetLineOffset() const { return LineOffset; }

	void NotifyUsingPreProcessor() { UsingPreProcessor = true; }

	void Dump() const;

private:
	static void ConsoleOutput(const risse_char *msg, void *data);

public:
	static void (*GetConsoleOutput())(const risse_char *msg, void *data)
		{ return ConsoleOutput; }

	void SetText(tRisseVariant *result, const risse_char *text, iRisseDispatch2 * context,
		bool isexpression);

	void ExecuteTopLevelScript(tRisseVariant *result, iRisseDispatch2 * context);

};
//---------------------------------------------------------------------------
}

#endif
