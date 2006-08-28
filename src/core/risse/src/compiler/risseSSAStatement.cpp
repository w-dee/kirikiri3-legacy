//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief SSA形式における「文」
//---------------------------------------------------------------------------
#include "../prec.h"

#include "risseSSAStatement.h"
#include "risseSSAVariable.h"
#include "risseSSABlock.h"
#include "risseCodeGen.h"
#include "risseSSAForm.h"
#include "../risseException.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(33139,58829,49251,19299,61572,36837,14859,14043);



//---------------------------------------------------------------------------
tRisseSSAStatement::tRisseSSAStatement(tRisseSSAForm * form,
	risse_size position, tRisseOpCode code)
{
	// フィールドの初期化
	Form = form;
	Position = position;
	Code = code;
	Block = NULL;
	Pred = NULL;
	Succ = NULL;
	Declared = NULL;
	Order = risse_size_max;
	FuncExpandFlags = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAStatement::AddUsed(tRisseSSAVariable * var)
{
	var->AddUsed(this);
	Used.push_back(var);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAStatement::DeleteUsed(risse_size index)
{
	RISSE_ASSERT(Used.size() > index);
	tRisseSSAVariable * var = Used[index];
	Used.erase(Used.begin() + index);
	var->DeleteUsed(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAStatement::DeleteUsed()
{
	risse_size i = Used.size();
	while(i > 0)
	{
		i--;

		DeleteUsed(i);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAStatement::SetTrueBranch(tRisseSSABlock * block)
{
	RISSE_ASSERT(Code == ocBranch);
	if(Targets.size() != 2) Targets.resize(2, NULL);
	Targets[0] = block;
	block->AddPred(Block);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSABlock * tRisseSSAStatement::GetTrueBranch() const
{
	RISSE_ASSERT(Code == ocBranch);
	RISSE_ASSERT(Targets.size() == 2);
	return Targets[0];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAStatement::SetFalseBranch(tRisseSSABlock * block)
{
	RISSE_ASSERT(Code == ocBranch);
	if(Targets.size() != 2) Targets.resize(2, NULL);
	Targets[1] = block;
	block->AddPred(Block);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSABlock * tRisseSSAStatement::GetFalseBranch() const
{
	RISSE_ASSERT(Code == ocBranch);
	RISSE_ASSERT(Targets.size() == 2);
	return Targets[1];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAStatement::SetJumpTarget(tRisseSSABlock * block)
{
	RISSE_ASSERT(Code == ocJump);
	if(Targets.size() != 1) Targets.resize(1, NULL);
	Targets[0] = block;
	block->AddPred(Block);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSABlock * tRisseSSAStatement::GetJumpTarget() const
{
	RISSE_ASSERT(Code == ocJump);
	RISSE_ASSERT(Targets.size() == 1);
	return Targets[0];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAStatement::SetTryExitTarget(tRisseSSABlock * block)
{
	RISSE_ASSERT(Code == ocCatchBranch);
	if(Targets.size() < 2) Targets.resize(2, NULL);
	Targets[0] = block;
	block->AddPred(Block);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSABlock * tRisseSSAStatement::GetTryExitTarget() const
{
	RISSE_ASSERT(Code == ocCatchBranch);
	RISSE_ASSERT(Targets.size() >= 2);
	return Targets[0];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAStatement::SetTryCatchTarget(tRisseSSABlock * block)
{
	RISSE_ASSERT(Code == ocCatchBranch);
	if(Targets.size() < 2) Targets.resize(2, NULL);
	Targets[1] = block;
	block->AddPred(Block);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSABlock * tRisseSSAStatement::GetTryCatchTarget() const
{
	RISSE_ASSERT(Code == ocCatchBranch);
	RISSE_ASSERT(Targets.size() >= 2);
	return Targets[1];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAStatement::AddTarget(tRisseSSABlock * block)
{
	RISSE_ASSERT(Code == ocCatchBranch);
	RISSE_ASSERT(Targets.size() >= 2);
	Targets.push_back(block);
	block->AddPred(Block);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAStatement::SetName(const tRisseString & name)
{
	RISSE_ASSERT(
			Code == ocParentRead || Code == ocParentWrite ||
			Code == ocChildRead || Code == ocChildWrite ||
			Code == ocReadVar || Code == ocWriteVar ||
			Code == ocRead || Code == ocWrite ||
			Code == ocDefineLazyBlock);
	Name = new tRisseString(name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const tRisseString & tRisseSSAStatement::GetName() const
{
	RISSE_ASSERT(
			Code == ocParentRead || Code == ocParentWrite ||
			Code == ocChildRead || Code == ocChildWrite ||
			Code == ocReadVar || Code == ocWriteVar ||
			Code == ocRead || Code == ocWrite ||
			Code == ocDefineLazyBlock);
	RISSE_ASSERT(Name != NULL);
	return *Name;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAStatement::AnalyzeVariableStatementLiveness()
{
	if(Declared) Declared->AnalyzeVariableStatementLiveness(this);
	for(gc_vector<tRisseSSAVariable*>::const_iterator i = Used.begin();
		i != Used.end(); i++)
		(*i)->AnalyzeVariableStatementLiveness(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAStatement::GenerateCode(tRisseCodeGenerator * gen) const
{
	bool free_unused_var = true;

	// gen に一つ文を追加する
	switch(Code)
	{
	case ocNoOperation:
		gen->PutNoOperation();
		break;

	case ocAssign:
		RISSE_ASSERT(Declared != NULL);
		RISSE_ASSERT(Used.size() == 1);
		gen->PutAssign(Declared, Used[0]);
		break;

	case ocAssignConstant:
		RISSE_ASSERT(Declared != NULL);
		RISSE_ASSERT(Declared->GetValue() != NULL);
		gen->PutAssign(Declared, *Declared->GetValue());
		break;

	case ocAssignThis:
	case ocAssignSuper:
	case ocAssignGlobal:
	case ocAssignNewArray:
	case ocAssignNewDict:
	case ocAssignNewRegExp:
		RISSE_ASSERT(Declared != NULL);
		gen->PutAssign(Declared, Code);
		break;

	case ocAssignParam:
		RISSE_ASSERT(Declared != NULL);
		gen->PutAssignParam(Declared, Index);
		break;

	case ocAssignBlockParam:
		RISSE_ASSERT(Declared != NULL);
		gen->PutAssignBlockParam(Declared, Index);
		break;

	case ocNew:
	case ocFuncCall:
	case ocTryFuncCall:
		RISSE_ASSERT(Declared != NULL);
		RISSE_ASSERT(Used.size() >= 1);

		{
			gc_vector<const tRisseSSAVariable *> args, blocks;

			risse_size arg_count = Used.size() - 1 - BlockCount;
			risse_size block_count = BlockCount;

			for(risse_size i = 0; i < arg_count; i++)
				args.push_back(Used[i + 1]);
			for(risse_size i = 0; i < block_count; i++)
				blocks.push_back(Used[i + arg_count + 1]);

			gen->PutFunctionCall(Declared, Used[0], Code, 
								FuncExpandFlags, args, blocks);
		}
		break;

	case ocJump:
		gen->PutJump(GetJumpTarget());
		break;

	case ocBranch:
		RISSE_ASSERT(Used.size() == 1);
		gen->PutBranch(Used[0], GetTrueBranch(), GetFalseBranch());
		break;

	case ocCatchBranch:
		RISSE_ASSERT(Used.size() == 1);
		gen->PutCatchBranch(Used[0], TryIdentifierIndex, Targets);
		break;

	case ocReturn:
		RISSE_ASSERT(Used.size() == 1);
		gen->PutReturn(Used[0]);
		break;

	case ocDebugger:
		gen->PutDebugger();
		break;

	case ocThrow:
		RISSE_ASSERT(Used.size() == 1);
		gen->PutThrow(Used[0]);
		break;

	case ocExitTryException:
		RISSE_ASSERT(Used.size() <= 1);
		gen->PutExitTryException(Used.size() >= 1 ? Used[0]: NULL, TryIdentifierIndex, Index);
		break;

	case ocGetExitTryValue:
		RISSE_ASSERT(Declared != NULL);
		RISSE_ASSERT(Used.size() == 1);
		gen->PutGetExitTryValue(Declared, Used[0]);
		break;

	case ocLogNot:
	case ocBitNot:
	case ocPlus:
	case ocMinus:
		RISSE_ASSERT(Declared != NULL);
		RISSE_ASSERT(Used.size() == 1);
		gen->PutOperator(Code, Declared, Used[0]);
		break;

	case ocLogOr:
	case ocLogAnd:
	case ocBitOr:
	case ocBitXor:
	case ocBitAnd:
	case ocNotEqual:
	case ocEqual:
	case ocDiscNotEqual:
	case ocDiscEqual:
	case ocLesser:
	case ocGreater:
	case ocLesserOrEqual:
	case ocGreaterOrEqual:
	case ocRBitShift:
	case ocLShift:
	case ocRShift:
	case ocMod:
	case ocDiv:
	case ocIdiv:
	case ocMul:
	case ocAdd:
	case ocSub:
	case ocIncontextOf:
		RISSE_ASSERT(Declared != NULL);
		RISSE_ASSERT(Used.size() == 2);
		gen->PutOperator(Code, Declared, Used[0], Used[1]);
		break;

	case ocDGet:
	case ocIGet:
	case ocDDelete:
	case ocIDelete:
		RISSE_ASSERT(Declared != NULL);
		RISSE_ASSERT(Used.size() == 2);
		gen->PutOperator(Code, Declared, Used[0], Used[1]);
		break;

	case ocDSet:
	case ocISet:
		RISSE_ASSERT(Used.size() == 3);
		gen->PutSet(Code, Used[0], Used[1], Used[2]);
		break;

	case ocDefineAccessMap:
		{
			RISSE_ASSERT(Declared != NULL);
			// Declared で使用している文のうち、ocChildWrite/ocChildRead
			// してる物を探し、そこで使用されている名前で gen の VariableMapForChildren に
			// マップを作成する
			// (実際の変数へのアクセスは後の ocChildWrite や ocChildRead で処理する)
			const gc_vector<tRisseSSAStatement *> & child_access_stmts =
				Declared->GetUsed();
			for(gc_vector<tRisseSSAStatement *>::const_iterator i =
				child_access_stmts.begin(); i != child_access_stmts.end(); i++)
			{
				tRisseSSAStatement * stmt = *i;
				switch((*i)->Code)
				{
				case ocChildWrite:
				case ocChildRead:
					gen->FindVariableMapForChildren(*stmt->Name); // マップに変数名だけを追加する
					break;
				default: ;
				}
			}
		}
		break;

	case ocDefineLazyBlock:
		{
			tRisseSSAForm * child_form = DefinedForm;
			RISSE_ASSERT(child_form != NULL);
			RISSE_ASSERT(Declared != NULL);
			// この文のDeclaredは、子SSA形式を作成して返すようになっているが、
			// コードブロックの参照の問題があるので注意
			gen->PutCodeBlockRelocatee(Declared, child_form->GetCodeBlockIndex());
			if(child_form->GetUseParentFrame())
				gen->PutSetFrame(Declared);
			else
				gen->PutSetShare(Declared);
		}
		break;

	case ocChildWrite:
		{
			RISSE_ASSERT(Used.size() == 2);
			RISSE_ASSERT(Name != NULL);
			// Used[0] が define された文は ocDefineAccessMap であるはずである
			// TODO: この部分は変数の併合を実装するに当たり書き換わる可能性が高い。
			//       現状の実装は暫定的なもの。
			tRisseSSAStatement * lazy_stmt = Used[0]->GetDeclared();
			RISSE_ASSERT(lazy_stmt->GetCode() == ocDefineAccessMap);
			gen->PutAssign(gen->FindVariableMapForChildren(*Name), Used[1]);
		}
		break;

	case ocChildRead:
		{
			RISSE_ASSERT(Used.size() == 1);
			RISSE_ASSERT(Name != NULL);
			// Used[0] が define された文は ocDefineAccessMap であるはずである
			// TODO: この部分は変数の併合を実装するに当たり書き換わる可能性が高い。
			//       現状の実装は暫定的なもの。
			tRisseSSAStatement * lazy_stmt = Used[0]->GetDeclared();
			RISSE_ASSERT(lazy_stmt->GetCode() == ocDefineAccessMap);
			gen->PutAssign(Declared, gen->FindVariableMapForChildren(*Name));
		}
		break;

	case ocEndAccessMap:
		// アクセスマップの使用終了
		// 暫定実装
		{
			RISSE_ASSERT(Used.size() == 1);
			// 変数を開放する
			gen->FreeVariableMapForChildren();
		}
		free_unused_var = false;
			// この文で使用が終わった変数(すなわち ocDefineAccessMapで
			// 宣言した変数) の開放はおこなわない。なぜならば、ocDefineAccessMapで
			// 宣言された変数は擬似的な変数であり、実際に変数マップに登録されないから。
		
		break;


	case ocParentWrite:
		// 暫定実装
		RISSE_ASSERT(Used.size() == 1);
		RISSE_ASSERT(Name != NULL);
		gen->PutAssign(gen->GetParent()->FindVariableMapForChildren(*Name), Used[0]);
		break;

	case ocParentRead:
		// 暫定実装
		RISSE_ASSERT(Used.size() == 0);
		RISSE_ASSERT(Name != NULL);
		RISSE_ASSERT(Declared != NULL);
		gen->PutAssign(Declared, gen->GetParent()->FindVariableMapForChildren(*Name));
		break;

	case ocWrite: // 共有変数領域への書き込み
		RISSE_ASSERT(Name != NULL);
		RISSE_ASSERT(Used.size() == 1);
		gen->PutWrite(*Name, Used[0]);
		break;

	case ocRead: // 共有変数領域からの読み込み
		RISSE_ASSERT(Name != NULL);
		RISSE_ASSERT(Declared != NULL);
		RISSE_ASSERT(Used.size() == 0);
		gen->PutRead(Declared, *Name);
		break;

	default:
		RISSE_ASSERT(!"not acceptable SSA operation code");
		break;
	}

	if(free_unused_var)
	{
		// この文で使用が終了している変数を解放する
		if(Declared)
		{
			if(Declared->GetLastUsedStatement() == this)
				gen->FreeRegister(Declared);
		}
		for(gc_vector<tRisseSSAVariable*>::const_iterator i = Used.begin();
				i != Used.end(); i++)
		{
			if((*i)->GetLastUsedStatement() == this)
				gen->FreeRegister(*i);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseSSAStatement::Dump() const
{
	switch(Code)
	{
	case ocNoOperation:			// なにもしない
		return RISSE_WS("nop");

	case ocPhi:		// φ関数
		{
			tRisseString ret;

			RISSE_ASSERT(Declared != NULL);
			ret += Declared->Dump() + RISSE_WS(" = PHI(");

			// φ関数の引数を追加
			tRisseString used;
			for(gc_vector<tRisseSSAVariable*>::const_iterator i = Used.begin();
					i != Used.end(); i++)
			{
				if(!used.IsEmpty()) used += RISSE_WS(", ");
				used += (*i)->Dump();
			}

			ret += used + RISSE_WS(")");

			// 変数のコメントを追加
			tRisseString comment = Declared->GetTypeComment();
			if(!comment.IsEmpty())
				ret += RISSE_WS(" // ") + Declared->Dump() + RISSE_WS(" = ") + comment;

			return ret;
		}

	case ocAssignNewRegExp: // 新しい正規表現オブジェクト
		{
			tRisseString ret;
			RISSE_ASSERT(Used.size() == 2);
			ret += Declared->Dump() + RISSE_WS(" = AssignNewRegExp(");

			ret +=	Used[0]->Dump() + RISSE_WS(", ") +
					Used[1]->Dump() + RISSE_WS(")");

			// 変数のコメントを追加
			tRisseString comment = Declared->GetTypeComment();
			if(!comment.IsEmpty())
				ret += RISSE_WS(" // ") + Declared->Dump() + RISSE_WS(" = ") + comment;

			return ret;
		}

	case ocAssignParam:
		{
			tRisseString ret;
			RISSE_ASSERT(Used.size() == 0);
			ret += Declared->Dump() + RISSE_WS(" = AssignParam(") +
				tRisseString::AsString((risse_int)Index) + RISSE_WS(")");
			return ret;
		}

	case ocJump:
		{
			RISSE_ASSERT(GetJumpTarget() != NULL);
			return RISSE_WS("goto *") + GetJumpTarget()->GetName();
		}

	case ocBranch:
		{
			RISSE_ASSERT(GetTrueBranch() != NULL);
			RISSE_ASSERT(GetFalseBranch() != NULL);
			RISSE_ASSERT(Used.size() == 1);
			return
					RISSE_WS("if ") + (*Used.begin())->Dump() + 
					RISSE_WS(" then *") + GetTrueBranch()->GetName() +
					RISSE_WS(" else *") + GetFalseBranch()->GetName();
		}

	case ocCatchBranch:
		{
			RISSE_ASSERT(GetTryExitTarget() != NULL);
			RISSE_ASSERT(GetTryCatchTarget() != NULL);
			RISSE_ASSERT(Used.size() == 1);
			tRisseString ret =
					RISSE_WS("catch branch ") + (*Used.begin())->Dump() + 
					RISSE_WS(" exit:*") + GetTryExitTarget()->GetName() +
					RISSE_WS(" catch:*") + GetTryCatchTarget()->GetName();
			for(risse_size n = 2; n < Targets.size(); n++)
			{
				ret += RISSE_WS(" ") + tRisseString::AsString((risse_int64)(n)) +
					RISSE_WS(": *") + Targets[n]->GetName();
			}
			return ret;
		}

	case ocDefineLazyBlock: // 遅延評価ブロックの定義
		{
			RISSE_ASSERT(Name != NULL);
			RISSE_ASSERT(Declared != NULL);

			tRisseString ret;
			ret += Declared->Dump() + RISSE_WS(" = DefineLazyBlock(");

			ret +=	Name->AsHumanReadable() + RISSE_WS(")");

			// 変数のコメントを追加
			tRisseString comment = Declared->GetTypeComment();
			if(!comment.IsEmpty())
				ret += RISSE_WS(" // ") + Declared->Dump() + RISSE_WS(" = ") + comment;

			return ret;
		}

	case ocParentWrite: // 親名前空間への書き込み
		{
			RISSE_ASSERT(Name != NULL);
			RISSE_ASSERT(Used.size() == 1);
			return (*Used.begin())->Dump()  + RISSE_WS(".ParentWrite(") + Name->AsHumanReadable() +
					RISSE_WS(")");
		}

	case ocParentRead: // 親名前空間からの読み込み
		{
			RISSE_ASSERT(Name != NULL);
			RISSE_ASSERT(Declared != NULL);

			tRisseString ret;
			ret += Declared->Dump() + RISSE_WS(" = ParentRead(");

			ret +=	Name->AsHumanReadable() + RISSE_WS(")");

			// 変数のコメントを追加
			tRisseString comment = Declared->GetTypeComment();
			if(!comment.IsEmpty())
				ret += RISSE_WS(" // ") + Declared->Dump() + RISSE_WS(" = ") + comment;

			return ret;
		}

	case ocChildWrite: // 子名前空間への書き込み
		{
			RISSE_ASSERT(Name != NULL);
			RISSE_ASSERT(Used.size() == 2);
			return Used[0]->Dump()  + RISSE_WS(".ChildWrite(") +
					Name->AsHumanReadable() + RISSE_WS(", ") + Used[1]->Dump() +
					RISSE_WS(")");
		}

	case ocChildRead: // 子名前空間からの読み込み
		{
			RISSE_ASSERT(Name != NULL);
			RISSE_ASSERT(Declared != NULL);
			RISSE_ASSERT(Used.size() == 1);

			tRisseString ret;
			ret += Declared->Dump() + RISSE_WS(" = ") + Used[0]->Dump();
			ret += RISSE_WS(".ChildRead(");
			ret +=	Name->AsHumanReadable() + RISSE_WS(")");

			// 変数のコメントを追加
			tRisseString comment = Declared->GetTypeComment();
			if(!comment.IsEmpty())
				ret += RISSE_WS(" // ") + Declared->Dump() + RISSE_WS(" = ") + comment;

			return ret;
		}

	case ocWrite: // 共有変数領域への書き込み
		{
			RISSE_ASSERT(Name != NULL);
			RISSE_ASSERT(Used.size() == 1);
			return Used[0]->Dump() + RISSE_WS(".Write(") +
					Name->AsHumanReadable() +
					RISSE_WS(")");
		}

	case ocRead: // 共有変数領域からの読み込み
		{
			RISSE_ASSERT(Name != NULL);
			RISSE_ASSERT(Declared != NULL);
			RISSE_ASSERT(Used.size() == 0);

			tRisseString ret;
			ret += Declared->Dump() + RISSE_WS(" = ");
			ret += RISSE_WS("Read(");
			ret +=	Name->AsHumanReadable() + RISSE_WS(")");

			// 変数のコメントを追加
			tRisseString comment = Declared->GetTypeComment();
			if(!comment.IsEmpty())
				ret += RISSE_WS(" // ") + Declared->Dump() + RISSE_WS(" = ") + comment;

			return ret;
		}

	default:
		{
			tRisseString ret;

			// 変数の宣言
			if(Declared)
				ret += Declared->Dump() + RISSE_WS(" = "); // この文で宣言された変数がある

			if(Code == ocAssign || Code == ocReadVar || Code == ocWriteVar)
			{
				// 単純代入/共有変数の読み書き
				RISSE_ASSERT(Used.size() == 1);
				ret += (*Used.begin())->Dump();

				if(Code == ocReadVar)
					ret += RISSE_WS(" (read from ") + Name->AsHumanReadable()
						+ RISSE_WS(")");
				else if(Code == ocWriteVar)
					ret += RISSE_WS(" (write to ") + Name->AsHumanReadable()
						+ RISSE_WS(")");
			}
			else
			{
				// 関数呼び出しの類？
				bool is_funccall = (Code == ocFuncCall || Code == ocNew || Code == ocTryFuncCall);

				// 使用している引数の最初の引数はメッセージの送り先なので
				// オペレーションコードよりも前に置く
				if(Used.size() != 0)
					ret += (*Used.begin())->Dump() + RISSE_WC('.');

				// オペレーションコード
				ret += tRisseString(RisseVMInsnInfo[Code].Name);

				// 使用している引数
				if(is_funccall && (FuncExpandFlags & RisseFuncCallFlag_Omitted))
				{
					// 関数呼び出しかnew
					ret += RISSE_WS("(...)");
				}
				else if(Used.size() >= 2)
				{
					// 引数がある
					tRisseString used;
					risse_int arg_index = 0;
					for(gc_vector<tRisseSSAVariable*>::const_iterator i = Used.begin() + 1;
							i != Used.end(); i++, arg_index++)
					{
						if(!used.IsEmpty()) used += RISSE_WS(", ");
						used += (*i)->Dump();
						if(is_funccall && (FuncExpandFlags & (1<<arg_index)))
							used += RISSE_WC('*'); // 展開フラグ
					}
					ret += RISSE_WS("(") + used +
									RISSE_WS(")");
				}
				else
				{
					// 引数が無い
					ret += RISSE_WS("()");
				}
			}

			// 変数の宣言に関してコメントがあればそれを追加
			if(Declared)
			{
				tRisseString comment = Declared->GetTypeComment();
				if(!comment.IsEmpty())
					ret += RISSE_WS(" // ") + Declared->Dump() + RISSE_WS(" = ") + comment;
			}
			return ret;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseSSAStatement::DumpVariableStatementLiveness(bool is_start) const
{
	tRisseString ret;
	tRisseSSAVariable *var;
	var = Declared;
	if(var)
		if((is_start?var->GetFirstUsedStatement():var->GetLastUsedStatement()) == this)
			ret += var->GetQualifiedName();

	for(gc_vector<tRisseSSAVariable*>::const_iterator i = Used.begin();
			i != Used.end(); i++)
	{
		var = *i;
		if((is_start?var->GetFirstUsedStatement():var->GetLastUsedStatement()) == this)
		{
			if(!ret.IsEmpty()) ret += RISSE_WS(", ");
			ret += var->GetQualifiedName();
		}
	}

	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
