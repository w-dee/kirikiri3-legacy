//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief バイトコードジェネレータ
//---------------------------------------------------------------------------
#include "../prec.h"

#include "risseCodeGen.h"
#include "risseSSAForm.h"
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
		if(Consts[const_size - n - 1].DiscEqual(value))
			return const_size - n - 1; // 見つかった
	}

	// 見つからなかったので Consts に追加
	Consts.push_back(value);
	return const_size; // これは Consts に push_back された値のインデックスを表しているはず
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tCodeGenerator::FindRegMap(const tSSAVariable * var)
{
	// RegMap から指定された変数を探す
	// 指定された変数が無ければ変数の空きマップからさがし、変数を割り当てる
	tRegMap::iterator f = RegMap.find(var);
	if(f != RegMap.end())
	{
		 // 変数が見つかった
		 return f->second;
	}

	risse_size assigned_reg = AllocateRegister();

	RegMap.insert(tRegMap::value_type(var, assigned_reg)); // RegMapに登録

	return assigned_reg;
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
risse_size tCodeGenerator::AllocateRegister()
{
	// 空きレジスタを探す
	risse_size assigned_reg;
	if(RegFreeMap.size() == 0)
	{
		// 空きレジスタがない
		assigned_reg = NumUsedRegs + RegisterBase; // 新しいレジスタを割り当てる
	}
	else
	{
		// 空きレジスタがある
		assigned_reg = RegFreeMap.back();
		RegFreeMap.pop_back();
	}

	NumUsedRegs ++;
	UpdateMaxNumUsedRegs(NumUsedRegs + RegisterBase); // 最大値を更新

	return assigned_reg;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::FreeRegister(risse_size reg)
{
	RISSE_ASSERT(NumUsedRegs > 0);
	RegFreeMap.push_back(reg); // 変数を空き配列に追加
	NumUsedRegs --;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::FreeRegister(const tSSAVariable *var)
{
	// RegMap から指定された変数を開放する
	tRegMap::iterator f = RegMap.find(var);
	if(f != RegMap.end())
	{
		// 同じ変数が２度開放される可能性があることに注意すること
		// (一つの文で同じ変数を２回以上useしてたりするとそうなるかもしれない)
		FreeRegister(f->second);
		RegMap.erase(f);
	}
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
risse_size tCodeGenerator::FindOrRegisterVariableMapForChildren(const tString & name)
{
	// VariableMapForChildren から指定された変数を探す
	// 指定された変数が無ければ変数の空きマップからさがし、変数を割り当てる
	tNamedRegMap::iterator f = VariableMapForChildren.find(name);
	if(f != VariableMapForChildren.end())
	{
		// 変数が見つかった
		return f->second;
	}

	// 変数がないので自分からレジスタを割り当てる
	risse_size assigned_reg = AllocateRegister();
	VariableMapForChildren.insert(tNamedRegMap::value_type(name, assigned_reg));

	return assigned_reg;
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
void tCodeGenerator::FreeVariableMapForChildren()
{
	// VariableMapForChildrenにある変数を自分からすべて開放する
	// TODO: 一時的にこれ無効化
/*
	for(tNamedRegMap::iterator i = VariableMapForChildren.begin(); i != VariableMapForChildren.end(); i++)
		FreeRegister(i->second);
	VariableMapForChildren.clear();
*/
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
	PutWord(ocNoOperation);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssign(const tSSAVariable * dest, const tSSAVariable * src)
{
	if(dest != src)
	{
		// 同じ変数同士のコピーは明らかに要らないのでここでもブロック
		PutWord(ocAssign);
		PutWord(FindRegMap(dest));
		PutWord(FindRegMap(src));
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssign(risse_size dest, const tSSAVariable * src)
{
	PutWord(ocAssign);
	PutWord(dest);
	PutWord(FindRegMap(src));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssign(const tSSAVariable * dest, risse_size src)
{
	PutWord(ocAssign);
	PutWord(FindRegMap(dest));
	PutWord(src);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssign(const tSSAVariable * dest, const tVariant & value)
{
	PutWord(ocAssignConstant);
	PutWord(FindRegMap(dest));
	PutWord(FindConst(value));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssign(const tSSAVariable * dest, tOpCode code)
{
	// 一応 code を assert (完全ではない)
	RISSE_ASSERT(VMInsnInfo[code].Flags[0] == tVMInsnInfo::vifRegister);
	RISSE_ASSERT(VMInsnInfo[code].Flags[1] == tVMInsnInfo::vifVoid);

	PutWord(static_cast<risse_uint32>(code));
	PutWord(FindRegMap(dest));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssignNewRegExp(const tSSAVariable * dest,
			const tSSAVariable * pattern, const tSSAVariable * flags)
{
	PutWord(ocAssignNewRegExp);
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(pattern));
	PutWord(FindRegMap(flags));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssignNewFunction(const tSSAVariable * dest,
			const tSSAVariable * body)
{
	PutWord(ocAssignNewFunction);
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(body));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssignNewProperty(const tSSAVariable * dest,
			const tSSAVariable * getter, const tSSAVariable * setter)
{
	PutWord(ocAssignNewProperty);
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(getter));
	PutWord(FindRegMap(setter));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssignNewClass(const tSSAVariable * dest,
			const tSSAVariable * super, const tSSAVariable * name)
{
	PutWord(ocAssignNewClass);
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(super));
	PutWord(FindRegMap(name));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssignNewModule(const tSSAVariable * dest,
			const tSSAVariable * name)
{
	PutWord(ocAssignNewModule);
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(name));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssignParam(const tSSAVariable * dest, risse_size index)
{
	PutWord(ocAssignParam);
	PutWord(FindRegMap(dest));
	PutWord(static_cast<risse_uint32>(index));
	// index の最大値は MaxArgCount で確実に risse_uint32 で表現できる範囲のため安全にキャストできる
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssignBlockParam(const tSSAVariable * dest, risse_size index)
{
	PutWord(ocAssignBlockParam);
	PutWord(FindRegMap(dest));
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
	PutWord(ocAddBindingMap);
	PutWord(FindRegMap(map));
	PutWord(FindRegMap(name));
	PutWord((nestlevel << 16) + regnum);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutWrite(const tString & dest, const tSSAVariable * src)
{
	risse_uint16 nestlevel = 0, regnum = 0;
	FindSharedRegNameMap(dest, nestlevel, regnum);
	PutWord(ocWrite);
	PutWord((nestlevel << 16) + regnum);
	PutWord(FindRegMap(src));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutRead(const tSSAVariable * dest, const tString & src)
{
	risse_uint16 nestlevel = 0, regnum = 0;
	FindSharedRegNameMap(src, nestlevel, regnum);
	PutWord(ocRead);
	PutWord(FindRegMap(dest));
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
	PutWord(ocAssignConstant);
	PutWord(FindRegMap(dest));
	PutWord(reloc_pos);

	// CodeBlockRelocations に情報を入れる
	CodeBlockRelocations.push_back(std::pair<risse_size, risse_size>(reloc_pos, index));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutSetFrame(const tSSAVariable * dest)
{
	PutWord(ocSetFrame);
	PutWord(FindRegMap(dest));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutSetShare(const tSSAVariable * dest)
{
	PutWord(ocSetShare);
	PutWord(FindRegMap(dest));
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

	PutWord(static_cast<risse_uint32>(code));
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(func));

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
		PutWord(FindRegMap(*i));
	// ブロックをput
	for(gc_vector<const tSSAVariable *>::const_iterator i = blocks.begin();
		i != blocks.end(); i++)
		PutWord(FindRegMap(*i));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutSync(const tSSAVariable * dest,
	const tSSAVariable * func, const tSSAVariable * lockee)
{
	PutWord(ocSync);
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(func));
	PutWord(FindRegMap(lockee));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutJump(const tSSABlock * target)
{
	risse_size insn_pos = Code.size();
	PutWord(ocJump);
	AddPendingBlockJump(target, insn_pos);
	PutWord(0); // 仮
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutBranch(const tSSAVariable * ref,
		const tSSABlock * truetarget, const tSSABlock * falsetarget)
{
	risse_size insn_pos = Code.size();
	PutWord(ocBranch);
	PutWord(FindRegMap(ref));
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
	PutWord(ocCatchBranch);
	PutWord(FindRegMap(ref));
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
	PutWord(ocDebugger);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutThrow(const tSSAVariable * throwee)
{
	PutWord(ocThrow);
	PutWord(FindRegMap(throwee));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutReturn(const tSSAVariable * value)
{
	PutWord(ocReturn);
	PutWord(FindRegMap(value));
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
	PutWord(ocExitTryException);
	PutWord(value?FindRegMap(value):InvalidRegNum);
	PutWord(static_cast<risse_uint32>(reloc_pos));
	PutWord(static_cast<risse_uint32>(idx));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutGetExitTryValue(const tSSAVariable * dest,
											const tSSAVariable * src)
{
	PutWord(ocGetExitTryValue);
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(src));
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

	PutWord(static_cast<risse_uint32>(op));
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(arg1));
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

	PutWord(static_cast<risse_uint32>(op));
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(arg1));
	PutWord(FindRegMap(arg2));
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

	PutWord(static_cast<risse_uint32>(op));
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(arg1));
	PutWord(FindRegMap(arg2));
	PutWord(FindRegMap(arg3));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutInContextOf(const tSSAVariable * dest,
		const tSSAVariable * instance, const tSSAVariable * context)
{
	PutWord(static_cast<risse_uint32>(context ? ocInContextOf : ocInContextOfDyn));
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(instance));
	if(context) PutWord(FindRegMap(context));
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
	PutWord(static_cast<risse_uint32>(op));
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(obj));
	PutWord(FindRegMap(name));
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
	PutWord(static_cast<risse_uint32>(op));
	PutWord(FindRegMap(obj));
	PutWord(FindRegMap(name));
	PutWord(FindRegMap(value));
	if(op == ocDSetF) PutWord(flags); // フラグを置く
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutSetAttribute(const tSSAVariable * obj,
		const tSSAVariable * name, risse_uint32 attrib)
{
	// 一応 code を assert (完全ではない)
	PutWord(static_cast<risse_uint32>(ocDSetAttrib));
	PutWord(FindRegMap(obj));
	PutWord(FindRegMap(name));
	PutWord(attrib);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCodeGenerator::PutAssert(const tSSAVariable *cond, const tString & msg)
{
	// assertion コードを置く
	RISSE_ASSERT(VMInsnInfo[ocAssert].Flags[0] == tVMInsnInfo::vifRegister);
	RISSE_ASSERT(VMInsnInfo[ocAssert].Flags[1] == tVMInsnInfo::vifConstant);
	PutWord(ocAssert);
	PutWord(FindRegMap(cond));
	PutWord(FindConst(msg));
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
} // namespace Risse
