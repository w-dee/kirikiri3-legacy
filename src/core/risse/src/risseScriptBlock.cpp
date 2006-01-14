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
#include "risseCommHead.h"

#include "risseScriptBlock.h"
#include "risseInterCodeGen.h"



namespace Risse
{
RISSE_DEFINE_SOURCE_ID(1028);
//---------------------------------------------------------------------------
int yyparse(void*);
//---------------------------------------------------------------------------
// tRisseScriptBlock
//---------------------------------------------------------------------------
tRisseScriptBlock::tRisseScriptBlock(tRisse * owner)
{
	RefCount = 1;
	Owner = owner;
	Owner->AddRef();

	Script = NULL;
	Name = NULL;

	InterCodeContext = NULL;
	TopLevelContext = NULL;
	LexicalAnalyzer = NULL;

	UsingPreProcessor = false;

	LineOffset = 0;

	Owner->AddScriptBlock(this);
}
//---------------------------------------------------------------------------
tRisseScriptBlock::~tRisseScriptBlock()
{
	if(TopLevelContext) TopLevelContext->Release(), TopLevelContext = NULL;
	while(ContextStack.size())
	{
		ContextStack.top()->Release();
		ContextStack.pop();
	}

	Owner->RemoveScriptBlock(this);

	if(LexicalAnalyzer) delete LexicalAnalyzer;

	if(Script) delete [] Script;
	if(Name) delete [] Name;

	Owner->Release();
}
//---------------------------------------------------------------------------
void tRisseScriptBlock::AddRef(void)
{
	RefCount ++;
}
//---------------------------------------------------------------------------
void tRisseScriptBlock::Release(void)
{
	if(RefCount <= 1)
		delete this;
	else
		RefCount--;
}
//---------------------------------------------------------------------------
void tRisseScriptBlock::SetName(const risse_char *name, risse_int lineofs)
{
	if(Name) delete [] Name, Name = NULL;
	if(name)
	{
		LineOffset = lineofs;
		Name = new risse_char[ Risse_strlen(name) + 1];
		Risse_strcpy(Name, name);
	}
}
//---------------------------------------------------------------------------
void tRisseScriptBlock::Add(tRisseInterCodeContext * cntx)
{
	InterCodeContextList.push_back(cntx);
}
//---------------------------------------------------------------------------
void tRisseScriptBlock::Remove(tRisseInterCodeContext * cntx)
{
	InterCodeContextList.remove(cntx);
}
//---------------------------------------------------------------------------
risse_uint tRisseScriptBlock::GetTotalVMCodeSize() const
{
	risse_uint size = 0;

	std::list<tRisseInterCodeContext *>::const_iterator i;
	for(i = InterCodeContextList.begin(); i != InterCodeContextList.end(); i++)
	{
		size += (*i)->GetCodeSize();
	}
	return size;
}
//---------------------------------------------------------------------------
risse_uint tRisseScriptBlock::GetTotalVMDataSize() const
{
	risse_uint size = 0;

	std::list<tRisseInterCodeContext *>::const_iterator i;
	for(i = InterCodeContextList.begin(); i != InterCodeContextList.end(); i++)
	{
		size += (*i)->GetDataSize();
	}
	return size;
}
//---------------------------------------------------------------------------
risse_char * tRisseScriptBlock::GetLine(risse_int line, risse_int *linelength) const
{
	// note that this function DOES matter LineOffset
	line -= LineOffset;
	if(linelength) *linelength = LineLengthVector[line];
	return Script + LineVector[line];
}
//---------------------------------------------------------------------------
risse_int tRisseScriptBlock::SrcPosToLine(risse_int pos) const
{
	risse_uint s = 0;
	risse_uint e = LineVector.size();
	while(true)
	{
		if(e-s <= 1) return s + LineOffset; // LineOffset is added
		risse_uint m = s + (e-s)/2;
		if(LineVector[m] > pos)
			e = m;
		else
			s = m;
	}
}
//---------------------------------------------------------------------------
risse_int tRisseScriptBlock::LineToSrcPos(risse_int line) const
{
	// assumes line is added by LineOffset
	line -= LineOffset;
	return LineVector[line];
}
//---------------------------------------------------------------------------
ttstr tRisseScriptBlock::GetLineDescriptionString(risse_int pos) const
{
	// get short description, like "mainwindow.risse(321)"
	// pos is in character count from the first of the script
	risse_int line =SrcPosToLine(pos)+1;
	ttstr name;
	if(Name)
	{
		name = Name;
	}
	else
	{
		risse_char ptr[128];
		Risse_pointer_to_str(this, ptr);
		name = ttstr(RISSE_WS("anonymous@0x")) + ptr;
	}

	return name + RISSE_WS1("(") + ttstr(line) + RISSE_WS2(")");
}
//---------------------------------------------------------------------------
void tRisseScriptBlock::ConsoleOutput(const risse_char *msg, void *data)
{
	tRisseScriptBlock *blk = (tRisseScriptBlock*)data;
	blk->Owner->OutputToConsole(msg);
}
//---------------------------------------------------------------------------
#ifdef RISSE_DEBUG_PROFILE_TIME
risse_uint parsetime = 0;
extern risse_uint time_make_np;
extern risse_uint time_PutData;
extern risse_uint time_PutCode;
extern risse_uint time_this_proxy;
extern risse_uint time_Commit;
extern risse_uint time_yylex;
extern risse_uint time_GenNodeCode;

extern risse_uint time_ns_Push;
extern risse_uint time_ns_Pop;
extern risse_uint time_ns_Find;
extern risse_uint time_ns_Add;
extern risse_uint time_ns_Remove;
extern risse_uint time_ns_Commit;

#endif

void tRisseScriptBlock::SetText(tRisseVariant *result, const risse_char *text,
	iRisseDispatch2 * context, bool isexpression)
{
	// compiles text and executes its global level scripts.
	// the script will be compiled as an expression if isexpressn is true.
	if(!text) return;
	if(!text[0]) return;

	Script = new risse_char[Risse_strlen(text)+1];
	Risse_strcpy(Script, text);

	// calculation of line-count
	risse_char *ls = Script;
	risse_char *p = Script;
	while(*p)
	{
		if(*p == RISSE_WC('\r') || *p == RISSE_WC('\n'))
		{
			LineVector.push_back(int(ls - Script));
			LineLengthVector.push_back(int(p - ls));
			if(*p == RISSE_WC('\r') && p[1] == RISSE_WC('\n')) p++;
			p++;
			ls = p;
		}
		else
		{
			p++;
		}
	}

	if(p!=ls)
	{
		LineVector.push_back(int(ls - Script));
		LineLengthVector.push_back(int(p - ls));
	}

	try
	{

		// parse and execute
#ifdef RISSE_DEBUG_PROFILE_TIME
		{
		tRisseTimeProfiler p(parsetime);
#endif

		Parse(text, isexpression, result != NULL);

#ifdef RISSE_DEBUG_PROFILE_TIME
		}

		{
			char buf[256];
			sprintf(buf, "parsing : %d", parsetime);
			OutputDebugString(buf);
			if(parsetime)
			{
			sprintf(buf, "Commit : %d (%d%%)", time_Commit, time_Commit*100/parsetime);
			OutputDebugString(buf);
			sprintf(buf, "yylex : %d (%d%%)", time_yylex, time_yylex*100/parsetime);
			OutputDebugString(buf);
			sprintf(buf, "MakeNP : %d (%d%%)", time_make_np, time_make_np*100/parsetime);
			OutputDebugString(buf);
			sprintf(buf, "GenNodeCode : %d (%d%%)", time_GenNodeCode, time_GenNodeCode*100/parsetime);
			OutputDebugString(buf);
			sprintf(buf, "  PutCode : %d (%d%%)", time_PutCode, time_PutCode*100/parsetime);
			OutputDebugString(buf);
			sprintf(buf, "  PutData : %d (%d%%)", time_PutData, time_PutData*100/parsetime);
			OutputDebugString(buf);
			sprintf(buf, "  this_proxy : %d (%d%%)", time_this_proxy, time_this_proxy*100/parsetime);
			OutputDebugString(buf);

			sprintf(buf, "ns::Push : %d (%d%%)", time_ns_Push, time_ns_Push*100/parsetime);
			OutputDebugString(buf);
			sprintf(buf, "ns::Pop : %d (%d%%)", time_ns_Pop, time_ns_Pop*100/parsetime);
			OutputDebugString(buf);
			sprintf(buf, "ns::Find : %d (%d%%)", time_ns_Find, time_ns_Find*100/parsetime);
			OutputDebugString(buf);
			sprintf(buf, "ns::Remove : %d (%d%%)", time_ns_Remove, time_ns_Remove*100/parsetime);
			OutputDebugString(buf);
			sprintf(buf, "ns::Commit : %d (%d%%)", time_ns_Commit, time_ns_Commit*100/parsetime);
			OutputDebugString(buf);

			}
		}
#endif

#ifdef RISSE_DEBUG_DISASM
		std::list<tRisseInterCodeContext *>::iterator i =
			InterCodeContextList.begin();
		while(i != InterCodeContextList.end())
		{
			ConsoleOutput(RISSE_WS(""), (void*)this);
			ConsoleOutput((*i)->GetName(), (void*)this);
			(*i)->Disassemble(ConsoleOutput, (void*)this);
			i++;
		}
#endif

		// execute global level script
		ExecuteTopLevelScript(result, context);
	}
	catch(...)
	{
		if(InterCodeContextList.size() != 1)
		{
			if(TopLevelContext) TopLevelContext->Release(), TopLevelContext = NULL;
			while(ContextStack.size())
			{
				ContextStack.top()->Release();
				ContextStack.pop();
			}
		}
		throw;
	}

	if(InterCodeContextList.size() != 1)
	{
		// this is not a single-context script block
		// (may hook itself)
		// release all contexts and global at this time
		if(TopLevelContext) TopLevelContext->Release(), TopLevelContext = NULL;
		while(ContextStack.size())
		{
			ContextStack.top()->Release();
			ContextStack.pop();
		}
	}
}
//---------------------------------------------------------------------------
void tRisseScriptBlock::ExecuteTopLevelScript(tRisseVariant *result,
	iRisseDispatch2 * context)
{
	if(TopLevelContext)
	{
#ifdef RISSE_DEBUG_PROFILE_TIME
		clock_t start = clock();
#endif
		TopLevelContext->FuncCall(0, NULL, NULL, result, 0, NULL, context);
#ifdef RISSE_DEBUG_PROFILE_TIME
		risse_char str[100];
		Risse_sprintf(str, RISSE_WS("%d"), clock() - start);
		ConsoleOutput(str, (void*)this);
#endif
	}
}
//---------------------------------------------------------------------------
void tRisseScriptBlock::PushContextStack(const risse_char *name, tRisseContextType type)
{
	tRisseInterCodeContext *cntx;
	cntx = new tRisseInterCodeContext(InterCodeContext, name, this, type);
	if(InterCodeContext==NULL)
	{
		if(TopLevelContext) Risse_eRisseError(RisseInternalError);
		TopLevelContext = cntx;
		TopLevelContext->AddRef();
	}
	ContextStack.push(cntx);
	InterCodeContext = cntx;
}
//---------------------------------------------------------------------------
void tRisseScriptBlock::PopContextStack(void)
{
	InterCodeContext->Commit();
	InterCodeContext->Release();
	ContextStack.pop();
	if(ContextStack.size() >= 1)
		InterCodeContext = ContextStack.top();
	else
		InterCodeContext = NULL;
}
//---------------------------------------------------------------------------
void tRisseScriptBlock::Parse(const risse_char *script, bool isexpr, bool resultneeded)
{
	if(!script) return;

	CompileErrorCount = 0;

	LexicalAnalyzer = new tRisseLexicalAnalyzer(this, script, isexpr, resultneeded);

	try
	{
		yyparse(this);
	}
	catch(...)
	{
		delete LexicalAnalyzer; LexicalAnalyzer=NULL;
		throw;
	}

	delete LexicalAnalyzer; LexicalAnalyzer=NULL;

	if(CompileErrorCount)
	{
		Risse_eRisseScriptError(FirstError, this, FirstErrorPos);
	}
}
//---------------------------------------------------------------------------
void tRisseScriptBlock::SetFirstError(const risse_char *error, risse_int pos)
{
	if(CompileErrorCount == 0)
	{
		FirstError = error;
		FirstErrorPos = pos;
	}
}
//---------------------------------------------------------------------------
ttstr tRisseScriptBlock::GetNameInfo() const
{
	if(LineOffset == 0)
	{
		return ttstr(Name);
	}
	else
	{
		return ttstr(Name) + RISSE_WS1("(line +") + ttstr(LineOffset) + RISSE_WS2(")");
	}
}
//---------------------------------------------------------------------------
void tRisseScriptBlock::Dump() const
{
	std::list<tRisseInterCodeContext *>::const_iterator i =
		InterCodeContextList.begin();
	while(i != InterCodeContextList.end())
	{
		ConsoleOutput(RISSE_WS(""), (void*)this);
		risse_char ptr[256];
		Risse_strcpy(ptr, RISSE_WS(" 0x"));
		Risse_pointer_to_str((*i), ptr + 3);
		ConsoleOutput((ttstr(RISSE_WS("(")) + ttstr((*i)->GetContextTypeName()) +
			RISSE_WS(") ") + ttstr((*i)->GetName()) + ptr).c_str(), (void*)this);
		(*i)->Disassemble(ConsoleOutput, (void*)this);
		i++;
	}
}
//---------------------------------------------------------------------------



} // namespace



