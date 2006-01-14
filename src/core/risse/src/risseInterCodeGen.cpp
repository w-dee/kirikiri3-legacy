//---------------------------------------------------------------------------
/*
	Risse [�肹]
	alias RISE [りせ], acronym of "Rise Is a Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief VM用コンパイラ
//---------------------------------------------------------------------------
#include "risseCommHead.h"

#include <algorithm>

#include "risseInterCodeGen.h"
#include "risseScriptBlock.h"
#include "risseGlobalStringMap.h"

#include "risse.tab.h"
#include "risseError.h"
#include "risseUtils.h"
#include "risseDebug.h"

#define LEX_POS (Block->GetLexicalAnalyzer() -> GetCurrentPosition())
#define NODE_POS (node?node->GetPosition():-1)


/*
	'intermediate code' means that it is not a final code, yes.
	I thought this should be converted to native machine code before execute,
	at early phase of developping Risse.
	But Risse intermediate VM code has functions that are too abstract to
	be converted to native machine code, so I decided that I write a code
	which directly drives intermediate VM code.
*/


//---------------------------------------------------------------------------
namespace Risse  // following is in the namespace
{
RISSE_DEFINE_SOURCE_ID(1017);
//---------------------------------------------------------------------------


int __yyerror(char *, void*);

#ifdef RISSE_DEBUG_PROFILE_TIME
risse_uint time_yylex = 0;
#endif

//---------------------------------------------------------------------------
int yylex(YYSTYPE *yylex, void *pm)
{
	// yylex ( is called from bison parser, returns lexical analyzer's return value )
#ifdef RISSE_DEBUG_PROFILE_TIME
	tRisseTimeProfiler prof(time_yylex);
#endif

	risse_int n;
	risse_int t;
	t = ((tRisseScriptBlock *)pm)->GetLexicalAnalyzer()->GetNext(n);
	yylex->num = n;
	return t;
}
//---------------------------------------------------------------------------
int _yyerror(const risse_char * msg, void *pm, risse_int pos)
{
	// handles errors that happen in the compilation

	tRisseScriptBlock *sb = (tRisseScriptBlock*)pm;

	// message conversion
	ttstr str;
	/*if(!Risse_strncmp(msg, RISSE_WS("parse error, expecting "), 23))
	{
		str = RisseExpected;
		if(!Risse_strncmp(msg+23, RISSE_WS("T_SYMBOL"), 8))
			str.Replace(RISSE_WS("%1"), ttstr(RisseSymbol), false);
		else
			str.Replace(RISSE_WS("%1"), msg+23, false);

	}
	else */if(!Risse_strncmp(msg, RISSE_WS("syntax error"), 11))
	{
		str = RisseSyntaxError;
		str.Replace(RISSE_WS("%1"), ttstr(msg), false);
	}
	else
	{
		str = msg;
	}

	risse_int errpos =
		pos == -1 ? sb->GetLexicalAnalyzer()->GetCurrentPosition(): pos;

	if(sb->CompileErrorCount == 0)
	{
		sb->SetFirstError(str.c_str(), errpos);
	}

	sb->CompileErrorCount++;

	str += ttstr(RISSE_WS(" at line ")) + ttstr(1+sb->SrcPosToLine(errpos)) ;

	sb->GetRisse()->OutputToConsole(str.c_str());

	return 0;
}

//---------------------------------------------------------------------------
int __yyerror(char * msg, void * pm)
{
	// yyerror ( for bison )
	ttstr str(msg);
	return _yyerror(str.c_str(), pm);
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tRisseExprNode -- expression node
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
tRisseExprNode::tRisseExprNode()
{
	Op = 0;
	Nodes = NULL;
	Val = NULL;
	Position = -1;
}
//---------------------------------------------------------------------------
void tRisseExprNode::Clear()
{
	if(Nodes) delete Nodes;
	if(Val) delete Val;
	Nodes = NULL;
	Val = NULL;
}
//---------------------------------------------------------------------------
void tRisseExprNode::Add(tRisseExprNode *n)
{
	if(!Nodes)
		Nodes = new std::vector<tRisseExprNode*>;
	Nodes->push_back(n);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tRisseInterCodeContext -- intermediate context
//---------------------------------------------------------------------------
static risse_int RisseGetContextHashSize(tRisseContextType type)
{
	switch(type)
	{
	case ctTopLevel:		return 0;
	case ctFunction:		return 1;
	case ctExprFunction: 	return 1;
	case ctProperty:		return 1;
	case ctPropertySetter:	return 0;
	case ctPropertyGetter:	return 0;
	case ctClass:			return RISSE_NAMESPACE_DEFAULT_HASH_BITS;
	case ctSuperClassGetter:return 0;
	default:				return RISSE_NAMESPACE_DEFAULT_HASH_BITS;
	}
}
//---------------------------------------------------------------------------
tRisseInterCodeContext::tRisseInterCodeContext(tRisseInterCodeContext *parent,
	const risse_char *name, tRisseScriptBlock *block, tRisseContextType type) :
		inherited(RisseGetContextHashSize(type))
{
	inherited::CallFinalize = false;
		// this notifies to the class ancestor - "tRisseCustomObject", not to
		// call "finalize" risse method at the invalidation.

	Parent = parent;

	PropGetter = PropSetter = SuperClassGetter = NULL;

	CodeArea = NULL;
	CodeAreaCapa = 0;
	CodeAreaSize = 0;

	_DataArea = NULL;
	_DataAreaCapa = 0;
	_DataAreaSize = 0;
	DataArea = NULL;
	DataAreaSize = 0;

	FrameBase = 1;

	SuperClassExpr = NULL;

	MaxFrameCount = 0;
	MaxVariableCount = 0;

	FuncDeclArgCount = 0;
	FuncDeclUnnamedArgArrayBase = 0;
	FuncDeclCollapseBase = -1;

	FunctionRegisterCodePoint = 0;


	PrevSourcePos = -1;
	SourcePosArraySorted = false;
	SourcePosArray = NULL;
	SourcePosArrayCapa = 0;
	SourcePosArraySize = 0;


	if(name)
	{
		Name = new risse_char[Risse_strlen(name)+1];
		Risse_strcpy(Name, name);
	}
	else
	{
		Name = NULL;
	}

	try
	{
		AsGlobalContextMode = false;

		ContextType = type;

		switch(ContextType) // decide variable reservation count with context type
		{
			case ctTopLevel:		VariableReserveCount = 2; break;
			case ctFunction:		VariableReserveCount = 2; break;
			case ctExprFunction: 	VariableReserveCount = 2; break;
			case ctProperty:		VariableReserveCount = 0; break;
			case ctPropertySetter:	VariableReserveCount = 2; break;
			case ctPropertyGetter:	VariableReserveCount = 2; break;
			case ctClass:			VariableReserveCount = 2; break;
			case ctSuperClassGetter:VariableReserveCount = 2; break;
		}


		Block = block;
		block->Add(this);
		if(ContextType != ctTopLevel) Block->AddRef();
			// owner ScriptBlock hooks global object, so to avoid mutual reference lock.


		if(ContextType == ctClass)
		{
			// add class information to the class instance information
			if(MaxFrameCount < 1) MaxFrameCount = 1;

			risse_int dp = PutData(tRisseVariant(Name));
			// const %1, name
			// addci %-1, %1
			// cl %1
			PutCode(VM_CONST, LEX_POS);
			PutCode(RISSE_TO_VM_REG_ADDR(1));
			PutCode(RISSE_TO_VM_REG_ADDR(dp));
			PutCode(VM_ADDCI);
			PutCode(RISSE_TO_VM_REG_ADDR(-1));
			PutCode(RISSE_TO_VM_REG_ADDR(1));
			PutCode(VM_CL);
			PutCode(RISSE_TO_VM_REG_ADDR(1));

			// update FunctionRegisterCodePoint
			FunctionRegisterCodePoint = CodeAreaSize; // update FunctionRegisterCodePoint
		}
	}
	catch(...)
	{
		delete [] Name;
		throw;
	}
}
//---------------------------------------------------------------------------
tRisseInterCodeContext::~tRisseInterCodeContext()
{
	if(Name) delete [] Name;
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::Finalize(void)
{
	if(PropSetter) PropSetter->Release(), PropSetter = NULL;
	if(PropGetter) PropGetter->Release(), PropGetter = NULL;
	if(SuperClassGetter) SuperClassGetter->Release(), SuperClassGetter = NULL;

	if(CodeArea) Risse_free(CodeArea), CodeArea = NULL;
	if(_DataArea)
	{
		for(risse_int i=0; i<_DataAreaSize; i++) delete _DataArea[i];
		Risse_free(_DataArea);
		_DataArea = NULL;
	}
	if(DataArea)
	{
		delete [] DataArea;
		DataArea = NULL;
	}

	Block->Remove(this);

	if(ContextType!=ctTopLevel && Block) Block->Release();

	Namespace.Clear();

	ClearNodesToDelete();

	if(SourcePosArray) Risse_free(SourcePosArray), SourcePosArray = NULL;

	inherited::Finalize();
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::ClearNodesToDelete(void)
{
	if(NodeToDeleteVector.size())
	{
		for(risse_int i=NodeToDeleteVector.size()-1; i>=0; i--)
		{
			delete NodeToDeleteVector[i];
		}
	}

	NodeToDeleteVector.clear();
}
//---------------------------------------------------------------------------
const risse_char* tRisseInterCodeContext::GetContextTypeName() const
{
	switch(ContextType)
	{
	case ctTopLevel:		return RISSE_WS("top level script");
	case ctFunction:		return RISSE_WS("function");
	case ctExprFunction:	return RISSE_WS("function expression");
	case ctProperty:		return RISSE_WS("property");
	case ctPropertySetter:	return RISSE_WS("property setter");
	case ctPropertyGetter:	return RISSE_WS("property getter");
	case ctClass:			return RISSE_WS("class");
	case ctSuperClassGetter:return RISSE_WS("super class getter proxy");
	default:				return RISSE_WS("unknown");
	}
}
//---------------------------------------------------------------------------
ttstr tRisseInterCodeContext::GetShortDescription() const
{
	ttstr ret(RISSE_WS1("(") + ttstr(GetContextTypeName()) + RISSE_WS2(")"));

	const risse_char *name;
	if(ContextType == ctPropertySetter || ContextType == ctPropertyGetter)
	{
		if(Parent)
			name = Parent->Name;
		else
			name = NULL;
	}
	else
	{
		name = Name;
	}

	if(name) ret += RISSE_WS(" ") + ttstr(name);

	return ret;
}
//---------------------------------------------------------------------------
ttstr tRisseInterCodeContext::GetShortDescriptionWithClassName() const
{
	ttstr ret(RISSE_WS1("(") + ttstr(GetContextTypeName()) + RISSE_WS2(")"));

	tRisseInterCodeContext * parent;

	const risse_char *classname;
	const risse_char *name;

	if(ContextType == ctPropertySetter || ContextType == ctPropertyGetter)
		parent = Parent ? Parent->Parent : NULL;
	else
		parent = Parent;

	if(parent)
		classname = parent->Name;
	else
		classname = NULL;

	if(ContextType == ctPropertySetter || ContextType == ctPropertyGetter)
	{
		if(Parent)
			name = Parent->Name;
		else
			name = NULL;
	}
	else
	{
		name = Name;
	}

	if(name)
	{
		ret += RISSE_WS(" ");
		if(classname) ret += ttstr(classname) + RISSE_WS(".");
		ret += ttstr(name);
	}

	return ret;
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::OutputWarning(const risse_char *msg, risse_int pos)
{
	ttstr str(RisseWarning);

	str += msg;

	risse_int errpos =
		pos == -1 ? Block->GetLexicalAnalyzer()->GetCurrentPosition(): pos;

	str += RISSE_WS(" at ");
	str += Block->GetName();

	str += RISSE_WS(" line ");
	str += ttstr(1+Block->SrcPosToLine(errpos));

	Block->GetRisse()->OutputToConsole(str.c_str());
}
//---------------------------------------------------------------------------

#define RISSE_INC_SIZE 256

#ifdef RISSE_DEBUG_PROFILE_TIME
risse_uint time_PutCode = 0;
#endif

risse_int tRisseInterCodeContext::PutCode(risse_int32 num, risse_int32 pos)
{
	// puts code
	// num = operation code
	// pos = position in the script
	// this returns code address of newly put code
#ifdef RISSE_DEBUG_PROFILE_TIME
	tRisseTimeProfiler prof(time_PutCode);
#endif

	if(CodeAreaSize >= CodeAreaCapa)
	{
		// must inflate the code area
		CodeArea = (risse_int32*)Risse_realloc(CodeArea,
			sizeof(risse_int32)*(CodeAreaCapa + RISSE_INC_SIZE));
		if(!CodeArea) Risse_eRisseScriptError(RisseInsufficientMem, Block, pos);
		CodeAreaCapa += RISSE_INC_SIZE;
	}

	if(pos!=-1)
	{
		if(PrevSourcePos != pos)
		{
			PrevSourcePos = pos;
			SourcePosArraySorted = false;
			if(!SourcePosArray)
			{
				SourcePosArray = (tSourcePos*)Risse_malloc(RISSE_INC_SIZE *
					sizeof(tSourcePos));
				if(!SourcePosArray) _yyerror(RisseInsufficientMem, Block);
				SourcePosArrayCapa = RISSE_INC_SIZE;
				SourcePosArraySize = 0;
			}
			if(SourcePosArraySize >= SourcePosArrayCapa)
			{
				SourcePosArray = (tSourcePos*)Risse_realloc(SourcePosArray,
					(SourcePosArrayCapa + RISSE_INC_SIZE) * sizeof(tSourcePos));
				if(!SourcePosArray) Risse_eRisseScriptError(RisseInsufficientMem, Block, pos);
				SourcePosArrayCapa += RISSE_INC_SIZE;
			}
			SourcePosArray[SourcePosArraySize].CodePos = CodeAreaSize;
			SourcePosArray[SourcePosArraySize].SourcePos = pos;
			SourcePosArraySize++;
		}
	}

	CodeArea[CodeAreaSize] = num;

	return CodeAreaSize++;
}
//---------------------------------------------------------------------------
#ifdef RISSE_DEBUG_PROFILE_TIME
risse_uint time_PutData = 0;
#endif

risse_int tRisseInterCodeContext::PutData(const tRisseVariant &val)
{
	// puts data
	// val = data
	// return the data address
#ifdef RISSE_DEBUG_PROFILE_TIME
	tRisseTimeProfiler prof(time_PutData);
#endif

	if(_DataAreaSize >= _DataAreaCapa)
	{
		// inflation of data area
		_DataArea = (tRisseVariant**)Risse_realloc(_DataArea,
			sizeof(tRisseVariant*)*(_DataAreaCapa + RISSE_INC_SIZE));
		if(!_DataArea) Risse_eRisseScriptError(RisseInsufficientMem, Block, LEX_POS);
		_DataAreaCapa += RISSE_INC_SIZE;
	}

	// search same data in the area
	if(_DataAreaSize)
	{
		tRisseVariant **ptr = _DataArea + _DataAreaSize-1;
		risse_int count = 0;
		while(count < 20 // is waste of time if it exceeds 20 limit?
			)
		{
			if((*ptr)->DiscernCompareStrictReal(val))
			{
				return ptr - _DataArea; // re-use this
			}
			count ++;
			if(ptr == _DataArea) break;
			ptr --;
		}
	}

	tRisseVariant *v;
	if(val.Type() == tvtString)
	{
		// check whether the string can be shared
		v = new tRisseVariant(RisseMapGlobalStringMap(val));
	}
	else
	{
		v = new tRisseVariant(val);
	}
	_DataArea[_DataAreaSize] = v;

	return _DataAreaSize++;
}
//---------------------------------------------------------------------------
int tRisseInterCodeContext::tSourcePos::
	SortFunction(const void *a, const void *b)
{
	const tSourcePos *aa = (const tSourcePos*)a;
	const tSourcePos *bb = (const tSourcePos*)b;
	return aa->CodePos - bb->CodePos;
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::SortSourcePos()
{
	if(!SourcePosArraySorted)
	{
		qsort(SourcePosArray, SourcePosArraySize, sizeof(tSourcePos),
			tSourcePos::SortFunction);
		SourcePosArraySorted = true;
	}
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::FixCode(void)
{
	// code re-positioning and patch processing
	// TODO: tRisseInterCodeContext::FixCode fasten the algorithm

	// create 'regmember' instruction to register class members to 
	// newly created object
	if(ContextType == ctClass)
	{
		// generate a code
		risse_int32 * code = new risse_int32[1];
		code[0] = VM_REGMEMBER;

		// make a patch information
		// use FunctionRegisterCodePoint for insertion point
		FixList.push_back(tFixData(FunctionRegisterCodePoint, 0, 1, code, true));
	}

	// process funtion reservation to enable backward reference of
	// global/method functions
	if(NonLocalFunctionDeclVector.size() >= 1)
	{
		if(MaxFrameCount < 1) MaxFrameCount = 1;

		std::vector<tNonLocalFunctionDecl>::iterator func;

		// make function registration code to objthis

		// compute codesize
		risse_int codesize = 2;
		for(func = NonLocalFunctionDeclVector.begin();
			func!= NonLocalFunctionDeclVector.end();
			func++)
		{
			if(func->ChangeThis) codesize += 10; else codesize += 7;
		}

		risse_int32 *code = new risse_int32[codesize];

		// generate code
		risse_int32 *codep = code;
		for(func = NonLocalFunctionDeclVector.begin();
			func!= NonLocalFunctionDeclVector.end();
			func++)
		{
			// const %1, #funcdata
			*(codep++) = VM_CONST;
			*(codep++) = RISSE_TO_VM_REG_ADDR(1);
			*(codep++) = RISSE_TO_VM_REG_ADDR(func->DataPos);

			// chgthis %1, %-1
			if(func->ChangeThis)
			{
				*(codep++) = VM_CHGTHIS;
				*(codep++) = RISSE_TO_VM_REG_ADDR(1);
				*(codep++) = RISSE_TO_VM_REG_ADDR(-1);
			}

			// spds %-1.#funcname, %1
			*(codep++) = VM_SPDS;
			*(codep++) = RISSE_TO_VM_REG_ADDR(-1); // -1 =  objthis
			*(codep++) = RISSE_TO_VM_REG_ADDR(func->NameDataPos);
			*(codep++) = RISSE_TO_VM_REG_ADDR(1);
		}

		// cl %1
		*(codep++) = VM_CL;
		*(codep++) = RISSE_TO_VM_REG_ADDR(1);

		// make a patch information
		FixList.push_back(tFixData(FunctionRegisterCodePoint, 0, codesize, code, true));

		NonLocalFunctionDeclVector.clear();
	}

	// sort SourcePosVector
	SortSourcePos();

	// re-position patch
	std::list<tFixData>::iterator fix;

	for(fix = FixList.begin(); fix!=FixList.end(); fix++)
	{
		std::list<risse_int>::iterator jmp;
		for(jmp = JumpList.begin(); jmp!=JumpList.end(); jmp++)
		{
			risse_int jmptarget = CodeArea[*jmp + 1] + *jmp;
			if(*jmp >= fix->StartIP && *jmp < fix->Size + fix->StartIP)
			{
				// jmp is in the re-positioning target -> delete
				jmp = JumpList.erase(jmp);
			}
			else if(fix->BeforeInsertion?
				(jmptarget < fix->StartIP):(jmptarget <= fix->StartIP)
				&& *jmp > fix->StartIP + fix->Size ||
				*jmp < fix->StartIP && jmptarget >= fix->StartIP + fix->Size)
			{
				// jmp and its jumping-target is in the re-positioning target
				CodeArea[*jmp + 1] += fix->NewSize - fix->Size;
			}

			if(*jmp >= fix->StartIP + fix->Size)
			{
				// fix up jmp
				*jmp += fix->NewSize - fix->Size;
			}
		}

		// move the code
		if(fix->NewSize > fix->Size)
		{
			// when code inflates on fixing
			CodeArea = (risse_int32*)Risse_realloc(CodeArea,
				sizeof(risse_int32)*(CodeAreaSize + (fix->NewSize - fix->Size)));
			if(!CodeArea) Risse_eRisseScriptError(RisseInsufficientMem, Block, 0);
		}

		if(CodeAreaSize - (fix->StartIP + fix->Size) > 0)
		{
			// move the existing code
			memmove(CodeArea + fix->StartIP + fix->NewSize,
				CodeArea + fix->StartIP + fix->Size,
				sizeof(risse_int32)* (CodeAreaSize - (fix->StartIP + fix->Size)));

			// move sourcepos
			for(risse_int i = 0; i < SourcePosArraySize; i++)
			{
				if(SourcePosArray[i].CodePos >= fix->StartIP + fix->Size)
					SourcePosArray[i].CodePos += fix->NewSize - fix->Size;
			}
		}

		if(fix->NewSize && fix->Code)
		{
			// copy the new code
			memcpy(CodeArea + fix->StartIP, fix->Code, sizeof(risse_int32)*fix->NewSize);
		}

		CodeAreaSize += fix->NewSize - fix->Size;
	}

	// eliminate redundant jump codes
	for(std::list<risse_int>::iterator jmp = JumpList.begin();
		jmp!=JumpList.end(); jmp++)
	{
		risse_int32 jumptarget = CodeArea[*jmp + 1] + *jmp;
		risse_int32 jumpcode = CodeArea[*jmp];
		risse_int addr = *jmp;
		addr += CodeArea[addr + 1];
		for(;;)
		{
			if(CodeArea[addr] == VM_JMP ||
				CodeArea[addr] == jumpcode)
			{
				// simple jump code or
				// JF after JF or JNF after JNF
				jumptarget = CodeArea[addr + 1] + addr; // skip jump after jump
				if(CodeArea[addr + 1] != 0)
					addr += CodeArea[addr + 1];
				else
					break; // must be an error
			}
			else if(CodeArea[addr] == VM_JF && jumpcode == VM_JNF ||
				CodeArea[addr] == VM_JNF && jumpcode == VM_JF)
			{
				// JF after JNF or JNF after JF
				jumptarget = addr + 2;
					// jump code after jump will not jump
				addr += 2;
			}
			else
			{
				// other codes
				break;
			}
		}
		CodeArea[*jmp + 1] = jumptarget - *jmp;
	}

	// convert jump addresses to VM address
	for(std::list<risse_int>::iterator jmp = JumpList.begin();
		jmp!=JumpList.end(); jmp++)
	{
		CodeArea[*jmp + 1] = RISSE_TO_VM_CODE_ADDR(CodeArea[*jmp + 1]);
	}

	JumpList.clear();
	FixList.clear();

}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::RegisterFunction()
{
	// registration of function to the parent's context

	if(!Parent) return;

	if(ContextType == ctPropertySetter)
	{
		Parent->PropSetter = this; AddRef();
		return;
	}
	if(ContextType == ctPropertyGetter)
	{
		Parent->PropGetter = this; AddRef();
		return;
	}

	if(ContextType == ctSuperClassGetter)
	{
		return; // these are already registered to parent context
	}


	if( ContextType != ctFunction &&  // ctExprFunction is not concerned here
		ContextType != ctProperty &&
		ContextType != ctClass)
	{
		return;
	}


	risse_int data = -1;

	if(Parent->ContextType == ctTopLevel)
	{
		tRisseVariant val;
		val = this;
		data = Parent->PutData(val);
		val = Name;
		risse_int name = Parent->PutData(val);
		bool changethis = ContextType == ctFunction ||
							ContextType == ctProperty;
		Parent->NonLocalFunctionDeclVector.push_back(
			tNonLocalFunctionDecl(data, name, changethis));
	}

	if(ContextType == ctFunction && Parent->ContextType == ctFunction)
	{
		// local functions
		// adds the function as a parent's local variable
		if(data == -1)
		{
			tRisseVariant val;
			val = this;
			data = Parent->PutData(val);
		}

		Parent->InitLocalFunction(Name, data);

	}

	if( Parent->ContextType == ctFunction ||
		Parent->ContextType == ctClass)
	{
		// register members to the parent object
		tRisseVariant val = this;
		Parent->PropSet(RISSE_MEMBERENSURE|RISSE_IGNOREPROP, Name, NULL, &val, Parent);
	}

}
//---------------------------------------------------------------------------
#ifdef RISSE_DEBUG_PROFILE_TIME
risse_uint time_Commit = 0;
#endif

void tRisseInterCodeContext::Commit()
{
#ifdef RISSE_DEBUG_PROFILE_TIME
	tRisseTimeProfiler prof(time_Commit);
#endif

	// some context-related processing at final, and commits it
	if(ContextType == ctClass)
	{
		// clean up super class proxy
		if(SuperClassGetter) SuperClassGetter->Commit();
	}

	if(ContextType != ctProperty && ContextType != ctSuperClassGetter)
	{
		PutCode(VM_SRV, LEX_POS);
		PutCode(RISSE_TO_VM_REG_ADDR(0));
		PutCode(VM_RET);
	}

	RegisterFunction();

	if(ContextType != ctProperty && ContextType != ctSuperClassGetter) FixCode();

	if(!DataArea)
	{
		DataArea = new tRisseVariant[_DataAreaSize];
		DataAreaSize = _DataAreaSize;

		for(risse_int i = 0; i<_DataAreaSize; i++)
		{
			DataArea[i].CopyRef( *_DataArea[i]);
		}

		if(_DataArea)
		{
			for(risse_int i = 0; i<_DataAreaSize; i++) delete _DataArea[i];
			Risse_free(_DataArea);
			_DataArea = NULL;
		}
	}

	if(ContextType == ctSuperClassGetter)
		MaxVariableCount = 2; // always 2
	else
		MaxVariableCount = Namespace.GetMaxCount();

	SuperClassExpr = NULL;

	ClearNodesToDelete();

	// compact SourcePosArray to just size
	if(SourcePosArraySize && SourcePosArray)
	{
		SourcePosArray = (tSourcePos*)Risse_realloc(SourcePosArray,
			SourcePosArraySize * sizeof(tSourcePos));
		if(!SourcePosArray) Risse_eRisseScriptError(RisseInsufficientMem, Block, 0);
		SourcePosArrayCapa = SourcePosArraySize;
	}

	// compact CodeArea to just size
	if(CodeAreaSize && CodeArea)
	{
		// must inflate the code area
		CodeArea = (risse_int32*)Risse_realloc(CodeArea,
			sizeof(risse_int32)*CodeAreaSize);
		if(!CodeArea) Risse_eRisseScriptError(RisseInsufficientMem, Block, 0);
		CodeAreaCapa = CodeAreaSize;
	}


	// set object type info for debugging
	if(RisseObjectHashMapEnabled())
		RisseObjectHashSetType(this, GetShortDescriptionWithClassName());


	// we do thus nasty thing because the std::vector does not free its storage
	// even we call 'clear' method...
#define RE_CREATE(place, type, classname) (&place)->type::~classname(); \
	new (&place) type ();

	RE_CREATE(NodeToDeleteVector, std::vector<tRisseExprNode *>, vector);
	RE_CREATE(CurrentNodeVector, std::vector<tRisseExprNode *>, vector);
	RE_CREATE(FuncArgStack, std::stack<tFuncArg>, stack);
	RE_CREATE(ArrayArgStack, std::stack<tArrayArg>, stack);
	RE_CREATE(NestVector, std::vector<tNestData>, vector);
	RE_CREATE(JumpList, std::list<risse_int>, list);
	RE_CREATE(FixList, std::list<tFixData>, list);
	RE_CREATE(NonLocalFunctionDeclVector, std::vector<tNonLocalFunctionDecl>, vector);
}
//---------------------------------------------------------------------------
risse_int tRisseInterCodeContext::CodePosToSrcPos(risse_int codepos) const
{
	// converts from
	// CodeArea oriented position to source oriented position
	if(!SourcePosArray) return 0;

	const_cast<tRisseInterCodeContext*>(this)->SortSourcePos();

	risse_uint s = 0;
	risse_uint e = SourcePosArraySize;
	if(e==0) return 0;
	while(true)
	{
		if(e-s <= 1) return SourcePosArray[s].SourcePos;
		risse_uint m = s + (e-s)/2;
		if(SourcePosArray[m].CodePos > codepos)
			e = m;
		else
			s = m;
	}
}
//---------------------------------------------------------------------------
risse_int tRisseInterCodeContext::FindSrcLineStartCodePos(risse_int codepos) const
{
	// find code address which is the first instruction of the source line
	if(!SourcePosArray) return 0;

	risse_int srcpos = CodePosToSrcPos(codepos);
	risse_int line = Block->SrcPosToLine(srcpos);
	srcpos = Block->LineToSrcPos(line);

	risse_int codeposmin = -1;
	for(risse_int i = 0; i < SourcePosArraySize; i++)
	{
		if(SourcePosArray[i].SourcePos >= srcpos)
		{
			if(codeposmin == -1 || SourcePosArray[i].CodePos< codeposmin)
				codeposmin = SourcePosArray[i].CodePos;
		}
	}
	if(codeposmin < 0) codeposmin = 0;
	return codeposmin;
}
//---------------------------------------------------------------------------
ttstr tRisseInterCodeContext::GetPositionDescriptionString(risse_int codepos) const
{
	return Block->GetLineDescriptionString(CodePosToSrcPos(codepos)) +
		RISSE_WS1("[") + GetShortDescription() + RISSE_WS2("]");
}
//---------------------------------------------------------------------------
static bool inline RisseIsModifySubType(tRisseSubType type)
	{ return type != stNone; }
static bool inline RisseIsCondFlagRetValue(risse_int r)
	{ return r == RISSE_GNC_CFLAG || r == RISSE_GNC_CFLAG_I; }
#ifdef RISSE_DEBUG_PROFILE_TIME
risse_uint time_this_proxy = 0;
#endif

risse_int tRisseInterCodeContext::GenNodeCode(risse_int & frame, tRisseExprNode *node,
	risse_uint32 restype, risse_int reqresaddr, const tSubParam  & param)
{
	// code generation of a given node

	// frame = register stack frame
	// node = target node
	// restype = required result type
	// reqresaddr = variable address which should receives the result
	//              ( currently not used )
	// param = additional parameters
	// returns: a register address that contains the result ( RISSE_GNC_CFLAG
	//          for condition flags )

	risse_int resaddr;

	risse_int node_pos = NODE_POS;

	switch(node->GetOpecode())
	{
	case T_CONSTVAL: // constant value
	  {
		// a code that refers the constant value
		if(RisseIsModifySubType(param.SubType)) _yyerror(RisseCannotModifyLHS, Block);
		if(!(restype & RISSE_RT_NEEDED)) return 0; // why here is called without a result necessity? ;-)
		risse_int dp = PutData(node->GetValue());
		PutCode(VM_CONST, node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(frame), node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(dp), node_pos);
		return frame++;
	  }

	case T_IF: // 'if' operator
	  {
		// "if" operator
		// evaluate right node. then evaluate left node if the right results true.
		if(restype & RISSE_RT_NEEDED) _yyerror(RisseCannotGetResult, Block);
//		if(RisseIsModifySubType(param.SubType)) _yyerror(RisseCannotModifyLHS, Block);
		risse_int resaddr = _GenNodeCode(frame, (*node)[1], RISSE_RT_NEEDED|RISSE_RT_CFLAG,
			0, tSubParam());
		bool inv = false;
		if(!RisseIsCondFlagRetValue(resaddr))
		{
			PutCode(VM_TT, node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);
		}
		else
		{
			if(resaddr == RISSE_GNC_CFLAG_I) inv = true;
		}
		risse_int addr = CodeAreaSize;
		AddJumpList();
		PutCode(inv?VM_JF:VM_JNF, node_pos);
		PutCode(0, node_pos); // *
		_GenNodeCode(frame, (*node)[0], 0, 0, param);
		CodeArea[addr + 1] = CodeAreaSize - addr; //  patch "*"
		return 0;
	  }

	case T_INCONTEXTOF: // 'incontextof' operator
	  {
		// "incontextof" operator
		// a special operator that changes objeect closure's context
		if(!(restype & RISSE_RT_NEEDED)) return 0;
		risse_int resaddr1, resaddr2;
		resaddr1 = _GenNodeCode(frame, (*node)[0], RISSE_RT_NEEDED, 0, param);
		resaddr2 = _GenNodeCode(frame, (*node)[1], RISSE_RT_NEEDED, 0, tSubParam());
		if(resaddr1 < 0)
		{
			PutCode(VM_CP, node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(frame), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(resaddr1), node_pos);
			resaddr1 = frame;
			frame++;
		}
		PutCode(VM_CHGTHIS, node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(resaddr1), node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(resaddr2), node_pos);

		return resaddr1;
	  }

	case T_COMMA: // ',' operator
		// comma operator
		_GenNodeCode(frame, (*node)[0], 0, 0, tSubParam());
		return _GenNodeCode(frame, (*node)[1], restype, reqresaddr, param);


	case T_SWAP: // '<->' operator
	  {
		// swap operator
		if(restype & RISSE_RT_NEEDED) _yyerror(RisseCannotGetResult, Block);
		if(param.SubType) _yyerror(RisseCannotModifyLHS, Block);

		risse_int resaddr1 = _GenNodeCode(frame, (*node)[0], RISSE_RT_NEEDED, 0,
			tSubParam());

		if(resaddr1 < 0)
		{
			PutCode(VM_CP, node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(frame), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(resaddr1), node_pos);
			resaddr1 = frame;
			frame++;
		}

		risse_int resaddr2 = _GenNodeCode(frame, (*node)[1], RISSE_RT_NEEDED, 0,
			tSubParam());

		// create substitutions
		tSubParam param2;
		param2.SubType = stEqual;
		param2.SubAddress = resaddr2;
		_GenNodeCode(frame, (*node)[0], 0, 0, param2);

		param2.SubType = stEqual;
		param2.SubAddress = resaddr1;
		_GenNodeCode(frame, (*node)[1], 0, 0, param2);

		return 0;
	  }

	case T_EQUAL: // '=' operator
	  {
		// simple substitution
		if(param.SubType) _yyerror(RisseCannotModifyLHS, Block);

		if(restype & RISSE_RT_CFLAG)
		{
			// '=' operator in boolean context
			OutputWarning(RisseSubstitutionInBooleanContext, node_pos);
		}

		resaddr = _GenNodeCode(frame, (*node)[1], RISSE_RT_NEEDED, 0, param);

		tSubParam param2;
		param2.SubType = stEqual;
		param2.SubAddress = resaddr;
		_GenNodeCode(frame, (*node)[0], 0, 0, param2);
		return resaddr;
	  }

	case T_AMPERSANDEQUAL:		// '&=' operator
	case T_VERTLINEEQUAL:		// '|=' operator
	case T_CHEVRONEQUAL:		// '^=' operator
	case T_MINUSEQUAL:			// ^-=' operator
	case T_PLUSEQUAL:			// '+=' operator
	case T_PERCENTEQUAL:		// '%=' operator
	case T_SLASHEQUAL:			// '/=' operator
	case T_BACKSLASHEQUAL:		// '\=' operator
	case T_ASTERISKEQUAL:		// '*=' operator
	case T_LOGICALOREQUAL:		// '||=' operator
	case T_LOGICALANDEQUAL:		// '&&=' operator
	case T_RARITHSHIFTEQUAL:	// '>>=' operator
	case T_LARITHSHIFTEQUAL:	// '<<=' operator
	case T_RBITSHIFTEQUAL:		// '>>>=' operator
	  {
		// operation and substitution operators like "&="
		if(param.SubType) _yyerror(RisseCannotModifyLHS, Block);
		resaddr = _GenNodeCode(frame, (*node)[1], RISSE_RT_NEEDED, 0, tSubParam());

		tSubParam param2;
		switch(node->GetOpecode()) // this may be sucking...
		{
		case T_AMPERSANDEQUAL:		param2.SubType = stBitAND;		break;
		case T_VERTLINEEQUAL:		param2.SubType = stBitOR;		break;
		case T_CHEVRONEQUAL:		param2.SubType = stBitXOR;		break;
		case T_MINUSEQUAL:			param2.SubType = stSub;			break;
		case T_PLUSEQUAL:			param2.SubType = stAdd;			break;
		case T_PERCENTEQUAL:		param2.SubType = stMod;			break;
		case T_SLASHEQUAL:			param2.SubType = stDiv;			break;
		case T_BACKSLASHEQUAL:		param2.SubType = stIDiv;		break;
		case T_ASTERISKEQUAL:		param2.SubType = stMul;			break;
		case T_LOGICALOREQUAL:		param2.SubType = stLogOR;		break;
		case T_LOGICALANDEQUAL:		param2.SubType = stLogAND;		break;
		case T_RARITHSHIFTEQUAL:	param2.SubType = stSAR;			break;
		case T_LARITHSHIFTEQUAL:	param2.SubType = stSAL;			break;
		case T_RBITSHIFTEQUAL:		param2.SubType = stSR;			break;
		}
		param2.SubAddress = resaddr;
		return _GenNodeCode(frame, (*node)[0], restype, reqresaddr, param2);
	  }

	case T_QUESTION: // '?' ':' operator
	  {
		// three-term operator ( ?  :  )
		risse_int resaddr1, resaddr2;
		int frame1, frame2;
		resaddr = _GenNodeCode(frame, (*node)[0], RISSE_RT_NEEDED|RISSE_RT_CFLAG, 0, tSubParam());
		bool inv = false;
		if(!RisseIsCondFlagRetValue(resaddr))
		{
			PutCode(VM_TT, node_pos);    // tt resaddr
			PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);
		}
		else
		{
			if(resaddr == RISSE_GNC_CFLAG_I) inv = true;
		}

		risse_int cur_frame = frame;
		risse_int addr1 = CodeAreaSize;
		AddJumpList();
		PutCode(inv?VM_JF:VM_JNF, node_pos);
		PutCode(0, node_pos); // patch

		resaddr1 = _GenNodeCode(frame, (*node)[1], restype, reqresaddr, param);

		if(restype & RISSE_RT_CFLAG)
		{
			// condition flag required
			if(!RisseIsCondFlagRetValue(resaddr1))
			{
				PutCode(VM_TT, node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(resaddr1));
			}
			else
			{
				if(resaddr1 == RISSE_GNC_CFLAG_I)
					PutCode(VM_NF, node_pos); // invert flag
			}
		}
		else
		{
			if((restype & RISSE_RT_NEEDED) &&
					!RisseIsCondFlagRetValue(resaddr1) && resaddr1 < 0)
			{
				PutCode(VM_CP, node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(frame), node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(resaddr1), node_pos);
				resaddr1 = frame;
				frame++;
			}
		}
		frame1 = frame;

		risse_int addr2 = CodeAreaSize;
		AddJumpList();
		PutCode(VM_JMP, node_pos);
		PutCode(0, node_pos); // patch
		CodeArea[addr1+1] = CodeAreaSize - addr1;
		frame = cur_frame;
		resaddr2 = _GenNodeCode(frame, (*node)[2], restype, reqresaddr, param);

		if(restype & RISSE_RT_CFLAG)
		{
			// condition flag required
			if(!RisseIsCondFlagRetValue(resaddr2))
			{
				PutCode(VM_TT, node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(resaddr2));
			}
			else
			{
				if(resaddr2 == RISSE_GNC_CFLAG_I)
					PutCode(VM_NF, node_pos); // invert flag
			}
		}
		else
		{
			if((restype & RISSE_RT_NEEDED) &&
					!RisseIsCondFlagRetValue(resaddr1) && resaddr1 != resaddr2)
			{
				PutCode(VM_CP, node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(resaddr1), node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(resaddr2), node_pos);
				frame++;
			}
		}
		frame2 = frame;

	  	CodeArea[addr2+1] = CodeAreaSize - addr2;
		frame = frame2 < frame1 ? frame1 : frame2;
		return (restype & RISSE_RT_CFLAG)?RISSE_GNC_CFLAG: resaddr1;
	  }

	case T_LOGICALOR: // '||' operator
	case T_LOGICALAND: // '&&' operator
	  {
		// "logical or" and "logical and"
		// these process with the "shortcut" :
		// OR  : does not evaluate right when left results true
		// AND : does not evaluate right when left results false
		if(param.SubType) _yyerror(RisseCannotModifyLHS, Block);
		risse_int resaddr1, resaddr2;
		resaddr1 = _GenNodeCode(frame, (*node)[0], RISSE_RT_NEEDED|RISSE_RT_CFLAG,
			0, tSubParam());
		bool inv = false;
		if(!RisseIsCondFlagRetValue(resaddr1))
		{
			PutCode(VM_TT, node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(resaddr1), node_pos);
		}
		if(resaddr1 == RISSE_GNC_CFLAG_I) inv = true;
		risse_int addr1 = CodeAreaSize;
		AddJumpList();
		PutCode(node->GetOpecode() == T_LOGICALOR ?
			(inv?VM_JNF:VM_JF) : (inv?VM_JF:VM_JNF), node_pos);
		PutCode(0, node_pos); // *A
		resaddr2 = _GenNodeCode(frame, (*node)[1], RISSE_RT_NEEDED|RISSE_RT_CFLAG,
			0, tSubParam());
		if(!RisseIsCondFlagRetValue(resaddr2))
		{
			PutCode(inv?VM_TF:VM_TT, node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(resaddr2), node_pos);
		}
		else
		{
			if((inv != false) != (resaddr2==RISSE_GNC_CFLAG_I))
				PutCode(VM_NF, node_pos); // invert flag
		}
		CodeArea[addr1 + 1] = CodeAreaSize - addr1; // patch *A
		if(!(restype & RISSE_RT_CFLAG))
		{
			// requested result type is not condition flag
			if(RisseIsCondFlagRetValue(resaddr1) || resaddr1 < 0)
			{
				PutCode(inv?VM_SETNF:VM_SETF, node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(frame), node_pos);
				resaddr1 = frame;
				frame++;
			}
			else
			{
				PutCode(inv?VM_SETNF:VM_SETF, node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(resaddr1), node_pos);
			}
		}
		return (restype & RISSE_RT_CFLAG) ?
			(inv?RISSE_GNC_CFLAG_I:RISSE_GNC_CFLAG) : resaddr1;
	  }

	case T_INSTANCEOF: // 'instanceof' operator
	  {
		// instanceof operator
		risse_int resaddr1, resaddr2;
		resaddr1 = _GenNodeCode(frame, (*node)[0], RISSE_RT_NEEDED, 0, tSubParam());
		if(resaddr1 < 0)
		{
			PutCode(VM_CP, node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(frame), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(resaddr1), node_pos);
			resaddr1 = frame;
			frame++;
		}
		resaddr2 = _GenNodeCode(frame, (*node)[1], RISSE_RT_NEEDED, 0, tSubParam());
		PutCode(VM_CHKINS, node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(resaddr1), node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(resaddr2), node_pos);
        return resaddr1;
	  }

	case T_VERTLINE:	// '|' operator
	case T_CHEVRON:		// '^' operator
	case T_AMPERSAND:	// binary '&' operator
	case T_RARITHSHIFT:	// '>>' operator
	case T_LARITHSHIFT:	// '<<' operator
	case T_RBITSHIFT:	// '>>>' operator
	case T_PLUS:		// binary '+' operator
	case T_MINUS:		// '-' operator
	case T_PERCENT:		// '%' operator
	case T_SLASH:		// '/' operator
	case T_BACKSLASH:	// '\' operator
	case T_ASTERISK:	// binary '*' operator
	  {
		// general two-term operators
		risse_int resaddr1, resaddr2;
		if(RisseIsModifySubType(param.SubType)) _yyerror(RisseCannotModifyLHS, Block);
		resaddr1 = _GenNodeCode(frame, (*node)[0], RISSE_RT_NEEDED, 0, tSubParam());
		if(resaddr1 < 0)
		{
			PutCode(VM_CP, node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(frame), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(resaddr1), node_pos);
			resaddr1 = frame;
			frame++;
		}
		resaddr2 = _GenNodeCode(frame, (*node)[1], RISSE_RT_NEEDED, 0, tSubParam());
		risse_int32 code;
		switch(node->GetOpecode())  // sucking....
		{
		case T_VERTLINE:		code = VM_BOR;		break;
		case T_CHEVRON:			code = VM_BXOR;		break;
		case T_AMPERSAND:		code = VM_BAND;		break;
		case T_RARITHSHIFT:		code = VM_SAR;		break;
		case T_LARITHSHIFT:		code = VM_SAL;		break;
		case T_RBITSHIFT:		code = VM_SR;		break;
		case T_PLUS:			code = VM_ADD;		break;
		case T_MINUS:			code = VM_SUB;		break;
		case T_PERCENT:			code = VM_MOD;		break;
		case T_SLASH:			code = VM_DIV;		break;
		case T_BACKSLASH:		code = VM_IDIV;		break;
		case T_ASTERISK:		code = VM_MUL;		break;
		}

		PutCode(code, node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(resaddr1), node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(resaddr2), node_pos);
		return resaddr1;
	  }

	case T_NOTEQUAL:		// '!=' operator
	case T_EQUALEQUAL:		// '==' operator
	case T_DISCNOTEQUAL:	// '!==' operator
	case T_DISCEQUAL:		// '===' operator
	case T_LT:				// '<' operator
	case T_GT:				// '>' operator
	case T_LTOREQUAL:		// '<=' operator
	case T_GTOREQUAL:		// '>=' operator
	  {
		// comparison operators
		risse_int resaddr1, resaddr2;
		if(RisseIsModifySubType(param.SubType)) _yyerror(RisseCannotModifyLHS, Block);
		resaddr1 = _GenNodeCode(frame, (*node)[0], RISSE_RT_NEEDED, 0, tSubParam());
		if(!(restype & RISSE_RT_CFLAG))
		{
			if(resaddr1 < 0)
			{
				PutCode(VM_CP, node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(frame), node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(resaddr1), node_pos);
				resaddr1 = frame;
				frame++;
			}
		}
		resaddr2 = _GenNodeCode(frame, (*node)[1], RISSE_RT_NEEDED, 0, tSubParam());
		risse_int32 code1, code2;
		switch(node->GetOpecode())  // ...
		{
		case T_NOTEQUAL:		code1 = VM_CEQ;		code2 = VM_SETNF; 	break;
		case T_EQUALEQUAL:		code1 = VM_CEQ;		code2 = VM_SETF;	break;
		case T_DISCNOTEQUAL:	code1 = VM_CDEQ;	code2 = VM_SETNF;	break;
		case T_DISCEQUAL:		code1 = VM_CDEQ;	code2 = VM_SETF;	break;
		case T_LT:				code1 = VM_CLT;		code2 = VM_SETF;	break;
		case T_GT:				code1 = VM_CGT;		code2 = VM_SETF;	break;
		case T_LTOREQUAL:		code1 = VM_CGT;		code2 = VM_SETNF;	break;
		case T_GTOREQUAL:		code1 = VM_CLT;		code2 = VM_SETNF;	break;
		}

		PutCode(code1, node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(resaddr1), node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(resaddr2), node_pos);

		if(!(restype & RISSE_RT_CFLAG))
		{
			PutCode(code2, node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(resaddr1), node_pos);
		}

		return (restype & RISSE_RT_CFLAG) ?
			(code2 == VM_SETNF?RISSE_GNC_CFLAG_I:RISSE_GNC_CFLAG):resaddr1;
	  }

	case T_EXCRAMATION: // pre-positioned '!' operator
	  {
		// logical not
		if(RisseIsModifySubType(param.SubType)) _yyerror(RisseCannotModifyLHS, Block);
		resaddr = _GenNodeCode(frame, (*node)[0], restype, reqresaddr, tSubParam());
		if(!(restype & RISSE_RT_CFLAG))
		{
			// value as return value required
			if(resaddr < 0)
			{
				PutCode(VM_CP, node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(frame), node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);
				resaddr = frame;
				frame++;
			}
			PutCode(VM_LNOT, node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);
			return resaddr;
		}
		else
		{
			// condifion flag required
			if(!RisseIsCondFlagRetValue(resaddr))
			{
				PutCode(VM_TF, node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(resaddr));
				return RISSE_GNC_CFLAG;
			}

			return resaddr == RISSE_GNC_CFLAG_I ? RISSE_GNC_CFLAG : RISSE_GNC_CFLAG_I;
				// invert flag
		}

	  }

	case T_TILDE:		// '~' operator
	case T_SHARP:		// '#' operator
	case T_DOLLAR:		// '$' operator
	case T_UPLUS:		// unary '+' operator
	case T_UMINUS:		// unary '-' operator
	case T_INVALIDATE:	// 'invalidate' operator
	case T_ISVALID:		// 'isvalid' operator
	case T_EVAL:		// post-positioned '!' operator
	case T_INT:			// 'int' operator
	case T_REAL:		// 'real' operator
	case T_STRING:		// 'string' operator
	case T_OCTET:		// 'octet' operator
	  {
		// general unary operators
		if(RisseIsModifySubType(param.SubType)) _yyerror(RisseCannotModifyLHS, Block);
		resaddr = _GenNodeCode(frame, (*node)[0], RISSE_RT_NEEDED, 0, tSubParam());
		if(resaddr < 0)
		{
			PutCode(VM_CP, node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(frame), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);
			resaddr = frame;
			frame++;
		}
		risse_int32 code;
		switch(node->GetOpecode())
		{
//			case T_EXCRAMATION:		code = VM_LNOT;			break;
			case T_TILDE:			code = VM_BNOT;			break;
			case T_SHARP:			code = VM_ASC;			break;
			case T_DOLLAR:			code = VM_CHR;			break;
			case T_UPLUS:			code = VM_NUM;			break;
			case T_UMINUS:			code = VM_CHS;			break;
			case T_INVALIDATE:		code = VM_INV;			break;
			case T_ISVALID:			code = VM_CHKINV;		break;
			case T_TYPEOF:			code = VM_TYPEOF;		break;
			case T_EVAL:			code = (restype & RISSE_RT_NEEDED)?
										   VM_EVAL:VM_EEXP;

							// warn if T_EVAL is used in non-global position
							if(RisseWarnOnNonGlobalEvalOperator &&
								ContextType != ctTopLevel)
								OutputWarning(RisseWarnEvalOperator);

															break;
			case T_INT:				code = VM_INT;			break;
			case T_REAL:			code = VM_REAL;			break;
			case T_STRING:			code = VM_STR;			break;
			case T_OCTET:			code = VM_OCTET;		break;
		}
		PutCode(code, node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);


		return resaddr;
	  }


	case T_TYPEOF:  // 'typeof' operator
	  {
		// typeof
		if(RisseIsModifySubType(param.SubType)) _yyerror(RisseCannotModifyLHS, Block);
		bool haspropnode;
		tRisseExprNode *cnode = (*node)[0];
		if(cnode->GetOpecode() == T_DOT || cnode->GetOpecode() == T_LBRACKET ||
			cnode->GetOpecode() == T_WITHDOT)
			haspropnode = true;
		else
			haspropnode = false;

		if(haspropnode)
		{
			// has property access node
			tSubParam param2;
			param2.SubType = stTypeOf;
			return _GenNodeCode(frame, cnode, RISSE_RT_NEEDED, 0, param2);
		}
		else
		{
			// normal operation
			resaddr = _GenNodeCode(frame, cnode, RISSE_RT_NEEDED, 0, tSubParam());

			if(resaddr < 0)
			{
				PutCode(VM_CP, node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(frame), node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);
				resaddr = frame;
				frame++;
			}
			PutCode(VM_TYPEOF, node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);
			return resaddr;

		}
	  }

	case T_DELETE:			// 'delete' operator
	case T_INCREMENT:		// pre-positioned '++' operator
	case T_DECREMENT:		// pre-positioned '--' operator
	case T_POSTINCREMENT:	// post-positioned '++' operator
	case T_POSTDECREMENT:	// post-positioned '--' operator
	  {
		// delete, typeof, increment and decrement
		if(RisseIsModifySubType(param.SubType)) _yyerror(RisseCannotModifyLHS, Block);
		tSubParam param2;
		switch(node->GetOpecode())
		{
		case T_TYPEOF:			param2.SubType = stTypeOf;		break;
		case T_DELETE:			param2.SubType = stDelete;		break;
		case T_INCREMENT:		param2.SubType = stPreInc;		break;
		case T_DECREMENT:		param2.SubType = stPreDec;		break;
		case T_POSTINCREMENT:	param2.SubType = stPostInc;		break;
		case T_POSTDECREMENT:	param2.SubType = stPostDec;		break;
		}
//		param2.SubAddress = frame-1;
		return _GenNodeCode(frame, (*node)[0], restype, reqresaddr, param2);
	  }

	case T_LPARENTHESIS:	// '( )' operator
	case T_NEW:				// 'new' operator
	  {
		// function call or create-new object

		// does (*node)[0] have a node that acceesses any properties ?
		bool haspropnode, hasnonlocalsymbol;
		tRisseExprNode *cnode = (*node)[0];
		if(node->GetOpecode() == T_LPARENTHESIS &&
			(cnode->GetOpecode() == T_DOT || cnode->GetOpecode() == T_LBRACKET))
			haspropnode = true;
		else
			haspropnode = false;

		// does (*node)[0] have a node that accesses non-local functions ?
		if(node->GetOpecode() == T_LPARENTHESIS && cnode->GetOpecode() == T_SYMBOL)
		{
			if(AsGlobalContextMode)
			{
				hasnonlocalsymbol = true;
			}
			else
			{
				tRisseVariantString *str = cnode->GetValue().AsString();
				if(Namespace.Find(str->operator const risse_char *()) == -1)
					hasnonlocalsymbol = true;
				else
					hasnonlocalsymbol = false;
				str->Release();
			}
		}
		else
		{
			hasnonlocalsymbol = false;
		}

		// flag which indicates whether to do direct or indirect call access
		bool do_direct_access = haspropnode || hasnonlocalsymbol;

		// reserve frame
		if(!do_direct_access && (restype & RISSE_RT_NEEDED) )
			frame++; // reserve the frame for a result value

		// generate function call codes
		StartFuncArg();
		risse_int framestart = frame;
		risse_int res;
		try
		{
			// arguments is
			if((*node)[1]->GetSize() == 1 && (*(*node)[1])[0] == NULL)
			{
				// empty
			}
			else
			{
				// exist
				_GenNodeCode(frame, (*node)[1], RISSE_RT_NEEDED, 0, tSubParam());
			}

			// compilation of expression that represents the function
			tSubParam param2;


			if(do_direct_access)
			{
				param2.SubType = stFuncCall; // creates code with stFuncCall
				res = _GenNodeCode(frame, (*node)[0], restype, reqresaddr, param2);
			}
			else
			{
				param2.SubType = stNone;
				resaddr = _GenNodeCode(frame, (*node)[0], RISSE_RT_NEEDED, 0, param2);

				// code generatio of function calling
				PutCode(node->GetOpecode() == T_NEW ? VM_NEW: VM_CALL, node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(
					res = (restype & RISSE_RT_NEEDED)?(framestart-1):0),
					node_pos); // result target
				PutCode(RISSE_TO_VM_REG_ADDR(resaddr),
					node_pos); // iRisseDispatch2 that points the function

				// generate argument code
				GenerateFuncCallArgCode();

				// clears the frame
				ClearFrame(frame, framestart);

			}
		}
		catch(...)
		{
			EndFuncArg();
			throw;
		}

		EndFuncArg();

		return res;
	  }

	case T_ARG:
		// a function argument
		if(node->GetSize() >= 2)
		{
			if((*node)[1]) _GenNodeCode(frame, (*node)[1], RISSE_RT_NEEDED, 0, tSubParam());
		}
		if((*node)[0])
		{
			tRisseExprNode *n = (*node)[0];
			if(n->GetOpecode() == T_EXPANDARG)
			{
				// expanding argument
				if((*n)[0])
					AddFuncArg(_GenNodeCode(
						frame, (*n)[0], RISSE_RT_NEEDED, 0, tSubParam()), fatExpand);
				else
					AddFuncArg(0, fatUnnamedExpand);
			}
			else
			{
				AddFuncArg(_GenNodeCode(
					frame, (*node)[0], RISSE_RT_NEEDED, 0, tSubParam()), fatNormal);
			}
		}
		else
		{
			AddFuncArg(0, fatNormal);
		}
		return 0;

	case T_OMIT:
		// omitting of the function arguments
		AddOmitArg();
        return 0;

	case T_DOT:			// '.' operator
	case T_LBRACKET:	// '[ ]' operator
	  {
		// member access ( direct or indirect )
		bool direct = node->GetOpecode() == T_DOT;
		risse_int dp;

		tSubParam param2;
		param2.SubType = stNone;
		resaddr = _GenNodeCode(frame, (*node)[0], RISSE_RT_NEEDED, 0, param2);

		if(direct)
			dp = PutData((*node)[1]->GetValue());
		else
			dp = _GenNodeCode(frame, (*node)[1], RISSE_RT_NEEDED, 0, tSubParam());

		switch(param.SubType)
		{
		case stNone:
		case stIgnorePropGet:
			if(param.SubType == stNone)
				PutCode(direct ? VM_GPD : VM_GPI, node_pos);
			else
				PutCode(direct ? VM_GPDS : VM_GPIS, node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(frame), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(dp), node_pos);
			frame++;
			return frame-1;

		case stEqual:
		case stIgnorePropSet:
			if(param.SubType == stEqual)
			{
				if((*node)[0]->GetOpecode() == T_THIS_PROXY)
					PutCode(direct ? VM_SPD : VM_SPI, node_pos);
				else
					PutCode(direct ? VM_SPDE : VM_SPIE, node_pos);
			}
			else
			{
				PutCode(direct ? VM_SPDS : VM_SPIS, node_pos);
			}
			PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(dp), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(param.SubAddress), node_pos);
			return param.SubAddress;

		case stBitAND:
		case stBitOR:
		case stBitXOR:
		case stSub:
		case stAdd:
		case stMod:
		case stDiv:
		case stIDiv:
		case stMul:
		case stLogOR:
		case stLogAND:
		case stSAR:
		case stSAL:
		case stSR:
			PutCode((risse_int32)param.SubType + (direct?1:2), node_pos);
				// here adds 1 or 2 to the ope-code
				// ( see the ope-code's positioning order )
			PutCode(RISSE_TO_VM_REG_ADDR((restype & RISSE_RT_NEEDED) ? frame: 0), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(dp), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(param.SubAddress), node_pos);
			if(restype & RISSE_RT_NEEDED) frame++;
			return (restype & RISSE_RT_NEEDED)?frame-1:0;

		case stPreInc:
		case stPreDec:
			PutCode((param.SubType == stPreInc ? VM_INC : VM_DEC) +
				(direct? 1:2), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR((restype & RISSE_RT_NEEDED) ? frame: 0), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(dp), node_pos);
			if((restype & RISSE_RT_NEEDED)) frame++;
			return (restype & RISSE_RT_NEEDED)?frame-1:0;

		case stPostInc:
		case stPostDec:
		  {
			risse_int retresaddr = 0;
			if(restype & RISSE_RT_NEEDED)
			{
				// need result ...
				PutCode(direct ? VM_GPD : VM_GPI, node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(frame), node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(dp), node_pos);
				retresaddr = frame;
				frame++;
			}
			PutCode((param.SubType == stPostInc ? VM_INC : VM_DEC) +
				(direct? 1:2), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(0), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(dp), node_pos);
			return retresaddr;
		  }
		case stTypeOf:
		  {
			// typeof
			PutCode(direct? VM_TYPEOFD:VM_TYPEOFI, node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR((restype & RISSE_RT_NEEDED) ? frame:0), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(dp), node_pos);
			if(restype & RISSE_RT_NEEDED) frame++;
			return (restype & RISSE_RT_NEEDED)?frame-1:0;
		  }
		case stDelete:
		  {
			// deletion
			PutCode(direct? VM_DELD:VM_DELI, node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR((restype & RISSE_RT_NEEDED) ? frame:0), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR(dp), node_pos);
			if(restype & RISSE_RT_NEEDED) frame++;
			return (restype & RISSE_RT_NEEDED)?frame-1:0;
		  }
		case stFuncCall:
		  {
			// function call
			PutCode(direct ? VM_CALLD:VM_CALLI, node_pos);
			PutCode(RISSE_TO_VM_REG_ADDR((restype & RISSE_RT_NEEDED) ? frame:0),
				node_pos); // result target
			PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos); // the object
			PutCode(RISSE_TO_VM_REG_ADDR(dp), node_pos); // function name

			// generate argument code
			GenerateFuncCallArgCode();

			// extend frame and return
			if(restype & RISSE_RT_NEEDED) frame++;
			return (restype & RISSE_RT_NEEDED)?frame-1:0;
		  }

		default:
			_yyerror(RisseCannotModifyLHS, Block);
			return 0;
		}
	  }


	case T_SYMBOL:	// symbol
	  {
		// accessing to a variable
		risse_int n;
		if(AsGlobalContextMode)
		{
			n = -1; // global mode cannot access local variables
		}
		else
		{
			tRisseVariantString *str = node->GetValue().AsString();
			n = Namespace.Find(str->operator const risse_char *());
			str->Release();
		}

		if(n!=-1)
		{
			bool isstnone = !RisseIsModifySubType(param.SubType);

			if(!isstnone)
			{
				// substitution, or like it
				switch(param.SubType)
				{
				case stEqual:
					PutCode(VM_CP, node_pos);
					PutCode(RISSE_TO_VM_REG_ADDR(-n-VariableReserveCount-1), node_pos);
					PutCode(RISSE_TO_VM_REG_ADDR(param.SubAddress), node_pos);
					break;

				case stBitAND:
				case stBitOR:
				case stBitXOR:
				case stSub:
				case stAdd:
				case stMod:
				case stDiv:
				case stIDiv:
				case stMul:
				case stLogOR:
				case stLogAND:
				case stSAR:
				case stSAL:
				case stSR:
					PutCode(param.SubType, node_pos);
					PutCode(RISSE_TO_VM_REG_ADDR(-n-VariableReserveCount-1), node_pos);
					PutCode(RISSE_TO_VM_REG_ADDR(param.SubAddress), node_pos);
					return (restype & RISSE_RT_NEEDED)?-n-VariableReserveCount-1:0;

				case stPreInc: // pre-positioning
					PutCode(VM_INC, node_pos);
					PutCode(RISSE_TO_VM_REG_ADDR(-n-VariableReserveCount-1), node_pos);
					return (restype & RISSE_RT_NEEDED)?-n-VariableReserveCount-1:0;

				case stPreDec: // pre-
					PutCode(VM_DEC, node_pos);
					PutCode(RISSE_TO_VM_REG_ADDR(-n-VariableReserveCount-1), node_pos);
					return (restype & RISSE_RT_NEEDED)?-n-VariableReserveCount-1:0;

				case stPostInc: // post-
					if(restype & RISSE_RT_NEEDED)
					{
						PutCode(VM_CP, node_pos);
						PutCode(RISSE_TO_VM_REG_ADDR(frame), node_pos);
						PutCode(RISSE_TO_VM_REG_ADDR(-n-VariableReserveCount-1), node_pos);
						frame++;
					}
					PutCode(VM_INC, node_pos);
					PutCode(RISSE_TO_VM_REG_ADDR(-n-VariableReserveCount-1), node_pos);
					return (restype & RISSE_RT_NEEDED)?frame-1:0;

				case stPostDec: // post-
					if(restype & RISSE_RT_NEEDED)
					{
						PutCode(VM_CP, node_pos);
						PutCode(RISSE_TO_VM_REG_ADDR(frame), node_pos);
						PutCode(RISSE_TO_VM_REG_ADDR(-n-VariableReserveCount-1), node_pos);
						frame++;
					}
					PutCode(VM_DEC, node_pos);
					PutCode(RISSE_TO_VM_REG_ADDR(-n-VariableReserveCount-1), node_pos);
					return (restype & RISSE_RT_NEEDED)?frame-1:0;

				case stDelete: // deletion
				  {
#if 0
					PutCode(VM_CL, node_pos);
					PutCode(RISSE_TO_VM_REG_ADDR(-n-VariableReserveCount-1), node_pos);
#endif
					tRisseVariantString *str = node->GetValue().AsString();
					Namespace.Remove(*str);
					str->Release();
					if(restype & RISSE_RT_NEEDED)
					{
						risse_int dp = PutData(tRisseVariant(tTVInteger(true)));
						PutCode(VM_CONST, node_pos);
						PutCode(RISSE_TO_VM_REG_ADDR(frame), node_pos);
						PutCode(RISSE_TO_VM_REG_ADDR(dp), node_pos);
						return frame-1;
					}
					return 0;
				  }
				default:
					_yyerror(RisseCannotModifyLHS, Block);
				}
				return 0;
			}
			else
			{
				// read
				tRisseVariantString *str = node->GetValue().AsString();
//				Namespace.Add(str->operator risse_char *());
				risse_int n = Namespace.Find(str->operator const risse_char *());
				str->Release();
				return -n-VariableReserveCount-1;
			}
		}
		else
		{
			// n==-1 ( indicates the variable is not found in the local  )
			// assume the variable is in "this".
			// make nodes that refer "this" and process it
#ifdef RISSE_DEBUG_PROFILE_TIME
	tRisseTimeProfiler prof(time_this_proxy);
#endif
			tRisseExprNode nodep;
			nodep.SetOpecode(T_DOT);
			nodep.SetPosition(node_pos);
			tRisseExprNode *node1 = new tRisseExprNode;
			NodeToDeleteVector.push_back(node1);
			nodep.Add(node1);
			node1->SetOpecode(AsGlobalContextMode?T_GLOBAL:T_THIS_PROXY);
			node1->SetPosition(node_pos);
			tRisseExprNode *node2 = new tRisseExprNode;
			NodeToDeleteVector.push_back(node2);
			nodep.Add(node2);
			node2->SetOpecode(T_SYMBOL);
			node2->SetPosition(node_pos);
			node2->SetValue(node->GetValue());
			return _GenNodeCode(frame, &nodep, restype, reqresaddr, param);
		}
	  }

	case T_IGNOREPROP: // unary '&' operator
	case T_PROPACCESS: // unary '*' operator
		if(node->GetOpecode() ==
			(RisseUnaryAsteriskIgnoresPropAccess?T_PROPACCESS:T_IGNOREPROP))
		{
			// unary '&' operator
			// substance accessing (ignores property operation)
		  	tSubParam sp = param;
			if(sp.SubType == stNone) sp.SubType = stIgnorePropGet;
			else if(sp.SubType == stEqual) sp.SubType = stIgnorePropSet;
			else _yyerror(RisseCannotModifyLHS, Block);
			return _GenNodeCode(frame, (*node)[0], restype, reqresaddr, sp);
		}
		else
		{
			// unary '*' operator
			// force property access
			resaddr = _GenNodeCode(frame, (*node)[0], RISSE_RT_NEEDED, 0, tSubParam());
			switch(param.SubType)
			{
			case stNone: // read from property object
				PutCode(VM_GETP, node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(frame), node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);
				frame ++;
				return frame - 1;

			case stEqual: // write to property object
				PutCode(VM_SETP, node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(param.SubAddress), node_pos);
				return param.SubAddress;

			case stBitAND:
			case stBitOR:
			case stBitXOR:
			case stSub:
			case stAdd:
			case stMod:
			case stDiv:
			case stIDiv:
			case stMul:
			case stLogOR:
			case stLogAND:
			case stSAR:
			case stSAL:
			case stSR:
				PutCode((risse_int32)param.SubType + 3, node_pos);
					// +3 : property access
					// ( see the ope-code's positioning order )
				PutCode(RISSE_TO_VM_REG_ADDR((restype & RISSE_RT_NEEDED) ? frame: 0), node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(param.SubAddress), node_pos);
				if(restype & RISSE_RT_NEEDED) frame++;
				return (restype & RISSE_RT_NEEDED)?frame-1:0;

			case stPreInc:
			case stPreDec:
				PutCode((param.SubType == stPreInc ? VM_INC : VM_DEC) + 3, node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR((restype & RISSE_RT_NEEDED) ? frame: 0), node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);
				if((restype & RISSE_RT_NEEDED)) frame++;
				return (restype & RISSE_RT_NEEDED)?frame-1:0;

			case stPostInc:
			case stPostDec:
			  {
				risse_int retresaddr = 0;
				if(restype & RISSE_RT_NEEDED)
				{
					// need result ...
					PutCode(VM_GETP, node_pos);
					PutCode(RISSE_TO_VM_REG_ADDR(frame), node_pos);
					PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);
					retresaddr = frame;
					frame++;
				}
				PutCode((param.SubType == stPostInc ? VM_INC : VM_DEC) + 3, node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(0), node_pos);
				PutCode(RISSE_TO_VM_REG_ADDR(resaddr), node_pos);
				return retresaddr;
			  }

			default:
				_yyerror(RisseCannotModifyLHS, Block);
				return 0;
			}
		}


	case T_SUPER: // 'super'
	  {
		// refer super class

		tRisseExprNode * node;
		if(Parent && Parent->ContextType == ctProperty)
		{
			if((node = Parent->Parent->SuperClassExpr) == NULL)
			{
				_yyerror(RisseCannotGetSuper, Block);
				return 0;
			}
		}
		else
		{
			if(!Parent || (node = Parent->SuperClassExpr) == NULL)
			{
				_yyerror(RisseCannotGetSuper, Block);
				return 0;
			}
		}

		AsGlobalContextMode = true;
			// the code must be generated in global context
			
		try
		{
			resaddr = _GenNodeCode(frame, node, restype, reqresaddr, param);
		}
		catch(...)
		{
			AsGlobalContextMode = false;
			throw;
		}

   		AsGlobalContextMode = false;

		return resaddr;
	  }

	case T_THIS:
		if(param.SubType) _yyerror(RisseCannotModifyLHS, Block);
		return -1;

	case T_THIS_PROXY:
		// this-proxy is a special register that points
		// both "objthis" and "global"
		// if refering member is not in "objthis", this-proxy
		// refers "global".
		return -VariableReserveCount;

	case T_WITHDOT: // unary '.' operator
	  {
		// dot operator omitting object name
		tRisseExprNode nodep;
		nodep.SetOpecode(T_DOT);
		nodep.SetPosition(node_pos);
		tRisseExprNode *node1 = new tRisseExprNode;
		NodeToDeleteVector.push_back(node1);
		nodep.Add(node1);
		node1->SetOpecode(T_WITHDOT_PROXY);
		node1->SetPosition(node_pos);
		nodep.Add((*node)[0]);
		return _GenNodeCode(frame, &nodep, restype, reqresaddr, param);
 	  }

	case T_WITHDOT_PROXY:
	  {
		// virtual left side of "." operator which omits object

		// search in NestVector
		risse_int i = NestVector.size() -1;
		for(; i>=0; i--)
		{
			tNestData &data = NestVector[i];
			if(data.Type == ntWith)
			{
				// found
				return data.RefRegister;
			}
		}

		// not found in NestVector ...
	  }

		// NO "break" HERE!!!!!! (pass thru to global)

	case T_GLOBAL:
	  {
		if(param.SubType) _yyerror(RisseCannotModifyLHS, Block);
		if(!(restype & RISSE_RT_NEEDED)) return 0;
		PutCode(VM_GLOBAL, node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(frame), node_pos);
		frame++;
		return frame-1;
	  }

	case T_INLINEARRAY:
	  {
		// inline array

		risse_int arraydp = PutData(tRisseVariant(RISSE_WS("Array")));
		//	global %frame0
		//	gpd %frame1, %frame0 . #arraydp // #arraydp = Array
		risse_int frame0 = frame;
		PutCode(VM_GLOBAL, node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(frame+0), node_pos);
		PutCode(VM_GPD, node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(frame+1), node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(frame+0), node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(arraydp), node_pos);
		//	new %frame0, %frame1()
		PutCode(VM_NEW, node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(frame+0), node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(frame+1), node_pos);
		PutCode(0);  // argument count for "new Array"
		//	const %frame1, #zerodp
		risse_int zerodp = PutData(tRisseVariant(tTVInteger(0)));
		PutCode(VM_CONST, node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(frame+1), node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(zerodp), node_pos);
		frame += 2;

		ArrayArgStack.push(tArrayArg());
		ArrayArgStack.top().Object = frame0;
		ArrayArgStack.top().Counter = frame0 + 1;

		risse_int nodesize = node->GetSize();
		if(node->GetSize() == 1 && (*(*node)[0])[0] == NULL)
		{
			// the element is empty
		}
		else
		{
			for(risse_int i = 0; i<nodesize; i++)
			{
				_GenNodeCode(frame, (*node)[i], RISSE_RT_NEEDED, 0, tSubParam()); // elements
			}
		}

		ArrayArgStack.pop();
		return (restype & RISSE_RT_NEEDED)?(frame0):0;
	  }

	case T_ARRAYARG:
	  {
		// an element of inline array
		risse_int framestart = frame;

		resaddr = (*node)[0]?_GenNodeCode(frame, (*node)[0], RISSE_RT_NEEDED, 0, tSubParam()):0;

		// spis %object.%count, %resaddr
		PutCode(VM_SPIS, node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(ArrayArgStack.top().Object));
		PutCode(RISSE_TO_VM_REG_ADDR(ArrayArgStack.top().Counter));
		PutCode(RISSE_TO_VM_REG_ADDR(resaddr));
		// inc %count
		PutCode(VM_INC);
		PutCode(RISSE_TO_VM_REG_ADDR(ArrayArgStack.top().Counter));

		ClearFrame(frame, framestart);

		return 0;
	  }


	case T_INLINEDIC:
	  {
		// inline dictionary
		risse_int dicdp = PutData(tRisseVariant(RISSE_WS("Dictionary")));
		//	global %frame0
		//	gpd %frame1, %frame0 . #dicdp // #dicdp = Dictionary
		risse_int frame0 = frame;
		PutCode(VM_GLOBAL, node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(frame+0), node_pos);
		PutCode(VM_GPD, node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(frame+1), node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(frame+0), node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(dicdp), node_pos);
		//	new %frame0, %frame1()
		PutCode(VM_NEW, node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(frame+0), node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(frame+1), node_pos);
		PutCode(0);  // argument count for "Dictionary" class
		frame += 2;
		ClearFrame(frame, frame0 + 1);  // clear register at frame+1

		ArrayArgStack.push(tArrayArg());
		ArrayArgStack.top().Object = frame0;

		risse_int nodesize = node->GetSize();
		for(risse_int i = 0; i < nodesize; i++)
		{
			_GenNodeCode(frame, (*node)[i], RISSE_RT_NEEDED, 0, tSubParam()); // element
		}

		ArrayArgStack.pop();
		return (restype & RISSE_RT_NEEDED) ? (frame0): 0;
	  }

	case T_DICELM:
	  {
		// an element of inline dictionary
		risse_int framestart = frame;
		risse_int name;
		risse_int value;
		name = _GenNodeCode(frame, (*node)[0], RISSE_RT_NEEDED, 0, tSubParam());
		value = _GenNodeCode(frame, (*node)[1], RISSE_RT_NEEDED, 0, tSubParam());
		// spis %object.%name, %value
		PutCode(VM_SPIS, node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(ArrayArgStack.top().Object));
		PutCode(RISSE_TO_VM_REG_ADDR(name));
		PutCode(RISSE_TO_VM_REG_ADDR(value));

		ClearFrame(frame, framestart);

		return 0;
	  }

	case T_REGEXP:
	  {
		// constant regular expression
		if(!(restype & RISSE_RT_NEEDED)) return 0;
		risse_int regexpdp = PutData(tRisseVariant(RISSE_WS("RegExp")));
		risse_int patdp = PutData(node->GetValue());
		risse_int compiledp = PutData(tRisseVariant(RISSE_WS("_compile")));
		// global %frame0
		//	gpd %frame1, %frame0 . #regexpdp // #regexpdp = RegExp
		risse_int frame0 = frame;
		PutCode(VM_GLOBAL, node_pos);
		PutCode(RISSE_TO_VM_REG_ADDR(frame));
		PutCode(VM_GPD);
		PutCode(RISSE_TO_VM_REG_ADDR(frame + 1));
		PutCode(RISSE_TO_VM_REG_ADDR(frame));
		PutCode(RISSE_TO_VM_REG_ADDR(regexpdp));
		// const frame2, patdp;
		PutCode(VM_CONST);
		PutCode(RISSE_TO_VM_REG_ADDR(frame + 2));
		PutCode(RISSE_TO_VM_REG_ADDR(patdp));
		// new frame0 , frame1();
		PutCode(VM_NEW);
		PutCode(RISSE_TO_VM_REG_ADDR(frame));
		PutCode(RISSE_TO_VM_REG_ADDR(frame+1));
		PutCode(0);
		// calld 0, frame0 . #compiledp(frame2)
		PutCode(VM_CALLD);
		PutCode(RISSE_TO_VM_REG_ADDR(0));
		PutCode(RISSE_TO_VM_REG_ADDR(frame0));
		PutCode(RISSE_TO_VM_REG_ADDR(compiledp));
		PutCode(1);
		PutCode(RISSE_TO_VM_REG_ADDR(frame+2));
		frame+=3;
		ClearFrame(frame, frame0 + 1);

		return frame0;
	  }

	case T_VOID:
		if(param.SubType) _yyerror(RisseCannotModifyLHS, Block);
		if(!(restype & RISSE_RT_NEEDED)) return 0;
		return 0; // 0 is always void
	}

	return 0;
}
//---------------------------------------------------------------------------
#ifdef RISSE_DEBUG_PROFILE_TIME
risse_uint time_GenNodeCode = 0;
#endif

risse_int tRisseInterCodeContext::_GenNodeCode(risse_int & frame, tRisseExprNode *node,
	risse_uint32 restype, risse_int reqresaddr,
	const tSubParam & param)
{
#ifdef RISSE_DEBUG_PROFILE_TIME
	tRisseTimeProfiler prof(time_GenNodeCode);
#endif
	risse_int res = GenNodeCode(frame, node, restype, reqresaddr, param);
	return res;
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::StartFuncArg()
{
	// notify the start of function arguments
	// create a stack for function arguments
	tFuncArg arg;
	FuncArgStack.push(arg);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::AddFuncArg(const risse_int addr, tRisseFuncArgType type)
{
	// add a function argument
	// addr = register address to add
	FuncArgStack.top().ArgVector.push_back(tFuncArgItem(addr, type));
	if(type == fatExpand || type == fatUnnamedExpand)
		FuncArgStack.top().HasExpand = true; // has expanding node
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::EndFuncArg()
{
	// notify the end of function arguments
	FuncArgStack.pop();
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::AddOmitArg()
{
	// omit of the function arguments
	if(ContextType != ctFunction && ContextType != ctExprFunction)
	{
		_yyerror(RisseCannotOmit, Block);
	}
	FuncArgStack.top().IsOmit = true;
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::DoNestTopExitPatch(void)
{
	// process the ExitPatchList which must be in the top of NextVector
	std::vector<risse_int> & vector = NestVector.back().ExitPatchVector;
	std::vector<risse_int>::iterator i;
	for(i = vector.begin(); i != vector.end(); i++)
	{
		CodeArea[*i +1] = CodeAreaSize - *i;
	}
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::DoContinuePatch(tNestData & nestdata)
{
	// process the ContinuePatchList which must be in the top of NextVector

	std::vector<risse_int> & vector = nestdata.ContinuePatchVector;
	std::vector<risse_int>::iterator i;
	for(i = vector.begin(); i != vector.end(); i++)
	{
		CodeArea[*i +1] = CodeAreaSize - *i;
	}
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::ClearLocalVariable(risse_int top, risse_int bottom)
{
	// clear local variable registers from top-1 to bottom
#if 0
	if(top - bottom >= 3)
	{
		PutCode(VM_CCL); // successive clear instruction
		PutCode(RISSE_TO_VM_REG_ADDR(-(top-1)-VariableReserveCount-1));
		PutCode(top-bottom);
	}
	else
	{
		for(risse_int i = bottom; i<top; i++)
		{
			PutCode(VM_CL);
			PutCode(RISSE_TO_VM_REG_ADDR(-i-VariableReserveCount-1));
		}
	}
#endif
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::ClearFrame(risse_int &frame, risse_int base)
{
	// clear frame registers from "frame-1" to "base"
	// "base" is regaeded as "FrameBase" when "base" had been omitted.
	// "frame" may be changed.

	if(base == -1) base = FrameBase;

	if(frame-1 > MaxFrameCount) MaxFrameCount = frame-1;

	if(frame - base >= 3)
	{
#if 0
		PutCode(VM_CCL);
		PutCode(RISSE_TO_VM_REG_ADDR(base));
		PutCode(frame-base);
#endif
		frame = base;
	}
	else
	{
		while(frame > base)
		{
			frame--;
#if 0
			PutCode(VM_CL);
			PutCode(RISSE_TO_VM_REG_ADDR(frame));
#endif
		}
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void tRisseInterCodeContext::AddLocalVariable(const risse_char *name, risse_int init)
{
	// create a local variable
	// ( however create it in "this" when the variable is defined at global )
	// name = variable name

	// init = register address that points initial value ( 0 = no initial value )

	risse_int base = ContextType == ctClass ? 2: 1;
	if(Namespace.GetLevel() >= base)
	{
		// create on local

//		risse_int ff = Namespace.Find(name);
		Namespace.Add(name);
		if(init != 0)
		{
			// initial value is given
			risse_int n = Namespace.Find(name);
			PutCode(VM_CP, LEX_POS);
			PutCode(RISSE_TO_VM_REG_ADDR(-n-VariableReserveCount-1), LEX_POS);
			PutCode(RISSE_TO_VM_REG_ADDR(init), LEX_POS);
		}
		else/* if(ff==-1) */
		{
			// first initialization
			risse_int n = Namespace.Find(name);
			PutCode(VM_CL, LEX_POS);
			PutCode(RISSE_TO_VM_REG_ADDR(-n-VariableReserveCount-1), LEX_POS);
		}
	}
	else
	{
		// create member on this
		risse_int	dp = PutData(tRisseVariant(name));
		PutCode(VM_SPDE, LEX_POS);
		PutCode(RISSE_TO_VM_REG_ADDR(-1), LEX_POS);
		PutCode(RISSE_TO_VM_REG_ADDR(dp), LEX_POS);
		PutCode(RISSE_TO_VM_REG_ADDR(init), LEX_POS);
	}
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::InitLocalVariable(const risse_char *name, tRisseExprNode *node)
{
	// create a local variable named "name", with inial value of the
	// expression node of "node".

	risse_int fr = FrameBase;
	risse_int resaddr = GenNodeCode(fr, node, RISSE_RT_NEEDED, 0, tSubParam());
	AddLocalVariable(name, resaddr);
	ClearFrame(fr);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::InitLocalFunction(const risse_char *name, risse_int data)
{
	// create a local function variable pointer by data ( in DataArea ),
	// named "name".

	risse_int fr = FrameBase;
	PutCode(VM_CONST, LEX_POS);
	PutCode(RISSE_TO_VM_REG_ADDR(fr), LEX_POS);
	PutCode(RISSE_TO_VM_REG_ADDR(data));
	fr++;
	AddLocalVariable(name, fr-1);
	ClearFrame(fr);
}
//---------------------------------------------------------------------------

void tRisseInterCodeContext::CreateExprCode(tRisseExprNode *node)
{
	// create code of node
	risse_int fr = FrameBase;
	GenNodeCode(fr, node, 0, 0, tSubParam());
	ClearFrame(fr);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::EnterWhileCode(bool do_while)
{
	// enter to "while"
	// ( do_while = true indicates do-while syntax )
	NestVector.push_back(tNestData());
	NestVector.back().Type = do_while?ntDoWhile:ntWhile;
	NestVector.back().LoopStartIP = CodeAreaSize;
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::CreateWhileExprCode(tRisseExprNode *node, bool do_while)
{
	// process the condition expression "node"

	if(do_while)
	{
		DoContinuePatch(NestVector.back());
	}

	risse_int fr = FrameBase;
	risse_int resaddr = GenNodeCode(fr, node, RISSE_RT_NEEDED|RISSE_RT_CFLAG, 0, tSubParam());
	bool inv = false;
	if(!RisseIsCondFlagRetValue(resaddr))
	{
		PutCode(VM_TT, NODE_POS);
		PutCode(RISSE_TO_VM_REG_ADDR(resaddr), NODE_POS);
	}
	else
	{
		if(resaddr == RISSE_GNC_CFLAG_I) inv = true;
	}
	ClearFrame(fr);

	if(!do_while)
	{
		NestVector.back().ExitPatchVector.push_back(CodeAreaSize);
		AddJumpList();
		PutCode(inv?VM_JF:VM_JNF, NODE_POS);
		PutCode(0, NODE_POS);
	}
	else
	{
		risse_int jmp_ip = CodeAreaSize;
		AddJumpList();
		PutCode(inv?VM_JNF:VM_JF, NODE_POS);
		PutCode(NestVector.back().LoopStartIP - jmp_ip, NODE_POS);
	}
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::ExitWhileCode(bool do_while)
{
	// exit from "while"
	if(NestVector.size() == 0)
	{
		_yyerror(RisseSyntaxError, Block);
		return;
	}
	if(do_while)
	{
		if(NestVector.back().Type != ntDoWhile)
		{
			_yyerror(RisseSyntaxError, Block);
			return;
		}
	}
	else
	{
		if(NestVector.back().Type != ntWhile)
		{
			_yyerror(RisseSyntaxError, Block);
			return;
		}
	}

	if(!do_while)
	{
		risse_int jmp_ip = CodeAreaSize;
		AddJumpList();
		PutCode(VM_JMP, LEX_POS);
		PutCode(NestVector.back().LoopStartIP - jmp_ip, LEX_POS);
	}
	DoNestTopExitPatch();
	NestVector.pop_back();
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::EnterIfCode()
{
	// enter to "if"

	NestVector.push_back(tNestData());
	NestVector.back().Type = ntIf;
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::CreateIfExprCode(tRisseExprNode *node)
{
	// process condition expression "node"

	risse_int fr = FrameBase;
	risse_int resaddr = GenNodeCode(fr, node, RISSE_RT_NEEDED|RISSE_RT_CFLAG, 0, tSubParam());
	bool inv = false;
	if(!RisseIsCondFlagRetValue(resaddr))
	{
		PutCode(VM_TT, NODE_POS);
		PutCode(RISSE_TO_VM_REG_ADDR(resaddr), NODE_POS);
	}
	else
	{
		if(resaddr == RISSE_GNC_CFLAG_I) inv = true;
	}
	ClearFrame(fr);
	NestVector.back().Patch1 = CodeAreaSize;
	AddJumpList();
	PutCode(inv?VM_JF:VM_JNF, NODE_POS);
	PutCode(0, NODE_POS);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::ExitIfCode()
{
	// exit from if
	if(NestVector.size() == 0)
	{
		_yyerror(RisseSyntaxError, Block);
		return;
	}
	if(NestVector.back().Type != ntIf)
	{
		_yyerror(RisseSyntaxError, Block);
		return;
	}

	CodeArea[NestVector.back().Patch1 + 1] = CodeAreaSize - NestVector.back().Patch1;
	PrevIfExitPatch = NestVector.back().Patch1;
	NestVector.pop_back();
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::EnterElseCode()
{
	// enter to "else".
	// before is "if", is clear from syntax definition.

	NestVector.push_back(tNestData());
	NestVector.back().Type = ntElse;
	NestVector.back().Patch2 = CodeAreaSize;
	AddJumpList();
	PutCode(VM_JMP, LEX_POS);
	PutCode(0, LEX_POS);
	CodeArea[PrevIfExitPatch + 1] = CodeAreaSize - PrevIfExitPatch;
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::ExitElseCode()
{
	// exit from else
	if(NestVector.size() == 0)
	{
		_yyerror(RisseSyntaxError, Block);
		return;
	}
	if(NestVector.back().Type != ntElse)
	{
		_yyerror(RisseSyntaxError, Block);
		return;
	}

	CodeArea[NestVector.back().Patch2 + 1] = CodeAreaSize - NestVector.back().Patch2;
	NestVector.pop_back();
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::EnterForCode(bool varcreate)
{
	// enter to "for".
	// ( varcreate = true, indicates that the variable is to be created in the
	//	first clause )

	NestVector.push_back(tNestData());
	NestVector.back().Type = ntFor;
	if(varcreate) EnterBlock(); // create a scope
	NestVector.back().VariableCreated = varcreate;
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::CreateForExprCode(tRisseExprNode *node)
{
	// process the "for"'s second clause; a condition expression

	NestVector.back().LoopStartIP = CodeAreaSize;
	if(node)
	{
		risse_int fr = FrameBase;
		risse_int resaddr = GenNodeCode(fr, node, RISSE_RT_NEEDED|RISSE_RT_CFLAG,
			0, tSubParam());
		bool inv = false;
		if(!RisseIsCondFlagRetValue(resaddr))
		{
			PutCode(VM_TT, NODE_POS);
			PutCode(RISSE_TO_VM_REG_ADDR(resaddr), NODE_POS);
		}
		else
		{
			if(resaddr == RISSE_GNC_CFLAG_I) inv = true;
		}
		ClearFrame(fr);
		NestVector.back().ExitPatchVector.push_back(CodeAreaSize);
		AddJumpList();
		PutCode(inv?VM_JF:VM_JNF, NODE_POS);
		PutCode(0, NODE_POS);
	}
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::SetForThirdExprCode(tRisseExprNode *node)
{
	// process the "for"'s third clause; a post-loop expression

	NestVector.back().PostLoopExpr = node;
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::ExitForCode()
{
	// exit from "for"
	risse_int nestsize = NestVector.size();
	if(nestsize == 0)
	{
		_yyerror(RisseSyntaxError, Block);
		return;
	}
	if(NestVector.back().Type != ntFor && NestVector.back().Type != ntBlock)
	{
		_yyerror(RisseSyntaxError, Block);
		return;
	}

	if(NestVector.back().Type == ntFor)
		DoContinuePatch(NestVector.back());
	if(nestsize >= 2 && NestVector[nestsize-2].Type == ntFor)
		DoContinuePatch(NestVector[nestsize-2]);


	if(NestVector.back().PostLoopExpr)
	{
		risse_int fr = FrameBase;
		GenNodeCode(fr, NestVector.back().PostLoopExpr, false, 0, tSubParam());
		ClearFrame(fr);
	}
	risse_int jmp_ip = CodeAreaSize;
	AddJumpList();
	PutCode(VM_JMP, LEX_POS);
	PutCode(NestVector.back().LoopStartIP - jmp_ip, LEX_POS);
	DoNestTopExitPatch();
	if(NestVector.back().VariableCreated) ExitBlock();
	DoNestTopExitPatch();
	NestVector.pop_back();
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::EnterSwitchCode(tRisseExprNode *node)
{
	// enter to "switch"
	// "node" indicates a reference expression

	NestVector.push_back(tNestData());
	NestVector.back().Type = ntSwitch;
	NestVector.back().Patch1 = -1;
	NestVector.back().Patch2 = -1;
	NestVector.back().IsFirstCase = true;

	risse_int fr = FrameBase;
	risse_int resaddr = GenNodeCode(fr, node, RISSE_RT_NEEDED, 0, tSubParam());

	if(FrameBase != resaddr)
	{
		PutCode(VM_CP, NODE_POS);
		PutCode(RISSE_TO_VM_REG_ADDR(FrameBase), NODE_POS); // FrameBase points the reference value
		PutCode(RISSE_TO_VM_REG_ADDR(resaddr), NODE_POS);
	}

	NestVector.back().RefRegister = FrameBase;

	if(fr-1 > MaxFrameCount) MaxFrameCount = fr-1;

	FrameBase ++; // increment FrameBase
	if(FrameBase-1 > MaxFrameCount) MaxFrameCount = FrameBase-1;

	ClearFrame(fr);

	EnterBlock();
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::ExitSwitchCode()
{
	// exit from switch

	ExitBlock();

	if(NestVector.size() == 0)
	{
		_yyerror(RisseSyntaxError, Block);
		return;
	}
	if(NestVector.back().Type != ntSwitch)
	{
		_yyerror(RisseSyntaxError, Block);
		return;
	}

	risse_int patch3;
	if(!NestVector.back().IsFirstCase)
	{
		patch3 = CodeAreaSize;
		AddJumpList();
		PutCode(VM_JMP, LEX_POS);
		PutCode(0, LEX_POS);
	}


	if(NestVector.back().Patch1 != -1)
	{
		CodeArea[NestVector.back().Patch1 +1] = CodeAreaSize - NestVector.back().Patch1;
	}

	if(NestVector.back().Patch2 != -1)
	{
		AddJumpList();
		risse_int jmp_start = CodeAreaSize;
		PutCode(VM_JMP, LEX_POS);
		PutCode(NestVector.back().Patch2 - jmp_start, LEX_POS);
	}

	if(!NestVector.back().IsFirstCase)
	{
		CodeArea[patch3 +1] = CodeAreaSize - patch3;
	}


	DoNestTopExitPatch();
#if 0
	PutCode(VM_CL, LEX_POS);
	PutCode(RISSE_TO_VM_REG_ADDR(NestVector.back().RefRegister), LEX_POS);
#endif
	FrameBase--;
	NestVector.pop_back();
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::ProcessCaseCode(tRisseExprNode *node)
{
	// process "case expression :".
	// process "default :" if node == NULL.

	risse_int nestsize = NestVector.size();

	if(nestsize < 3)
	{
		_yyerror(RisseMisplacedCase, Block);
		return;
	}
	if(NestVector[nestsize-1].Type != ntBlock ||
		NestVector[nestsize-2].Type != ntBlock ||
		NestVector[nestsize-3].Type != ntSwitch)
	{
		// the stack layout must be ( from top )
		// ntBlock, ntBlock, ntSwitch
		_yyerror(RisseMisplacedCase, Block);
		return;
	}

	tNestData &data = NestVector[NestVector.size()-3];

	risse_int patch3;
	if(!data.IsFirstCase)
	{
		patch3 = CodeAreaSize;
		AddJumpList();
		PutCode(VM_JMP, NODE_POS);
		PutCode(0, NODE_POS);
	}

	ExitBlock();
	if(data.Patch1 != -1)
	{
		CodeArea[data.Patch1 +1] = CodeAreaSize -data.Patch1;
	}

	if(node)
	{
		risse_int fr = FrameBase;
		risse_int resaddr = GenNodeCode(fr, node, RISSE_RT_NEEDED, 0, tSubParam());
		PutCode(VM_CEQ, NODE_POS);
		PutCode(RISSE_TO_VM_REG_ADDR(data.RefRegister), NODE_POS);
			// compare to reference value with normal comparison
		PutCode(RISSE_TO_VM_REG_ADDR(resaddr), NODE_POS);
		ClearFrame(fr);
		data.Patch1 = CodeAreaSize;
		AddJumpList();
		PutCode(VM_JNF, NODE_POS);
		PutCode(0, NODE_POS);
	}
	else
	{
		data.Patch1 = CodeAreaSize;
		AddJumpList();
		PutCode(VM_JMP, NODE_POS);
		PutCode(0, NODE_POS);

		data.Patch2 = CodeAreaSize; // Patch2 = "default:"'s position
	}


	if(!data.IsFirstCase)
	{
		CodeArea[patch3 +1] = CodeAreaSize - patch3;
	}
	data.IsFirstCase = false;

	EnterBlock();
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::EnterWithCode(tRisseExprNode *node)
{
	// enter to "with"
	// "node" indicates a reference expression

	// this method and ExitWithCode are very similar to switch's code.
	// (those are more simple than that...)

	risse_int fr = FrameBase;
	risse_int resaddr = GenNodeCode(fr, node, RISSE_RT_NEEDED, 0, tSubParam());

	if(FrameBase != resaddr)
	{
		// bring the reference variable to frame base top
		PutCode(VM_CP, NODE_POS);
		PutCode(RISSE_TO_VM_REG_ADDR(FrameBase), NODE_POS); // FrameBase points the reference value
		PutCode(RISSE_TO_VM_REG_ADDR(resaddr), NODE_POS);
	}

	NestVector.push_back(tNestData());
	NestVector.back().Type = ntWith;

	NestVector.back().RefRegister = FrameBase;

	if(fr-1 > MaxFrameCount) MaxFrameCount = fr-1;

	FrameBase ++; // increment FrameBase
	if(FrameBase-1 > MaxFrameCount) MaxFrameCount = FrameBase-1;

	ClearFrame(fr);

	EnterBlock();
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::ExitWithCode()
{
	// exit from switch
	ExitBlock();

	if(NestVector.size() == 0)
	{
		_yyerror(RisseSyntaxError, Block);
		return;
	}
	if(NestVector.back().Type != ntWith)
	{
		_yyerror(RisseSyntaxError, Block);
		return;
	}

#if 0
	PutCode(VM_CL, LEX_POS);
	PutCode(RISSE_TO_VM_REG_ADDR(NestVector.back().RefRegister), LEX_POS);
#endif
	FrameBase--;
	NestVector.pop_back();
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::DoBreak(void)
{
	// process "break".

	// search in NestVector backwards
	risse_int vc = Namespace.GetCount();
	risse_int pvc = vc;

	risse_int i = NestVector.size() -1;
	for(; i>=0; i--)
	{
		tNestData &data = NestVector[i];
		if(data.Type == ntSwitch ||
			data.Type == ntWhile || data.Type == ntDoWhile ||
			data.Type == ntFor)
		{
			// "break" can apply on this syntax
			ClearLocalVariable(vc, pvc); // clear local variables
			data.ExitPatchVector.push_back(CodeAreaSize);
			AddJumpList();
			PutCode(VM_JMP, LEX_POS);
			PutCode(0, LEX_POS); // later patches here
			return;
		}
		else if(data.Type == ntBlock)
		{
			pvc = data.VariableCount;
		}
		else if(data.Type == ntTry)
		{
			PutCode(VM_EXTRY);
		}
		else if(data.Type == ntSwitch || data.Type == ntWith)
		{
			// clear reference register of "switch" or "with" syntax
#if 0
			PutCode(VM_CL, LEX_POS);
			PutCode(RISSE_TO_VM_REG_ADDR(data.RefRegister), LEX_POS);
#endif
		}
	}

	_yyerror(RisseMisplacedBreakContinue, Block);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::DoContinue(void)
{
	// process "continue".

	// generate code that jumps before '}' ( the end of the loop ).
	// for "while" loop, the jump code immediately jumps to the condition check code.

	// search in NestVector backwards
	risse_int vc = Namespace.GetCount();
	risse_int pvc = vc;

	risse_int i = NestVector.size() -1;
	for(; i>=0; i--)
	{
		tNestData &data = NestVector[i];
		if(data.Type == ntWhile)
		{
			// for "while" loop
			ClearLocalVariable(vc, pvc); // clear local variables
			risse_int jmpstart = CodeAreaSize;
			AddJumpList();
			PutCode(VM_JMP, LEX_POS);
			PutCode(data.LoopStartIP - jmpstart, LEX_POS);
			return;
		}
		else if(data.Type == ntDoWhile || data.Type == ntFor)
		{
			// "do-while" or "for" loop needs forward jump
			ClearLocalVariable(vc, pvc); // clears local variables
			data.ContinuePatchVector.push_back(CodeAreaSize);
			AddJumpList();
			PutCode(VM_JMP, LEX_POS);
			PutCode(0, LEX_POS); // later patch this
			return;
		}
		else if(data.Type == ntBlock)
		{
			// does not count variables which created at for loop's
			// first clause
			if(i < 1 || NestVector[i-1].Type != ntFor ||
				!NestVector[i].VariableCreated)
				pvc = data.VariableCount;
		}
		else if(data.Type == ntTry)
		{
			PutCode(VM_EXTRY, LEX_POS);
		}
		else if(data.Type == ntSwitch || data.Type == ntWith)
		{
			// clear reference register of "switch" or "with" syntax
#if 0
			PutCode(VM_CL, LEX_POS);
			PutCode(RISSE_TO_VM_REG_ADDR(data.RefRegister), LEX_POS);
#endif
		}
	}

	_yyerror(RisseMisplacedBreakContinue, Block);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::DoDebugger()
{
	// process "debugger" statement.
	PutCode(VM_DEBUGGER, LEX_POS);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::ReturnFromFunc(tRisseExprNode *node)
{
	// precess "return"
	// note: the "return" positioned in global immediately returns without
	// execution of the remainder code.

	if(!node)
	{
		// no return value
		PutCode(VM_SRV, NODE_POS);
		PutCode(RISSE_TO_VM_REG_ADDR(0), NODE_POS);  // returns register #0 = void
	}
	else
	{
		// generates return expression
		risse_int fr = FrameBase;
		risse_int resaddr = GenNodeCode(fr, node, RISSE_RT_NEEDED, 0, tSubParam());

		PutCode(VM_SRV, NODE_POS);

		PutCode(RISSE_TO_VM_REG_ADDR(resaddr), NODE_POS);

		ClearFrame(fr);

	}

	// clear the frame
	risse_int org_framebase = FrameBase;
	ClearFrame(FrameBase, 1);
	FrameBase = org_framebase;

	// clear local variables
	ClearLocalVariable(Namespace.GetCount(), 0);

	// check try block
	risse_int i = NestVector.size() -1;
	for(; i>=0; i--)
	{
		tNestData &data = NestVector[i];
		if(data.Type == ntTry)
		{
			PutCode(VM_EXTRY, LEX_POS); // exit from try-protected block
		}
	}


	PutCode(VM_RET, LEX_POS);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::EnterTryCode()
{
	// enter to "try"

	NestVector.push_back(tNestData());
	NestVector.back().Type = ntTry;
	NestVector.back().VariableCreated = false;

	NestVector.back().Patch1 = CodeAreaSize;
	AddJumpList();
	PutCode(VM_ENTRY, LEX_POS);
	PutCode(0, LEX_POS);
	PutCode(RISSE_TO_VM_REG_ADDR(FrameBase), LEX_POS); // an exception object will be held here

	if(FrameBase > MaxFrameCount) MaxFrameCount = FrameBase;
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::EnterCatchCode(const risse_char *name)
{
	// enter to "catch"

	PutCode(VM_EXTRY, LEX_POS);
	NestVector.back().Patch2 = CodeAreaSize;
	AddJumpList();
	PutCode(VM_JMP, LEX_POS);
	PutCode(0, LEX_POS);

	CodeArea[NestVector.back().Patch1 + 1] = CodeAreaSize - NestVector.back().Patch1;

	// clear local variables
	ClearLocalVariable(Namespace.GetMaxCount(), Namespace.GetCount());

	// clear frame
	risse_int fr = MaxFrameCount + 1;
	risse_int base = name ? FrameBase+1 : FrameBase;
	ClearFrame(fr, base);

	if(name)
	{
		NestVector.back().VariableCreated = true;
		EnterBlock();
		AddLocalVariable(name, FrameBase);
			// cleate a variable that receives the exception object
	}

}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::ExitTryCode()
{
	// exit from "try"
	if(NestVector.size() >= 2)
	{
		if(NestVector[NestVector.size()-2].Type == ntTry)
		{
			if(NestVector[NestVector.size()-2].VariableCreated)
			{
				ExitBlock();
			}
		}
	}
	
	if(NestVector.size() == 0)
	{
		_yyerror(RisseSyntaxError, Block);
		return;
	}
	if(NestVector.back().Type != ntTry)
	{
		_yyerror(RisseSyntaxError, Block);
		return;
	}


	risse_int p2addr = NestVector.back().Patch2;

	CodeArea[p2addr + 1] = CodeAreaSize - p2addr;
	NestVector.pop_back();
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::ProcessThrowCode(tRisseExprNode *node)
{
	// process "throw".
	// node = expressoin to throw

	risse_int fr = FrameBase;
	risse_int resaddr = GenNodeCode(fr, node, RISSE_RT_NEEDED, 0, tSubParam());
	PutCode(VM_THROW, NODE_POS);
	PutCode(RISSE_TO_VM_REG_ADDR(resaddr), NODE_POS);
	if(fr-1 > MaxFrameCount) MaxFrameCount = fr-1;
#if 0
	while(fr-->FrameBase)
	{
		PutCode(VM_CL);
		PutCode(RISSE_TO_VM_REG_ADDR(fr));
	}
#endif
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::CreateExtendsExprCode(tRisseExprNode *node, bool hold)
{
	// process class extender

	risse_int fr = FrameBase;
	risse_int resaddr = GenNodeCode(fr, node, RISSE_RT_NEEDED, 0, tSubParam());

	PutCode(VM_CHGTHIS, NODE_POS);
	PutCode(RISSE_TO_VM_REG_ADDR(resaddr), NODE_POS);
	PutCode(RISSE_TO_VM_REG_ADDR(-1), NODE_POS);

	PutCode(VM_CALL, NODE_POS);
	PutCode(0, NODE_POS);
	PutCode(RISSE_TO_VM_REG_ADDR(resaddr), NODE_POS);
	PutCode(0, NODE_POS);

	if(hold)
	{
		SuperClassExpr = node;
	}

	FunctionRegisterCodePoint = CodeAreaSize; // update FunctionRegisterCodePoint

	// create a Super Class Proxy context
	if(!SuperClassGetter)
	{
		SuperClassGetter =
			new tRisseInterCodeContext(this, Name, Block, ctSuperClassGetter);
	}

	SuperClassGetter->CreateExtendsExprProxyCode(node);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::CreateExtendsExprProxyCode(tRisseExprNode *node)
{
	// create super class proxy to retrieve super class
	SuperClassGetterPointer.push_back(CodeAreaSize);

	risse_int fr = FrameBase;
	risse_int resaddr = GenNodeCode(fr, node, RISSE_RT_NEEDED, 0, tSubParam());

	PutCode(VM_SRV);
	PutCode(RISSE_TO_VM_REG_ADDR(resaddr));
	ClearFrame(fr);

	PutCode(VM_RET);

	PutCode(VM_NOP, NODE_POS);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::EnterBlock()
{
	// enter to block

	Namespace.Push();
	risse_int varcount = Namespace.GetCount();
	NestVector.push_back(tNestData());
	NestVector.back().Type = ntBlock;
	NestVector.back().VariableCount = varcount;
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::ExitBlock()
{
	// exit from block
	if(NestVector.size() == 0)
	{
		_yyerror(RisseSyntaxError, Block);
		return;
	}
	if(NestVector.back().Type != ntBlock)
	{
		_yyerror(RisseSyntaxError, Block);
		return;
	}

	NestVector.pop_back();
	risse_int prevcount = Namespace.GetCount();
	Namespace.Pop();
	risse_int curcount = Namespace.GetCount();
	ClearLocalVariable(prevcount, curcount);
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::GenerateFuncCallArgCode()
{
	if(FuncArgStack.top().IsOmit)
	{
		PutCode(-1, LEX_POS); // omit (...) is specified
	}
	else if(FuncArgStack.top().HasExpand)
	{
		PutCode(-2, LEX_POS); // arguments have argument expanding node
		std::vector<tFuncArgItem> & vec = FuncArgStack.top().ArgVector;
		PutCode(vec.size(), LEX_POS); // count of the arguments
		risse_uint i;
		for(i=0; i<vec.size(); i++)
		{
			PutCode((risse_int32)vec[i].Type, LEX_POS);
			PutCode(RISSE_TO_VM_REG_ADDR(vec[i].Register), LEX_POS);
		}
	}
	else
	{
		std::vector<tFuncArgItem> & vec = FuncArgStack.top().ArgVector;
		PutCode(vec.size(), LEX_POS); // count of arguments
		risse_uint i;
		for(i=0; i<vec.size(); i++)
			PutCode(RISSE_TO_VM_REG_ADDR(vec[i].Register), LEX_POS);
	}
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::AddFunctionDeclArg(const risse_char *varname, tRisseExprNode *node)
{
	// process the function argument of declaration
	// varname = argument name
	// init = initial expression

	Namespace.Add(varname);
//	AddLocalVariable(varname);

	if(node)
	{
		PutCode(VM_CDEQ, NODE_POS);
		PutCode(RISSE_TO_VM_REG_ADDR(-3 - FuncDeclArgCount), NODE_POS);
		PutCode(RISSE_TO_VM_REG_ADDR(0), NODE_POS);
		risse_int jmp_ip = CodeAreaSize;
		AddJumpList();
		PutCode(VM_JNF), NODE_POS;
		PutCode(0, NODE_POS);

		risse_int fr = FrameBase;
		risse_int resaddr = GenNodeCode(fr, node, RISSE_RT_NEEDED, 0, tSubParam());
		PutCode(VM_CP, NODE_POS);
		PutCode(RISSE_TO_VM_REG_ADDR(-3 - FuncDeclArgCount), NODE_POS);
		PutCode(RISSE_TO_VM_REG_ADDR(resaddr), NODE_POS);
		ClearFrame(fr);

		CodeArea[jmp_ip+1] = CodeAreaSize - jmp_ip;

	}
	FuncDeclArgCount++;
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::AddFunctionDeclArgCollapse(const risse_char *varname)
{
	// process the function "collapse" argument of declaration.
	// collapse argument is available to receive arguments in array object form.

	if(varname == NULL)
	{
		// receive arguments in unnamed array
		FuncDeclUnnamedArgArrayBase = FuncDeclArgCount;
	}
	else
	{
		// receive arguments in named array
		FuncDeclCollapseBase = FuncDeclArgCount;
		Namespace.Add(varname);		
	}
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::SetPropertyDeclArg(const risse_char *varname)
{
	// process the setter argument

	Namespace.Add(varname);
	FuncDeclArgCount = 1;
}
//---------------------------------------------------------------------------
const risse_char * tRisseInterCodeContext::GetName() const
{
	return Name;
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::PushCurrentNode(tRisseExprNode *node)
{
	CurrentNodeVector.push_back(node);
}
//---------------------------------------------------------------------------
tRisseExprNode * tRisseInterCodeContext::GetCurrentNode()
{
	risse_uint size = CurrentNodeVector.size();
	if(size == 0) return NULL;
	return CurrentNodeVector[size-1];
}
//---------------------------------------------------------------------------
void tRisseInterCodeContext::PopCurrentNode()
{
	CurrentNodeVector.pop_back();
}
//---------------------------------------------------------------------------
tRisseExprNode * tRisseInterCodeContext::MakeConstValNode(const tRisseVariant &val)
{
	tRisseExprNode * n = new tRisseExprNode;
	NodeToDeleteVector.push_back(n);
	n->SetOpecode(T_CONSTVAL);
	n->SetValue(val);
	n->SetPosition(LEX_POS);
	return n;
}
//---------------------------------------------------------------------------
#ifdef RISSE_DEBUG_PROFILE_TIME
risse_uint time_make_np = 0;
#endif

tRisseExprNode * tRisseInterCodeContext::MakeNP0(risse_int opecode)
{
#ifdef RISSE_DEBUG_PROFILE_TIME
	tRisseTimeProfiler prof(time_make_np);
#endif
	tRisseExprNode * n = new tRisseExprNode;
	NodeToDeleteVector.push_back(n);
	n->SetOpecode(opecode);
	n->SetPosition(LEX_POS);
	return n;
}
//---------------------------------------------------------------------------
tRisseExprNode * tRisseInterCodeContext::MakeNP1(risse_int opecode, tRisseExprNode * node1)
{
#ifdef RISSE_DEBUG_PROFILE_TIME
	tRisseTimeProfiler prof(time_make_np);
#endif

#ifndef RISSE_NO_CONSTANT_FOLDING
	if(node1 && node1->GetOpecode() == T_CONSTVAL)
	{
		// constant folding
		tRisseExprNode * ret = NULL;

		switch(opecode)
		{
		case T_EXCRAMATION:
			ret = MakeConstValNode(!node1->GetValue());
			break;
		case T_TILDE:
			ret = MakeConstValNode(~node1->GetValue());
			break;
		case T_SHARP:
		  {
			tRisseVariant val(node1->GetValue());
			CharacterCodeOf(val);
			ret = MakeConstValNode(val);
		  }
			break;
		case T_DOLLAR:
		  {
			tRisseVariant val(node1->GetValue());
			CharacterCodeFrom(val);
			ret = MakeConstValNode(val);
		  }
			break;
		case T_UPLUS:
		  {
			tRisseVariant val(node1->GetValue());
			val.tonumber();
			ret = MakeConstValNode(val);
		  }
			break;
		case T_UMINUS:
		  {
			tRisseVariant val(node1->GetValue());
			val.changesign();
			ret = MakeConstValNode(val);
		  }
			break;
		case T_INT:
		  {
			tRisseVariant val(node1->GetValue());
			val.ToInteger();
			ret = MakeConstValNode(val);
		  }
			break;
		case T_REAL:
		  {
			tRisseVariant val(node1->GetValue());
			val.ToReal();
			ret = MakeConstValNode(val);
		  }
			break;
		case T_STRING:
		  {
			tRisseVariant val(node1->GetValue());
			val.ToString();
			ret = MakeConstValNode(val);
		  }
			break;
		case T_OCTET:
		  {
			tRisseVariant val(node1->GetValue());
			val.ToOctet();
			ret = MakeConstValNode(val);
		  }
			break;
		default: ;
		}
		if(ret)
		{
			node1->Clear(); // not to have data no longer
			return ret;
		}
	}
#endif

	tRisseExprNode * n = new tRisseExprNode;
	NodeToDeleteVector.push_back(n);
	n->SetOpecode(opecode);
	n->SetPosition(LEX_POS);
	n->Add(node1);
	return n;
}
//---------------------------------------------------------------------------
tRisseExprNode * tRisseInterCodeContext::MakeNP2(risse_int opecode, tRisseExprNode * node1, tRisseExprNode * node2)
{
#ifdef RISSE_DEBUG_PROFILE_TIME
	tRisseTimeProfiler prof(time_make_np);
#endif

#ifndef RISSE_NO_CONSTANT_FOLDING
	if(node1 && node1->GetOpecode() == T_CONSTVAL &&
		node2 && node2->GetOpecode() == T_CONSTVAL)
	{
		// constant folding
		switch(opecode)
		{
		case T_COMMA:
			return MakeConstValNode(node2->GetValue());
		case T_LOGICALOR:
			return MakeConstValNode(node1->GetValue() || node2->GetValue());
		case T_LOGICALAND:
			return MakeConstValNode(node1->GetValue() && node2->GetValue());
		case T_VERTLINE:
			return MakeConstValNode(node1->GetValue() | node2->GetValue());
		case T_CHEVRON:
			return MakeConstValNode(node1->GetValue() ^ node2->GetValue());
		case T_AMPERSAND:
			return MakeConstValNode(node1->GetValue() & node2->GetValue());
		case T_NOTEQUAL:
			return MakeConstValNode(node1->GetValue() != node2->GetValue());
		case T_EQUALEQUAL:
			return MakeConstValNode(node1->GetValue() == node2->GetValue());
		case T_DISCNOTEQUAL:
			return MakeConstValNode(!(node1->GetValue().DiscernCompare(node2->GetValue())));
		case T_DISCEQUAL:
			return MakeConstValNode( (node1->GetValue().DiscernCompare(node2->GetValue())));
		case T_LT:
			return MakeConstValNode(node1->GetValue() < node2->GetValue());
		case T_GT:
			return MakeConstValNode(node1->GetValue() > node2->GetValue());
		case T_LTOREQUAL:
			return MakeConstValNode(node1->GetValue() <= node2->GetValue());
		case T_GTOREQUAL:
			return MakeConstValNode(node1->GetValue() >= node2->GetValue());
		case T_RARITHSHIFT:
			return MakeConstValNode(node1->GetValue() >> node2->GetValue());
		case T_LARITHSHIFT:
			return MakeConstValNode(node1->GetValue() << node2->GetValue());
		case T_RBITSHIFT:
			return MakeConstValNode( (node1->GetValue().rbitshift(node2->GetValue())));
		case T_PLUS:
			return MakeConstValNode(node1->GetValue() + node2->GetValue());
		case T_MINUS:
			return MakeConstValNode(node1->GetValue() - node2->GetValue());
		case T_PERCENT:
			return MakeConstValNode(node1->GetValue() % node2->GetValue());
		case T_SLASH:
			return MakeConstValNode(node1->GetValue() / node2->GetValue());
		case T_BACKSLASH:
			return MakeConstValNode( (node1->GetValue().idiv(node2->GetValue())));
		case T_ASTERISK:
			return MakeConstValNode(node1->GetValue() * node2->GetValue());
		default: ;
		}
	}
#endif


	tRisseExprNode * n = new tRisseExprNode;
	NodeToDeleteVector.push_back(n);
	n->SetOpecode(opecode);
	n->SetPosition(LEX_POS);
	n->Add(node1);
	n->Add(node2);
	return n;
}
//---------------------------------------------------------------------------
tRisseExprNode * tRisseInterCodeContext::MakeNP3(risse_int opecode, tRisseExprNode * node1, tRisseExprNode * node2,
	tRisseExprNode * node3)
{
#ifdef RISSE_DEBUG_PROFILE_TIME
	tRisseTimeProfiler prof(time_make_np);
#endif
	tRisseExprNode * n = new tRisseExprNode;
	NodeToDeleteVector.push_back(n);
	n->SetOpecode(opecode);
	n->SetPosition(LEX_POS);
	n->Add(node1);
	n->Add(node2);
	n->Add(node3);
	return n;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risse


