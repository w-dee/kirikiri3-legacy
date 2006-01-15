//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @brief VM用コンパイラ
//---------------------------------------------------------------------------
#ifndef risseInterCodeGenH
#define risseInterCodeGenH


#include <vector>
#include <stack>
#include <list>
#include "risseVariant.h"
#include "risseInterface.h"
#include "risseNamespace.h"
#include "risseError.h"
#include "risseObject.h"



namespace Risse
{
//---------------------------------------------------------------------------
//
#define RISSE_TO_VM_CODE_ADDR(x)  ((x) * (risse_int)sizeof(risse_uint32))
#define RISSE_TO_VM_REG_ADDR(x) ((x) * (risse_int)sizeof(tRisseVariant))
#define RISSE_FROM_VM_CODE_ADDR(x)  ((risse_int)(x) / (risse_int)sizeof(risse_uint32))
#define RISSE_FROM_VM_REG_ADDR(x) ((risse_int)(x) / (risse_int)sizeof(tRisseVariant))
#define RISSE_ADD_VM_CODE_ADDR(dest, x)  ((*(char **)&(dest)) += (x))
#define RISSE_GET_VM_REG_ADDR(base, x) ((tRisseVariant*)((char *)(base) + (risse_int)(x)))
#define RISSE_GET_VM_REG(base, x) (*(RISSE_GET_VM_REG_ADDR(base, x)))

//---------------------------------------------------------------------------
extern int _yyerror(const risse_char * msg, void *pm, risse_int pos = -1);
//---------------------------------------------------------------------------
#define RISSE_NORMAL_AND_PROPERTY_ACCESSER(x) x, x##PD, x##PI, x##P

enum tRisseVMCodes{

	VM_NOP, VM_CONST, VM_CP, VM_CL, VM_CCL, VM_TT, VM_TF, VM_CEQ, VM_CDEQ, VM_CLT,
	VM_CGT, VM_SETF, VM_SETNF, VM_LNOT, VM_NF, VM_JF, VM_JNF, VM_JMP,

	RISSE_NORMAL_AND_PROPERTY_ACCESSER(VM_INC),
	RISSE_NORMAL_AND_PROPERTY_ACCESSER(VM_DEC),
	RISSE_NORMAL_AND_PROPERTY_ACCESSER(VM_LOR),
	RISSE_NORMAL_AND_PROPERTY_ACCESSER(VM_LAND),
	RISSE_NORMAL_AND_PROPERTY_ACCESSER(VM_BOR),
	RISSE_NORMAL_AND_PROPERTY_ACCESSER(VM_BXOR),
	RISSE_NORMAL_AND_PROPERTY_ACCESSER(VM_BAND),
	RISSE_NORMAL_AND_PROPERTY_ACCESSER(VM_SAR),
	RISSE_NORMAL_AND_PROPERTY_ACCESSER(VM_SAL),
	RISSE_NORMAL_AND_PROPERTY_ACCESSER(VM_SR),
	RISSE_NORMAL_AND_PROPERTY_ACCESSER(VM_ADD),
	RISSE_NORMAL_AND_PROPERTY_ACCESSER(VM_SUB),
	RISSE_NORMAL_AND_PROPERTY_ACCESSER(VM_MOD),
	RISSE_NORMAL_AND_PROPERTY_ACCESSER(VM_DIV),
	RISSE_NORMAL_AND_PROPERTY_ACCESSER(VM_IDIV),
	RISSE_NORMAL_AND_PROPERTY_ACCESSER(VM_MUL),

