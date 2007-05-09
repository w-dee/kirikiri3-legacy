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
#include "../risseExceptionClass.h"
#include "../risseScriptBlockBase.h"
#include "../risseCodeBlock.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(52364,51758,14226,19534,54934,29340,32493,12680);



//---------------------------------------------------------------------------
tRisseCodeGenerator::tRisseCodeGenerator(tRisseSSAForm * form,
	tRisseCodeGenerator * parent, bool useparentframe, risse_size nestlevel)
{
	Form = form;
	Parent = parent;
	UseParentFrame = useparentframe;
	NestLevel = nestlevel;
	RISSE_ASSERT(!(!Parent && UseParentFrame)); // UseParentFrame が真の場合は親がなければならない
	RegisterBase = 0;
	NumUsedRegs = 0;
	MaxNumUsedRegs = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::SetRegisterBase()
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
void tRisseCodeGenerator::UpdateMaxNumUsedRegs(risse_size max)
{
	if(MaxNumUsedRegs < max) MaxNumUsedRegs = max; // 最大値を更新
	if(Parent && UseParentFrame) Parent->UpdateMaxNumUsedRegs(max); // 再帰
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
	UpdateMaxNumUsedRegs(NumUsedRegs + RegisterBase); // 最大値を更新

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
	// RegMap から指定された変数を開放する
	tRegMap::iterator f = RegMap.find(var);
	RISSE_ASSERT(f != RegMap.end());

	FreeRegister(f->second);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::FindSharedRegNameMap(const tRisseString & name, risse_uint16 &nestlevel, risse_uint16 &regnum)
{
	// SharedRegNameMap を逆順に見ていく

	tNamedRegMap::iterator f = SharedRegNameMap.find(name);
	if(f != SharedRegNameMap.end())
	{
		// 見つかった
		// いまのところ、nestlevel と regnum は、VMコード中において
		// それぞれ 32bit 整数の 上位16bitと下位16ビットを使うので、
		// それぞれがその16bitの上限を超えることができない。
		if(NestLevel > 0xffff)
			tRisseCompileExceptionClass::Throw(
				tRisseString(RISSE_WS_TR("too deep function nest level")), Form->GetScriptBlock(),
					GetSourceCodePosition()); // まずあり得ないと思うが ...
		if(f->second > 0xffff)
			tRisseCompileExceptionClass::Throw(
				tRisseString(RISSE_WS_TR("too deep shared variables between functions")), Form->GetScriptBlock(),
					GetSourceCodePosition()); // まずあり得ないと思うが ...
		nestlevel = NestLevel;
		regnum = static_cast<risse_uint16>(f->second);
		return;
	}

	// 見つからなかったので親に再帰する
	if(!Parent) RISSE_ASSERT(!"shared variable not found"); // 変数は見つからなければならない

	Parent->FindSharedRegNameMap(name, nestlevel, regnum);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::AddSharedRegNameMap(const tRisseString & name)
{
	RisseFPrint(stderr, (RISSE_WS("Adding ") + name +
		RISSE_WS(" at nestlevel ") +
		tRisseString::AsString(risse_int64(NestLevel)) + RISSE_WS("\n")).c_str() );

	RISSE_ASSERT(SharedRegNameMap.find(name) == SharedRegNameMap.end()); // 二重挿入は許されない
	SharedRegNameMap.insert(tNamedRegMap::value_type(name, SharedRegNameMap.size()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tRisseCodeGenerator::GetSharedRegCount() const
{
	return SharedRegNameMap.size();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tRisseCodeGenerator::FindOrRegisterVariableMapForChildren(const tRisseString & name)
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
risse_size tRisseCodeGenerator::FindVariableMapForChildren(const tRisseString & name)
{
	// VariableMapForChildren から指定された変数を探す
	tNamedRegMap::iterator f = VariableMapForChildren.find(name);
	RISSE_ASSERT(f != VariableMapForChildren.end());
	return f->second;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::FreeVariableMapForChildren()
{
	// VariableMapForChildrenにある変数を自分からすべて開放する
	for(tNamedRegMap::iterator i = VariableMapForChildren.begin(); i != VariableMapForChildren.end(); i++)
		FreeRegister(i->second);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::FixCode()
{
	// サイズをチェック
	if(Code.size() != static_cast<risse_uint32>(Code.size()))
		tRisseCompileExceptionClass::Throw(
			tRisseString(RISSE_WS_TR("too large code size")), Form->GetScriptBlock(), 0); // まずあり得ないと思うが ...

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
void tRisseCodeGenerator::SetSourceCodePosition(risse_size pos)
{
	if(CodeToSourcePosition.size() > 0 &&
		CodeToSourcePosition.back().second == pos) return;
			// 連続する重複するソースコード上の位置は、たとえVMコードの位置が異なったとしても挿入しない
	CodeToSourcePosition.push_back(std::pair<risse_size, risse_size>(Code.size(), pos));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tRisseCodeGenerator::GetSourceCodePosition() const
{
	if(CodeToSourcePosition.size() > 0) return CodeToSourcePosition.back().second;
	return risse_size_max;
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
void tRisseCodeGenerator::PutAssignNewRegExp(const tRisseSSAVariable * dest,
			const tRisseSSAVariable * pattern, const tRisseSSAVariable * flags)
{
	PutWord(ocAssignNewRegExp);
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(pattern));
	PutWord(FindRegMap(flags));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutAssignNewFunction(const tRisseSSAVariable * dest,
			const tRisseSSAVariable * body)
{
	PutWord(ocAssignNewFunction);
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(body));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutAssignNewProperty(const tRisseSSAVariable * dest,
			const tRisseSSAVariable * getter, const tRisseSSAVariable * setter)
{
	PutWord(ocAssignNewProperty);
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(getter));
	PutWord(FindRegMap(setter));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutAssignNewClass(const tRisseSSAVariable * dest,
			const tRisseSSAVariable * super, const tRisseSSAVariable * name)
{
	PutWord(ocAssignNewClass);
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(super));
	PutWord(FindRegMap(name));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutAssignNewModule(const tRisseSSAVariable * dest,
			const tRisseSSAVariable * name)
{
	PutWord(ocAssignNewModule);
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(name));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutAssignParam(const tRisseSSAVariable * dest, risse_size index)
{
	PutWord(ocAssignParam);
	PutWord(FindRegMap(dest));
	PutWord(static_cast<risse_uint32>(index));
	// index の最大値は RisseMaxArgCount で確実に risse_uint32 で表現できる範囲のため安全にキャストできる
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutAssignBlockParam(const tRisseSSAVariable * dest, risse_size index)
{
	PutWord(ocAssignBlockParam);
	PutWord(FindRegMap(dest));
	PutWord(static_cast<risse_uint32>(index));
	// index の最大値は RisseMaxArgCount で確実に risse_uint32 で表現できる範囲のため安全にキャストできる
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutAddBindingMap(const tRisseSSAVariable * map,
	const tRisseSSAVariable *name, const tRisseString &nname)
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
void tRisseCodeGenerator::PutWrite(const tRisseString & dest, const tRisseSSAVariable * src)
{
	risse_uint16 nestlevel = 0, regnum = 0;
	FindSharedRegNameMap(dest, nestlevel, regnum);
	PutWord(ocWrite);
	PutWord((nestlevel << 16) + regnum);
	PutWord(FindRegMap(src));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutRead(const tRisseSSAVariable * dest, const tRisseString & src)
{
	risse_uint16 nestlevel = 0, regnum = 0;
	FindSharedRegNameMap(src, nestlevel, regnum);
	PutWord(ocRead);
	PutWord(FindRegMap(dest));
	PutWord((nestlevel << 16) + regnum);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutCodeBlockRelocatee(const tRisseSSAVariable * dest, risse_size index)
{
	// 定数領域に仮の値をpushする
	tRisseVariant value(tRisseString(RISSE_WS("<VM block #%1>"),
		tRisseString::AsString((risse_int64)index)));
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
void tRisseCodeGenerator::PutSetFrame(const tRisseSSAVariable * dest)
{
	PutWord(ocSetFrame);
	PutWord(FindRegMap(dest));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutSetShare(const tRisseSSAVariable * dest)
{
	PutWord(ocSetShare);
	PutWord(FindRegMap(dest));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutFunctionCall(const tRisseSSAVariable * dest,
		const tRisseSSAVariable * func,
		tRisseOpCode code, risse_uint32 expbit,
		const gc_vector<const tRisseSSAVariable *> & args,
		const gc_vector<const tRisseSSAVariable *> & blocks)
{
	RISSE_ASSERT(!(code == ocNew && blocks.size() != 0)); // ブロック付き new はない
	RISSE_ASSERT(!((expbit & RisseFuncCallFlag_Omitted) && args.size() != 0));
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
void tRisseCodeGenerator::PutCatchBranch(const tRisseSSAVariable * ref,
		risse_size try_id_idx,
		const gc_vector<tRisseSSABlock *> & targets)
{
	RISSE_ASSERT(targets.size() >= 2);
		// 少なくとも何事も無かった場合と例外が発生した場合のジャンプ先を含む

	// Try識別子用に定数領域に仮の値をpushする
	tRisseVariant cvalue(tRisseString(RISSE_WS("<try id #%1>"),
		tRisseString::AsString((risse_int64)try_id_idx)));
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
	for(gc_vector<tRisseSSABlock *>::const_iterator i = targets.begin();
		i != targets.end(); i ++)
	{
		AddPendingBlockJump(*i, insn_pos);
		PutWord(0); // 仮
	}
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
void tRisseCodeGenerator::PutExitTryException(const tRisseSSAVariable * value,
		risse_size try_id_idx, risse_size idx)
{
	// Try識別子用に定数領域に仮の値をpushする
	tRisseVariant cvalue(tRisseString(RISSE_WS("<try id #%1>"),
		tRisseString::AsString((risse_int64)try_id_idx)));
	risse_size reloc_pos = Consts.size();
	Consts.push_back(cvalue);
	TryIdentifierRelocations.push_back(std::pair<risse_size, risse_size>(reloc_pos, try_id_idx));

	// ocExitTryException 命令を書く
	PutWord(ocExitTryException);
	PutWord(value?FindRegMap(value):RisseInvalidRegNum);
	PutWord(static_cast<risse_uint32>(reloc_pos));
	PutWord(static_cast<risse_uint32>(idx));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutGetExitTryValue(const tRisseSSAVariable * dest,
											const tRisseSSAVariable * src)
{
	PutWord(ocGetExitTryValue);
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(src));
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
void tRisseCodeGenerator::PutInContextOf(const tRisseSSAVariable * dest,
		const tRisseSSAVariable * instance, const tRisseSSAVariable * context)
{
	PutWord(static_cast<risse_uint32>(context ? ocInContextOf : ocInContextOfDyn));
	PutWord(FindRegMap(dest));
	PutWord(FindRegMap(instance));
	if(context) PutWord(FindRegMap(context));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCodeGenerator::PutGet(tRisseOpCode op, const tRisseSSAVariable * dest,
		const tRisseSSAVariable * obj, const tRisseSSAVariable * name, risse_uint32 flags)
{
	// 一応 code を assert (完全ではない)
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[0] == tRisseVMInsnInfo::vifRegister);
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[1] == tRisseVMInsnInfo::vifRegister);
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[2] == tRisseVMInsnInfo::vifRegister);
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[3] == tRisseVMInsnInfo::vifVoid);

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
void tRisseCodeGenerator::PutSet(tRisseOpCode op, const tRisseSSAVariable * obj,
		const tRisseSSAVariable * name, const tRisseSSAVariable * value, risse_uint32 flags)
{
	// 一応 code を assert (完全ではない)
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[0] == tRisseVMInsnInfo::vifRegister);
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[1] == tRisseVMInsnInfo::vifRegister);
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[2] == tRisseVMInsnInfo::vifRegister);
	RISSE_ASSERT(RisseVMInsnInfo[op].Flags[3] == tRisseVMInsnInfo::vifVoid);

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
void tRisseCodeGenerator::PutSetAttribute(const tRisseSSAVariable * obj,
		const tRisseSSAVariable * name, risse_uint32 attrib)
{
	// 一応 code を assert (完全ではない)
	PutWord(static_cast<risse_uint32>(ocDSetAttrib));
	PutWord(FindRegMap(obj));
	PutWord(FindRegMap(name));
	PutWord(attrib);
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace Risse
