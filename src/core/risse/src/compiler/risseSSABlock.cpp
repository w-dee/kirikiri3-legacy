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
	Id = Form->GetFunction()->GetFunctionGroup()->GetCompiler()->GetUniqueNumber();
		// Id は文と文を識別できる程度にユニークであればよい
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
tSSAStatement * tSSABlock::AddStatement(risse_size pos, tOpCode code,
		tSSAVariable ** ret_var,
			tSSAVariable *using1,
			tSSAVariable *using2,
			tSSAVariable *using3,
			tSSAVariable *using4
			)
{
	// 文の作成
	tSSAStatement * stmt =
		new tSSAStatement(Form, pos, code);

	if(ret_var)
	{
		// 戻りの変数の作成
		*ret_var = new tSSAVariable(Form, stmt);
	}

	// 文のSSAブロックへの追加
	AddStatement(stmt);

	// 文に変数の使用を追加
	if(using1) stmt->AddUsed(using1);
	if(using2) stmt->AddUsed(using2);
	if(using3) stmt->AddUsed(using3);
	if(using4) stmt->AddUsed(using4);

	// 文を返す
	return stmt;
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
void tSSABlock::InsertStatement(tSSAStatement * stmt, tSSAStatement * after)
{
	// この時点で after は挿入したい文の直前の文
	tSSAStatement * after_succ = after->GetSucc();
	if(after_succ)	after_succ->SetPred(stmt);
	if(after)		after->SetSucc(stmt);
	stmt->SetPred(after);
	stmt->SetSucc(after_succ);
	if(after_succ == NULL) LastStatement = stmt;

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
	// 戻りの変数を作成する
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
void tSSABlock::DeleteSucc(risse_size index)
{
	// 直後の基本ブロックを削除する
	RISSE_ASSERT(Succ.size() > 0);

	Succ.erase(Succ.begin() + index);

	// 最後の文がどのような文であるかによって処理が異なる
	RISSE_ASSERT(LastStatement);
	switch(LastStatement->GetCode())
	{
	case ocBranch:
		// ocBranch は削除し、単純な jump 文に置き換える
		{
			tSSAStatement * orig_last_stmt = LastStatement;
			RISSE_ASSERT(index == 0 || index == 1);
			tSSAStatement * new_stmt =
				new tSSAStatement(Form, GetLastStatementPosition(), ocJump);
			new_stmt->SetBlock(this);
			new_stmt->SetJumpTargetNoSetPred(Succ[0]); // 残っている方のブロック
			InsertStatement(new_stmt, sipTail);
			orig_last_stmt->DeleteUsed();
			DeleteStatement(orig_last_stmt);
		}
		break;

	default:
wxFprintf(stderr, wxT("about to delete latent alive succ block #%d of %s\n"),
			(int)index,
			GetName().AsWxString().c_str());
		RISSE_ASSERT(!"about to delete latent alive block");
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
	// Pred が 1 の場合にその Pred を消すと自分自身も消さないとならないが
	// その場合は何もしないので注意
	risse_size i = Pred.size() - 1;
	while(Pred.size() > 1)
	{
		if(!Pred[i]->Alive) DeletePred(i);

		if(i == 0) break;
		i--;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::DeleteDeadSucc()
{
	// Succ は削除の効率を考え、逆順に見ていく
	// Succ が 1 の場合にその Succ を消すと自分自身も消さないとならないが
	// その場合は何もしないので注意
	risse_size i = Succ.size() - 1;
	while(Succ.size() > 1)
	{
		if(!Succ[i]->Alive) DeleteSucc(i);

		if(i == 0) break;
		i--;
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
	map->insert(tLiveVariableMap::value_type(var, risse_size_max));
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
void tSSABlock::CoalesceLiveness(const tSSAVariable * old_var, const tSSAVariable * new_var)
{
	RISSE_ASSERT(LiveOut && LiveIn);

	tLiveVariableMap::iterator i;
	tLiveVariableMap * map;

	map = LiveIn;
	i = map->find(old_var);
	if(i != map->end())
	{
		map->erase(i);
		i = map->find(new_var);
		if(i == map->end())
			map->insert(tLiveVariableMap::value_type(new_var, risse_size_max));
	}

	map = LiveOut;
	i = map->find(old_var);
	if(i != map->end())
	{
		map->erase(i);
		i = map->find(new_var);
		if(i == map->end())
			map->insert(tLiveVariableMap::value_type(new_var, risse_size_max));
	}
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
void tSSABlock::ClearStatementMarks()
{
	tSSAStatement *stmt;
	for(stmt = FirstStatement;
		stmt;
		stmt = stmt->GetSucc())
	{
		stmt->SetMark(NULL);
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
void tSSABlock::SetOrder(risse_size & order)
{
	tSSAStatement *stmt;
	for(stmt = FirstStatement; stmt;)
	{
		stmt->SetOrder(order);
		stmt = stmt->GetSucc();
		// φ関数間には順位の差はないので ocPhi の場合は順位をインクリメントしない(同順とする)
		if(stmt && stmt->GetCode() != ocPhi)
			order ++;
	}
	order++;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::CreateVariableInterferenceGraph()
{
	RISSE_ASSERT(LiveIn && LiveOut);

	tSSAStatement *stmt;

	// livemap は LiveIn から出発し、この関数の最後では LiveOut と同じになるはず

	tLiveVariableMap livemap(*LiveIn); // コピーを作成

	for(stmt = FirstStatement; stmt; stmt = stmt->GetSucc())
		stmt->CreateVariableInterferenceGraph(livemap);

	// この時点では livemap は LiveOut と同じになっているはず
	RISSE_ASSERT(livemap == *LiveOut);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::ListAllStatements(gc_map<risse_size, tSSAStatement *> &statements) const
{
	tSSAStatement *stmt;
	for(stmt = FirstStatement; stmt; stmt = stmt->GetSucc())
		statements.insert(gc_map<risse_size, tSSAStatement *>::value_type(stmt->GetId(), stmt));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::ListAllStatements(gc_vector<tSSAStatement *> &statements) const
{
	tSSAStatement *stmt;
	for(stmt = FirstStatement; stmt; stmt = stmt->GetSucc())
		statements.push_back(stmt);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::AnalyzeConstantPropagation(
		gc_vector<tSSAVariable *> &variables,
		gc_vector<tSSABlock *> &blocks)
{
	// すべての文に対して解析を行う
	tSSAStatement *stmt;
	for(stmt = FirstStatement; stmt; stmt = stmt->GetSucc())
		stmt->AnalyzeConstantPropagation(variables, blocks);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::RealizeConstantPropagationErrors()
{
	// すべての文に対して処理を行う
	tSSAStatement *stmt;
	for(stmt = FirstStatement; stmt; stmt = stmt->GetSucc())
		stmt->RealizeConstantPropagationErrors();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::InsertTypeAssertion()
{
	// すべての文に対して処理を行う
	tSSAStatement *stmt;
	for(stmt = FirstStatement; stmt; stmt = stmt->GetSucc())
		stmt->InsertTypeAssertion();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::ReplaceConstantAssign()
{
	// すべての文に対して処理を行う
	tSSAStatement *stmt;
	for(stmt = FirstStatement; stmt; )
	{
		tSSAStatement * next = stmt->GetSucc();
		stmt->ReplaceConstantAssign();
		stmt = next;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::TraceCoalescable()
{
	// phi 関数の Used の干渉を見る

	// phi 関数の Used の中に、そのブロックに属する他の phi 関数と
	// 同じ 変数が含まれていると後段の stmt->TraceCoalescable() で
	// 合併が行われてしまう。
	// 合併が行われないように、pred の最後にコピー文を挿入することにする。
	// たとえば以下のような場合
	//	a0 = phi(x0, y0)
	//	a1 = phi(x0, y1)
	// x0 が共通して使われている。この場合は x0 方向の pred の最後に x0' = x0 の文を
	// 挿入し、a1 = phi(x0', y1) に置き換える。
	// 条件としては、同じブロックに属するphi関数にまたがって、同じ変数が使われている
	// 場合 (上記の場合はx0)。その変数が来る pred の方向は関係ない。
	// 厳密には全く Used の構成が同じ phi 関数があった場合はこの処理はしなくていいが
	// ここではかんがえていない

	tSSAStatement *stmt = FirstStatement;
	risse_size pred_count = Pred.size();
	if(stmt) stmt = stmt->GetSucc();
	for(stmt = FirstStatement; stmt && stmt->GetCode() == ocPhi;
		stmt = stmt->GetSucc())
	{
		const gc_vector<tSSAVariable*> & stmt_used  = stmt->GetUsed();
		for(risse_size index = 0; index < pred_count; index ++)
		{
			for(tSSAStatement * prev = stmt->GetPred(); prev;
				prev = prev->GetPred())
			{
				RISSE_ASSERT(prev->GetUsed().size() == stmt->GetUsed().size());
				const gc_vector<tSSAVariable*> & prev_used  = prev->GetUsed();
				bool found = false;
				for(risse_size j = 0; j < pred_count; j++)
					if(stmt_used[index] == prev_used[j]) { found  = true; break; }
				if(found)
				{
					// 同じ変数を使ってることが分かった
					// pred の最後にコピー文を挿入
					tSSAVariable * tmp_var = new tSSAVariable(Form, NULL, stmt_used[index]->GetName());
wxFprintf(stderr, wxT("multiple use of the same variable in phi statements found; inserting %s at %s\n"),
	tmp_var->GetQualifiedName().AsWxString().c_str(),
	Pred[index]->GetName().AsWxString().c_str());
					tSSAStatement * new_stmt =
						new tSSAStatement(Form, Pred[index]->GetLastStatementPosition(), ocAssign);
					new_stmt->AddUsed(const_cast<tSSAVariable*>(stmt_used[index]));
					new_stmt->SetDeclared(tmp_var);
					tmp_var->SetDeclared(new_stmt);
					stmt_used[index]->DeleteUsed(stmt);
					stmt->OverwriteUsed(stmt_used[index], tmp_var); // 該当変数の使用を置き換え
					tmp_var->SetValueState(stmt_used[index]->GetValueState());
					tmp_var->SetValue(stmt_used[index]->GetValue());
					Pred[index]->InsertStatement(new_stmt, sipBeforeBranch);

					// 干渉グラフを更新する。
					// tmp_var は Pred の最後のすべての変数と干渉するはず
					for(tLiveVariableMap::iterator i = Pred[index]->LiveOut->begin();
						i != Pred[index]->LiveOut->end(); i++)
						tmp_var->SetInterferenceWith(const_cast<tSSAVariable * >(i->first));

					// tmp_var は this の最初のすべての変数と干渉するはず
					for(tLiveVariableMap::iterator i = this->LiveIn->begin();
						i != this->LiveIn->end(); i++)
						tmp_var->SetInterferenceWith(const_cast<tSSAVariable * >(i->first));

					// Pred の LiveOut にも tmp_var を追加する
					Pred[index]->AddLiveness(tmp_var, true);
					// this の LiveIn にも tmp_var を追加する
					AddLiveness(tmp_var, false);

					break;
				}
			}
		}
	}



	// すべての文で定義された変数を見る
	for(stmt = FirstStatement; stmt; stmt = stmt->GetSucc())
		stmt->TraceCoalescable();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::Coalesce()
{
	// すべての文で定義された変数に対して合併を実行する
	tSSAStatement *stmt;
	for(stmt = FirstStatement; stmt; stmt = stmt->GetSucc())
		stmt->Coalesce();
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
		// φ関数を除去
		stmt->DeleteUsed();
		DeleteStatement(stmt);
	}

	SetLastStatementPosition();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::Check3AddrAssignee()
{
	tSSAStatement *stmt;
	for(stmt = FirstStatement; stmt; stmt = stmt->GetSucc())
		stmt->Check3AddrAssignee();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSABlock::AssignRegisters(gc_vector<void*> & assign_work)
{
	tSSAStatement *stmt;
	for(stmt = FirstStatement; stmt; stmt = stmt->GetSucc())
		stmt->AssignRegisters(assign_work);
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
	ret += RISSE_WS(" // ");
	if(GetAlive())
		ret += RISSE_WS("alive");
	else
		ret += RISSE_WS("dead");
	if(Pred.size() != 0)
	{
		ret += RISSE_WS(", pred: ");
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
			// 文本体
			if(stmt->GetOrder() != risse_size_max)
				ret += RISSE_WS("[") + tString::AsString((risse_int64)stmt->GetOrder()) +
					RISSE_WS("] ");
			ret += stmt->Dump() + RISSE_WS("\n");
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