	VM_BNOT, VM_TYPEOF, VM_TYPEOFD, VM_TYPEOFI, VM_EVAL, VM_EEXP, VM_CHKINS,
	VM_ASC, VM_CHR, VM_NUM, VM_CHS, VM_INV, VM_CHKINV,
	VM_INT, VM_REAL, VM_STR, VM_OCTET,
	VM_CALL, VM_CALLD, VM_CALLI, VM_NEW,
	VM_GPD, VM_SPD, VM_SPDE, VM_SPDEH, VM_GPI, VM_SPI, VM_SPIE,
	VM_GPDS, VM_SPDS, VM_GPIS, VM_SPIS,  VM_SETP, VM_GETP,
	VM_DELD, VM_DELI, VM_SRV, VM_RET, VM_ENTRY, VM_EXTRY, VM_THROW,
	VM_CHGTHIS, VM_GLOBAL, VM_ADDCI, VM_REGMEMBER, VM_DEBUGGER,

	__VM_LAST /* = last mark ; this is not a real operation code */} ;

#undef RISSE_NORMAL_AND_PROPERTY_ACCESSER
//---------------------------------------------------------------------------
enum tRisseSubType{ stNone=VM_NOP, stEqual=VM_CP, stBitAND=VM_BAND, stBitOR=VM_BOR,
	stBitXOR=VM_BXOR, stSub=VM_SUB, stAdd=VM_ADD, stMod=VM_MOD, stDiv=VM_DIV,
	stIDiv = VM_IDIV,
	stMul=VM_MUL, stLogOR=VM_LOR, stLogAND=VM_LAND, stSAR=VM_SAR, stSAL=VM_SAL,
	stSR=VM_SR,

	stPreInc = __VM_LAST, stPreDec, stPostInc, stPostDec, stDelete, stFuncCall,
		stIgnorePropGet, stIgnorePropSet, stTypeOf} ;
//---------------------------------------------------------------------------
enum tRisseFuncArgType { fatNormal, fatExpand, fatUnnamedExpand };
//---------------------------------------------------------------------------
enum tRisseContextType
{
	ctTopLevel,
	ctFunction,
	ctExprFunction,
	ctProperty,
	ctPropertySetter,
	ctPropertyGetter,
	ctClass,
	ctSuperClassGetter,
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// tRisseExprNode
//---------------------------------------------------------------------------
class tRisseExprNode
{
	risse_int Op;
	risse_int Position;
	std::vector<tRisseExprNode*> *Nodes;
	tRisseVariant *Val;

public:
	tRisseExprNode();
	~tRisseExprNode() { Clear(); }

	void Clear();

	void SetOpecode(risse_int op) { Op = op; }
	void SetPosition(risse_int pos) { Position = pos; }
	void SetValue(const tRisseVariant &val)
	{
		if(!Val)
			Val = new tRisseVariant(val);
		else
			Val->CopyRef(val);
	}
	void Add(tRisseExprNode *n);

	risse_int GetOpecode() const { return Op; }
	risse_int GetPosition() const { return Position; }
	tRisseVariant & GetValue() { if(!Val) return *(tRisseVariant*)NULL; else return *Val; }
	tRisseExprNode * operator [] (risse_int i) const { if(!Nodes) return NULL; else return (*Nodes)[i]; }
	unsigned int GetSize() const { if(Nodes) return Nodes->size(); else return 0;}
};
//---------------------------------------------------------------------------
// tRisseInterCodeContext - Intermediate Code Context
//---------------------------------------------------------------------------
/*
	this class implements iRisseDispatch2;
	the object can be directly treated as function, class, property handlers.
*/

class tRisseScriptBlock;
class tRisseInterCodeContext : public tRisseCustomObject
{
	typedef tRisseCustomObject inherited;

public:
	tRisseInterCodeContext(tRisseInterCodeContext *parant,
		const risse_char *name, tRisseScriptBlock *block, tRisseContextType type);
	virtual ~tRisseInterCodeContext();

protected:
	void Finalize(void);
	//------------------------------------------------------- compiling stuff

public:
	void ClearNodesToDelete(void);

public:
	struct tSubParam
	{
		tRisseSubType SubType;
		risse_int SubFlag;
		risse_int SubAddress; 

		tSubParam()
			{ SubType = stNone, SubFlag = 0, SubAddress = 0; }
	};
	struct tSourcePos
	{
		risse_int CodePos;
		risse_int SourcePos;
		static int
			SortFunction(const void *a, const void *b);
	};

private:

