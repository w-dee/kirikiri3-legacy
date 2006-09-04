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
	for(gc_vector<tRisseSSAForm *>::reverse_iterator i = SSAForms.rbegin();
		i != SSAForms.rend(); i++)
		(*i)->BindAllLabels();

	// try 脱出時の分岐先を生成
	for(gc_vector<tRisseSSAForm *>::reverse_iterator i = SSAForms.rbegin();
		i != SSAForms.rend(); i++)
		(*i)->AddCatchBranchTargets();

	// 未バインドのラベルを結線する
	// (AddCatchBranchTargetsで再度未バインドのラベルが追加される可能性があるため)
	// goto のジャンプ先は子→親の順に見ていくので生成順とは逆に処理を行う
	// (この段階ではSSA形式を超えるようなジャンプは無いはず)
	for(gc_vector<tRisseSSAForm *>::reverse_iterator i = SSAForms.rbegin();
		i != SSAForms.rend(); i++)
		(*i)->BindAllLabels();
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
