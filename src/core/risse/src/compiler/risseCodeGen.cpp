//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief バイトコードジェネレータ
//---------------------------------------------------------------------------
#include "../prec.h"

#include "risseCodeGen.h"
#include "../risseException.h"
#include "../risseScriptBlockBase.h"
#include "../risseCodeBlock.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(52364,51758,14226,19534,54934,29340,32493,12680);



//---------------------------------------------------------------------------
tRisseCodeGenerator::tRisseCodeGenerator(
	tRisseCodeGenerator * parent)
{
	Parent = parent;
	RegisterBase = 0;
	NumUsedRegs = 0;
	MaxNumUsedRegs = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::SetRegisterBase()
{
	if(Parent)
		RegisterBase = Parent->RegisterBase + Parent->MaxNumUsedRegs;
	else
		RegisterBase = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::AddBlockMap(const tRisseSSABlock * block)
{
	BlockMap.insert(tBlockMap::value_type(block, Code.size()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::AddPendingBlockJump(const tRisseSSABlock * block, risse_size insn_pos)
{
	PendingBlockJumps.push_back(tPendingBlockJump(block, Code.size(), insn_pos));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tRisseCodeGenerator::FindConst(const tRisseVariant & value)
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
risse_size tRisseCodeGenerator::FindRegMap(const tRisseSSAVariable * var)
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
risse_size tRisseCodeGenerator::AllocateRegister()
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
	if(MaxNumUsedRegs < NumUsedRegs) MaxNumUsedRegs = NumUsedRegs; // 最大値を更新

	return assigned_reg;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::FreeRegister(risse_size reg)
{
	RegFreeMap.push_back(reg); // 変数を空き配列に追加
	NumUsedRegs --;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::FreeRegister(const tRisseSSAVariable *var)
{
	// RegMap から指定された変数を探す
	tRegMap::iterator f = RegMap.find(var);
	RISSE_ASSERT(f != RegMap.end());

	FreeRegister(f->second);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tRisseCodeGenerator::FindPinnedRegNameMap(const tRisseString & name)
{
	tNamedRegMap::iterator f = PinnedRegNameMap.find(name);

	RISSE_ASSERT(!(Parent == NULL && f == PinnedRegNameMap.end()));

	if(f == PinnedRegNameMap.end())
	{
		// 自分にない場合、親を見る
		if(Parent)
			return Parent->FindPinnedRegNameMap(name);
		RISSE_ASSERT(!"pinned variable not found in map");
	}
	return f->second;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::AddPinnedRegNameMap(const tRisseString & name)
{
	RISSE_ASSERT(PinnedRegNameMap.find(name) == PinnedRegNameMap.end()); // 二重挿入は許されない
	PinnedRegNameMap.insert(tNamedRegMap::value_type(name, RegisterBase++));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tRisseCodeGenerator::FindParentVariableMap(const tRisseString & name)
{
	RISSE_ASSERT(Parent != NULL);
	// ParentVariableMap から指定された変数を探す
	// 指定された変数が無ければ変数の空きマップからさがし、変数を割り当てる
	tNamedRegMap::iterator f = ParentVariableMap.find(name);
	if(f != ParentVariableMap.end())
	{
		 // 変数が見つかった
		 return f->second;
	}

	// 変数がないので「親から」レジスタを割り当てる
	risse_size assigned_reg = Parent->AllocateRegister();
	ParentVariableMap.insert(tNamedRegMap::value_type(name, assigned_reg));

	return assigned_reg;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::FreeParentVariableMapVariables()
{
	RISSE_ASSERT(Parent != NULL);
	// ParentVariableMapにある変数を「親から」すべて開放する
	for(tNamedRegMap::iterator i = ParentVariableMap.begin(); i != ParentVariableMap.end(); i++)
		Parent->FreeRegister(i->second);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::FixCode()
{
	// サイズをチェック
	if(Code.size() != static_cast<risse_uint32>(Code.size()))
		eRisseError::Throw(
			tRisseString(RISSE_WS_TR("too large code size"))); // まずあり得ないと思うが ...

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
void tRisseCodeGenerator::PutNoOperation()
{
	PutWord(ocNoOperation);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutAssign(const tRisseSSAVariable * dest, const tRisseSSAVariable * src)
{
	PutWord(ocAssign);
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(src));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutAssign(const tRisseString & dest, const tRisseSSAVariable * src)
{
	PutWord(ocAssign);
	PutWord(FindPinnedRegNameMap(dest));
	PutWord(FindRegMap(src));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutAssign(const tRisseSSAVariable * dest, const tRisseString & src)
{
	PutWord(ocAssign);
	PutWord(FindRegMap(dest));
	PutWord(FindPinnedRegNameMap(src));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutAssign(risse_size dest, const tRisseSSAVariable * src)
{
	PutWord(ocAssign);
	PutWord(dest);
	PutWord(FindRegMap(src));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutAssign(const tRisseSSAVariable * dest, risse_size src)
{
	PutWord(ocAssign);
	PutWord(FindRegMap(dest));
	PutWord(src);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutAssign(const tRisseSSAVariable * dest, const tRisseVariant & value)
{
	PutWord(ocAssignConstant);
	PutWord(FindRegMap(dest));
	PutWord(FindConst(value));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutAssign(const tRisseSSAVariable * dest, tRisseOpCode code)
{
	// 一応 code を assert (完全ではない)
	RISSE_ASSERT(RisseVMInsnInfo[code].Flags[0] == tRisseVMInsnInfo::vifRegister);
	RISSE_ASSERT(RisseVMInsnInfo[code].Flags[1] == tRisseVMInsnInfo::vifVoid);

	PutWord(static_cast<risse_uint32>(code));
	PutWord(FindRegMap(dest));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutRelocatee(const tRisseSSAVariable * dest, risse_size index)
{
	// 定数領域に仮の値をpushする
	tRisseVariant value(tRisseString(RISSE_WS("<relocation block #%1>"),
		tRisseString::AsString((risse_int64)index)));
	risse_size reloc_pos = Consts.size();
	Consts.push_back(value);

	// ocAssignConstant 命令を生成する
	PutWord(ocAssignConstant);
	PutWord(FindRegMap(dest));
	PutWord(reloc_pos);

	// Relocations に情報を入れる
	Relocations.push_back(std::pair<risse_size, risse_size>(reloc_pos, index));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutFunctionCall(const tRisseSSAVariable * dest,
		const tRisseSSAVariable * func,
		bool is_new, bool omit, risse_uint32 expbit,
		const gc_vector<const tRisseSSAVariable *> & args,
		const gc_vector<const tRisseSSAVariable *> & blocks)
{
	RISSE_ASSERT(!(is_new && blocks.size() != 0)); // ブロック付き new はない
	RISSE_ASSERT(!(omit && args.size() != 0)); // omit なのに引数があるということはない

	tRisseOpCode code;
	if(is_new)
		code = ocNew;
	else
		code = blocks.size() != 0 ? ocFuncCallBlock : ocFuncCall;

	PutWord(static_cast<risse_uint32>(code));
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(func));

	if(omit) expbit |= RisseFuncCallFlag_Omitted;
	PutWord(expbit); // フラグ

	PutWord(static_cast<risse_uint32>(args.size()));
	if(blocks.size() != 0) PutWord(static_cast<risse_uint32>(blocks.size()));

	// 引数をput
	for(gc_vector<const tRisseSSAVariable *>::const_iterator i = args.begin();
		i != args.end(); i++)
		PutWord(FindRegMap(*i));
	// ブロックをput
	for(gc_vector<const tRisseSSAVariable *>::const_iterator i = blocks.begin();
		i != blocks.end(); i++)
		PutWord(FindRegMap(*i));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutJump(const tRisseSSABlock * target)
{
	risse_size insn_pos = Code.size();
	PutWord(ocJump);
	AddPendingBlockJump(target, insn_pos);
	PutWord(0); // 仮
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutBranch(const tRisseSSAVariable * ref,
		const tRisseSSABlock * truetarget, const tRisseSSABlock * falsetarget)
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
void tRisseCodeGenerator::PutDebugger()
{
	PutWord(ocDebugger);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutThrow(const tRisseSSAVariable * throwee)
{
	PutWord(ocThrow);
	PutWord(FindRegMap(throwee));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutReturn(const tRisseSSAVariable * value)
{
	PutWord(ocReturn);
	PutWord(FindRegMap(value));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutOperator(tRisseOpCode op, const tRisseSSAVariable * dest,
		const tRisseSSAVariable * arg1)
{
	// 一応 op を assert (完全ではない)
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[0] == tRisseVMInsnInfo::vifRegister);
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[1] == tRisseVMInsnInfo::vifRegister);
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[2] == tRisseVMInsnInfo::vifVoid);

	PutWord(static_cast<risse_uint32>(op));
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(arg1));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutOperator(tRisseOpCode op, const tRisseSSAVariable * dest,
		const tRisseSSAVariable * arg1, const tRisseSSAVariable * arg2)
{
	// 一応 code を assert (完全ではない)
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[0] == tRisseVMInsnInfo::vifRegister);
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[1] == tRisseVMInsnInfo::vifRegister);
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[2] == tRisseVMInsnInfo::vifRegister);
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[3] == tRisseVMInsnInfo::vifVoid);

	PutWord(static_cast<risse_uint32>(op));
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(arg1));
	PutWord(FindRegMap(arg2));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutOperator(tRisseOpCode op, const tRisseSSAVariable * dest,
		const tRisseSSAVariable * arg1, const tRisseSSAVariable * arg2, const tRisseSSAVariable * arg3)
{
	// 一応 code を assert (完全ではない)
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[0] == tRisseVMInsnInfo::vifRegister);
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[1] == tRisseVMInsnInfo::vifRegister);
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[2] == tRisseVMInsnInfo::vifRegister);
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[3] == tRisseVMInsnInfo::vifRegister);
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[3] == tRisseVMInsnInfo::vifVoid);

	PutWord(static_cast<risse_uint32>(op));
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(arg1));
	PutWord(FindRegMap(arg2));
	PutWord(FindRegMap(arg3));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutSet(tRisseOpCode op, const tRisseSSAVariable * obj,
		const tRisseSSAVariable * name, const tRisseSSAVariable * value)
{
	// 一応 code を assert (完全ではない)
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[0] == tRisseVMInsnInfo::vifRegister);
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[1] == tRisseVMInsnInfo::vifRegister);
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[2] == tRisseVMInsnInfo::vifRegister);
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[3] == tRisseVMInsnInfo::vifVoid);

	PutWord(static_cast<risse_uint32>(op));
	PutWord(FindRegMap(obj));
	PutWord(FindRegMap(name));
	PutWord(FindRegMap(value));
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
} // namespace Risse
