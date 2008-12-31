//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オペレーションコード/VM命令定義
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseOpCodes.h"
#include "risseVariant.h"
#include "risseObject.h"
#include "risseStringTemplate.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(2759,19905,41807,16730,27783,55397,28774,14847);
//---------------------------------------------------------------------------
} // namespace Risse



namespace Risse
{

// 名前表の読み込み
#include "risseOpCodesDefs.def"

//---------------------------------------------------------------------------
risse_size tVMCodeIterator::GetInsnSize() const
{
	// 命令コード
	tOpCode insn_code = static_cast<tOpCode>(*CodePointer);
	RISSE_ASSERT(insn_code < ocVMCodeLast);

	// VMInsnInfo 内の該当するエントリ
	const tVMInsnInfo & entry = VMInsnInfo[insn_code];

	// enty.Flags をスキャン
	risse_size insn_size = 1; // 1 = 命令コードの分
	for(int i = 0; i < MaxVMInsnOperand; i++)
	{
		switch(entry.Flags[i])
		{
		case tVMInsnInfo::vifVoid:
			break;

		case tVMInsnInfo::vifNumber:
			insn_size += 1 + CodePointer[i+1]; // 追加のワード数はここに書いてある
			break;

		case tVMInsnInfo::vifConstant:
		case tVMInsnInfo::vifRegister:
		case tVMInsnInfo::vifAddress:
		case tVMInsnInfo::vifParameter:
		case tVMInsnInfo::vifShared:
		case tVMInsnInfo::vifOthers:
			insn_size ++;
			break;
		}
	}
	return insn_size;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tVMCodeIterator::Dump() const
{
	// 命令コード
	tOpCode insn_code = static_cast<tOpCode>(*CodePointer);
	RISSE_ASSERT(insn_code < ocVMCodeLast);

	// VMInsnInfo 内の該当するエントリ
	const tVMInsnInfo & entry = VMInsnInfo[insn_code];

	// 命令名部分
	tString ret(entry.Mnemonic);
	ret += RISSE_WC(' ');

	// 命令コードの処理はある程度形式化されているのでそれに従う
	for(int i = 0; i < MaxVMInsnOperand; i++)
	{
		if(entry.Flags[i] == tVMInsnInfo::vifVoid) break;
		switch(entry.Flags[i])
		{
		case tVMInsnInfo::vifVoid:
			break;

		case tVMInsnInfo::vifNumber:
		case tVMInsnInfo::vifOthers:
			// nothing to do; simply skip
			break;

		case tVMInsnInfo::vifConstant:
			if(i != 0) ret += RISSE_WS(", ");
			ret += RISSE_WS("*") + tString::AsString((int)CodePointer[i+1]);
			break;

		case tVMInsnInfo::vifRegister:
			if(i != 0) ret += RISSE_WS(", ");
			ret += RISSE_WS("%") + tString::AsString((int)CodePointer[i+1]);
			break;

		case tVMInsnInfo::vifAddress:
			{
				if(i != 0) ret += RISSE_WS(", ");
				char address[22];
				if(Address != risse_size_max)
					sprintf(address, "%05d", static_cast<int>(Address + CodePointer[i+1]));
				else
					sprintf(address, "%d", static_cast<int>(static_cast<risse_int32>(CodePointer[i+1])));
				ret += tString(address);
			}
			break;

		case tVMInsnInfo::vifParameter:
			if(i != 0) ret += RISSE_WS(", ");
			ret += RISSE_WS("(") + tString::AsString((int)CodePointer[i+1]) +
				RISSE_WS(")");
			break;

		case tVMInsnInfo::vifShared:
			// shared の上位16ビットはネストレベル、下位16ビットはレジスタ番号である
			if(i != 0) ret += RISSE_WS(", ");
			ret += RISSE_WS("[") + tString::AsString((int)((CodePointer[i+1]>>16)&0xffff)) +
					RISSE_WS(":") + tString::AsString((int)((CodePointer[i+1])&0xffff)) +
						RISSE_WS("]");
			break;

		}
	}

	// 特殊なフラグを持つものや、関数呼び出し系などはオペランド数が可変なので特別に処理をする
	switch(insn_code)
	{
	case ocTryFuncCall:
	case ocFuncCall:
	case ocNew:
	case ocFuncCallBlock:
		{
			// 最初の S (これがフラグのはず) と 最初の N (これが関数への引数の数のはず) と
			// 次の N (ないかもしれない; これがブロックの数のはず) と
			// 最初の 0 (これが引数の始まりのはず) を探す
			int i;
			risse_uint32 flags = 0;
			risse_uint32 num_args = 0;
			risse_uint32 num_blocks = 0;
			bool args_found = false;
			for(i = 0; i < MaxVMInsnOperand; i++)
			{
				switch(entry.Flags[i])
				{
				case tVMInsnInfo::vifNumber:
					if(!args_found)
					{
						args_found = true;
						num_args = CodePointer[i+1];
					}
					else
					{
						num_blocks = CodePointer[i+1];
					}
					break;
				case tVMInsnInfo::vifOthers:
					flags = CodePointer[i+1]; // フラグ
					break;
				case tVMInsnInfo::vifVoid:
					break;
				default: ;
				}
				if(entry.Flags[i] == tVMInsnInfo::vifVoid) break;
			}
			int arg_start = i;
			// 引数を列挙
			ret += RISSE_WC('(');
			for(risse_uint32 n = 0; n < num_args; n++)
			{
				if(n != 0) ret += RISSE_WS(", ");
				ret += RISSE_WS("%") +
					tString::AsString((int)CodePointer[arg_start + n +1]);
				if(flags & (1 << n)) ret += RISSE_WC('*'); // 引数展開の場合
			}
			ret += RISSE_WC(')');
			// ブロックを列挙
			if(num_blocks != 0)
			{
				ret += RISSE_WC(' ');
				for(risse_uint32 n = 0; n < num_blocks; n++)
				{
					if(n != 0) ret += RISSE_WS(", ");
					ret += RISSE_WS("%") +
						tString::AsString((int)CodePointer[arg_start + num_args + n +1]);
				}
			}
		}

		break;

	case ocCatchBranch:
		{
			char address[22];
			for(risse_uint32 n = 0; n < CodePointer[3]; n++)
			{
				ret += RISSE_WS(", ");
				if(Address != risse_size_max)
					sprintf(address, "%05d", static_cast<int>(Address + CodePointer[n+4]));
				else
					sprintf(address, "%d", static_cast<int>(static_cast<risse_int32>(CodePointer[n+4])));
				ret += tString(address);
			}
		}
		break;

	case ocExitTryException:
		{
			ret += RISSE_WS(", ");
			ret += tString::AsString(static_cast<int>(CodePointer[3]));
		};
		break;

	case ocDSetF:
	case ocDGetF:
		{
			ret += RISSE_WS(" <");
			ret += tOperateFlags(CodePointer[4]).AsString();
			ret += RISSE_WS(">");
		}
		break;

	case ocDSetAttrib:
		{
			ret += RISSE_WS(" <");
			ret += tOperateFlags(CodePointer[3]).AsString();
			ret += RISSE_WS(">");
		}
		break;

	case ocAssertType:
		{
			ret += RISSE_WS(" = ");
			ret += tVariant::GetTypeString(static_cast<tVariant::tType>(CodePointer[2]));
		}
		break;

	default:
		;
	}

	return ret;
}
//---------------------------------------------------------------------------
tString tVMCodeIterator::Dump(const tVariant * consts) const
{
	tString ret(Dump());

	// 命令中の定数領域についてコメントを追加する
	// 命令コード
	tOpCode insn_code = static_cast<tOpCode>(*CodePointer);
	RISSE_ASSERT(insn_code < ocVMCodeLast);

	// VMInsnInfo 内の該当するエントリ
	const tVMInsnInfo & entry = VMInsnInfo[insn_code];

	// 定数領域を示しているオペランドを探す
	bool first = true;
	for(int i = 0; i < MaxVMInsnOperand; i++)
	{
		if(entry.Flags[i] == tVMInsnInfo::vifConstant)
		{
			if(first)
				ret += RISSE_WS(" // ");
			else
				ret += RISSE_WS(", ");
			ret += RISSE_WS("*") + tString::AsString((int)CodePointer[i+1]) + 
				RISSE_WS("=") + consts[CodePointer[i+1]].AsHumanReadable(100);
		}
	}
	return ret;
}
//---------------------------------------------------------------------------

} // namespace Risse
