//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief バイトコードジェネレータ
//---------------------------------------------------------------------------
#include "../prec.h"

#include "risseCodeGen.h"
#include "risseSSAForm.h"
#include "risseSSAVariable.h"
#include "risseCompiler.h"
#include "../risseExceptionClass.h"
#include "../risseScriptBlockClass.h"
#include "../risseCodeBlock.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(52364,51758,14226,19534,54934,29340,32493,12680);



//---------------------------------------------------------------------------
tCodeGenerator::tCodeGenerator(tSSAForm * form,
	tCodeGenerator * parent, bool useparentframe, risse_size nestlevel)
{
	Form = form;
	Parent = parent;
	UseParentFrame = useparentframe;
	NestLevel = nestlevel;
	RISSE_ASSERT(!(!Parent && UseParentFrame)); // UseParentFrame が真の場合は親がなければならない
	RegisterBase = 0;
	LastCode = ocNoOperation;
	LastJumpTarget = NULL;
	NumUsedRegs = 0;
	MaxNumUsedRegs = 0;
	SharedRegCount = 0;
	SharedRegNameMap = parent ? parent->SharedRegNameMap : new tNamedRegMap();
		// SharedRegNameMap は親がある場合は親と共有する
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::SetRegisterBase()
{
	if(Parent)
	{
		// 親のコードジェネレータのフレームを使う場合は親のコードジェネレータと
		// 重ならない位置に子のレジスタを配置しなくてはならない
		if(UseParentFrame)
			RegisterBase = Parent->RegisterBase + Parent->MaxNumUsedRegs;
		else
			RegisterBase = 0;
	}
	else
	{
		RegisterBase = 0;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::AddBlockMap(const tSSABlock * block)
{
	// 直前がもしocJump で、そのジャンプ先が block だった場合、
	// 最後の jump 文を取り消す
	if(LastCode == ocJump && LastJumpTarget == block)
	{
		RISSE_ASSERT(Code.size() >= 2); // 2 = ジャンプ命令のワード数
		Code.resize(Code.size() - 2);
		PendingBlockJumps.pop_back(); 
	}

	BlockMap.insert(tBlockMap::value_type(block, Code.size()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::AddPendingBlockJump(const tSSABlock * block, risse_size insn_pos)
{
	PendingBlockJumps.push_back(tPendingBlockJump(block, Code.size(), insn_pos));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tCodeGenerator::FindConst(const tVariant & value)
{
	// 直近のMaxConstSearch個をConstsから探し、一致すればそのインデックスを返す
	risse_size const_size = Consts.size();
	risse_size search_limit = const_size > MaxConstSearch ? MaxConstSearch : const_size;
	for(risse_size n = 0; n < search_limit; n++)
	{
		if(Consts[const_size - n - 1].StrictEqual(value))  // DiscEqual による比較では不十分
			return const_size - n - 1; // 見つかった
	}

	// 見つからなかったので Consts に追加
	Consts.push_back(value);
	return const_size; // これは Consts に push_back された値のインデックスを表しているはず
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tCodeGenerator::GetRegNum(const tSSAVariable * var)
{
	risse_size reg = var->GetAssignedRegister();
	RISSE_ASSERT(reg != risse_size_max);

	reg += RegisterBase;

	UpdateMaxNumUsedRegs(reg + 1);

	return reg;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::UpdateMaxNumUsedRegs(risse_size max)
{
	if(MaxNumUsedRegs < max) MaxNumUsedRegs = max; // 最大値を更新
	if(Parent && UseParentFrame) Parent->UpdateMaxNumUsedRegs(max); // 再帰
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::FindSharedRegNameMap(const tString & name, risse_uint16 &nestlevel, risse_uint16 &regnum)
{
	tNamedRegMap::iterator f = SharedRegNameMap->find(name);
	if(f != SharedRegNameMap->end())
	{
		// 見つかった
		nestlevel = (f->second >> 16) & 0xffff;
		regnum    = f->second & 0xffff;
		return;
	}

	// 見つからなかった
	RISSE_ASSERT(!"shared variable not found"); // 変数は見つからなければならない

	// 親も同じマップを共有しているので親に再帰する必要はない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::AddSharedRegNameMap(const tString & name)
{
	FPrint(stderr, (RISSE_WS("Adding ") + name +
		RISSE_WS(" at nestlevel ") +
		tString::AsString(risse_int64(NestLevel)) + RISSE_WS("\n")).c_str() );

	RISSE_ASSERT(SharedRegNameMap->find(name) == SharedRegNameMap->end()); // 二重挿入は許されない

	// いまのところ、nestlevel と regnum は、VMコード中において
	// それぞれ 32bit 整数の 上位16bitと下位16ビットを使うので、
	// それぞれがその16bitの上限を超えることができない。
	if(NestLevel > 0xffff)
		tCompileExceptionClass::Throw(
			Form->GetFunction()->GetFunctionGroup()->
				GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
			tString(RISSE_WS_TR("too deep function nest level")), Form->GetScriptBlockInstance(),
				GetSourceCodePosition()); // まずあり得ないと思うが ...

	risse_size reg_num = SharedRegCount;
	if(reg_num > 0xffff)
		tCompileExceptionClass::Throw(
			Form->GetFunction()->GetFunctionGroup()->
				GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
			tString(RISSE_WS_TR("too many shared variables")), Form->GetScriptBlockInstance(),
				GetSourceCodePosition()); // まずあり得ないと思うが ...

	SharedRegNameMap->insert(tNamedRegMap::value_type(name, (NestLevel<<16) + reg_num));
	SharedRegCount ++;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::AddBindingRegNameMap(const tString & name,
								risse_uint16 nestlevel, risse_uint16 regnum)
{
	FPrint(stderr, (RISSE_WS("Adding binding ") + name +
		RISSE_WS(" at nestlevel ") +
		tString::AsString(risse_int64(nestlevel)) + RISSE_WS(", regnum ") +
		tString::AsString(risse_int64(regnum)) +  RISSE_WS("\n")).c_str() );

	RISSE_ASSERT(SharedRegNameMap->find(name) == SharedRegNameMap->end()); // 二重挿入は許されない

	SharedRegNameMap->insert(tNamedRegMap::value_type(name, (nestlevel<<16) + regnum));

	// この関数グループ内のどのネストレベルにも属さない変数を追加することを
	// 目的としているので、 SharedRegCount はインクリメントしない。
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tCodeGenerator::GetSharedRegCount() const
{
	return SharedRegCount;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tCodeGenerator::QuerySharedVariableNestCount() const
{
	risse_size max_nest_count = 0;
	for(tNamedRegMap::const_iterator i = SharedRegNameMap->begin();
		i != SharedRegNameMap->end(); i++)
	{
		risse_size nestlevel = (i->second >> 16) & 0xffff;
		nestlevel ++;
			// ↑ たとえば、nestlevel 0 に共有変数があるということはネストカウントは 1
			// なので 1 を加算する
		if(max_nest_count < nestlevel) max_nest_count = nestlevel;
	}

	return max_nest_count;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tCodeGenerator::RegisterVariableMapForChildren(const tSSAVariable * var, const tString & name)
{
	risse_size reg_num = var->GetAssignedRegister();
	RISSE_ASSERT(reg_num != risse_size_max);

	reg_num += RegisterBase;

	UpdateMaxNumUsedRegs(reg_num + 1);

	// VariableMapForChildren に名前と変数番号を登録する
	RISSE_ASSERT(VariableMapForChildren.find(name) == VariableMapForChildren.end());
	VariableMapForChildren.insert(tNamedRegMap::value_type(name, reg_num));

	return reg_num;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tCodeGenerator::FindVariableMapForChildren(const tString & name)
{
	// VariableMapForChildren から指定された変数を探す
	tNamedRegMap::iterator f = VariableMapForChildren.find(name);
	RISSE_ASSERT(f != VariableMapForChildren.end());
	return f->second;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::FixCode()
{
	// サイズをチェック
	if(Code.size() != static_cast<risse_uint32>(Code.size()))
		tCompileExceptionClass::Throw(
			Form->GetFunction()->GetFunctionGroup()->
				GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
			tString(RISSE_WS_TR("too large code size")),
			Form->GetScriptBlockInstance(), 0); // まずあり得ないと思うが ...

	// ジャンプアドレスのfixup
	// ジャンプアドレスは 命令開始位置に対する相対指定となる。
	// これにより 実行効率がよくなる (なぜならば絶対位置を保持する必要がないので)
	for(gc_vector<tPendingBlockJump>::iterator i =
		PendingBlockJumps.begin(); i != PendingBlockJumps.end(); i++)
	{
		tBlockMap::iterator b = BlockMap.find(i->Block);
		RISSE_ASSERT(b != BlockMap.end());
		Code[i->EmitPosition] = static_cast<risse_uint32>(
					static_cast<risse_int32>(b->second - i->InsnPosition));
			// コードサイズは uint32 内に収まることがこの関数の最初で保証されるので
			// オフセットは安全に uint32 にキャストしてよい
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::SetSourceCodePosition(risse_size pos)
{
	if(CodeToSourcePosition.size() > 0 &&
		CodeToSourcePosition.back().second == pos) return;
			// 連続する重複するソースコード上の位置は、たとえVMコードの位置が異なったとしても挿入しない
	CodeToSourcePosition.push_back(std::pair<risse_size, risse_size>(Code.size(), pos));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tCodeGenerator::GetSourceCodePosition() const
{
	if(CodeToSourcePosition.size() > 0) return CodeToSourcePosition.back().second;
	return risse_size_max;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutNoOperation()
{
	PutCode(ocNoOperation);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssign(const tSSAVariable * dest, const tSSAVariable * src)
{
	if(dest != src)
	{
		// 同じ変数同士のコピーは明らかに要らないのでここでもブロック
		risse_size dest_num = GetRegNum(dest);
		risse_size src_num  = GetRegNum(src);
		if(dest_num != src_num)
		{
			PutCode(ocAssign);
			PutWord(dest_num);
			PutWord(src_num);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssign(risse_size dest, const tSSAVariable * src)
{
	PutCode(ocAssign);
	PutWord(dest);
	PutWord(GetRegNum(src));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssign(const tSSAVariable * dest, risse_size src)
{
	PutCode(ocAssign);
	PutWord(GetRegNum(dest));
	PutWord(src);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssign(const tSSAVariable * dest, const tVariant & value)
{
	PutCode(ocAssignConstant);
	PutWord(GetRegNum(dest));
	PutWord(FindConst(value));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssign(const tSSAVariable * dest, tOpCode code)
{
	// 一応 code を assert (完全ではない)
	RISSE_ASSERT(VMInsnInfo[code].Flags[0] == tVMInsnInfo::vifRegister);
	RISSE_ASSERT(VMInsnInfo[code].Flags[1] == tVMInsnInfo::vifVoid);

	PutCode(code);
	PutWord(GetRegNum(dest));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssignNewRegExp(const tSSAVariable * dest,
			const tSSAVariable * pattern, const tSSAVariable * flags)
{
	PutCode(ocAssignNewRegExp);
	PutWord(GetRegNum(dest));
	PutWord(GetRegNum(pattern));
	PutWord(GetRegNum(flags));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssignNewFunction(const tSSAVariable * dest,
			const tSSAVariable * body)
{
	PutCode(ocAssignNewFunction);
	PutWord(GetRegNum(dest));
	PutWord(GetRegNum(body));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssignNewProperty(const tSSAVariable * dest,
			const tSSAVariable * getter, const tSSAVariable * setter)
{
	PutCode(ocAssignNewProperty);
	PutWord(GetRegNum(dest));
	PutWord(GetRegNum(getter));
	PutWord(GetRegNum(setter));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssignNewClass(const tSSAVariable * dest,
			const tSSAVariable * super, const tSSAVariable * name)
{
	PutCode(ocAssignNewClass);
	PutWord(GetRegNum(dest));
	PutWord(GetRegNum(super));
	PutWord(GetRegNum(name));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssignNewModule(const tSSAVariable * dest,
			const tSSAVariable * name)
{
	PutCode(ocAssignNewModule);
	PutWord(GetRegNum(dest));
	PutWord(GetRegNum(name));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssignParam(const tSSAVariable * dest, risse_size index)
{
	PutCode(ocAssignParam);
	PutWord(GetRegNum(dest));
	PutWord(static_cast<risse_uint32>(index));
	// index の最大値は MaxArgCount で確実に risse_uint32 で表現できる範囲のため安全にキャストできる
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssignBlockParam(const tSSAVariable * dest, risse_size index)
{
	PutCode(ocAssignBlockParam);
	PutWord(GetRegNum(dest));
	PutWord(static_cast<risse_uint32>(index));
	// index の最大値は MaxArgCount で確実に risse_uint32 で表現できる範囲のため安全にキャストできる
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAddBindingMap(const tSSAVariable * map,
	const tSSAVariable *name, const tString &nname)
{
	risse_uint16 nestlevel = 0, regnum = 0;
	FindSharedRegNameMap(nname, nestlevel, regnum);
	PutCode(ocAddBindingMap);
	PutWord(GetRegNum(map));
	PutWord(GetRegNum(name));
	PutWord((nestlevel << 16) + regnum);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutWrite(const tString & dest, const tSSAVariable * src)
{
	risse_uint16 nestlevel = 0, regnum = 0;
	FindSharedRegNameMap(dest, nestlevel, regnum);
	PutCode(ocWrite);
	PutWord((nestlevel << 16) + regnum);
	PutWord(GetRegNum(src));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutRead(const tSSAVariable * dest, const tString & src)
{
	risse_uint16 nestlevel = 0, regnum = 0;
	FindSharedRegNameMap(src, nestlevel, regnum);
	PutCode(ocRead);
	PutWord(GetRegNum(dest));
	PutWord((nestlevel << 16) + regnum);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutCodeBlockRelocatee(const tSSAVariable * dest, risse_size index)
{
	// 定数領域に仮の値をpushする
	tVariant value(tString(RISSE_WS("<VM block #%1>"),
		tString::AsString((risse_int64)index)));
	risse_size reloc_pos = Consts.size();
	Consts.push_back(value);

	// ocAssignConstant 命令を生成する
	PutCode(ocAssignConstant);
	PutWord(GetRegNum(dest));
	PutWord(reloc_pos);

	// CodeBlockRelocations に情報を入れる
	CodeBlockRelocations.push_back(std::pair<risse_size, risse_size>(reloc_pos, index));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutSetFrame(const tSSAVariable * dest)
{
	PutCode(ocSetFrame);
	PutWord(GetRegNum(dest));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutSetShare(const tSSAVariable * dest)
{
	PutCode(ocSetShare);
	PutWord(GetRegNum(dest));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutFunctionCall(const tSSAVariable * dest,
		const tSSAVariable * func,
		tOpCode code, risse_uint32 expbit,
		const gc_vector<const tSSAVariable *> & args,
		const gc_vector<const tSSAVariable *> & blocks)
{
	RISSE_ASSERT(!(code == ocNew && blocks.size() != 0)); // ブロック付き new はない
	RISSE_ASSERT(!((expbit & FuncCallFlag_Omitted) && args.size() != 0));
		// omit なのに引数があるということはない
	RISSE_ASSERT(code == ocNew || code == ocFuncCall || code == ocTryFuncCall);

	if(code == ocFuncCall && blocks.size() != 0)
		code = ocFuncCallBlock;

	PutCode(code);
	PutWord(GetRegNum(dest));
	PutWord(GetRegNum(func));

	PutWord(expbit); // フラグ

	PutWord(static_cast<risse_uint32>(args.size()));

	if(blocks.size() != 0 || code == ocTryFuncCall)
		PutWord(static_cast<risse_uint32>(blocks.size()));
			// ブロックの個数があるのは ocFuncCallBlock の場合と ocTryFuncCall の場合だけ
			// ocTryFuncCall の場合はブロックありとなしの間に命令の区別が無く、
			// ブロックがあっても無くてもかならずブロックの個数を入れる

	// 引数をput
	for(gc_vector<const tSSAVariable *>::const_iterator i = args.begin();
		i != args.end(); i++)
		PutWord(GetRegNum(*i));
	// ブロックをput
	for(gc_vector<const tSSAVariable *>::const_iterator i = blocks.begin();
		i != blocks.end(); i++)
		PutWord(GetRegNum(*i));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutSync(const tSSAVariable * dest,
	const tSSAVariable * func, const tSSAVariable * lockee)
{
	PutCode(ocSync);
	PutWord(GetRegNum(dest));
	PutWord(GetRegNum(func));
	PutWord(GetRegNum(lockee));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutJump(const tSSABlock * target)
{
	risse_size insn_pos = Code.size();
	PutCode(ocJump);
	LastJumpTarget = target;
	AddPendingBlockJump(target, insn_pos);
	PutWord(0); // 仮
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutBranch(const tSSAVariable * ref,
		const tSSABlock * truetarget, const tSSABlock * falsetarget)
{
	risse_size insn_pos = Code.size();
	PutCode(ocBranch);
	PutWord(GetRegNum(ref));
	AddPendingBlockJump(truetarget, insn_pos);
	PutWord(0); // 仮
	AddPendingBlockJump(falsetarget, insn_pos);
	PutWord(0); // 仮
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutCatchBranch(const tSSAVariable * ref,
		risse_size try_id_idx,
		const gc_vector<tSSABlock *> & targets)
{
	RISSE_ASSERT(targets.size() >= 2);
		// 少なくとも何事も無かった場合と例外が発生した場合のジャンプ先を含む

	// Try識別子用に定数領域に仮の値をpushする
	tVariant cvalue(tString(RISSE_WS("<try id #%1>"),
		tString::AsString((risse_int64)try_id_idx)));
	risse_size reloc_pos = Consts.size();
	Consts.push_back(cvalue);
	TryIdentifierRelocations.push_back(std::pair<risse_size, risse_size>(reloc_pos, try_id_idx));

	// ocCatchBranch を置く
	risse_size insn_pos = Code.size();
	PutCode(ocCatchBranch);
	PutWord(GetRegNum(ref));
	PutWord(static_cast<risse_uint32>(reloc_pos));

	if(static_cast<risse_uint32>(targets.size()) != targets.size())
	{
		// TODO: 例外の発生
	}
	PutWord(static_cast<risse_uint32>(targets.size()));
	for(gc_vector<tSSABlock *>::const_iterator i = targets.begin();
		i != targets.end(); i ++)
	{
		AddPendingBlockJump(*i, insn_pos);
		PutWord(0); // 仮
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutDebugger()
{
	PutCode(ocDebugger);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutThrow(const tSSAVariable * throwee)
{
	PutCode(ocThrow);
	PutWord(GetRegNum(throwee));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutReturn(const tSSAVariable * value)
{
	PutCode(ocReturn);
	PutWord(GetRegNum(value));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutExitTryException(const tSSAVariable * value,
		risse_size try_id_idx, risse_size idx)
{
	// Try識別子用に定数領域に仮の値をpushする
	tVariant cvalue(tString(RISSE_WS("<try id #%1>"),
		tString::AsString((risse_int64)try_id_idx)));
	risse_size reloc_pos = Consts.size();
	Consts.push_back(cvalue);
	TryIdentifierRelocations.push_back(std::pair<risse_size, risse_size>(reloc_pos, try_id_idx));

	// ocExitTryException 命令を書く
	PutCode(ocExitTryException);
	PutWord(value?GetRegNum(value):InvalidRegNum);
	PutWord(static_cast<risse_uint32>(reloc_pos));
	PutWord(static_cast<risse_uint32>(idx));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutGetExitTryValue(const tSSAVariable * dest,
											const tSSAVariable * src)
{
	PutCode(ocGetExitTryValue);
	PutWord(GetRegNum(dest));
	PutWord(GetRegNum(src));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutOperator(tOpCode op, const tSSAVariable * dest,
		const tSSAVariable * arg1)
{
	// 一応 op を assert (完全ではない)
	RISSE_ASSERT(VMInsnInfo[op].Flags[0] == tVMInsnInfo::vifRegister);
	RISSE_ASSERT(VMInsnInfo[op].Flags[1] == tVMInsnInfo::vifRegister);
	RISSE_ASSERT(VMInsnInfo[op].Flags[2] == tVMInsnInfo::vifVoid);

	PutCode(op);
	PutWord(GetRegNum(dest));
	PutWord(GetRegNum(arg1));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutOperator(tOpCode op, const tSSAVariable * dest,
		const tSSAVariable * arg1, const tSSAVariable * arg2)
{
	// 一応 code を assert (完全ではない)
	RISSE_ASSERT(VMInsnInfo[op].Flags[0] == tVMInsnInfo::vifRegister);
	RISSE_ASSERT(VMInsnInfo[op].Flags[1] == tVMInsnInfo::vifRegister);
	RISSE_ASSERT(VMInsnInfo[op].Flags[2] == tVMInsnInfo::vifRegister);
	RISSE_ASSERT(VMInsnInfo[op].Flags[3] == tVMInsnInfo::vifVoid);

	PutCode(op);
	PutWord(GetRegNum(dest));
	PutWord(GetRegNum(arg1));
	PutWord(GetRegNum(arg2));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutOperator(tOpCode op, const tSSAVariable * dest,
		const tSSAVariable * arg1, const tSSAVariable * arg2, const tSSAVariable * arg3)
{
	// 一応 code を assert (完全ではない)
	RISSE_ASSERT(VMInsnInfo[op].Flags[0] == tVMInsnInfo::vifRegister);
	RISSE_ASSERT(VMInsnInfo[op].Flags[1] == tVMInsnInfo::vifRegister);
	RISSE_ASSERT(VMInsnInfo[op].Flags[2] == tVMInsnInfo::vifRegister);
	RISSE_ASSERT(VMInsnInfo[op].Flags[3] == tVMInsnInfo::vifRegister);
	RISSE_ASSERT(VMInsnInfo[op].Flags[3] == tVMInsnInfo::vifVoid);

	PutCode(op);
	PutWord(GetRegNum(dest));
	PutWord(GetRegNum(arg1));
	PutWord(GetRegNum(arg2));
	PutWord(GetRegNum(arg3));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutInContextOf(const tSSAVariable * dest,
		const tSSAVariable * instance, const tSSAVariable * context)
{
	PutCode(context ? ocInContextOf : ocInContextOfDyn);
	PutWord(GetRegNum(dest));
	PutWord(GetRegNum(instance));
	if(context) PutWord(GetRegNum(context));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutGet(tOpCode op, const tSSAVariable * dest,
		const tSSAVariable * obj, const tSSAVariable * name, risse_uint32 flags)
{
	// 一応 code を assert (完全ではない)
	RISSE_ASSERT(VMInsnInfo[op].Flags[0] == tVMInsnInfo::vifRegister);
	RISSE_ASSERT(VMInsnInfo[op].Flags[1] == tVMInsnInfo::vifRegister);
	RISSE_ASSERT(VMInsnInfo[op].Flags[2] == tVMInsnInfo::vifRegister);
	RISSE_ASSERT(VMInsnInfo[op].Flags[3] == tVMInsnInfo::vifVoid);

	RISSE_ASSERT(!(op == ocIGet && flags != 0)); // フラグをもてるのはocDGetのみ

	if(op == ocDGet && flags != 0) op = ocDGetF; // フラグがある場合は ocDGetFを置く
	PutCode(op);
	PutWord(GetRegNum(dest));
	PutWord(GetRegNum(obj));
	PutWord(GetRegNum(name));
	if(op == ocDGetF) PutWord(flags); // フラグを置く
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutSet(tOpCode op, const tSSAVariable * obj,
		const tSSAVariable * name, const tSSAVariable * value, risse_uint32 flags)
{
	// 一応 code を assert (完全ではない)
	RISSE_ASSERT(VMInsnInfo[op].Flags[0] == tVMInsnInfo::vifRegister);
	RISSE_ASSERT(VMInsnInfo[op].Flags[1] == tVMInsnInfo::vifRegister);
	RISSE_ASSERT(VMInsnInfo[op].Flags[2] == tVMInsnInfo::vifRegister);
	RISSE_ASSERT(VMInsnInfo[op].Flags[3] == tVMInsnInfo::vifVoid);

	RISSE_ASSERT(!(op == ocISet && flags != 0));
		// フラグをもてるのはocDSetFのみ

	if(op == ocDSet && flags != 0) op = ocDSetF;
		// フラグがある場合は ocDSetFを置く
	PutCode(op);
	PutWord(GetRegNum(obj));
	PutWord(GetRegNum(name));
	PutWord(GetRegNum(value));
	if(op == ocDSetF) PutWord(flags); // フラグを置く
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutSetAttribute(const tSSAVariable * obj,
		const tSSAVariable * name, risse_uint32 attrib)
{
	// 一応 code を assert (完全ではない)
	PutCode(ocDSetAttrib);
	PutWord(GetRegNum(obj));
	PutWord(GetRegNum(name));
	PutWord(attrib);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssert(const tSSAVariable *cond, const tString & msg)
{
	// assertion コードを置く
	RISSE_ASSERT(VMInsnInfo[ocAssert].Flags[0] == tVMInsnInfo::vifRegister);
	RISSE_ASSERT(VMInsnInfo[ocAssert].Flags[1] == tVMInsnInfo::vifConstant);
	PutCode(ocAssert);
	PutWord(GetRegNum(cond));
	PutWord(FindConst(msg));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssertType(const tSSAVariable *var, tVariant::tType type)
{
	RISSE_ASSERT(VMInsnInfo[ocAssertType].Flags[0] == tVMInsnInfo::vifRegister);
	RISSE_ASSERT(VMInsnInfo[ocAssertType].Flags[1] == tVMInsnInfo::vifOthers);
	PutCode(ocAssertType);
	PutWord(GetRegNum(var));
	PutWord(static_cast<risse_uint32>(type));
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risse