	enum tNestType { ntBlock, ntWhile, ntDoWhile, ntFor,
		ntSwitch, ntIf, ntElse, ntTry, ntWith };

	struct tNestData
	{
		tNestType Type;
		risse_int VariableCount;
		union
		{
			bool VariableCreated;
			bool IsFirstCase;
		};
		risse_int RefRegister;
		risse_int StartIP;
		risse_int LoopStartIP;
		std::vector<risse_int> ContinuePatchVector;
		std::vector<risse_int> ExitPatchVector;
		risse_int Patch1;
		risse_int Patch2;
		tRisseExprNode *PostLoopExpr;
	};

	struct tFixData
	{
		risse_int StartIP;
		risse_int Size;
		risse_int NewSize;
		bool BeforeInsertion;
		risse_int32 *Code;

		tFixData(risse_int startip, risse_int size, risse_int newsize,
			risse_int32 *code, bool beforeinsertion)
			{ StartIP =startip, Size = size, NewSize = newsize,
				Code = code, BeforeInsertion = beforeinsertion; }
		tFixData(const tFixData &fixdata)
			{
				Code = NULL;
				operator =(fixdata);
			}
		tFixData & operator = (const tFixData &fixdata)
			{
				if(Code) delete [] Code;
				StartIP = fixdata.StartIP;
				Size = fixdata.Size;
				NewSize = fixdata.NewSize;
				BeforeInsertion = fixdata.BeforeInsertion;
				Code = new risse_int32[NewSize];
				memcpy(Code, fixdata.Code, sizeof(risse_int32)*NewSize);
				return *this;
			}
		~tFixData() { if(Code) delete [] Code; }
	};

	struct tNonLocalFunctionDecl
	{
		risse_int DataPos;
		risse_int NameDataPos;
		bool ChangeThis;
		tNonLocalFunctionDecl(risse_int datapos, risse_int namedatapos, bool changethis)
			{ DataPos = datapos, NameDataPos = namedatapos; ChangeThis = changethis; }
	};

	struct tFuncArgItem
	{
		risse_int Register;
		tRisseFuncArgType Type;
		tFuncArgItem(risse_int reg, tRisseFuncArgType type = fatNormal)
		{
			Register = reg;
			Type = type;
		}
	};

	struct tFuncArg
	{
		bool IsOmit;
		bool HasExpand;
		std::vector <tFuncArgItem> ArgVector;
		tFuncArg() { IsOmit = HasExpand = false; }
	};

	struct tArrayArg
	{
		risse_int Object;
		risse_int Counter;
	};

	risse_int FrameBase;

	risse_int32 * CodeArea;
	risse_int CodeAreaCapa;
	risse_int CodeAreaSize;

	tRisseVariant ** _DataArea;
	risse_int _DataAreaSize;
	risse_int _DataAreaCapa;
	tRisseVariant * DataArea;
	risse_int DataAreaSize;

	tRisseLocalNamespace Namespace;

	std::vector<tRisseExprNode *> NodeToDeleteVector;

	std::vector<tRisseExprNode *> CurrentNodeVector;

	std::stack<tFuncArg> FuncArgStack;

	std::stack<tArrayArg> ArrayArgStack;

	risse_int PrevIfExitPatch;
	std::vector<tNestData> NestVector;


	std::list<risse_int> JumpList;
	std::list<tFixData> FixList;

	std::vector<tNonLocalFunctionDecl> NonLocalFunctionDeclVector;

	risse_int FunctionRegisterCodePoint;

	risse_int PrevSourcePos;
	bool SourcePosArraySorted;
//	std::vector<tSourcePos> SourcePosVector;
	tSourcePos *SourcePosArray;
	risse_int SourcePosArraySize;
	risse_int SourcePosArrayCapa;

	tRisseExprNode *SuperClassExpr;

	risse_int VariableReserveCount;

	bool AsGlobalContextMode;

	risse_int MaxFrameCount;
	risse_int MaxVariableCount;

