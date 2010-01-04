//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

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
#include "risseCodeGen.h"
#include "risseCompilerNS.h"
#include "../risseExceptionClass.h"
#include "../risseScriptBlockClass.h"
#include "../risseCodeBlock.h"
#include "../risseStaticStrings.h"
#include "../risseBindingInfo.h"

/*
	コンパイルの単位

・スクリプトブロック(tScriptBlockInstance)
  一つのスクリプトからなる固まり

・関数グループ(tCompilerFunctionGroup)
  入れ子の関数のように、複数の関数が一つの環境(共有変数など)を共有
  しうる単位

・関数(tCompilerFunction)
  関数単位。中にスタックフレームを共有する複数のSSA形式が入る

・SSA形式(tSSAForm)
  SSA形式

*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(7695,16492,63400,17880,52365,22979,50413,3135);



//---------------------------------------------------------------------------
tCompilerFunction::tCompilerFunction(tCompilerFunctionGroup * function_group,
	tCompilerFunction * parent, risse_size nestlevel, const tString name)
{
	Name = name;
	Parent = parent;
	if(nestlevel != risse_size_max)
	{
		NestLevel = nestlevel;
	}
	else
	{
		if(Parent)
			NestLevel = Parent->NestLevel + 1;
		else
			NestLevel = 0;
	}
	FunctionGroup = function_group;
	FunctionGroup->AddFunction(this); // 自分自身を登録する
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompilerFunction::AddSSAForm(tSSAForm * form)
{
	SSAForms.push_back(form);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompilerFunction::CompleteSSAForm()
{
	// 未バインドのラベルを結線する
	// goto のジャンプ先は子→親の順に見ていくので生成順とは逆に処理を行う
	BindAllLabels();

	// try 脱出時の分岐先を生成
	for(gc_vector<tSSAForm *>::reverse_iterator i = SSAForms.rbegin();
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
void tCompilerFunction::RegisterSharedVariablesToCodeGenerator()
{
	// すべての共有されている変数をコードジェネレータに登録する
	// いまのところ、コードジェネレータはそれよりも前段の各クラスとは情報が独立
	// しているために、コードジェネレータが必要な情報はなんらかの形で前段が
	// コードジェネレータに対して与えなければならない
	for(tSharedVariableMap::const_iterator i = SharedVariableMap.begin();
		i != SharedVariableMap.end(); i++)
	{
		// コードジェネレータの SharedRegNameMap は一つの関数グループ内では同じ
		// マップを共有しているが、ネストレベルをともに記録する必要があるため
		// 各関数別にそれぞれ呼び出す。
		// 各関数内のSSA形式ではネストレベルが同じため、トップレベルのSSA形式
		// インスタンスが作成したコードジェネレータに対してのみ共有されている
		// 変数を登録するのでよい。
		GetTopSSAForm()->GetCodeGenerator()->AddSharedRegNameMap(i->first);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompilerFunction::GenerateVMCode()
{
	tString str;

	// 関数名表示
	FPrint(stderr, (RISSE_WS("######################################\n")));
	FPrint(stderr, (RISSE_WS("function ") + Name +
					RISSE_WS(" nest level ") + tString::AsString((risse_int64)NestLevel) +
					RISSE_WS("\n")).c_str());
	FPrint(stderr, (RISSE_WS("######################################\n")));

	// 最適化とSSA形式からの逆変換
	for(gc_vector<tSSAForm *>::iterator i = SSAForms.begin();
		i != SSAForms.end(); i++)
		(*i)->OptimizeAndUnSSA();

	// コードジェネレータの生成
	for(gc_vector<tSSAForm *>::iterator i = SSAForms.begin();
		i != SSAForms.end(); i++)
		(*i)->EnsureCodeGenerator();

	// VMコードの生成
	SSAForms.front()->GenerateCode();

	// VMコードのダンプ
	for(gc_vector<tSSAForm *>::iterator i = SSAForms.begin();
		i != SSAForms.end(); i++)
	{
		FPrint(stderr,(	RISSE_WS("========== VM block #") +
									tString::AsString((risse_int64)(*i)->GetCodeBlockIndex()) +
								RISSE_WS(" (") + (*i)->GetName() +
								RISSE_WS(") ==========\n")).c_str());
		tCodeBlock * cb = (*i)->GetCodeBlock();
		str = cb->Dump();
		FPrint(stderr, str.c_str());
	}
	fflush(stderr);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompilerFunction::SetSharedVariableNestCount()
{
	SSAForms.front()->SetSharedVariableNestCount();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompilerFunction::AddPendingLabelJump(tSSABlock * jump_block,
			const tString & labelname)
{
	// PendingLabelJump に追加
	PendingLabelJumps.push_back(tPendingLabelJump(jump_block, labelname));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompilerFunction::AddLabelMap(const tString &labelname, tSSABlock * block)
{
	tLabelMap::iterator i = LabelMap.find(labelname);

	if(i != LabelMap.end())
	{
		// すでにラベルがある
		tCompileExceptionClass::Throw(
			FunctionGroup->GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
			tString(RISSE_WS_TR("label '%1' is already defined"), labelname),
				FunctionGroup->GetCompiler()->GetScriptBlockInstance(), block->GetLastStatementPosition());
	}

	LabelMap.insert(tLabelMap::value_type(labelname, block)); // ラベルを挿入
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompilerFunction::ShareVariable(const tString & n_name)
{
	SharedVariableMap.insert(tSharedVariableMap::value_type(n_name, NULL));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tCompilerFunction::GetShared(const tString & n_name)
{
	bool result = SharedVariableMap.find(n_name) != SharedVariableMap.end();
/*
	FPrint(stderr, (RISSE_WS("Checking shared status of ") + n_name +
				RISSE_WS(" : ") +
				(result ? RISSE_WS("true"):RISSE_WS("false")) +
				RISSE_WS("\n")).c_str() );
*/
	return result;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompilerFunction::BindAllLabels()
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
			tCompileExceptionClass::Throw(
				FunctionGroup->GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
				tString(RISSE_WS_TR("label '%1' is not defined"), i->LabelName),
					FunctionGroup->GetCompiler()->GetScriptBlockInstance(),
						i->SourceBlock->GetLastStatementPosition());
		}

		// ラベルが見つかった

		// ラベルのジャンプ元とジャンプ先のSSA形式の親子関係を調べる
		tSSAForm * source_form = i->SourceBlock->GetForm();
		tSSAForm * target_form = label_pair->second->GetForm();

		if(source_form == target_form)
		{
			// 同じSSA形式インスタンス内
			// ジャンプ文を生成
			tSSAStatement * stmt =
				new tSSAStatement(source_form,
					i->SourceBlock->GetLastStatementPosition(), ocJump);
			i->SourceBlock->AddStatement(stmt);
			stmt->SetJumpTarget(label_pair->second);
		}
		else
		{
			// 同じSSA形式インスタンスではない
			// この場合は、source_form の親や先祖に target_form が無ければ
			// ならない (浅いSSA形式から深いSSA形式へのジャンプはできない)
			tSSAForm * child = NULL;
			tSSAForm * form = source_form;
			do
			{
				child = form;
				form = child->GetParent();
				if(!form) break;

				if(target_form == form)
				{
					// ジャンプ先のSSA形式が見つかった
					// この try id まで例外で抜けるためのコードを生成
					// tSSAForm の AddReturnStatement と AddBreakStatement と AddContinueStatement
					// も参照のこと。

					// _ の値を取得する
					tSSAVariable * var =
						i->SourceBlock->GetLocalNamespace()->Read(
							i->SourceBlock->GetLastStatementPosition(), ss_lastEvalResultHiddenVarName);

					// この try id まで例外で抜けるためのコードを生成
					tSSAStatement * stmt =
						new tSSAStatement(source_form,
							i->SourceBlock->GetLastStatementPosition(), ocExitTryException);
					stmt->AddUsed(var);
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
				tCompileExceptionClass::Throw(
					FunctionGroup->GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
					tString(RISSE_WS_TR("cannot jump into deeper try/synchronized/using/callback block")),
						FunctionGroup->GetCompiler()->GetScriptBlockInstance(),
							i->SourceBlock->GetLastStatementPosition());
			}
		}
	}

	PendingLabelJumps.clear(); // リストはクリアしておく
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
tCompilerFunctionGroup::tCompilerFunctionGroup(
	tCompiler * compiler, const tString & name)
{
	Compiler = compiler;
	Name = name;
	Compiler->AddFunctionGroup(this); // 自分自身を登録する
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompilerFunctionGroup::AddFunction(tCompilerFunction * function)
{
	Functions.push_back(function);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompilerFunctionGroup::CompleteSSAForm()
{
	// このインスタンスが所有しているすべての関数に対して処理を行わせる
	for(gc_vector<tCompilerFunction *>::reverse_iterator ri = Functions.rbegin();
		ri != Functions.rend(); ri++)
	{
		(*ri)->CompleteSSAForm();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompilerFunctionGroup::GenerateVMCode()
{
	// コードジェネレータに各関数が保持している共有変数を登録する
	for(gc_vector<tCompilerFunction *>::reverse_iterator ri = Functions.rbegin();
		ri != Functions.rend(); ri++)
	{
		(*ri)->RegisterSharedVariablesToCodeGenerator();
	}

	// 次にコードを生成する
	for(gc_vector<tCompilerFunction *>::reverse_iterator ri = Functions.rbegin();
		ri != Functions.rend(); ri++)
	{
		(*ri)->GenerateVMCode();
	}

	// 一番最初の関数 (つまり一番最初に実行される関数)のコードブロックに
	// 対して最大のネストレベルを調べ、設定させる。
	Functions.front()->SetSharedVariableNestCount();
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
void tCompiler::Compile(tASTNode * root, const tBindingInfo & binding,
	bool need_result, bool is_expression)
{
	// (テスト) ASTのダンプを行う
	FPrint(stderr, RISSE_WS("========== AST ==========\n"));
	tString str;
	root->Dump(str);
	FPrint(stderr, str.c_str());

	// トップレベルのSSA形式インスタンスを作成する
	tSSAForm * form = CreateTopLevelSSAForm(root->GetPosition(), RISSE_WS("toplevel"),
		&binding, need_result, is_expression);

	// トップレベルのSSA形式の内容を作成する
	// (その下にぶら下がる他のSSA形式などは順次芋づる式に作成される)
	form->Generate(root);

	// SSA形式を完結させる
	for(gc_vector<tCompilerFunctionGroup *>::iterator i = FunctionGroups.begin();
		i != FunctionGroups.end(); i++)
	{
		(*i)->CompleteSSAForm();
	}

	// VMコード生成を行う
	for(gc_vector<tCompilerFunctionGroup *>::iterator i = FunctionGroups.begin();
		i != FunctionGroups.end(); i++)
	{
		(*i)->GenerateVMCode();
	}

	// ルートのコードブロックを設定する
	ScriptBlockInstance->SetRootCodeBlock(form->GetCodeBlock());
/*
	// (テスト) 出力のフラッシュ
	FPrint(stderr, RISSE_WS("=========================\n"));
	fflush(stderr);
	fflush(stdout);
*/
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompiler::CompileClass(const gc_vector<tASTNode *> & roots, const tString & name,
	tSSAForm * form, tSSAForm *& new_form, tSSAVariable *& block_var, bool reg_super)
{
	RISSE_ASSERT(roots.size() >= 1);
	risse_size pos = roots[0]->GetPosition();

	// クラスの内部名称を決める
	tString numbered_class_name = RISSE_WS("class ") + name + RISSE_WS(" ") +
					tString::AsString(GetUniqueNumber());

	// トップレベルのSSA形式インスタンスを作成する
	new_form = CreateTopLevelSSAForm(pos, numbered_class_name, NULL, true, true);

	// クラス名を設定する
	new_form->GetFunction()->GetFunctionGroup()->SetClassName(name);

	// super クラスの情報を登録する場合
	if(reg_super)
	{
		// クラスの第一引数はスーパークラスなのでそれを変数 "super" に記録するための文を作成する。
		// "super" は一番浅い位置の名前空間に配置されるが、通常この位置の名前空間には普通の
		// 変数は記録されずに、この "super" のような特殊な変数が記録されることになる。
		tSSAVariable * param_var = NULL;
		tSSAStatement * assignparam_stmt = 
			new_form->AddStatement(pos, ocAssignParam, &param_var);
		assignparam_stmt->SetIndex(0);

		new_form->GetLocalNamespace()->Add(RISSE_WS("super"), NULL);
		new_form->GetLocalNamespace()->Write(pos, RISSE_WS("super"), param_var);
	}

	// トップレベルのSSA形式の内容を作成する
	// (その下にぶら下がる他のSSA形式などは順次芋づる式に作成される)
	new_form->Generate(roots);

	// クラスを生成する文を追加する
	tSSAStatement * defineclass_stmt =
		form->AddStatement(pos, ocDefineClass, &block_var);
	defineclass_stmt->SetName(numbered_class_name);
	defineclass_stmt->SetDefinedForm(new_form);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAForm * tCompiler::CreateTopLevelSSAForm(risse_size pos,
	const tString & name, const tBindingInfo * binding,
	bool need_result, bool is_expression)
{
	// バインディングを元に、名前空間オブジェクトを作成する。
	// この名前空間オブジェクトは、関数グループよりもさらに外側の名前空間オブジェクトとして
	// 作成される。
	// また同時に、バインディング内で用いられている最大の共有変数ネストレベルを求める。
	// これからコンパイルされる関数に置いては、このネストレベルよりも深い値が用いられる。
	risse_size nestlevel_start = 0;
	tSSALocalNamespace *ns = NULL;
	if(binding && binding->GetFrames())
	{
		ns = new tSSALocalNamespace();
		ns->Push(); // 名前空間を push
		ns->SetCompiler(this); // コンパイラインスタンスを設定
		const tBindingInfo::tBindingMap & map = binding->GetBindingMap();
		for(tBindingInfo::tBindingMap::const_iterator i = map.begin(); i != map.end(); i++)
		{
			ns->Add(i->first, NULL);
			risse_size nestlevel = (i->second >> 16) + 1;
			if(nestlevel_start < nestlevel) nestlevel_start = nestlevel;
		}
	}

	// トップレベルの関数グループを作成する
	tCompilerFunctionGroup *top_function_group = new tCompilerFunctionGroup(this, name);

	// トップレベルの関数を作成する
	tCompilerFunction *top_function =
		new tCompilerFunction(top_function_group, NULL, nestlevel_start, name);

	// トップレベルのSSA形式を作成する
	tSSAForm * form = new tSSAForm(pos, top_function, name, NULL, false);

	// トップレベルのSSA形式のローカル変数の名前空間の親に 先ほど作成した ns を設定する
	if(ns) form->GetLocalNamespace()->SetParent(ns);

	// ローカルバインディングがある場合はローカル変数の名前空間を一つ push する。
	// これにより eval 全体が {  } で囲まれているかのような状態にする。
	if(binding && binding->GetFrames()) form->GetLocalNamespace()->Push();

	// バインディングに登録されている変数を共有変数としてあらかじめコードジェネレータに登録する
	if(binding && binding->GetFrames())
	{
		const tBindingInfo::tBindingMap & map = binding->GetBindingMap();
		for(tBindingInfo::tBindingMap::const_iterator i = map.begin(); i != map.end(); i++)
		{
			// i->first は番号なしの名前だが、共有変数には番号付きの名前でアクセス
			// されるので、対応を ns からとってこなければならない。
			tString n_name;
			bool found = ns->Find(i->first, false, &n_name, NULL);
			(void)found; // (found変数は代入されたのに使われていないという)警告を抑制
			RISSE_ASSERT(found); // 変数は見つからなくてはならない
			form->GetCodeGenerator()->AddBindingRegNameMap(n_name,
						static_cast<risse_uint16>(i->second >> 16),
						static_cast<risse_uint16>(i->second & 0xffff));
		}
	}

	return form;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompiler::AddFunctionGroup(tCompilerFunctionGroup * function_group)
{
	FunctionGroups.push_back(function_group);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tCompiler::AddCodeBlock(tCodeBlock * block)
{
	// コードブロックの管理はスクリプトブロックが行っている
	return ScriptBlockInstance->AddCodeBlock(block);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
risse_int tCompiler::GetUniqueNumber()
{
	UniqueNumber++;
	// int のサイズにもよるが、32bit integer では 2^30 ぐらいまで。
	// もちろんこれはそれほど変数が使われることは無いだろうという推測の元。
	// コレを超えるとエラーになる。
	if(UniqueNumber >= 1 << (sizeof(risse_int) * 8 - 2))
		tCompileExceptionClass::Throw(
			ScriptBlockInstance->GetScriptEngine(),
			tString(RISSE_WS_TR("too large source code; compiler internal number exhausted")));
	return UniqueNumber;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace Risse
