//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オペレーションコード/VM命令定義
//---------------------------------------------------------------------------
#include "risseOpCodes.h"


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(2759,19905,41807,16730,27783,55397,28774,14847);
//---------------------------------------------------------------------------
} // namespace Risse


// 名前表の読み込み
// (ヘッダファイル中でもう一度 namespace Risse に入って
// しまうので、ここは namespace Risse 外に置く)
#undef risseOpCodesH
#define RISSE_OC_DEFINE_INFO
#include "risseOpCodes.h"



namespace Risse
{

//---------------------------------------------------------------------------
risse_size tRisseVMCodeIterator::GetInsnSize() const
{
	// 命令コード
	tRisseOpCode insn_code = static_cast<tRisseOpCode>(*CodePointer);

	// RisseVMInsnInfo 内の該当するエントリ
	const tRisseVMInsnInfo & entry = RisseVMInsnInfo[insn_code];

	// enty.Flags をスキャン
	risse_size insn_size = 1; // 1 = 命令コードの分
	for(int i = 0; i < RisseMaxVMInsnOperand; i++)
	{
		switch(entry.Flags[i])
		{
		case tRisseVMInsnInfo::vifVoid:
			break;

		case tRisseVMInsnInfo::vifNumber:
			insn_size += CodePointer[i+1]; // 追加のワード数はここに書いてある
			break;

		case tRisseVMInsnInfo::vifConstant:
		case tRisseVMInsnInfo::vifRegister:
		case tRisseVMInsnInfo::vifAddress:
		case tRisseVMInsnInfo::vifSomething:
			insn_size ++;
			break;
		}
	}
	return insn_size;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseVMCodeIterator::Dump() const
{
	// 命令コード
	tRisseOpCode insn_code = static_cast<tRisseOpCode>(*CodePointer);

	// RisseVMInsnInfo 内の該当するエントリ
	const tRisseVMInsnInfo & entry = RisseVMInsnInfo[insn_code];

	// 命令名部分
	tRisseString ret(entry.Mnemonic);
	ret += RISSE_WC(' ');

	// 命令ごとに分岐
	switch(insn_code)
	{
	case ocFuncCall:
	case ocNew:
	case ocFuncCallBlock:
		break;

	default:
		// それ以外の命令コードの処理は形式化されているのでそれに従う
		for(int i = 0; i < RisseMaxVMInsnOperand; i++)
		{
			if(entry.Flags[i] == tRisseVMInsnInfo::vifVoid) break;
			if(i != 0) ret += RISSE_WS(", ");
			switch(entry.Flags[i])
			{
			case tRisseVMInsnInfo::vifVoid:
				break;

			case tRisseVMInsnInfo::vifNumber:
			case tRisseVMInsnInfo::vifSomething:
				RISSE_ASSERT(!"not acceptable operand type");
				break;


			case tRisseVMInsnInfo::vifConstant:
				ret += RISSE_WS("*") + tRisseString::AsString((int)CodePointer[i+1]);
				break;

			case tRisseVMInsnInfo::vifRegister:
				ret += RISSE_WS("%") + tRisseString::AsString((int)CodePointer[i+1]);
				break;

			case tRisseVMInsnInfo::vifAddress:
				{
					char address[22];
					sprintf(address, "%05d", (int)CodePointer[i+1]);
					ret += tRisseString(address);
				}
				break;
			}
		}
	}
	return ret;
}
//---------------------------------------------------------------------------
tRisseString tRisseVMCodeIterator::Dump(const tRisseVariant * consts) const
{
	tRisseString ret(Dump());

	// 命令中の定数領域についてコメントを追加する
	// 命令コード
	tRisseOpCode insn_code = static_cast<tRisseOpCode>(*CodePointer);

	// RisseVMInsnInfo 内の該当するエントリ
	const tRisseVMInsnInfo & entry = RisseVMInsnInfo[insn_code];

	// 定数領域を示しているオペランドを探す
	bool first = true;
	for(int i = 0; i < RisseMaxVMInsnOperand; i++)
	{
		if(entry.Flags[i] == tRisseVMInsnInfo::vifConstant)
		{
			if(first)
				ret += RISSE_WS(" // ");
			else
				ret += RISSE_WS(", ");
			ret += RISSE_WS("*") + tRisseString::AsString((int)CodePointer[i+1]) + 
				RISSE_WS("=") + consts[CodePointer[i+1]].AsHumanReadable(100);
		}
	}
	return ret;
}
//---------------------------------------------------------------------------

} // namespace Risse
