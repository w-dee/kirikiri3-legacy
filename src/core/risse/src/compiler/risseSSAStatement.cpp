//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
#include "risseCompiler.h"
#include "../risseExceptionClass.h"
#include "../risseScriptBlockClass.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(33139,58829,49251,19299,61572,36837,14859,14043);



//---------------------------------------------------------------------------
tSSAStatement::tSSAStatement(tSSAForm * form,
	risse_size position, tOpCode code)
{
	// フィールドの初期化
	Form = form;
	Position = position;
	Id = Form->GetFunction()->GetFunctionGroup()->GetCompiler()->GetUniqueNumber();
		// Id は文と文を識別できる程度にユニークであればよい
	Code = code;
	Block = NULL;
	Pred = NULL;
	Succ = NULL;
	Declared = NULL;
	Mark = NULL;
	Effective = true;
	Order = risse_size_max;
	FuncExpandFlags = 0;

	// デフォルトの Effective は opecode に従う
	switch(VMInsnInfo[Code].Effect)
	{
	case tVMInsnInfo::vieEffective:
	case tVMInsnInfo::vieVarying:
		Effective = true; // 副作用があると見なす
		break;
	case tVMInsnInfo::vieNonEffective:
		Effective = false;
		break;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::AddUsed(tSSAVariable * var)
{
	var->AddUsed(this);
	Used.push_back(var);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::DeleteUsed(risse_size index)
{
	RISSE_ASSERT(Used.size() > index);
	tSSAVariable * var = Used[index];
	Used.erase(Used.begin() + index);
	var->DeleteUsed(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::OverwriteUsed(tSSAVariable * old_var, tSSAVariable * new_var)
{
	gc_vector<tSSAVariable*>::iterator i = std::find(Used.begin(), Used.end(), old_var);

	// すでに old_var が new_var に書き換わっている可能性もあるので
	// 一応 ASSERT はするが、old_var が無い場合は何もしない
	RISSE_ASSERT(i != Used.end() ||
		std::find(Used.begin(), Used.end(), new_var) != Used.end());

	if(i != Used.end())
		*i = new_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::DeleteUsed()
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
void tSSAStatement::TraceCoalescable()
{
	switch(Code)
	{
	case ocPhi: // phi関数
		{
			RISSE_ASSERT(Declared != NULL);

			// 1 引数以上のphi関数
			// Used と Declared 間の干渉を探る
			// Used を一つずつみていき、Declared と Used が干渉している場合は
			// 干渉を除去する。 Sreedhar らによる方法。
			bool interference_found = false;
			for(gc_vector<tSSAVariable*>::iterator i = Used.begin();
				i != Used.end(); i++)
			{
				if((*i)->CheckInterferenceWith(Declared))
				{
					interference_found = true;
					break;
				}
			}

			if(interference_found)
			{
				tSSAVariable * orig_decl_var = Declared;
				tSSAVariable * tmp_var = new tSSAVariable(Form, NULL, orig_decl_var->GetName());
wxFprintf(stderr, wxT("variable interference found at phi statement, inserting %s at %s\n"),
tmp_var->GetQualifiedName().AsWxString().c_str(),
Block->GetName().AsWxString().c_str());
				tSSAStatement * new_stmt =
					new tSSAStatement(Form, Position, ocAssign);
				new_stmt->AddUsed(const_cast<tSSAVariable*>(tmp_var));
				orig_decl_var->SetDeclared(new_stmt);
				new_stmt->SetDeclared(orig_decl_var);
				tmp_var->SetDeclared(this);
				this->SetDeclared(tmp_var);
				tmp_var->SetValueState(orig_decl_var->GetValueState());
				tmp_var->SetValue(orig_decl_var->GetValue());
				Block->InsertStatement(new_stmt, tSSABlock::sipAfterPhi);

				// 干渉グラフを更新する。
				// tmp_var は ここ一連のすべてのphi関数の宣言、およびその使用変数すべてと
				// 干渉すると見なす。
				// ブロック単位の livein, liveout には影響しない。
				tSSAStatement * cur = this;
				tSSAStatement * first = cur;
				while(cur) { first = cur; cur = cur->Pred; }
				for(cur = first; cur && cur->Code == ocPhi; cur = cur->Succ)
				{
					RISSE_ASSERT(cur->Declared);
					if(tmp_var != cur->Declared) tmp_var->SetInterferenceWith(cur->Declared);
					for(gc_vector<tSSAVariable*>::iterator i =cur-> Used.begin();
						i != cur->Used.end(); i++)
					{
						if(tmp_var != *i)
							tmp_var->SetInterferenceWith(*i);
					}
				}

				// この時点で Declared は、新しく定義されたテンポラリ変数を指しているはず
				RISSE_ASSERT(Declared == tmp_var);
			}

			// 関連する変数の合併を行う
			for(gc_vector<tSSAVariable*>::iterator i = Used.begin();
				i != Used.end(); i++)
					Declared->CoalesceCoalescableList(*i);

		}
		break;

	case ocAssign: // 単純代入

		// 昔はここでコピー伝播を行おうとしていたのだけれども
		// コピー伝播は他の所でやってます

	default: ;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::Coalesce()
{
	// Declared に対して合併を実行する
	// これにより SSA 性は破壊される
	if(Declared) Declared->Coalesce();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::SetTrueBranch(tSSABlock * block)
{
	RISSE_ASSERT(Code == ocBranch);
	if(Targets.size() != 2) Targets.resize(2, NULL);
	Targets[0] = block;
	block->AddPred(Block);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSABlock * tSSAStatement::GetTrueBranch() const
{
	RISSE_ASSERT(Code == ocBranch);
	RISSE_ASSERT(Targets.size() == 2);
	return Targets[0];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::SetFalseBranch(tSSABlock * block)
{
	RISSE_ASSERT(Code == ocBranch);
	if(Targets.size() != 2) Targets.resize(2, NULL);
	Targets[1] = block;
	block->AddPred(Block);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSABlock * tSSAStatement::GetFalseBranch() const
{
	RISSE_ASSERT(Code == ocBranch);
	RISSE_ASSERT(Targets.size() == 2);
	return Targets[1];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::SetJumpTarget(tSSABlock * block)
{
	SetJumpTargetNoSetPred(block);
	block->AddPred(Block);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::SetJumpTargetNoSetPred(tSSABlock * block)
{
	RISSE_ASSERT(Code == ocJump);
	if(Targets.size() != 1) Targets.resize(1, NULL);
	Targets[0] = block;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSABlock * tSSAStatement::GetJumpTarget() const
{
	RISSE_ASSERT(Code == ocJump);
	RISSE_ASSERT(Targets.size() == 1);
	return Targets[0];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::SetTryExitTarget(tSSABlock * block)
{
	RISSE_ASSERT(Code == ocCatchBranch);
	if(Targets.size() < 2) Targets.resize(2, NULL);
	Targets[0] = block;
	block->AddPred(Block);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSABlock * tSSAStatement::GetTryExitTarget() const
{
	RISSE_ASSERT(Code == ocCatchBranch);
	RISSE_ASSERT(Targets.size() >= 2);
	return Targets[0];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::SetTryCatchTarget(tSSABlock * block)
{
	RISSE_ASSERT(Code == ocCatchBranch);
	if(Targets.size() < 2) Targets.resize(2, NULL);
	Targets[1] = block;
	block->AddPred(Block);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSABlock * tSSAStatement::GetTryCatchTarget() const
{
	RISSE_ASSERT(Code == ocCatchBranch);
	RISSE_ASSERT(Targets.size() >= 2);
	return Targets[1];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::AddTarget(tSSABlock * block)
{
	RISSE_ASSERT(Code == ocCatchBranch);
	RISSE_ASSERT(Targets.size() >= 2);
	Targets.push_back(block);
	block->AddPred(Block);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::SetName(const tString & name)
{
	RISSE_ASSERT(
			Code == ocParentRead || Code == ocParentWrite ||
			Code == ocChildRead || Code == ocChildWrite ||
			Code == ocReadVar || Code == ocWriteVar ||
			Code == ocRead || Code == ocWrite ||
			Code == ocDefineLazyBlock || Code == ocDefineClass || Code == ocAddBindingMap);
	Name = new tString(name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const tString & tSSAStatement::GetName() const
{
	RISSE_ASSERT(
			Code == ocParentRead || Code == ocParentWrite ||
			Code == ocChildRead || Code == ocChildWrite ||
			Code == ocReadVar || Code == ocWriteVar ||
			Code == ocRead || Code == ocWrite ||
			Code == ocDefineLazyBlock || Code == ocDefineClass || Code == ocAddBindingMap);
	RISSE_ASSERT(Name != NULL);
	return *Name;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::SetAssertType(tVariant::tType type)
{
	RISSE_ASSERT(Code == ocAssertType);
	AssertType = type;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::CreateVariableInterferenceGraph(gc_map<const tSSAVariable *, risse_size> &livemap)
{
	// 一応このメソッドが実行時には SSA性が保持されていると見なす
	RISSE_ASSERT(Block->GetForm()->GetState() == tSSAForm::ssSSA);
	RISSE_ASSERT(Order != risse_size_max); // Order が設定されていること

wxFprintf(stderr, wxT("at [%d]:"), (int)Order);

	// この文で定義された変数があるならば livemap にその変数を追加する
	bool has_new_declared = false;
	if(Declared)
	{
		// ただし、dead store な場合は追加しない
		if(Declared->GetUsed().size() != 0)
		{
			// 既にlivemap にあるわけがない(LiveInにそれがある場合は除く)
			RISSE_ASSERT(Block->GetLiveness(Declared, false) ||
				livemap.find(Declared) == livemap.end());
			// livemap に追加
			livemap.insert(gc_map<const tSSAVariable *, risse_size>::value_type(Declared, risse_size_max));
			has_new_declared = true;
wxFprintf(stderr, wxT("adding %s  "), Declared->GetQualifiedName().AsWxString().c_str());
		}
	}

	bool interf_added = false;
	if(!(Code == ocAssign || Code == ocPhi))
	{
		// 一時的処置
		// TODO: これあとで削除
		// 現時点では各命令は3アドレス方式をとっており、また
		// レジスタの read と write が同じレジスタにたいしておこると
		// おかしくなる場合があるため、全面的に2アドレス方式をとるまでは
		// 一時的に read と write が干渉している (=別のレジスタに割り当たる)
		// ようにする。ただし ocAssign / ocPhi は別。
		if(Declared)
		{
			for(gc_map<const tSSAVariable *, risse_size>::iterator li = livemap.begin();
				li != livemap.end(); li++)
			{
				if(li->first == Declared) continue;
				Declared->SetInterferenceWith(const_cast<tSSAVariable *>(li->first));
wxFprintf(stderr, wxT("interf %s - %s  "), Declared->GetQualifiedName().AsWxString().c_str(), li->first->GetQualifiedName().AsWxString().c_str());
			}
		}
		interf_added = true;
	}

	// この文で使用された変数があり、それがこの文で使用が終了していればlivemapから削除する
	// この文で使用が終了しているかどうかの判定は、
	// ・ブロックのLiveOut にその変数がない
	// かつ
	// ・このブロックにこれ以降この変数を使用している箇所がない
	for(gc_vector<tSSAVariable*>::const_iterator i = Used.begin();
		i != Used.end(); i++)
		(*i)->SetMark(NULL); // マークをいったんクリア

	for(gc_vector<tSSAVariable*>::const_iterator i = Used.begin();
		i != Used.end(); i++)
	{
		// この変数は既に処理をしたか
		if((*i)->GetMark() != NULL) continue; // 既に処理されている
		(*i)->SetMark(this);

		// Block の LiveOut にその変数があるか
		if(Block->GetLiveness(*i, true)) continue; // まだ生きている
		// このブロックにこれ以降この変数を使用している箇所がないか
		bool is_last = true;
		risse_size current_order = Order;
		const gc_vector<tSSAStatement *> & used_list = (*i)->GetUsed();
		for(gc_vector<tSSAStatement *>::const_iterator si = used_list.begin();
			si != used_list.end(); si++)
		{
			if((*si)->Block == Block)
			{
				risse_size order = (*si)->GetOrder();
				if(order > current_order)
				{
					is_last = false;
					break;
				}
			}
		}

		if(is_last)
		{
			// livemapからこれを削除する
			// φ関数などで前の関数と同じ順位の場合は
			// 前の関数ですでに変数が削除されている可能性がある
wxFprintf(stderr, wxT("deleting %s:"), (*i)->GetQualifiedName().AsWxString().c_str());
			gc_map<const tSSAVariable *, risse_size>::iterator fi =
				livemap.find((*i));
			RISSE_ASSERT(Code == ocPhi || fi != livemap.end());
			if(fi != livemap.end())
			{
wxFprintf(stderr, wxT("deleted  "), (*i)->GetQualifiedName().AsWxString().c_str());
				livemap.erase(fi);
			}
			else
			{
wxFprintf(stderr, wxT("not found  "), (*i)->GetQualifiedName().AsWxString().c_str());
			}
		}
	}

	// 宣言された変数がある場合は、変数の干渉を追加する
	if(!interf_added)
	{
		if(Declared)
		{
			for(gc_map<const tSSAVariable *, risse_size>::iterator li = livemap.begin();
				li != livemap.end(); li++)
			{
				if(li->first == Declared) continue;
				Declared->SetInterferenceWith(const_cast<tSSAVariable *>(li->first));
wxFprintf(stderr, wxT("interf %s - %s  "), Declared->GetQualifiedName().AsWxString().c_str(), li->first->GetQualifiedName().AsWxString().c_str());
			}
		}
	}

wxFprintf(stderr, wxT("\n"));

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::OptimizeAtStatementLevel(gc_map<risse_size, tSSAStatement *> &statements)
{
	// コピー伝播
	if(Code == ocPhi && Used.size() == 1 ||
		Code == ocAssign)
	{
		// 引数が一個の phi あるいは単純コピーの場合
		// コピー伝播を行う
		RISSE_ASSERT(Declared != NULL);
		RISSE_ASSERT(Used.size() == 1);
		// コピー先である Declared で使用している文に対し、
		// Declared を使用している所をすべて Used[0] に置き換える。
		const gc_vector<tSSAStatement *> & used_list = Declared->GetUsed();
		for(gc_vector<tSSAStatement *>::const_iterator si = used_list.begin();
			si != used_list.end(); si++)
		{
			if(*si == this) continue; // 自分自身は除外
			(*si)->OverwriteUsed(Declared, Used[0]); // Declared を Used[0] に置き換え
			// Used[0] の Used に (*si) を追加
			Used[0]->AddUsed(*si);
		}
		// Used[0] の Used から this を削除
		Used[0]->DeleteUsed(this);
		// this を block から削除
		Block->DeleteStatement(this);
		// statements から this を削除
		gc_map<risse_size, tSSAStatement *>::iterator sti = statements.find(this->GetId());
		if(sti != statements.end()) statements.erase(sti);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::AnalyzeConstantPropagation(
		gc_vector<tSSAVariable *> &variables,
		gc_vector<tSSABlock *> &blocks)
{
	// Code ごとに処理を行う

	// phi 関数以外は、この文を含むブロックが実行可能であると分かっていない場合は
	// 精査しない
	if(Code != ocPhi && !Block->GetAlive()) return;

wxFprintf(stderr, wxT("at block %s : %s\n"), Block->GetName().AsWxString().c_str(), Dump().AsWxString().c_str());

	// まずは分岐系
	switch(Code)
	{
	//--------------- 分岐関連
	case ocJump:
		// ジャンプ先は生き残る
		{
			tSSABlock * block = GetJumpTarget();
			if(!block->GetAlive())
			{
wxFprintf(stderr, wxT("ocJump at %s, pushing the target %s\n"),
		Block->GetName().AsWxString().c_str(),
		block->GetName().AsWxString().c_str());
				block->SetAlive(true);
				blocks.push_back(block);
			}
		}
		return;

	case ocBranch:
		{
			// Used[0] の状態による
			bool push_true = false;
			bool push_false = false;
			if(Used[0]->GetValueState() == tSSAVariable::vsConstant)
			{
				// Used[0] が定数ならばどっちに行くかがわかるはず
				if((bool)Used[0]->GetValue())
					push_true = true;
				else
					push_false = true;
			}
			else if(Used[0]->GetValueState() == tSSAVariable::vsTypeConstant)
			{
				// タイプによっては必ずfalseとして評価される物がある
				switch(Used[0]->GetValue().GetType())
				{
				case tVariant::vtVoid:
				case tVariant::vtNull:
					// この二つは必ず偽になる
					push_false = true;
					break;
				default:
					// どっちに行くかわからない
					push_false = true;
					push_true = true;
				}
			}
			else if(Used[0]->GetValueState() == tSSAVariable::vsVarying)
			{
				// Used[0] がとる値が複数あり得るならば、どちらに行くかわからない
				push_false = true;
				push_true = true;
			}

			tSSABlock * block;
			if(push_false)
			{
				block = GetFalseBranch();
				if(!block->GetAlive())
				{
wxFprintf(stderr, wxT("ocBranch at %s, pushing the false target %s\n"),
		Block->GetName().AsWxString().c_str(),
		block->GetName().AsWxString().c_str());
					block->SetAlive(true);
					blocks.push_back(block);
				}
			}
			if(push_true)
			{
				block = GetTrueBranch();
				if(!block->GetAlive())
				{
wxFprintf(stderr, wxT("ocBranch at %s, pushing the true target %s\n"),
		Block->GetName().AsWxString().c_str(),
		block->GetName().AsWxString().c_str());
					block->SetAlive(true);
					blocks.push_back(block);
				}
			}
		}
		return;

	case ocCatchBranch:
		// すべての target に分岐する可能性があるのですべてを生存していると見なす
		for(gc_vector<tSSABlock *>::iterator i = Targets.begin(); i != Targets.end();
			i++)
		{
			if(!(*i)->GetAlive())
			{
wxFprintf(stderr, wxT("ocCatchBranch at %s, pushing the target %s\n"),
		Block->GetName().AsWxString().c_str(),
		(*i)->GetName().AsWxString().c_str());
				(*i)->SetAlive(true);
				blocks.push_back(*i);
			}
		}
		return;

	default: ;
	}



	// Declared の古い状態をとっておく
	tSSAVariable::tValueState old_value_state     =
		Declared ? Declared->GetValueState() : tSSAVariable::vsUnknown;

	// phi 関数以外は、基本的に使用されいてる値によって定義されている
	// 変数の型や値がどうなるかを考える
	// たぶんここら辺の話は Modern Compiler Implementation in * に
	// 書いてあると思う


	switch(Code)
	{
	//--------------- φ関数
	case ocPhi:
	{
		const gc_vector<tSSABlock *> & pred_blocks = Block->GetPred();
		RISSE_ASSERT(Declared);
		RISSE_ASSERT(Used.size() == pred_blocks.size());

		// φ関数に副作用はない
		Effective = false;

		// Modern Compiler Implementation in * では条件付き定数伝播における
		// 変数の状態において ⊥とCとT の3つの状態を元にアルゴリズムを解説しているが、
		// Risse の場合はさらにこれに「型が決まっているが値が決まっていない状態」を
		// 追加して、vsUnknown, vsConstant, vsTypeConstant, vsVarying の4つの状態をとると
		// している。

		// 実行可能な pred ブロックの中から最も強い 状態を一つ得る
		risse_size t_idx = risse_size_max; // そのインデックス
		tVariant t_value; // その値または型
		tSSAVariable::tValueState t_state; // その状態

		for(risse_size i = 0; i < pred_blocks.size(); i++)
		{
			if(pred_blocks[i]->GetAlive())
			{
				tSSAVariable::tValueState i_state = Used[i]->GetValueState();
				if(t_idx == risse_size_max || t_state < i_state)
				{
					t_state = i_state;
					t_idx = i;
					if(i_state == tSSAVariable::vsConstant || i_state == tSSAVariable::vsTypeConstant)
						t_value = Used[i]->GetValue();
				}
			}
		}

		// その最も強い状態に従って:
		if(t_idx != risse_size_max)
		{
			switch(t_state)
			{
			case tSSAVariable::vsUnknown:
				// うーん、この場合はどういう場合なんだろう
				break;

			case tSSAVariable::vsConstant:
				// 定数
				// 他のすべてのUsedが下のいずれかの場合
				// ・vsUnknown
				// ・値がC
				// ・その方向のPredが実行不可能
				// このφ関数で定義された変数は C になる
				// ただし、実行可能なブロックのうち値がCと異なる
				// 物があれば、C の型が同じならば vsTypeConstant 、
				// C の型が異なれば vsVarying になる
				{
					tSSAVariable::tValueState new_state = tSSAVariable::vsConstant;
					for(risse_size i = 0; i < pred_blocks.size(); i++)
					{
						if(i == t_idx) continue;
						bool value_same = false;
						if(pred_blocks[i]->GetAlive())
						{
							if(Used[i]->GetValueState() == tSSAVariable::vsConstant)
							{
								// その変数は定数であることがわかっている
								if(!Used[i]->GetValue().StrictEqual(t_value))
								{
									// 値が違う
									if(Used[i]->GetValue().GetType() != t_value.GetType())
									{
										// 型も違う
										// これは vsVarying にしかならない
										new_state = tSSAVariable::vsVarying;
										break;
									}
									else
									{
										// 値が違うが型は同じ
										// これは vsTypeConstant になる可能性がある
										if(new_state < tSSAVariable::vsTypeConstant)
											new_state = tSSAVariable::vsTypeConstant;
										// break はしない。さらに見つかった変数によっては
										// vsVarying になるかもしれないから。
										value_same = true;
									}
								}
								else
								{
									value_same = true;
								}
							}
							else if(Used[i]->GetValueState() == tSSAVariable::vsTypeConstant)
							{
								// その変数は型が決まっていることがわかっている
								if(Used[i]->GetValue().GetType() != t_value.GetType())
								{
									// 型が違う
									// これは vsVarying にしかならない
									new_state = tSSAVariable::vsVarying;
									break;
								}
								else
								{
									// 型が同じ
									// これは vsTypeConstant になる可能性がある
									if(new_state < tSSAVariable::vsTypeConstant)
										new_state = tSSAVariable::vsTypeConstant;
									// break はしない。さらに見つかった変数によっては
									// vsVarying になるかもしれないから。
									value_same = true;
								}
							}
						}

						if(!(
							Used[i]->GetValueState() == tSSAVariable::vsUnknown ||
							value_same ||
							!pred_blocks[i]->GetAlive()
							))
						{
							// 条件と違うのが見つかった
							new_state = tSSAVariable::vsUnknown;
							break;
						}
					}
					// Declared に情報を設定
					if(new_state == tSSAVariable::vsConstant)
						Declared->SuggestValue(t_value);
					else if(new_state == tSSAVariable::vsTypeConstant)
						Declared->SuggestValue(t_value.GetType());
					if(Block->GetAlive())
					{
						if(new_state == tSSAVariable::vsVarying)
							Declared->RaiseValueState(tSSAVariable::vsVarying);
					}
				}

				break;

			case tSSAVariable::vsTypeConstant:
				// 型が決まっている
				// 他のすべてのUsedが下のいずれかの場合
				// ・vsUnknown
				// ・vsConstantかつ値の型がC
				// ・vsTypeConstantでC
				// ・その方向のPredが実行不可能
				// このφ関数で定義された変数の型 C になる
				// ただし、実行可能なブロックのうち値の型がCと異なる
				// 物があれば vsVarying になる
				{
					tSSAVariable::tValueState new_state = tSSAVariable::vsTypeConstant;
					for(risse_size i = 0; i < pred_blocks.size(); i++)
					{
						if(i == t_idx) continue;
						bool value_same = false;
						if(pred_blocks[i]->GetAlive())
						{
							if(Used[i]->GetValueState() == tSSAVariable::vsConstant ||
								Used[i]->GetValueState() == tSSAVariable::vsTypeConstant)
							{
								// その変数は定数であること、あるいは型が決まっていることがわかっている
								if(Used[i]->GetValue().GetType() != t_value.GetType())
								{
									// 型も違う
									// これは vsVarying にしかならない
									new_state = tSSAVariable::vsVarying;
									break;
								}
								else
								{
									// 値が違うが型は同じ
									// これは vsTypeConstant になる可能性がある
									if(new_state < tSSAVariable::vsTypeConstant)
										new_state = tSSAVariable::vsTypeConstant;
									// break はしない。さらに見つかった変数によっては
									// vsVarying になるかもしれないから。
									value_same = true;
								}
							}
						}

						if(!(
							Used[i]->GetValueState() == tSSAVariable::vsUnknown ||
							value_same ||
							!pred_blocks[i]->GetAlive()
							))
						{
							// 条件と違うのが見つかった
							new_state = tSSAVariable::vsUnknown;
							break;
						}
					}
					// Declared に情報を設定
					if(new_state == tSSAVariable::vsTypeConstant)
						Declared->SuggestValue(t_value.GetType());
					if(Block->GetAlive())
					{
						if(new_state == tSSAVariable::vsVarying)
							Declared->RaiseValueState(tSSAVariable::vsVarying);
					}
				}

				break;

			case tSSAVariable::vsVarying:
				// Declared は必ず Varying
				Declared->RaiseValueState(tSSAVariable::vsVarying);
				break;
			}
		}

		break;
	}

	//--------------- 単純代入
	case ocAssign:
		RISSE_ASSERT(Used.size() == 1);
		RISSE_ASSERT(Declared);

		Effective = false; // 単純代入に副作用はない

		Declared->RaiseValueState(Used[0]->GetValueState());
		if(Declared->GetValueState() == tSSAVariable::vsConstant)
			Declared->SuggestValue(Used[0]->GetValue());
		else if(Declared->GetValueState() == tSSAVariable::vsTypeConstant)
			Declared->SuggestValue(Used[0]->GetValue().GetType());
		break;

	//--------------- 定数代入
	case ocAssignConstant:

		Effective = false; // 定数代入に副作用はない

		RISSE_ASSERT(Declared);
		RISSE_ASSERT(Value != NULL);
		Declared->SuggestValue(*Value);
		break;

	//--------------- 値はどうなるかわからないが、型は vtObject を代入する物
	//------ かつ、副作用がない物
	case ocAssignNewBinding:
	case ocAssignThisProxy:
	case ocAssignGlobal:
		RISSE_ASSERT(Declared);
		Effective = false; // 副作用なし
		Declared->SuggestValue(tVariant::vtObject);
		break;

	//------ かつ、副作用がある物
	case ocAssignNewArray:
	case ocAssignNewDict:
	case ocAssignNewRegExp:
	case ocAssignNewFunction:
	case ocAssignNewProperty:
	case ocAssignNewClass:
	case ocAssignNewModule:
	case ocTryFuncCall:
	case ocSync:
	case ocSetFrame:
	case ocSetShare:
		RISSE_ASSERT(Declared);
		Effective = true; // 副作用あり
		Declared->SuggestValue(tVariant::vtObject);
		break;

	//--------------- 値も型もどうなるかわからないもの
	//------ かつ、副作用がない物
	case ocGetExitTryValue:
	case ocAssignThis:
	case ocAssignParam:
	case ocAssignBlockParam:
	case ocRead:
		RISSE_ASSERT(Declared);
		Effective = false; // 副作用なし
		Declared->RaiseValueState(tSSAVariable::vsVarying); // どんな値になるかはわからない
		break;

	//------ かつ、副作用がある物
	case ocNew:
	case ocFuncCall:
	case ocFuncCallBlock:
		RISSE_ASSERT(Declared);
		Effective = true; // 副作用あり
		Declared->RaiseValueState(tSSAVariable::vsVarying); // どんな値になるかはわからない
		break;

	//--------------- 宣言する変数がない物
	//------ かつ、副作用がある物
	case ocAddBindingMap:
	case ocWrite:
	case ocReturn:
	case ocDebugger:
	case ocThrow:
	case ocExitTryException:
		// declared の無いタイプ
		RISSE_ASSERT(Declared == NULL);
		Effective = true; // 副作用あり
		break;

	//-- 単項
	case ocLogNot:
	case ocBitNot:
	case ocPlus:
	case ocMinus:
	case ocString:
	case ocBoolean:
	case ocReal:
	case ocInteger:
	case ocOctet:
		RISSE_ASSERT(Declared);

		{
			RISSE_ASSERT(Used.size() == 1);
			tSSAVariable::tValueState vs = Used[0]->GetValueState();
			int gt;
			// 特定の型あるいは tVariant::gtAny を渡してみて、どの様な型が返ってくる可能性があるかを推測する
#define RISA_GUESS_UNARY(C) \
	case oc##C:	gt = tVariant::GuessType##C			(Used[0]->GetGuessType());	break;
			switch(Code)
			{
				RISA_GUESS_UNARY(LogNot)
				RISA_GUESS_UNARY(BitNot)
				RISA_GUESS_UNARY(Plus)
				RISA_GUESS_UNARY(Minus)
				case ocString:	gt = tVariant::GuessTypeCastToString	(Used[0]->GetGuessType());	break;
				case ocBoolean:	gt = tVariant::GuessTypeCastToBoolean	(Used[0]->GetGuessType());	break;
				case ocReal:	gt = tVariant::GuessTypeCastToReal		(Used[0]->GetGuessType());	break;
				case ocInteger:	gt = tVariant::GuessTypeCastToInteger	(Used[0]->GetGuessType());	break;
				case ocOctet:	gt = tVariant::GuessTypeCastToOctet		(Used[0]->GetGuessType());	break;
				default: RISSE_ASSERT(!"Unhandled type here!"); ;
			}
			Effective = gt & tVariant::gtEffective; // 副作用があるかどうか

			switch(vs)
			{
			case tSSAVariable::vsUnknown:
				break;	// なにもしない
			case tSSAVariable::vsConstant:
				// 定数畳み込みをする
				try
				{
#define RISA_FOLD_UNARY(C) \
	case oc##C:	Declared->SuggestValue(Used[0]->GetValue().C());			break;
					switch(Code)
					{
						RISA_FOLD_UNARY(LogNot)
						RISA_FOLD_UNARY(BitNot)
						RISA_FOLD_UNARY(Plus)
						RISA_FOLD_UNARY(Minus)
						case ocString:	Declared->SuggestValue(Used[0]->GetValue().CastToString	());			break;
						case ocBoolean:	Declared->SuggestValue(Used[0]->GetValue().CastToBoolean());			break;
						case ocReal:	Declared->SuggestValue(Used[0]->GetValue().CastToReal	());			break;
						case ocInteger:	Declared->SuggestValue(Used[0]->GetValue().CastToInteger());			break;
						case ocOctet:	Declared->SuggestValue(Used[0]->GetValue().CastToOctet	());			break;
						default: RISSE_ASSERT(!"Unhandled type here!"); ;
					}
					Effective = false; // コンパイル時に定数畳込みができると言うことは実行時に副作用がないということ
					break;
				}
				catch(...)
				{
					// 何らかの例外が発生したとき
					// この場合は break せずにそのまま下に行って
					// エラーになるような組み合わせならば警告メッセージを表示するようになる
				}

			case tSSAVariable::vsTypeConstant: // 特定の型になる場合
			case tSSAVariable::vsVarying: // 任意の型になる場合

				switch((tVariant::tGuessType)(gt & tVariant::gtTypeMask))
				{
				case tVariant::gtAny: // 任意の型が帰ってくる可能性がある
					Declared->RaiseValueState(tSSAVariable::vsVarying);
					break;
				case tVariant::gtError: // 確実にエラーになるということ
					{
						tString method_name(VMInsnInfo[Code].GetMemberName());
						Mark = new tErrorWarningInfo(
							tString(RISSE_WS_TR("%1::%2() will cause an exception at runtime"),
								tVariant::GetGuessTypeString(Used[0]->GetGuessType()),
								method_name),
							false);
					}
					break;
				default:
					// 必ず特定の型になる場合
					Declared->SuggestValue((tVariant::tType)(gt & tVariant::gtTypeMask));
					break;
				}
				break;
			}
		}
		break;

	//-- 二項
	case ocAdd:
	case ocSub:
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
		RISSE_ASSERT(Declared);

		{
			// ・最初のパラメータがvaryingならば宣言された変数もvarying
			// ・両方とも定数ならば定数たたみ込みできる
			RISSE_ASSERT(Used.size() == 2);

			tSSAVariable::tValueState vs_l = Used[0]->GetValueState();
			tSSAVariable::tValueState vs_r = Used[1]->GetValueState();

			// 両方の値の状態・型から、どの様な結果が返ってくるかを推測する
			tVariant::tGuessType input_gt_l = Used[0]->GetGuessType();
			tVariant::tGuessType input_gt_r = Used[1]->GetGuessType();
			int gt;
#define RISA_GUESS_BINARY(C) \
	case oc##C:			gt = tVariant::GuessType##C			(input_gt_l, input_gt_r); break;
			switch(Code)
			{
				RISA_GUESS_BINARY(Add)
				RISA_GUESS_BINARY(Sub)
				RISA_GUESS_BINARY(BitOr)
				RISA_GUESS_BINARY(BitXor)
				RISA_GUESS_BINARY(BitAnd)
				RISA_GUESS_BINARY(NotEqual)
				RISA_GUESS_BINARY(Equal)
				RISA_GUESS_BINARY(DiscNotEqual)
				RISA_GUESS_BINARY(DiscEqual)
				RISA_GUESS_BINARY(Lesser)
				RISA_GUESS_BINARY(Greater)
				RISA_GUESS_BINARY(LesserOrEqual)
				RISA_GUESS_BINARY(GreaterOrEqual)
				default: RISSE_ASSERT(!"Unhandled type here!"); ;
			}
			Effective = gt & tVariant::gtEffective; // 副作用があるかどうか

			if(vs_l == tSSAVariable::vsConstant && vs_r == tSSAVariable::vsConstant)
			{
				// 定数畳み込みをする
				try
				{
#define RISA_FOLD_BINARY(C) \
	case oc##C:			Declared->SuggestValue(Used[0]->GetValue().C		(Used[1]->GetValue())); break;
					switch(Code)
					{
						RISA_FOLD_BINARY(Add)
						RISA_FOLD_BINARY(Sub)
						RISA_FOLD_BINARY(BitOr)
						RISA_FOLD_BINARY(BitXor)
						RISA_FOLD_BINARY(BitAnd)
						RISA_FOLD_BINARY(NotEqual)
						RISA_FOLD_BINARY(Equal)
						RISA_FOLD_BINARY(DiscNotEqual)
						RISA_FOLD_BINARY(DiscEqual)
						RISA_FOLD_BINARY(Lesser)
						RISA_FOLD_BINARY(Greater)
						RISA_FOLD_BINARY(LesserOrEqual)
						RISA_FOLD_BINARY(GreaterOrEqual)
						default: RISSE_ASSERT(!"Unhandled type here!"); ;
					}
					Effective = false; // コンパイル時に定数畳込みができると言うことは実行時に副作用がないということ
					break; //--------- 一番外側の switch をここで抜けるので注意
				}
				catch(...)
				{
					// 何らかの例外が発生したとき
					// この場合は break せずにそのまま下に行って
					// エラーになるような組み合わせならば警告メッセージを表示するようになる
				}
			}

			if(vs_l != tSSAVariable::vsUnknown && vs_r != tSSAVariable::vsUnknown)
			{
				// この時点で vs_l および vs_r は vsUnknown を除くいずれかの状態
				switch((tVariant::tGuessType)(gt & tVariant::gtTypeMask))
				{
				case tVariant::gtAny: // 任意の型が帰ってくる可能性がある
					Declared->RaiseValueState(tSSAVariable::vsVarying);
					break;
				case tVariant::gtError: // 確実にエラーになるということ
					{
						tString method_name(VMInsnInfo[Code].GetMemberName());
						Mark = new tErrorWarningInfo(
							tString(RISSE_WS_TR("%1::%3(%2) will cause an exception at runtime"),
								tVariant::GetGuessTypeString(input_gt_l),
								tVariant::GetGuessTypeString(input_gt_r),
								method_name),
							false);
					}
					break;
				default:
					// 必ず特定の型になる場合
					Declared->SuggestValue((tVariant::tType)(gt & tVariant::gtTypeMask));
					break;
				}
			}
		}
		break;

	case ocLogOr:
	case ocLogAnd:

	case ocDecAssign:
	case ocIncAssign:
	case ocRBitShift:
	case ocLShift:
	case ocRShift:
	case ocMod:
	case ocDiv:
	case ocIdiv:
	case ocMul:
	case ocInContextOf:
	case ocInContextOfDyn:
	case ocInstanceOf:
	case ocDGet:
	case ocDGetF:
	case ocIGet:
	case ocDDelete:
	case ocIDelete:
	case ocDSetAttrib:
	case ocDSet:
	case ocDSetF:
	case ocISet:
	case ocAssert:
	case ocAssertType:
	case ocBitAndAssign:
	case ocBitOrAssign:
	case ocBitXorAssign:
	case ocSubAssign:
	case ocAddAssign:
	case ocModAssign:
	case ocDivAssign:
	case ocIdivAssign:
	case ocMulAssign:
	case ocLogOrAssign:
	case ocLogAndAssign:
	case ocRBitShiftAssign:
	case ocLShiftAssign:
	case ocRShiftAssign:
	case ocSetDefaultContext:
	case ocGetDefaultContext:
	case ocDefineAccessMap:
	case ocDefineLazyBlock:
	case ocDefineClass:
	case ocEndAccessMap:
	case ocParentWrite:
	case ocParentRead:
	case ocChildWrite:
	case ocChildRead:
	case ocWriteVar:
	case ocReadVar:
	case ocOpCodeLast:


	//--------------- ???? な物
	case ocAssignSuper:
	case ocNoOperation:
	case ocVMCodeLast:
		// とりあえず Declared を varying に設定してしまおう
		if(Declared) Declared->RaiseValueState(tSSAVariable::vsVarying); // どんな値になるかはわからない
		break;

	case ocJump:
	case ocBranch:
	case ocCatchBranch:
		; // すでに処理済み
	}

	if(Declared)
	{
		// Declared の ValueState や ValueTypeState がランクアップしているようだったら
		// variables に Declared を push する
		if(old_value_state < Declared->GetValueState())
		{
wxFprintf(stderr, wxT("changing state of %s from %s to %s\n"),
Declared->GetQualifiedName().AsWxString().c_str(),

old_value_state==tSSAVariable::vsUnknown ? wxT("unknown"):
old_value_state==tSSAVariable::vsConstant ? wxT("constant"):
old_value_state==tSSAVariable::vsTypeConstant ? wxT("type constant"):
old_value_state==tSSAVariable::vsVarying ? wxT("varying") : wxT(""),

Declared->GetValueState()==tSSAVariable::vsUnknown ? wxT("unknown"):
Declared->GetValueState()==tSSAVariable::vsConstant ? wxT("constant"):
Declared->GetValueState()==tSSAVariable::vsTypeConstant ? wxT("type constant"):
Declared->GetValueState()==tSSAVariable::vsVarying ? wxT("varying") : wxT("")

);
			variables.push_back(Declared);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::RealizeConstantPropagationErrors()
{
	if(!Mark) return;
	tErrorWarningInfo * info = reinterpret_cast<tErrorWarningInfo *>(Mark);
	if(info->Error)
	{
		tCompileExceptionClass::Throw(
			Form->GetFunction()->GetFunctionGroup()->
				GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
			info->Message,
				Form->GetScriptBlockInstance(), GetPosition());
	}
	else
	{
		Form->GetFunction()->GetFunctionGroup()->
			GetCompiler()->GetScriptBlockInstance()->OutputWarning(GetPosition(), 
			info->Message);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::InsertTypeAssertion()
{
	// Declared の ValueState が vsTypeConstant あるいは vsConstant の場合、直後に
	// ocAssertType 文を挿入する
	if(Declared &&
		(Declared->GetValueState() == tSSAVariable::vsTypeConstant ||
		 Declared->GetValueState() == tSSAVariable::vsConstant))
	{
		tSSAStatement * new_stmt =
			new tSSAStatement(Form, Position, ocAssertType);
		new_stmt->AddUsed(Declared);
		new_stmt->SetAssertType(Declared->GetValue().GetType());
		if(Code == ocPhi)
			Block->InsertStatement(new_stmt, tSSABlock::sipAfterPhi);
		else
			Block->InsertStatement(new_stmt, this);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::ReplaceConstantAssign()
{
	// 定数が代入されることが分かっている文を定数代入文に置き換える
	if(Code != ocAssignConstant && Declared &&
		 Declared->GetValueState() == tSSAVariable::vsConstant)
	{
		// この文を定数代入文に置き換える
		tVariant *constant = new tVariant(Declared->GetValue());

		// 文の作成
		tSSAStatement * new_stmt =
			new tSSAStatement(Form, Position, ocAssignConstant);
		new_stmt->SetValue(constant);
		new_stmt->SetDeclared(Declared);
		Declared->SetDeclared(new_stmt);

		// この文で使用していた変数をすべて開放する
		DeleteUsed();

		// 文の追加
		if(Code == ocPhi)
			Block->InsertStatement(new_stmt, tSSABlock::sipAfterPhi);
		else
			Block->InsertStatement(new_stmt, this);

		// この文を削除する
		Block->DeleteStatement(this);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::Check3AddrAssignee()
{
	// VM命令の中にはdestinationとその他の引数が同じだった場合に異常な動作をする
	// 命令があるため、ここで暫定的に対処。本来はコードジェネレータがやるべき
	// 仕事かもしれないね。
	if(!Declared) return;
	if(Code == ocAssign || Code == ocPhi) return; // 単純コピーやφ関数は無視

	bool do_save = false;
	for(gc_vector<tSSAVariable*>::const_iterator i = Used.begin();
		i != Used.end(); i++)
	{
		if((*i) == Declared)
		{
			do_save = true;
			break;
		}
	}

	if(do_save)
	{
		tSSAVariable * orig_decl_var = Declared;
		tSSAVariable * tmp_var = new tSSAVariable(Form, NULL, orig_decl_var->GetName());
wxFprintf(stderr, wxT("assignee is the same with the argument, inserting %s after [%d]\n"),
tmp_var->GetQualifiedName().AsWxString().c_str(),
(int)Order);
		tSSAStatement * new_stmt =
			new tSSAStatement(Form, Position, ocAssign);
		new_stmt->AddUsed(const_cast<tSSAVariable*>(tmp_var));
		orig_decl_var->SetDeclared(new_stmt);
		new_stmt->SetDeclared(orig_decl_var);
		tmp_var->SetDeclared(this);
		this->SetDeclared(tmp_var);
		tmp_var->SetValueState(orig_decl_var->GetValueState());
		tmp_var->SetValue(orig_decl_var->GetValue());
		Block->InsertStatement(new_stmt, this);

		// 干渉グラフを更新する。
		// tmp_var はここで生きているすべての文と干渉すると見なす。
		// ここで生きているすべての文というのは、この文の使用と宣言
		// そのものと、それが干渉している物すべて。
		// ブロック単位の livein, liveout には影響しない。
		for(gc_vector<tSSAVariable*>::iterator i =Used.begin();
			i != Used.end(); i++)
			tmp_var->SetInterferenceWithAll(*i);
		tmp_var->SetInterferenceWithAll(orig_decl_var);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::AssignRegisters(gc_vector<void*> & assign_work)
{
	// Declared および Used をみて、それぞれの変数にレジスタが割り当たってないようならば
	// レジスタを割り当てる
	if(Declared) Declared->AssignRegister(assign_work);
	for(gc_vector<tSSAVariable*>::const_iterator i = Used.begin();
		i != Used.end(); i++)
		(*i)->AssignRegister(assign_work);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAStatement::GenerateCode(tCodeGenerator * gen) const
{
	// gen にソースコード上の位置を伝える
	gen->SetSourceCodePosition(Position);

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
		RISSE_ASSERT(Value != NULL);
		gen->PutAssign(Declared, *Value);
		break;

	case ocAssignNewBinding:
	case ocAssignThis:
	case ocAssignThisProxy:
	case ocAssignSuper:
	case ocAssignGlobal:
	case ocAssignNewArray:
	case ocAssignNewDict:
		RISSE_ASSERT(Declared != NULL);
		gen->PutAssign(Declared, Code);
		break;

	case ocAssignNewRegExp:
		RISSE_ASSERT(Declared != NULL);
		RISSE_ASSERT(Used.size() == 2);
		gen->PutAssignNewRegExp(Declared, Used[0], Used[1]);
		break;

	case ocAssignNewFunction:
		RISSE_ASSERT(Declared != NULL);
		RISSE_ASSERT(Used.size() == 1);
		gen->PutAssignNewFunction(Declared, Used[0]);
		break;

	case ocAssignNewProperty:
		RISSE_ASSERT(Declared != NULL);
		RISSE_ASSERT(Used.size() == 2);
		gen->PutAssignNewProperty(Declared, Used[0], Used[1]);
		break;

	case ocAssignNewClass:
		RISSE_ASSERT(Declared != NULL);
		RISSE_ASSERT(Used.size() == 2);
		gen->PutAssignNewClass(Declared, Used[0], Used[1]);
		break;

	case ocAssignNewModule:
		RISSE_ASSERT(Declared != NULL);
		RISSE_ASSERT(Used.size() == 1);
		gen->PutAssignNewModule(Declared, Used[0]);
		break;

	case ocAssignParam:
		RISSE_ASSERT(Declared != NULL);
		gen->PutAssignParam(Declared, Index);
		break;

	case ocAssignBlockParam:
		RISSE_ASSERT(Declared != NULL);
		gen->PutAssignBlockParam(Declared, Index);
		break;

	case ocAddBindingMap:
		RISSE_ASSERT(Used.size() == 2);
		gen->PutAddBindingMap(Used[0], Used[1], *Name);
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

	case ocNew:
	case ocFuncCall:
	case ocTryFuncCall:
		RISSE_ASSERT(Declared != NULL);
		RISSE_ASSERT(Used.size() >= 1);

		{
			gc_vector<const tSSAVariable *> args, blocks;

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

	case ocSync:
		RISSE_ASSERT(Declared != NULL);
		RISSE_ASSERT(Used.size() == 2);
		gen->PutSync(Declared, Used[0], Used[1]);
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
	case ocString:
	case ocBoolean:
	case ocReal:
	case ocInteger:
	case ocOctet:
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
	case ocInstanceOf:
	case ocRBitShift:
	case ocLShift:
	case ocRShift:
	case ocMod:
	case ocDiv:
	case ocIdiv:
	case ocMul:
	case ocAdd:
	case ocSub:
		RISSE_ASSERT(Declared != NULL);
		RISSE_ASSERT(Used.size() == 2);
		gen->PutOperator(Code, Declared, Used[0], Used[1]);
		break;

	case ocInContextOf:
		RISSE_ASSERT(Declared != NULL);
		RISSE_ASSERT(Used.size() == 1 || Used.size() == 2);
		if(Used.size() == 2)
			gen->PutInContextOf(Declared, Used[0], Used[1]); // 普通の incontextof
		else
			gen->PutInContextOf(Declared, Used[0], NULL); // incontextof dynamic
		break;

	case ocDGet:
	case ocIGet:
		RISSE_ASSERT(Declared != NULL);
		RISSE_ASSERT(Used.size() == 2);
		gen->PutGet(Code, Declared, Used[0], Used[1], OperateFlagsValue);
		break;

	case ocDDelete:
	case ocIDelete:
		RISSE_ASSERT(Declared != NULL);
		RISSE_ASSERT(Used.size() == 2);
		gen->PutOperator(Code, Declared, Used[0], Used[1]);
		break;

	case ocDSet:
	case ocISet:
		RISSE_ASSERT(Used.size() == 3);
		gen->PutSet(Code, Used[0], Used[1], Used[2], OperateFlagsValue);
		break;

	case ocDSetAttrib:
		RISSE_ASSERT(Used.size() == 2);
		gen->PutSetAttribute(Used[0], Used[1], OperateFlagsValue);
		break;

	case ocAssert:
		RISSE_ASSERT(Used.size() == 1);
		gen->PutAssert(Used[0], GetMessage());
		break;

	case ocAssertType:
		RISSE_ASSERT(Used.size() == 1);
		gen->PutAssertType(Used[0], GetAssertType());
		break;

	case ocDefineAccessMap:
		{
			RISSE_ASSERT(Declared != NULL);
		}
		break;

	case ocDefineLazyBlock:
		{
			tSSAForm * child_form = DefinedForm;
			RISSE_ASSERT(child_form != NULL);
			RISSE_ASSERT(Declared != NULL);
			// この文のDeclaredは、子SSA形式を作成して返すようになっているが、
			// コードブロックの参照の問題があるのでいったんリロケーション用の機構を通す
			gen->PutCodeBlockRelocatee(Declared, child_form->GetCodeBlockIndex());
			if(child_form->GetUseParentFrame())
				gen->PutSetFrame(Declared);
			else
				gen->PutSetShare(Declared);
		}
		break;

	case ocDefineClass:
		{
			tSSAForm * class_form = DefinedForm;
			RISSE_ASSERT(class_form != NULL);
			RISSE_ASSERT(Declared != NULL);
			// この文のDeclaredは、子SSA形式を作成して返すようになっているが、
			// コードブロックの参照の問題があるのでいったんリロケーション用の機構を通す
			gen->PutCodeBlockRelocatee(Declared, class_form->GetCodeBlockIndex());
		}
		break;

	case ocChildWrite:
		{
			RISSE_ASSERT(Used.size() == 2);
			RISSE_ASSERT(Declared != NULL);
			RISSE_ASSERT(Name != NULL);
			// Used[0] が define された文は ocDefineAccessMap であるはずである
			// TODO: この部分は変数の併合を実装するに当たり書き換わる可能性が高い。
			//       現状の実装は暫定的なもの。
			tSSAStatement * lazy_stmt = Used[0]->GetDeclared();
			RISSE_ASSERT(lazy_stmt->GetCode() == ocDefineAccessMap);
wxFprintf(stderr, wxT("registering %s\n"), Name->AsWxString().c_str());
			gen->RegisterVariableMapForChildren(Declared, *Name);
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
			tSSAStatement * lazy_stmt = Used[0]->GetDeclared();
			RISSE_ASSERT(lazy_stmt->GetCode() == ocDefineAccessMap);
			gen->PutAssign(Declared, gen->FindVariableMapForChildren(*Name));
		}
		break;

	case ocEndAccessMap:
		// アクセスマップの使用終了
		// 暫定実装
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

	default:
wxFprintf(stderr, wxT("not acceptable SSA operation code %d\n"), (int)Code);
		RISSE_ASSERT(!"not acceptable SSA operation code");
		break;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tSSAStatement::DumpBody() const
{
	switch(Code)
	{
	case ocNoOperation:			// なにもしない
		return RISSE_WS("nop");

	case ocPhi:		// φ関数
		{
			tString ret;

			RISSE_ASSERT(Declared != NULL);
			ret += Declared->Dump() + RISSE_WS(" = PHI(");

			// φ関数の引数を追加
			tString used;
			for(gc_vector<tSSAVariable*>::const_iterator i = Used.begin();
					i != Used.end(); i++)
			{
				if(!used.IsEmpty()) used += RISSE_WS(", ");
				used += (*i)->Dump();
			}

			ret += used + RISSE_WS(")");

			return ret;
		}

	case ocAssignNewRegExp: // 新しい正規表現オブジェクト
		{
			tString ret;
			RISSE_ASSERT(Used.size() == 2);
			ret += Declared->Dump() + RISSE_WS(" = AssignNewRegExp(");

			ret +=	Used[0]->Dump() + RISSE_WS(", ") +
					Used[1]->Dump() + RISSE_WS(")");

			return ret;
		}

	case ocAssignNewFunction: // 新しい関数インスタンス
		{
			tString ret;
			RISSE_ASSERT(Used.size() == 1);
			ret += Declared->Dump() + RISSE_WS(" = AssignNewFunction(");

			ret +=	Used[0]->Dump() + RISSE_WS(")");

			return ret;
		}

	case ocAssignNewProperty: // 新しいプロパティインスタンス
		{
			tString ret;
			RISSE_ASSERT(Used.size() == 2);
			ret += Declared->Dump() + RISSE_WS(" = AssignNewProperty(");

			ret +=	Used[0]->Dump() + RISSE_WS(", ") +
					Used[1]->Dump() + RISSE_WS(")");

			return ret;
		}

	case ocAssignNewClass: // 新しいクラスインスタンス
		{
			tString ret;
			RISSE_ASSERT(Used.size() == 2);
			ret += Declared->Dump() + RISSE_WS(" = AssignNewClass(");

			ret +=	Used[0]->Dump() + RISSE_WS(", ") +
					Used[1]->Dump() + RISSE_WS(")");

			return ret;
		}

	case ocAssignNewModule: // 新しいモジュールインスタンス
		{
			tString ret;
			RISSE_ASSERT(Used.size() == 1);
			ret += Declared->Dump() + RISSE_WS(" = AssignNewModule(");

			ret +=	Used[0]->Dump() + RISSE_WS(")");

			return ret;
		}

	case ocAssignParam:
		{
			tString ret;
			RISSE_ASSERT(Used.size() == 0);
			ret += Declared->Dump() + RISSE_WS(" = AssignParam(") +
				tString::AsString((risse_int)Index) + RISSE_WS(")");

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
			tString ret =
					RISSE_WS("catch branch (try id=") +
					tString::AsString((risse_int64)(TryIdentifierIndex)) +
					RISSE_WS(") ") +
					(*Used.begin())->Dump() + 
					RISSE_WS(" exit:*") + GetTryExitTarget()->GetName() +
					RISSE_WS(" catch:*") + GetTryCatchTarget()->GetName();
			for(risse_size n = 2; n < Targets.size(); n++)
			{
				ret += RISSE_WS(" ") + tString::AsString((risse_int64)(n)) +
					RISSE_WS(": *") + Targets[n]->GetName();
			}
			return ret;
		}

	case ocExitTryException:
		{
			RISSE_ASSERT(Used.size() <= 1);
			tString ret;
			ret = RISSE_WS("ExitTryException(") +
				((Used.size() >= 1)?(Used[0]->Dump()):tString(RISSE_WS("<none>"))) +
				RISSE_WS(", try_id=")+
				tString::AsString((risse_int64)(TryIdentifierIndex)) +
				RISSE_WS(", index=")+
				tString::AsString((risse_int64)(Index)) +
				RISSE_WS(")");
			return ret;
		}

	case ocDefineLazyBlock: // 遅延評価ブロックの定義
	case ocDefineClass: // クラスの定義
		{
			RISSE_ASSERT(Name != NULL);
			RISSE_ASSERT(Declared != NULL);

			tString ret;
			ret += Declared->Dump() +
				(Code == ocDefineLazyBlock ?
					RISSE_WS(" = DefineLazyBlock(") :
					RISSE_WS(" = DefineClass(") );

			ret +=	Name->AsHumanReadable() + RISSE_WS(")");

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

			tString ret;
			ret += Declared->Dump() + RISSE_WS(" = ParentRead(");

			ret +=	Name->AsHumanReadable() + RISSE_WS(")");

			return ret;
		}

	case ocChildWrite: // 子名前空間への書き込み
		{
			RISSE_ASSERT(Name != NULL);
			RISSE_ASSERT(Used.size() == 2);
			RISSE_ASSERT(Declared != NULL);
			tString ret;
			if(Declared)
				ret += Declared->Dump() + RISSE_WS(" = "); // この文で宣言された変数がある
			ret += Used[0]->Dump()  + RISSE_WS(".ChildWrite(") +
					Name->AsHumanReadable() + RISSE_WS(", ") + Used[1]->Dump() +
					RISSE_WS(")");

			return ret;
		}

	case ocChildRead: // 子名前空間からの読み込み
		{
			RISSE_ASSERT(Name != NULL);
			RISSE_ASSERT(Declared != NULL);
			RISSE_ASSERT(Used.size() == 1);

			tString ret;
			ret += Declared->Dump() + RISSE_WS(" = ") + Used[0]->Dump();
			ret += RISSE_WS(".ChildRead(");
			ret +=	Name->AsHumanReadable() + RISSE_WS(")");

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

			tString ret;
			ret += Declared->Dump() + RISSE_WS(" = ");
			ret += RISSE_WS("Read(");
			ret +=	Name->AsHumanReadable() + RISSE_WS(")");

			return ret;
		}

	case ocSync: // synchronized
		{
			RISSE_ASSERT(Used.size() == 2);
			RISSE_ASSERT(Declared != NULL);

			tString ret;
			ret += Declared->Dump() + RISSE_WS(" = ");
			ret += Used[0]->Dump() + RISSE_WS("()");
			ret += RISSE_WS(" Synchronized(") + Used[1]->Dump() + 
				RISSE_WS(")");

			return ret;
		}

	case ocAddBindingMap: // ローカル変数のバインディング情報を追加
		{
			RISSE_ASSERT(Name != NULL);
			RISSE_ASSERT(Used.size() == 2);

			return Used[0]->Dump() + RISSE_WS(".AddBindingMap(") +
					Used[1]->Dump() + RISSE_WS(", ") +
					Name->AsHumanReadable() +
					RISSE_WS(")");
		}

	case ocAssertType: // 型のassert
		{
			RISSE_ASSERT(Used.size() == 1);

			return Used[0]->Dump() + RISSE_WS(".AssertType(") +
					tVariant::GetTypeString(GetAssertType()) +
					RISSE_WS(")");
		}

	default:
		{
			tString ret;

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
				ret += tString(VMInsnInfo[Code].Name);

				// 使用している引数
				if(is_funccall && (FuncExpandFlags & FuncCallFlag_Omitted))
				{
					// 関数呼び出しかnew
					ret += RISSE_WS("(...)");
				}
				else if(Used.size() >= 2)
				{
					// 引数がある
					tString used;
					risse_int arg_index = 0;
					for(gc_vector<tSSAVariable*>::const_iterator i = Used.begin() + 1;
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
			return ret;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tSSAStatement::Dump() const
{
	tString ret = DumpBody();

	// Value があればそれを追加
	if(Value && Code == ocAssignConstant)
	{
		ret += RISSE_WS(" Value=");
		ret += Value->AsHumanReadable();
	}

	// DSet あるいは DGet についてフラグがあればそれを追加
	if(Code == ocDGet || Code == ocDSet)
	{
		ret +=
			RISSE_WS(" Flags=(") +
			tOperateFlags(OperateFlagsValue).AsString() +
			RISSE_WS(")");
	}

	// DSetAttrib についてフラグがあればそれを追加
	if(Code == ocDSetAttrib)
	{
		ret +=
			RISSE_WS(" Flags=(") +
			tOperateFlags(OperateFlagsValue).AsString() +
			RISSE_WS(")");
	}

	// 副作用を持つならばそれを追加
	if(Effective)
	{
		ret += RISSE_WS(" Effective");
	}

	// 変数の宣言に関してコメントがあればそれを追加
	if(Declared)
		ret += Declared->GetComment();

	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