	risse_int FuncDeclArgCount;
	risse_int FuncDeclUnnamedArgArrayBase;
	risse_int FuncDeclCollapseBase;

	std::vector<risse_int> SuperClassGetterPointer;

	risse_char *Name;
	tRisseInterCodeContext *Parent;
	tRisseScriptBlock *Block;
	tRisseContextType ContextType;
	tRisseInterCodeContext *PropSetter;
	tRisseInterCodeContext *PropGetter;
	tRisseInterCodeContext *SuperClassGetter;

public:
	tRisseContextType GetContextType() const { return ContextType; }
	const risse_char *GetContextTypeName() const;

	ttstr GetShortDescription() const;
	ttstr GetShortDescriptionWithClassName() const;

	tRisseScriptBlock * GetBlock() const { return Block; }

private:
	void OutputWarning(const risse_char *msg, risse_int pos = -1);

private:
	risse_int PutCode(risse_int32 num, risse_int32 pos=-1);
	risse_int PutData(const tRisseVariant &val);

	void AddJumpList(void) { JumpList.push_back(CodeAreaSize); }

	void SortSourcePos();

	void FixCode(void);
	void RegisterFunction();

	risse_int _GenNodeCode(risse_int & frame, tRisseExprNode *node, risse_uint32 restype,
		risse_int reqresaddr, const tSubParam & param);
	risse_int GenNodeCode(risse_int & frame, tRisseExprNode *node, risse_uint32 restype,
		risse_int reqresaddr, const tSubParam & param);

	// restypes
	#define RISSE_RT_NEEDED 0x0001   // result needed
	#define RISSE_RT_CFLAG  0x0002   // condition flag needed
	// result value
	#define RISSE_GNC_CFLAG (1<<(sizeof(risse_int)*8-1)) // true logic
	#define RISSE_GNC_CFLAG_I (RISSE_GNC_CFLAG+1) // inverted logic


	void StartFuncArg();
	void AddFuncArg(risse_int addr, tRisseFuncArgType type = fatNormal);
	void EndFuncArg();
	void AddOmitArg();

	void DoNestTopExitPatch(void);
	void DoContinuePatch(tNestData & nestdata);

	void ClearLocalVariable(risse_int top, risse_int bottom);

	void ClearFrame(risse_int &frame, risse_int base = -1);

	static void _output_func(const risse_char *msg, const risse_char *comment,
		risse_int addr, const risse_int32 *codestart, risse_int size, void *data);
	static void _output_func_src(const risse_char *msg, const risse_char *name,
		risse_int line, void *data);

public:
	void Commit();

	risse_uint GetCodeSize() const { return CodeAreaSize; }
	risse_uint GetDataSize() const { return DataAreaSize; }

	risse_int CodePosToSrcPos(risse_int codepos) const;
	risse_int FindSrcLineStartCodePos(risse_int codepos) const;

	ttstr GetPositionDescriptionString(risse_int codepos) const;

	void AddLocalVariable(const risse_char *name, risse_int init=0);
	void InitLocalVariable(const risse_char *name, tRisseExprNode *node);
	void InitLocalFunction(const risse_char *name, risse_int data);

	void CreateExprCode(tRisseExprNode *node);

	void EnterWhileCode(bool do_while);
	void CreateWhileExprCode(tRisseExprNode *node, bool do_while);
	void ExitWhileCode(bool do_while);

	void EnterIfCode();
	void CreateIfExprCode(tRisseExprNode *node);
	void ExitIfCode();
	void EnterElseCode();
	void ExitElseCode();

	void EnterForCode(bool varcreate);
	void CreateForExprCode(tRisseExprNode *node);
	void SetForThirdExprCode(tRisseExprNode *node);
	void ExitForCode();

	void EnterSwitchCode(tRisseExprNode *node);
	void ExitSwitchCode();
	void ProcessCaseCode(tRisseExprNode *node);

	void EnterWithCode(tRisseExprNode *node);
	void ExitWithCode();

