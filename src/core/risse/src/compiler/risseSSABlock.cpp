//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief SSA形式における「基本ブロック」
//---------------------------------------------------------------------------
#include "../prec.h"

#include "risseCodeGen.h"
#include "risseSSABlock.h"
#include "risseSSAForm.h"
#include "risseSSAStatement.h"
#include "risseSSAVariable.h"
#include "risseCompiler.h"
#include "risseCompilerNS.h"
#include "../risseExceptionClass.h"
#include "../risseScriptBlockClass.h"
#include "../risseCodeBlock.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(35032,16995,23933,19143,62087,6357,32511,34753);





//---------------------------------------------------------------------------
tSSABlock::tSSABlock(tSSAForm * form, const tString & name,
	const tSSALocalNamespace * ns)
{
	Form = form;
	FirstStatement = LastStatement = NULL;
	LocalNamespace = new tSSALocalNamespace(*ns);
	LocalNamespace->SetBlock(this);
	Mark = NULL;
	Traversing = false;
	LiveIn = LiveOut = NULL;
	LastStatementPosition = risse_size_max;
	Alive = false;

	// 通し番号の準備
	Name = name + RISSE_WC('_') +
		tString::AsString(form->GetFunction()->GetFunctionGroup()->
								GetCompiler()->GetUniqueNumber());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::SetLastStatementPosition()
{
	if(!LastStatement)
		LastStatementPosition = risse_size_max;
	else
		LastStatementPosition = LastStatement->GetPosition();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::InsertStatement(tSSAStatement * stmt, tStatementInsertPoint point)
{
	tSSAStatement * cs = NULL;
	switch(point)
	{
	case sipHead:
		cs = FirstStatement;
		break;

	case sipAfterPhi:
		for(cs = FirstStatement;
			cs != NULL && cs->GetCode() == ocPhi;
			cs = cs->GetSucc()) ; /* ←空文 */
		break;

	case sipBeforeBranch:
		{
			tSSAStatement * i;
			cs = NULL;
			for(i = LastStatement; i != NULL; i = i->GetPred())
			{
				if(i->IsBranchStatement())
					cs = i;
				else
					break;
			}
			// 分岐文が見つからなかった場合は cs = NULL (つまり最後に追加)
		}
		break;

	case sipTail:
		cs = NULL;
		break;
	}

	// この時点で cs は挿入したい文の直後の文
	// cs==NULLの場合は最後に追加
	tSSAStatement * cs_pred = cs ? cs->GetPred() : LastStatement;
	if(cs_pred)	cs_pred->SetSucc(stmt);
	if(cs)		cs->SetPred(stmt);
	stmt->SetPred(cs_pred);
	if(cs_pred == NULL) FirstStatement = stmt;
	stmt->SetSucc(cs);
	if(cs == NULL) LastStatement = stmt;

	stmt->SetBlock(this);

	SetLastStatementPosition();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::DeleteStatement(tSSAStatement * stmt)
{
	tSSAStatement * stmt_pred = stmt->GetPred();
	tSSAStatement * stmt_succ = stmt->GetSucc();
	if(stmt_pred) stmt_pred->SetSucc(stmt_succ);
	if(stmt_pred == NULL) FirstStatement = stmt_succ;
	if(stmt_succ) stmt_succ->SetPred(stmt_pred);
	if(stmt_succ == NULL) LastStatement = stmt_pred;
	SetLastStatementPosition();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::ReplaceStatement(tSSAStatement * old_stmt,
	tSSAStatement * new_stmt)
{
	RISSE_ASSERT(old_stmt->GetBlock() == this);
	if(FirstStatement == old_stmt) FirstStatement = new_stmt;
	if(LastStatement == old_stmt) LastStatement = new_stmt;
	tSSAStatement * pred = old_stmt->GetPred();
	tSSAStatement * succ = old_stmt->GetSucc();
	new_stmt->SetPred(pred);
	new_stmt->SetSucc(succ);
	if(pred) pred->SetSucc(new_stmt);
	if(succ) succ->SetPred(new_stmt);
	new_stmt->SetBlock(this);
	SetLastStatementPosition();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::AddPhiFunctionToBlocks(
	risse_size pos, const tString & name,
	const tString & n_name, tSSAVariable *& last_phi_var)
{
	// 見つかったがφ関数を作成する必要がある場合
	gc_vector<tSSABlock *> block_stack;
	gc_vector<tSSAStatement *> phi_stmt_stack;

	// φ関数を追加する
	last_phi_var =
		AddPhiFunction(pos, name, n_name, block_stack, phi_stmt_stack);

	while(block_stack.size() > 0)
	{
		// スタックからpop
		tSSABlock * quest_block = block_stack.back();
		block_stack.pop_back();
		tSSAStatement * quest_phi_stmt = phi_stmt_stack.back();
		phi_stmt_stack.pop_back();

		// quest_block の LocalNamespace から変数を検索
		tSSAVariable **var = NULL;
		RISSE_ASSERT(quest_block->LocalNamespace);
		bool found = quest_block->LocalNamespace->Find(n_name, true, NULL, &var);

		if(!found)
		{
			// 変数が見つからない
			// エラーにする
			// TODO: もっと親切なエラーメッセージ
			tCompileExceptionClass::Throw(
				Form->GetFunction()->GetFunctionGroup()->GetCompiler()->
					GetScriptBlockInstance()->GetScriptEngine(),
				tString(
					RISSE_WS_TR("local variable '%1' is from out of scope"),
					name),
					quest_block->Form->GetScriptBlockInstance(), quest_phi_stmt->GetPosition());
		}

		if(*var == NULL)
			*var = quest_block->AddPhiFunction(pos, name, n_name, block_stack, phi_stmt_stack);
				// ↑見つかったがφ関数を作成する必要がある場合

		// phi 関数の文に変数を追加
		quest_phi_stmt->AddUsed(*var); 
	}

	SetLastStatementPosition();

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tSSABlock::AddPhiFunction(
		risse_size pos, const tString & name,
		const tString & n_name,
			gc_vector<tSSABlock *> & block_stack,
			gc_vector<tSSAStatement *> & phi_stmt_stack)
{
	tSSAVariable *ret_var;
	// φ関数を追加する
	tSSAStatement * phi_stmt = new tSSAStatement(Form, pos, ocPhi);
	// 戻りの変数を作成し、*var に入れる
	ret_var = new tSSAVariable(Form, phi_stmt, name);
	ret_var->SetNumberedName(n_name);
	// φ関数は必ずブロックの先頭に追加される
	InsertStatement(phi_stmt, tSSABlock::sipHead);

	// pred に対して再帰するためにスタックにPredを積む
	for(gc_vector<tSSABlock *>::reverse_iterator i = Pred.rbegin();
			i != Pred.rend(); i ++)
	{
		block_stack.push_back(*i);
		phi_stmt_stack.push_back(phi_stmt);
	}

	SetLastStatementPosition();

	return ret_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::AddPred(tSSABlock * block)
{
	// 直前の基本ブロックを追加する
	Pred.push_back(block);

	// block の直後基本ブロックとして this を追加する
	block->AddSucc(this);

	// 既存のφ関数は、すべて再調整しなければならない
	tSSAStatement *stmt = FirstStatement;
	while(stmt)
	{
		if(stmt->GetCode() != ocPhi) break;
		RISSE_ASSERT(!stmt->GetDeclared()->GetName().IsEmpty());
		RISSE_ASSERT(block->LocalNamespace != NULL);

		tSSAVariable * decl_var = stmt->GetDeclared();

		tSSAVariable * found_var =
			block->LocalNamespace->MakePhiFunction(stmt->GetPosition(),
				decl_var->GetName(), decl_var->GetNumberedName());
		if(!found_var)
		{
			// 変数が見つからない
			// エラーにする
			// TODO: もっと親切なエラーメッセージ
			tCompileExceptionClass::Throw(
				Form->GetFunction()->GetFunctionGroup()->
					GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
				tString(
					RISSE_WS_TR("local variable '%1' is from out of scope"),
					decl_var->GetName()),
					Form->GetScriptBlockInstance(), stmt->GetPosition());
		}
		stmt->AddUsed(found_var);

		stmt = stmt->GetSucc();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::DeletePred(risse_size index)
{
	// 直前の基本ブロックを削除する
	RISSE_ASSERT(Pred.size() > 0);
	Pred.erase(Pred.begin() + index);

	// 既存のφ関数からも削除する
	tSSAStatement *stmt = FirstStatement;
	while(stmt)
	{
		if(stmt->GetCode() != ocPhi) break;

		stmt->DeleteUsed(index);
			// φ関数の Used の順番は Pred の順番と同じなので index を
			// そのまま DeleteUsed() に渡す

		stmt = stmt->GetSucc();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::AddSucc(tSSABlock * block)
{
	Succ.push_back(block);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::DeleteDeadPred()
{
	// Pred は削除の効率を考え、逆順に見ていく
	if(Pred.size() > 0)
	{
		risse_size i = Pred.size() - 1;
		while(true)
		{
			if(!Pred[i]->Alive) DeletePred(i);

			if(i == 0) break;
			i--;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::DeleteDeadStatementsFromVariables()
{
	// すべてのマークされていない変数に対して処理を行う

	// すべての文を検査し、それぞれの文で宣言されている「変数」について解析を行う
	// 「文」につく「マーク」は 文のポインタそのもの
	tSSAStatement *stmt;
	for(stmt = FirstStatement;
		stmt;
		stmt = stmt->GetSucc())
	{
		tSSAVariable * decl_var = stmt->GetDeclared();
		if(decl_var != NULL)
		{
			if(decl_var->GetMark() != stmt)
			{
				decl_var->DeleteDeadStatements();
				decl_var->SetMark(stmt);
			}
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::AddLiveness(const tSSAVariable * var, bool out)
{
	tLiveVariableMap * map = out ? LiveOut : LiveIn;
	RISSE_ASSERT(map != NULL);
	map->insert(tLiveVariableMap::value_type(var, NULL));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tSSABlock::GetLiveness(const tSSAVariable * var, bool out) const
{
	tLiveVariableMap * map = out ? LiveOut : LiveIn;
	RISSE_ASSERT(map != NULL);
	return map->find(var) != map->end();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::ClearMark() const
{
	gc_vector<tSSABlock *> blocks;
	Traverse(blocks);
	for(gc_vector<tSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->SetMark(NULL);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::Traverse(gc_vector<tSSABlock *> & blocks) const
{
	risse_size new_start = 0;
	risse_size new_count = 1;
	// 自分自身を blocks に追加
	blocks.push_back(const_cast<tSSABlock*>(this));
	Traversing = true;

	// new_count が 0 で無い限り繰り返す(幅優先探索)
	while(new_count != 0)
	{
		risse_size start = blocks.size();
		// [new_start, new_start + new_count) の直後基本ブロックを追加する
		for(risse_size n = new_start; n < new_start + new_count; n++)
		{
			blocks.reserve(blocks.size() + blocks[n]->Succ.size());
			for(gc_vector<tSSABlock *>::iterator
				i = blocks[n]->Succ.begin(); i != blocks[n]->Succ.end(); i++)
			{
				if(!(*i)->Traversing)
				{
					(*i)->Traversing = true; // 二度と追加しないように
					blocks.push_back(*i);
				}
			}
		}
		new_start = start;
		new_count = blocks.size() - new_start;
	}

	// Traversing フラグを倒す
	for(gc_vector<tSSABlock *>::iterator
		i = blocks.begin(); i != blocks.end(); i++) (*i)->Traversing = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::ClearVariableMarks()
{
	tSSAStatement *stmt;
	for(stmt = FirstStatement;
		stmt;
		stmt = stmt->GetSucc())
	{
		tSSAVariable * decl_var = stmt->GetDeclared();
		if(decl_var != NULL)
		{
			decl_var->SetMark(NULL);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::ConvertSharedVariableAccess()
{
	// すべての文について
	tSSAStatement *stmt;
	for(stmt = FirstStatement;
		stmt;
		stmt = stmt->GetSucc())
	{
		// ocReadVar と ocWriteVar を探し、共有されていなければ普通の
		// ocAssign に、共有されていれば ocRead と ocWrite にそれぞれ変換する
		tOpCode code = stmt->GetCode();
		if(code == ocReadVar)
		{
			if(Form->GetFunction()->GetShared(stmt->GetName()))
			{
				tSSAStatement *new_stmt = new
					tSSAStatement(Form, stmt->GetPosition(), ocRead);
				new_stmt->SetName(stmt->GetName());
				new_stmt->SetDeclared(stmt->GetDeclared());
				// stmt は消えるため、stmt の Used をすべて解放しなければならない
				stmt->DeleteUsed();
				// 文を置き換える
				ReplaceStatement(stmt, new_stmt);
			}
			else
			{
				// ocAssign に置き換える
				stmt->SetCode(ocAssign);
			}
		}
		else if(code == ocWriteVar)
		{
			if(Form->GetFunction()->GetShared(stmt->GetName()))
			{
				tSSAStatement *new_stmt = new
					tSSAStatement(Form, stmt->GetPosition(), ocWrite);
				new_stmt->SetName(stmt->GetName());
				new_stmt->AddUsed(stmt->GetUsed()[0]);
				// stmt は消えるため、stmt の Used をすべて解放しなければならない
				stmt->DeleteUsed();
				// stmt->Declared で宣言された変数は以降使われることはないはず
				// だが一応クリアしておく
				stmt->SetDeclared(NULL);
				// 文を置き換える
				ReplaceStatement(stmt, new_stmt);
			}
			else
			{
				// ocAssign に置き換える
				stmt->SetCode(ocAssign);
			}
		}

		// この時点でnew_stmt と stmt の Succ は同じはずなので
		// stmt が new_stmt に取って代わられても このループの
		// イテレーション (stmt->GetSucc()) は正常に働くはず
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::CreateLiveInAndLiveOut()
{
	if(!LiveIn)  LiveIn  = new tLiveVariableMap();
	if(!LiveOut) LiveOut = new tLiveVariableMap();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::AnalyzeVariableBlockLiveness()
{
	// すべてのマークされていない変数に対して生存区間解析を行う
	// なぜマークをするのかと言えば、一度すでに生存解析を行った文を二度と解析しないようにするため。
	// マークは ClearVariableMarks() ですべてクリアできる。

	// すべての文を検査し、それぞれの文で宣言されている「変数」について解析を行う
	// 「文」につく「マーク」は 文のポインタそのもの
	tSSAStatement *stmt;
	for(stmt = FirstStatement;
		stmt;
		stmt = stmt->GetSucc())
	{
		tSSAVariable * decl_var = stmt->GetDeclared();
		if(decl_var != NULL)
		{
			if(decl_var->GetMark() != stmt)
			{
				Form->AnalyzeVariableBlockLiveness(decl_var);
				decl_var->SetMark(stmt);
			}
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::AnalyzeVariableStatementLiveness()
{
	// すべての文で宣言された変数について文単位の有効範囲解析を行う
	// この時点では状態はすでにSSAではない; phi関数の削除などにより、
	// 変数のDeclaredが一カ所ではなくて複数箇所になっている場合があるので注意
	tSSAStatement *stmt;
	for(stmt = FirstStatement; stmt; stmt = stmt->GetSucc())
		stmt->AnalyzeVariableStatementLiveness();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::RemovePhiStatements()
{
	// すべてのφ関数について処理
	tSSAStatement *stmt;
	for(stmt = FirstStatement;
		stmt && stmt->GetCode() == ocPhi;
		stmt = FirstStatement)
	{
		// φ関数を削除する処理は簡単にはそれぞれの pred ブロックの分岐の
		// 最後に代入文を生成し、φ関数を削除する。
		// ただし、この代入文の挿入によって変数が上書きされてしまう場合が
		// ある(lost copy problem)ため、変数の有効範囲の解析を行う

		// TODO: 変数の併合(coalescing)

		// 代入文を生成するに先立ち、代入文を生成することにより変数が上書き
		// されないことを確認する。具体的には、代入文を生成する場所で
		// 代入先変数が生存していれば、そこに代入文を生成してしまうと
		// 変になる。この場合は phi 変数の戻り値をいったん一時変数にとるような
		// 文を挿入することで生存範囲の干渉を避ける。

		// stmt の used の配列
		const gc_vector<tSSAVariable *> phi_used = stmt->GetUsed();

		// stmt で宣言された変数
		tSSAVariable * stmt_decld = stmt->GetDeclared();

		// pred をたどる
		bool var_used = false;
		for(gc_vector<tSSABlock *>::iterator i = Pred.begin();
			i != Pred.end(); i++)
		{
			// pred の最後で stmt_decld が存在しているかどうかを調べる
			if((*i)->GetLiveness(stmt_decld))
				{ var_used = true; break; }
		}

		if(var_used)
		{
			// TODO: 変数の干渉が見つかった場合の処理
			FPrint(stderr, tString(RISSE_WS("variable interference found at block %1\n"), Dump()).c_str());
			RISSE_ASSERT(!"variable interference found");
		}

		// 各 pred の分岐文の直前に 代入文を生成する
		for(risse_size index = 0; index < Pred.size(); index ++)
		{
			tSSAStatement * new_stmt =
				new tSSAStatement(Form, Pred[index]->GetLastStatementPosition(), ocAssign);
			new_stmt->AddUsed(const_cast<tSSAVariable*>(phi_used[index]));
			new_stmt->SetDeclared(stmt_decld);
			stmt_decld->SetDeclared(new_stmt); // 一応代入
				/* 注意
					「この変数が宣言された位置」(tSSAVariable::Declared) は
					φ関数を削除した後は意味がなくなる。φ関数の削除により、一つの変数
					への代入が複数箇所になり、SSA性が保持されなくなるため。
				*/

			Pred[index]->InsertStatement(new_stmt, sipBeforeBranch);

			// LiveOut にも stmt_decld を追加する
			Pred[index]->AddLiveness(stmt_decld);
		}

		// φ関数を除去
		stmt->DeleteUsed();
		DeleteStatement(stmt);
	}

	SetLastStatementPosition();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::SetOrder(risse_size & order)
{
	tSSAStatement *stmt;
	for(stmt = FirstStatement; stmt; stmt = stmt->GetSucc())
	{
		stmt->SetOrder(order);
		order ++;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::GenerateCode(tCodeGenerator * gen) const
{
	// この基本ブロックを gen に登録する
	gen->AddBlockMap(this);

	// それぞれの文についてコードを生成させる
	tSSAStatement *stmt;
	for(stmt = FirstStatement;
		stmt;
		stmt = stmt->GetSucc())
	{
		stmt->GenerateCode(gen);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tSSABlock::Dump() const
{
	tString ret;

	// ラベル名と直前のブロックを列挙
	ret +=  + RISSE_WS("*") + Name;
	if(Pred.size() != 0)
	{
		ret += RISSE_WS(" // pred: ");
		for(gc_vector<tSSABlock *>::const_iterator i = Pred.begin();
										i != Pred.end(); i ++)
		{
			if(i != Pred.begin()) ret += RISSE_WS(", ");
			ret += RISSE_WS("*") + (*i)->GetName();
		}
	}
	ret += RISSE_WS("\n");

	// LiveIn を列挙
	if(LiveIn && LiveIn->size() > 0)
	{
		ret += RISSE_WS("// LiveIn: ");
		for(tLiveVariableMap::iterator i = LiveIn->begin(); i != LiveIn->end(); i++)
		{
			if(i != LiveIn->begin()) ret += RISSE_WS(", ");
			ret += i->first->GetQualifiedName();
		}
		ret += RISSE_WS("\n");
	}

	// 文を列挙
	if(!FirstStatement)
	{
		// 一つも文を含んでいない？？
		ret += RISSE_WS("This SSA basic block does not contain any statements\n");
	}
	else
	{
		// すべての文をダンプ
		for(tSSAStatement * stmt = FirstStatement; stmt != NULL; stmt = stmt->GetSucc())
		{
			tString vars;
			// この文で使用が開始された変数
			vars = stmt->DumpVariableStatementLiveness(true);
			if(!vars.IsEmpty()) ret += RISSE_WS("// Use start: ") + vars +
																RISSE_WS("\n");
			// 文本体
			if(stmt->GetOrder() != risse_size_max)
				ret += RISSE_WS("[") + tString::AsString((risse_int64)stmt->GetOrder()) +
					RISSE_WS("] ");
			ret += stmt->Dump() + RISSE_WS("\n");
			// この文で使用が終了した変数
			vars = stmt->DumpVariableStatementLiveness(false);
			if(!vars.IsEmpty()) ret += RISSE_WS("// Use end: ") + vars +
																RISSE_WS("\n");
		}
	}

	// LiveOut を列挙
	if(LiveOut && LiveOut->size() > 0)
	{
		ret += RISSE_WS("// LiveOut: ");
		for(tLiveVariableMap::iterator i = LiveOut->begin(); i != LiveOut->end(); i++)
		{
			if(i != LiveOut->begin()) ret += RISSE_WS(", ");
			ret += i->first->GetQualifiedName();
		}
		ret += RISSE_WS("\n");
	}

	return ret + RISSE_WS("\n");
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
} // namespace Risse
