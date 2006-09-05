//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief コンパイラ
//---------------------------------------------------------------------------
#include "../prec.h"

#include "risseCompiler.h"
#include "risseSSAForm.h"
#include "risseAST.h"
#include "risseSSABlock.h"
#include "risseSSAStatement.h"
#include "../risseException.h"
#include "../risseScriptBlockBase.h"
#include "../risseCodeBlock.h"

/*
	コンパイルの単位

・スクリプトブロック(tRisseScriptBlock)
  一つのスクリプトからなる固まり

・関数グループ(tRisseCompilerFunctionGroup)
  入れ子の関数のように、複数の関数が一つの環境(共有変数など)を共有
  する単位

・関数(tRisseCompilerFunction)
  関数単位。中にスタックフレームを共有する複数のSSA形式が入る

・SSA形式(tRisseSSAForm)
  SSA形式

*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(7695,16492,63400,17880,52365,22979,50413,3135);


//---------------------------------------------------------------------------
tRisseCompilerFunction::tRisseCompilerFunction(tRisseCompilerFunctionGroup * function_group)
{
	FunctionGroup = function_group;
	FunctionGroup->AddFunction(this); // 自分自身を登録する
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCompilerFunction::AddSSAForm(tRisseSSAForm * form)
{
	SSAForms.push_back(form);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCompilerFunction::CompleteSSAForm()
{
	// 未バインドのラベルを結線する
	// goto のジャンプ先は子→親の順に見ていくので生成順とは逆に処理を行う
	BindAllLabels();

	// try 脱出時の分岐先を生成
	for(gc_vector<tRisseSSAForm *>::reverse_iterator i = SSAForms.rbegin();
		i != SSAForms.rend(); i++)
		(*i)->AddCatchBranchTargets();

	// 未バインドのラベルを結線する
	// (AddCatchBranchTargetsで再度未バインドのラベルが追加される可能性があるため)
	// goto のジャンプ先は子→親の順に見ていくので生成順とは逆に処理を行う
	// (この段階ではSSA形式を超えるようなジャンプは無いはず)
	BindAllLabels();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCompilerFunction::GenerateVMCode()
{
	tRisseString str;

	// 最適化とSSA形式からの逆変換
	for(gc_vector<tRisseSSAForm *>::iterator i = SSAForms.begin();
		i != SSAForms.end(); i++)
		(*i)->OptimizeAndUnSSA();

	// SSA 形式のダンプ
	for(gc_vector<tRisseSSAForm *>::iterator i = SSAForms.begin();
		i != SSAForms.end(); i++)
	{
		RisseFPrint(stderr,(	RISSE_WS("========== SSA (") + (*i)->GetName() +
								RISSE_WS(") ==========\n")).c_str());
		str = (*i)->Dump();
		RisseFPrint(stderr, str.c_str());
	}

	// コードジェネレータの生成
	for(gc_vector<tRisseSSAForm *>::iterator i = SSAForms.begin();
		i != SSAForms.end(); i++)
		(*i)->EnsureCodeGenerator();

	// VMコードの生成
	SSAForms.front()->GenerateCode();

	// VMコードのダンプ
	for(gc_vector<tRisseSSAForm *>::iterator i = SSAForms.begin();
		i != SSAForms.end(); i++)
	{
		RisseFPrint(stderr,(	RISSE_WS("========== VM block #") +
									tRisseString::AsString((risse_int64)(*i)->GetCodeBlockIndex()) +
								RISSE_WS(" (") + (*i)->GetName() +
								RISSE_WS(") ==========\n")).c_str());
		tRisseCodeBlock * cb = (*i)->GetCodeBlock();
		str = cb->Dump();
		RisseFPrint(stderr, str.c_str());
	}

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCompilerFunction::AddLabelMap(const tRisseString &labelname, tRisseSSABlock * block)
{
	tLabelMap::iterator i = LabelMap.find(labelname);

	if(i != LabelMap.end())
	{
		// すでにラベルがある
		eRisseCompileError::Throw(
			tRisseString(RISSE_WS_TR("label '%1' is already defined"), labelname),
				FunctionGroup->GetCompiler()->GetScriptBlock(), block->GetLastStatementPosition());
	}

	LabelMap.insert(tLabelMap::value_type(labelname, block)); // ラベルを挿入
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCompilerFunction::BindAllLabels()
{
	for(tPendingLabelJumps::iterator i = PendingLabelJumps.begin();
		i != PendingLabelJumps.end(); i++)
	{
		// それぞれの i について、その基本ブロックの最後にジャンプ文を生成する

		// ジャンプ先を検索
		tLabelMap::iterator label_pair = LabelMap.find(i->LabelName);
		if(label_pair == LabelMap.end())
		{
			// ラベルは見つからなかった
			eRisseCompileError::Throw(
				tRisseString(RISSE_WS_TR("label '%1' is not defined"), i->LabelName),
					FunctionGroup->GetCompiler()->GetScriptBlock(),
						i->SourceBlock->GetLastStatementPosition());
		}

		// ラベルが見つかった

		// ラベルのジャンプ元とジャンプ先のSSA形式の親子関係を調べる
		tRisseSSAForm * source_form = i->SourceBlock->GetForm();
		tRisseSSAForm * target_form = label_pair->second->GetForm();

		if(source_form == target_form)
		{
			// 同じSSA形式インスタンス内
			// ジャンプ文を生成
			tRisseSSAStatement * stmt =
				new tRisseSSAStatement(source_form,
					i->SourceBlock->GetLastStatementPosition(), ocJump);
			i->SourceBlock->AddStatement(stmt);
			stmt->SetJumpTarget(label_pair->second);
		}
		else
		{
			// 同じSSA形式インスタンスではない
			// この場合は、source_form の親や先祖に target_form が無ければ
			// ならない (浅いSSA形式から深いSSA形式へのジャンプはできない)
			tRisseSSAForm * child = NULL;
			tRisseSSAForm * form = source_form;
			do
			{
				child = form;
				form = child->GetParent();
				if(!form) break;

				if(target_form == form)
				{
					// ジャンプ先のSSA形式が見つかった
					// この try id まで例外で抜けるためのコードを生成
					tRisseSSAStatement * stmt =
						new tRisseSSAStatement(source_form,
							i->SourceBlock->GetLastStatementPosition(), ocExitTryException);
					i->SourceBlock->AddStatement(stmt);
					risse_size label_idx = form->AddExitTryBranchTargetLabel(
								child->GetTryIdentifierIndex(), i->LabelName);
					stmt->SetTryIdentifierIndex(child->GetTryIdentifierIndex()); // try id を設定
					stmt->SetIndex(label_idx + 2); // インデックスを設定
						// +2 = 最初の2つは例外が発生しなかったときと通常の例外が発生したとき
						// に割り当てられているので
					break;
				}
			} while(true);

			if(form == NULL)
			{
				// ジャンプ先ラベルはどうやら自分よりも深い場所にいるようだ
				// そういうことは今のところできないのでエラーにする
				eRisseCompileError::Throw(
					tRisseString(RISSE_WS_TR("cannot jump into deeper try block or callback block")),
						FunctionGroup->GetCompiler()->GetScriptBlock(),
							i->SourceBlock->GetLastStatementPosition());
			}
		}
	}

	PendingLabelJumps.clear(); // リストはクリアしておく
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
tRisseCompilerFunctionGroup::tRisseCompilerFunctionGroup(
	tRisseCompiler * compiler)
{
	Compiler = compiler;
	Compiler->AddFunctionGroup(this); // 自分自身を登録する
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCompilerFunctionGroup::AddFunction(tRisseCompilerFunction * function)
{
	Functions.push_back(function);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void tRisseCompilerFunctionGroup::CompleteSSAForm()
{
	// このインスタンスが所有しているすべての関数に対して処理を行わせる
	for(gc_vector<tRisseCompilerFunction *>::reverse_iterator ri = Functions.rbegin();
		ri != Functions.rend(); ri++)
	{
		(*ri)->CompleteSSAForm();
	}
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void tRisseCompilerFunctionGroup::GenerateVMCode()
{
	// このインスタンスが所有しているすべての関数に対して処理を行わせる
	for(gc_vector<tRisseCompilerFunction *>::reverse_iterator ri = Functions.rbegin();
		ri != Functions.rend(); ri++)
	{
		(*ri)->GenerateVMCode();
	}
}
//---------------------------------------------------------------------------














//---------------------------------------------------------------------------
void tRisseCompiler::Compile(tRisseASTNode * root, bool need_result, bool is_expression)
{
	// (テスト) ASTのダンプを行う
	RisseFPrint(stderr, RISSE_WS("========== AST ==========\n"));
	tRisseString str;
	root->Dump(str);
	RisseFPrint(stderr, str.c_str());

	// トップレベルの関数グループを作成する
	tRisseCompilerFunctionGroup *top_function_group = new tRisseCompilerFunctionGroup(this);

	// トップレベルの関数を作成する
	tRisseCompilerFunction *top_function = new tRisseCompilerFunction(top_function_group);

	// トップレベルのSSA形式を作成する
	tRisseSSAForm * form = new tRisseSSAForm(top_function, RISSE_WS("root"), NULL, false);

	// トップレベルのSSA形式の内容を作成する
	// (その下にぶら下がる他のSSA形式などは順次芋づる式に作成される)
	form->Generate(root);

	// SSA形式を完結させる
	for(gc_vector<tRisseCompilerFunctionGroup *>::iterator i = FunctionGroups.begin();
		i != FunctionGroups.end(); i++)
	{
		(*i)->CompleteSSAForm();
	}

	// VMコード生成を行う
	for(gc_vector<tRisseCompilerFunctionGroup *>::iterator i = FunctionGroups.begin();
		i != FunctionGroups.end(); i++)
	{
		(*i)->GenerateVMCode();
	}

	// ルートのコードブロックを設定する
	ScriptBlock->SetRootCodeBlock(form->GetCodeBlock());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCompiler::AddFunctionGroup(tRisseCompilerFunctionGroup * function_group)
{
	FunctionGroups.push_back(function_group);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tRisseCompiler::AddCodeBlock(tRisseCodeBlock * block)
{
	// コードブロックの管理はスクリプトブロックが行っている
	return ScriptBlock->AddCodeBlock(block);
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
} // namespace Risse