	void DoBreak();
	void DoContinue();

	void DoDebugger();

	void ReturnFromFunc(tRisseExprNode *node);

	void EnterTryCode();
	void EnterCatchCode(const risse_char *name);
	void ExitTryCode();

	void ProcessThrowCode(tRisseExprNode *node);

	void CreateExtendsExprCode(tRisseExprNode *node, bool hold);
	void CreateExtendsExprProxyCode(tRisseExprNode *node);

	void EnterBlock();
	void ExitBlock();

	void GenerateFuncCallArgCode();

	void AddFunctionDeclArg(const risse_char *varname, tRisseExprNode *init);
	void AddFunctionDeclArgCollapse(const risse_char *varname);

	void SetPropertyDeclArg(const risse_char *varname);

	const risse_char * GetName() const ;

	void PushCurrentNode(tRisseExprNode *node);
	tRisseExprNode * GetCurrentNode();
	void PopCurrentNode();

	tRisseExprNode * MakeConstValNode(const tRisseVariant &val);

	tRisseExprNode * MakeNP0(risse_int opecode);
	tRisseExprNode * MakeNP1(risse_int opecode, tRisseExprNode * node1);
	tRisseExprNode * MakeNP2(risse_int opecode, tRisseExprNode * node1, tRisseExprNode * node2);
	tRisseExprNode * MakeNP3(risse_int opecode, tRisseExprNode * node1, tRisseExprNode * node2,
		tRisseExprNode * node3);

	//---------------------------------------------------------- disassembler
	// implemented in risseDisassemble.cpp

	static tRisseString GetValueComment(const tRisseVariant &val);

	void Disassemble(
		void (*output_func)(const risse_char *msg, const risse_char *comment,
		risse_int addr, const risse_int32 *codestart, risse_int size, void *data),
		void (*output_func_src)(const risse_char *msg, const risse_char *name,
			risse_int line, void *data), void *data, risse_int start = 0, risse_int end = 0);
	void Disassemble(void (*output_func)(const risse_char *msg, void* data), void *data,
		risse_int start = 0, risse_int end = 0);
	void Disassemble(risse_int start = 0, risse_int end = 0);
	void DisassenbleSrcLine(risse_int codepos);


	//--------------------------------------------------------- execute stuff
	// implemented in InterCodeExec.cpp
private:
	void DisplayExceptionGeneratedCode(risse_int codepos, const tRisseVariant *ra);

	void ThrowScriptException(tRisseVariant &val, tRisseScriptBlock *block, risse_int srcpos);

//	void ExecuteAsGlobal(tRisseVariant *result);
	void ExecuteAsFunction(iRisseDispatch2 *objthis, tRisseVariant **args,
		risse_int numargs,tRisseVariant *result, risse_int start_ip);
	risse_int ExecuteCode(tRisseVariant *ra, risse_int startip, tRisseVariant **args,
		risse_int numargs, tRisseVariant *result);
	risse_int ExecuteCodeInTryBlock(tRisseVariant *ra, risse_int startip,
		tRisseVariant **args, risse_int numargs, tRisseVariant *result,
		risse_int catchip, risse_int exobjreg);

	static void ContinuousClear(tRisseVariant *ra, const risse_int32 *code);
	void GetPropertyDirect(tRisseVariant *ra, const risse_int32 *code,
		risse_uint32 flags);
	void SetPropertyDirect(tRisseVariant *ra, const risse_int32 *code,
		risse_uint32 flags);
	static void GetProperty(tRisseVariant *ra, const risse_int32 *code);
	static void SetProperty(tRisseVariant *ra, const risse_int32 *code);
	static void GetPropertyIndirect(tRisseVariant *ra, const risse_int32 *code,
		risse_uint32 flags);
	static void SetPropertyIndirect(tRisseVariant *ra, const risse_int32 *code,
		risse_uint32 flags);
	void OperatePropertyDirect(tRisseVariant *ra, const risse_int32 *code,
		risse_uint32 ope);
	static void OperatePropertyIndirect(tRisseVariant *ra, const risse_int32 *code,
		risse_uint32 ope);
	static void OperateProperty(tRisseVariant *ra, const risse_int32 *code, risse_uint32 ope);
	void OperatePropertyDirect0(tRisseVariant *ra, const risse_int32 *code,
		risse_uint32 ope);
	static void OperatePropertyIndirect0(tRisseVariant *ra, const risse_int32 *code,
		risse_uint32 ope);
	static void OperateProperty0(tRisseVariant *ra, const risse_int32 *code, risse_uint32 ope);
	void DeleteMemberDirect(tRisseVariant *ra, const risse_int32 *code);
	static void DeleteMemberIndirect(tRisseVariant *ra, const risse_int32 *code);
	void TypeOfMemberDirect(tRisseVariant *ra, const risse_int32 *code,
		risse_uint32 flags);
	static void TypeOfMemberIndirect(tRisseVariant *ra, const risse_int32 *code,
		risse_uint32 flags);
	risse_int CallFunction(tRisseVariant *ra, const risse_int32 *code,
		tRisseVariant **args,
		risse_int numargs);
	risse_int CallFunctionDirect(tRisseVariant *ra, const risse_int32 *code,
		tRisseVariant **args, risse_int numargs);
	risse_int CallFunctionIndirect(tRisseVariant *ra, const risse_int32 *code,
		tRisseVariant **args, risse_int numargs);
	static void AddClassInstanceInfo(tRisseVariant *ra, const risse_int32 *code);
	void ProcessStringFunction(const risse_char *member, const ttstr & target,
		tRisseVariant **args, risse_int numargs, tRisseVariant *result);
	void ProcessOctetFunction(const risse_char *member, const ttstr & target,
		tRisseVariant **args, risse_int numargs, tRisseVariant *result);
	static void TypeOf(tRisseVariant &val);
	void Eval(tRisseVariant &val, iRisseDispatch2 * objthis, bool resneed);
	static void CharacterCodeOf(tRisseVariant &val);
	static void CharacterCodeFrom(tRisseVariant &val);
	static void InstanceOf(const tRisseVariant &name, tRisseVariant &targ);

	void RegisterObjectMember(iRisseDispatch2 * dest);

public:
	// iRisseDispatch2 implementation
	risse_error
	FuncCall(
		risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
			tRisseVariant *result,
		risse_int numparams, tRisseVariant **param, iRisseDispatch2 *objthis);

	risse_error
	PropGet(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
		tRisseVariant *result,
		iRisseDispatch2 *objthis);

	risse_error
	PropSet(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		const tRisseVariant *param,
		iRisseDispatch2 *objthis);


	risse_error
	CreateNew(risse_uint32 flag, const risse_char * membername, risse_uint32 *hint,
		iRisseDispatch2 **result, risse_int numparams, tRisseVariant **param,
		iRisseDispatch2 *objthis);

	risse_error
	IsInstanceOf(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		const risse_char *classname,
		iRisseDispatch2 *objthis);

	risse_error
		GetCount(risse_int *result, const risse_char *membername, risse_uint32 *hint,
		 iRisseDispatch2 *objthis);

	risse_error
	PropSetByVS(risse_uint32 flag, tRisseVariantString *mambername,
		const tRisseVariant *param,
		iRisseDispatch2 *objthis)
	{
		return RISSE_E_NOTIMPL;
	}

	risse_error
		DeleteMember(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		 iRisseDispatch2 *objthis);

	risse_error
		Invalidate(risse_uint32 flag, const risse_char *membername,  risse_uint32 *hint,
		iRisseDispatch2 *objthis);

	risse_error
		IsValid(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		 iRisseDispatch2 *objthis);

	risse_error
		Operation(risse_uint32 flag, const risse_char *membername, risse_uint32 *hint,
		 tRisseVariant *result,
			const tRisseVariant *param,	iRisseDispatch2 *objthis);

};
//---------------------------------------------------------------------------
}
#endif
