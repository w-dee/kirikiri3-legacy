//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief SSA形式
//---------------------------------------------------------------------------
#include "../prec.h"

#include "risseSSAForm.h"
#include "risseSSAStatement.h"
#include "risseCompiler.h"
#include "risseCompilerNS.h"
#include "risseSSABlock.h"
#include "risseCodeGen.h"
#include "../risseExceptionClass.h"
#include "../risseScriptBlockClass.h"
#include "../risseCodeBlock.h"
#include "../risseStaticStrings.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(7246,55563,12549,16650,3991,47135,2967,64968);



//---------------------------------------------------------------------------
tBreakInfo::tBreakInfo(tSSAForm * form,
	const tString & label_prefix)
{
	Form = form;
	IsBlock = false;
	NonValueBreakShouldSetVoidToLastEvalValue = true;

	// ターゲットブロックのラベル名を生成
	JumpTargetLabel = 
				(label_prefix.IsEmpty() ?
					tString(RISSE_WS("@break_or_continue")):label_prefix) +
				RISSE_WS("_") +
				tString::AsString(Form->GetFunction()->GetFunctionGroup()->
									GetCompiler()->GetUniqueNumber());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tBreakInfo::AddJump(tSSABlock * jump_block)
{
	PendingJumpSourceBlocks.push_back(jump_block);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tBreakInfo::BindAll(tSSABlock * target)
{
	// ターゲットのブロックに対してラベルを設定する
	Form->GetFunction()->AddLabelMap(JumpTargetLabel, target);

	for(tPendingJumpSourceBlocks::iterator i = PendingJumpSourceBlocks.begin();
		i != PendingJumpSourceBlocks.end(); i++)
	{
		// それぞれの i について、Form の PendingLabelJumps に入れる
		Form->GetFunction()->AddPendingLabelJump(*i, JumpTargetLabel);
	}
}
//---------------------------------------------------------------------------
















//---------------------------------------------------------------------------
tSSAForm::tSSAForm(risse_size pos, tCompilerFunction * function,
	const tString & name, tSSAForm * parent, bool useparentframe)
{
	Function = function;
	Parent = parent;
	UseParentFrame = useparentframe; // ブロックの場合に真
	Name = name;
	LocalNamespace = new tSSALocalNamespace();
	EntryBlock = NULL;
	CurrentBlock = NULL;
	CurrentSwitchInfo = NULL;
	CurrentBreakInfo = NULL;
	CurrentContinueInfo = NULL;
	FunctionCollapseArgumentVariable = NULL;
	TryIdentifierIndex = risse_size_max;
	CanReturn = !UseParentFrame; // いまのところ CanReturn はこの式の通りで決定される
	ExitTryBranchTargetLabels = NULL;
	State = ssSSA; // 最初はSSA性が保持されていると見なす

	// 関数インスタンスに自身を登録する
	Function->AddSSAForm(this);

	// 一番浅い位置の名前空間を作成する
	LocalNamespace->Push();

	// コードジェネレータを作成する
	RISSE_ASSERT(!(Parent && Parent->CodeGenerator == NULL));
	CodeGenerator = new tCodeGenerator(this, Parent ? Parent->CodeGenerator : NULL,
							UseParentFrame, function->GetNestLevel());
	CodeBlock = new tCodeBlock(GetScriptBlockInstance());
	CodeBlockIndex = Function->GetFunctionGroup()->GetCompiler()->AddCodeBlock(CodeBlock);

	// エントリー位置の基本ブロックを生成する
	EntryBlock = new tSSABlock(this, RISSE_WS("entry"), LocalNamespace);
	LocalNamespace = EntryBlock->GetLocalNamespace();
	CurrentBlock = EntryBlock;

	// this-proxy を作成する
	// (もし関数などの中でthis-proxyが使われなかった場合は最適化によって
	//  この文も消えるはず)
	// 注意! ThisProxyのオブジェクトはスタック上に配置される可能性があるため
	// SSA形式間で共有したりしてはいけない (SSA形式ごとに毎回作成する)
	tSSAVariable * thisproxy = NULL;
	AddStatement(pos, ocAssignThisProxy, &thisproxy);
	LocalNamespace->Add(ss_thisProxyHiddenVarName, NULL);
	LocalNamespace->Write(pos, ss_thisProxyHiddenVarName, thisproxy);

	// 一番浅い位置の名前空間に、変数 ss_lastEvalResultHiddenVarName を登録する
	// 内容は void である
	tSSAVariable * voidvalue = AddConstantValueStatement(pos, tVariant());
	LocalNamespace->Add(ss_lastEvalResultHiddenVarName, NULL);
	LocalNamespace->Write(pos, ss_lastEvalResultHiddenVarName, voidvalue);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::Generate(const gc_vector<tASTNode *> & roots)
{
	// AST をたどり、それに対応する SSA 形式を作成する
	RISSE_ASSERT(roots.size() >= 1);

	// ルートノードを処理する
	for(gc_vector<tASTNode *>::const_iterator i = roots.begin(); i != roots.end(); i++)
		(*i)->GenerateReadSSA(this);

	// 実行ブロックの最後の return 文を生成する
	GenerateLastReturn(roots.back());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::Generate(const tASTNode * root)
{
	// AST をたどり、それに対応する SSA 形式を作成する

	// ルートノードを処理する
	root->GenerateReadSSA(this);

	// 実行ブロックの最後の return 文を生成する
	GenerateLastReturn(root);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::OptimizeAndUnSSA()
{
	// 到達しない基本ブロックからのパスを削除
	LeapDeadBlocks();

	// 共有変数へのアクセスを別形式の文に変換
	ConvertSharedVariableAccess();

	// 文レベルでの最適化を行う
	OptimizeStatement();

	// SSA 形式のダンプ(デバッグ)
	FPrint(stderr,(	RISSE_WS("========== SSA (") + GetName() +
							RISSE_WS(") ==========\n")).c_str());
	tString str = Dump();
	FPrint(stderr, str.c_str());

	// 変数の有効範囲をブロック単位で解析
	AnalyzeVariableBlockLiveness();

	// 文に通し番号を振る
	SetStatementOrder();

	// 変数の干渉グラフを作成する
	CreateVariableInterferenceGraph();

	// 変数の合併を行うために、どの変数が合併できそうかどうかを調査する
	TraceCoalescable();

	// 変数の合併を行う
	Coalesce();

	// φ関数を除去
	RemovePhiStatements();

	// 3番地形式の格納先が他の変数と異なっていることを保証(暫定処置)
	Check3AddrAssignee();

	// レジスタの割り当て
	AssignRegisters();

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tScriptBlockInstance * tSSAForm::GetScriptBlockInstance() const
{
	return Function->GetFunctionGroup()->GetCompiler()->GetScriptBlockInstance();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tSSAForm::GetThisProxy(risse_size pos)
{
	return LocalNamespace->Read(pos, ss_thisProxyHiddenVarName);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::WriteLastEvalResult(risse_size pos, tSSAVariable * value)
{
	if(value)
	{
		// _ 変数に値を書き込む
		bool result = LocalNamespace->Write(pos,
						ss_lastEvalResultHiddenVarName, value);
		(void)result;
		RISSE_ASSERT(result != false);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSABlock * tSSAForm::CreateNewBlock(const tString & name,
						const tSSALocalNamespace * ns)
{
	// 新しい基本ブロックを作成する
	// (この際、tSSABlock 内で ns あるいは LocalNamespace がコピーされる)
	tSSABlock * new_block = new tSSABlock(this, name, ns ? ns : LocalNamespace);

	// LocalNamespace を、新しい block のものを指すように更新
	LocalNamespace = new_block->GetLocalNamespace();

	// ローカル変数名前空間をいったんすべてφ関数を見るようにマークする
	LocalNamespace->MarkToCreatePhi();

	// 新しい「現在の」基本ブロックを設定し、それを返す
	CurrentBlock = new_block;
	return CurrentBlock;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tSSAForm::InternalAddExitTryBranchTargetLabel(
						tExitTryBranchTargetLabels * target_label, 
						const tString & label)
{
	RISSE_ASSERT(target_label != NULL);

	tExitTryBranchTargetLabels::iterator i = target_label->find(label);
	if(i != target_label->end())
		return i->second; // すでにマップにある

	// マップにないので追加する
	risse_size label_idx = target_label->size();
	target_label->insert(
		tExitTryBranchTargetLabels::value_type(
			label, label_idx));

	return label_idx;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tSSAForm::AddExitTryBranchTargetLabel(risse_size try_id,
													const tString & label)
{
	tCatchBranchAndExceptionMap::iterator i =
		CatchBranchAndExceptionMap.find(try_id);
	RISSE_ASSERT(i != CatchBranchAndExceptionMap.end());
	return InternalAddExitTryBranchTargetLabel(i->second->ExitTryBranchTargetLabels, label);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tSSAForm::AddExitTryBranchTargetLabel(const tString & label)
{
	RISSE_ASSERT(ExitTryBranchTargetLabels != NULL);
	return InternalAddExitTryBranchTargetLabel(ExitTryBranchTargetLabels, label);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tSSAForm::AddConstantValueStatement(
										risse_size pos,
										const tVariant & val)
{
	tVariant *constant = new tVariant(val);

	// 文の作成
	tSSAStatement * stmt =
		new tSSAStatement(this, pos, ocAssignConstant);
	stmt->SetValue(constant);
	// 変数の作成
	tSSAVariable * var = new tSSAVariable(this, stmt);
	// 文の追加
	CurrentBlock->AddStatement(stmt);
	// 戻る
	return var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAStatement * tSSAForm::AddStatement(risse_size pos, tOpCode code,
		tSSAVariable ** ret_var,
			tSSAVariable *using1,
			tSSAVariable *using2,
			tSSAVariable *using3,
			tSSAVariable *using4
			)
{
	return GetCurrentBlock()->AddStatement(pos, code, ret_var, using1, using2, using3, using4);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::AddReturnStatement(risse_size pos, tSSAVariable * var)
{
	// ***注意***
	// AddReturnStatement と AddBreakStatement と AddContinueStatement は
	// 共通点が多い。もし一つ変更を加えるならば、ほかのメソッドも見て、
	// 同様の変更点がないかを検討すること。
	// AddExitTryBranchTargetLabel() を呼び出すところは
	// tCompilerFunction::BindAllLabels() にもある。

	if(var == NULL)
	{
		// 値が無い場合はvoidにする
		var = AddConstantValueStatement(pos, tVariant()); // void
	}

	if(CanReturn)
	{
		// 単純に return 文を作成可能
		// return 文を作成
		AddStatement(pos, ocReturn, NULL, var);

		// 新しい基本ブロックを作成(ただしここには到達しない)
		CreateNewBlock("disconnected_by_return");
	}
	else
	{
		// このSSA形式からreturn文で呼び出し元に戻るには例外を使って脱出を行うしかない
		// return できるSSA形式を探す
		tSSAForm * child = this;
		tSSAForm * form = child->Parent;
		do
		{
			if(form->CanReturn)
			{
				// このSSA形式は return文で抜けることができる
				// この try id まで例外で抜けるためのコードを生成
				tSSAStatement * stmt = AddStatement(pos, ocExitTryException, NULL, var);
				risse_size label_idx = form->AddExitTryBranchTargetLabel(RISSE_WS("@return"));
				stmt->SetTryIdentifierIndex(child->TryIdentifierIndex); // try id を設定
				stmt->SetIndex(label_idx + 2); // インデックスを設定
					// +2 = 最初の2つは例外が発生しなかったときと通常の例外が発生したとき
					// に割り当てられているので

				CreateNewBlock(RISSE_WS("disconnected_by_return_by_exception"));
				break;
			}
			child = form;
			form = child->Parent;
		} while(form);
		RISSE_ASSERT(form != NULL); // 先祖は必ずどれかが CanReturn が真でなければならない
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::AddBreakStatement(risse_size pos,
											tSSAVariable * var)
{
	// ***注意***
	// AddReturnStatement と AddBreakStatement と AddContinueStatement は
	// 共通点が多い。もし一つ変更を加えるならば、ほかのメソッドも見て、
	// 同様の変更点がないかを検討すること。
	// AddExitTryBranchTargetLabel() を呼び出すところは
	// tCompilerFunction::BindAllLabels() にもある。

	// このSSA形式インスタンスはbreakできるか？
	tBreakInfo * info = GetCurrentBreakInfo();
	if(info)
	{
		// この SSA 形式は break できる

		if(var)
		{
			// 値がある場合、値を _ に設定
			WriteLastEvalResult(pos, var);
		}
		else
		{
			// 値がない場合は………
			if(info->GetNonValueBreakShouldSetVoidToLastEvalValue())
			{
				// void を設定する
				WriteLastEvalResult(pos, AddConstantValueStatement(pos, tVariant()));
			}
		}

		// ジャンプ文を info に登録
		info->AddJump(GetCurrentBlock());

		// 新しい基本ブロックを作成
		CreateNewBlock(RISSE_WS("disconnected_by_break"));
	}
	else
	{
		// break できる SSA 形式を探す
		tSSAForm * child = NULL;
		tSSAForm * form = this;
		do
		{
			if(form->CanReturn) { form = NULL; break; } // CanReturn な form を超えて break はできない
			child = form;
			form = child->Parent;
			if(!form) break;

			info = form->GetCurrentBreakInfo();
			if(info)
			{
				if(var == NULL)
				{
					// 値が指定されていない場合
					if(info->GetNonValueBreakShouldSetVoidToLastEvalValue())
					{
						// void を設定する
						var = AddConstantValueStatement(pos, tVariant());
					}
					else
					{
						// そうでなければ _ の値を得る
						var = LocalNamespace->Read(pos, ss_lastEvalResultHiddenVarName);
					}
				}
				if(var == NULL && info->GetIsBlock())
				{
					// 値を受け取ることができるのに値が無い場合はvoidにする
					var = AddConstantValueStatement(pos, tVariant()); // void
				}

				// このSSA形式はbreak文を受け取ることができる
				// この try id まで例外で抜けるためのコードを生成
				tSSAStatement * stmt = AddStatement(pos, ocExitTryException, NULL, var);
				risse_size label_idx = form->AddExitTryBranchTargetLabel(
													info->GetJumpTargetLabel());
				stmt->SetTryIdentifierIndex(child->TryIdentifierIndex); // try id を設定
				stmt->SetIndex(label_idx + 2); // インデックスを設定
					// +2 = 最初の2つは例外が発生しなかったときと通常の例外が発生したとき
					// に割り当てられているので

				CreateNewBlock(RISSE_WS("disconnected_by_break_by_exception"));
				break;
			}
		} while(true);

		if(form == NULL)
		{
			// break できないようだ
			tCompileExceptionClass::Throw(
				Function->GetFunctionGroup()->
					GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
				tString(RISSE_WS_TR("cannot place 'break' here")),
					GetScriptBlockInstance(), pos);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::AddContinueStatement(risse_size pos,
											tSSAVariable * var)
{
	// ***注意***
	// AddReturnStatement と AddBreakStatement と AddContinueStatement は
	// 共通点が多い。もし一つ変更を加えるならば、ほかのメソッドも見て、
	// 同様の変更点がないかを検討すること。
	// AddExitTryBranchTargetLabel() を呼び出すところは
	// tCompilerFunction::BindAllLabels() にもある。

	// このSSA形式インスタンスはcontinueできるか？
	tBreakInfo * info = GetCurrentContinueInfo();
	if(info)
	{
		// この SSA 形式は continue できる

		// continue が値を伴ってできるかをチェック
		if(var != NULL && !info->GetIsBlock())
			tCompileExceptionClass::Throw(
				Function->GetFunctionGroup()->
					GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
				tString(RISSE_WS_TR("cannot continue here with a value")),
					GetScriptBlockInstance(), pos);

		// ジャンプ文を info に登録
		info->AddJump(GetCurrentBlock());

		// 新しい基本ブロックを作成
		CreateNewBlock(RISSE_WS("disconnected_by_continue"));
	}
	else
	{
		// continue できる SSA 形式を探す
		tSSAForm * child = NULL;
		tSSAForm * form = this;
		do
		{
			if(form->CanReturn) { form = NULL; break; } // CanReturn な form を超えて continue はできない
			child = form;
			form = child->Parent;
			if(!form) break;

			info = form->GetCurrentContinueInfo();
			if(info)
			{
				// continue が値を伴ってできるかをチェック
				if(var != NULL && !info->GetIsBlock())
					tCompileExceptionClass::Throw(
						Function->GetFunctionGroup()->
							GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
						tString(RISSE_WS_TR("cannot continue here with a value")),
							GetScriptBlockInstance(), pos);

				if(var == NULL && info->GetIsBlock())
				{
					// 値を受け取ることができるのに値が無い場合はvoidにする
					var = AddConstantValueStatement(pos, tVariant()); // void
				}

				// continue が値を伴える場合は return と同じ動作になる
				if(info->GetIsBlock())
				{
					// 単純に return 文を作成可能
					// return 文を作成
					AddStatement(pos, ocReturn, NULL, var);

					// 新しい基本ブロックを作成(ただしここには到達しない)
					CreateNewBlock("disconnected_by_return");
					return;
				}

				// このSSA形式はcontinue文を受け取ることができる
				// この try id まで例外で抜けるためのコードを生成
				tSSAStatement * stmt = AddStatement(pos, ocExitTryException, NULL, var);
				risse_size label_idx = form->AddExitTryBranchTargetLabel(
													info->GetJumpTargetLabel());
				stmt->SetTryIdentifierIndex(child->TryIdentifierIndex); // try id を設定
				stmt->SetIndex(label_idx + 2); // インデックスを設定
					// +2 = 最初の2つは例外が発生しなかったときと通常の例外が発生したとき
					// に割り当てられているので

				CreateNewBlock(RISSE_WS("disconnected_by_continue_by_exception"));
				break;
			}
		} while(true);

		if(form == NULL)
		{
			// continue できないようだ
			tCompileExceptionClass::Throw(
				Function->GetFunctionGroup()->
					GetCompiler()->GetScriptBlockInstance()->GetScriptEngine(),
				tString(RISSE_WS_TR("cannot place 'continue' here")),
					GetScriptBlockInstance(), pos);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::AddCatchBranchTargetsForOne(tSSAStatement * catch_branch_stmt,
				tSSAVariable * except_value, tExitTryBranchTargetLabels * target_labels)
{
	RISSE_ASSERT(catch_branch_stmt->GetCode() == ocCatchBranch);
	RISSE_ASSERT(catch_branch_stmt->GetTargetCount() >= 2);

	// この処理の中では CurrentBlock や LocalNamespace が変更される
	// 可能性があるのでいったん保存する
	tSSABlock * block_save = CurrentBlock;
	tSSALocalNamespace * ns_save = LocalNamespace;

	// すべての分岐先をリストアップ
	gc_vector<tString> targets;
	targets.resize(target_labels->size(), tString::GetEmptyString());
	for(tExitTryBranchTargetLabels::iterator i = target_labels->begin();
		i != target_labels->end(); i++)
		targets[i->second] = i->first;

	// それぞれの分岐先に対してコードを生成
	for(gc_vector<tString>::iterator i = targets.begin(); i != targets.end(); i++)
	{
		RISSE_ASSERT(!i->IsEmpty());
		tSSABlock * target = NULL;

		if(*i == RISSE_WS("@return"))
		{
			// return 文の作成
			// 新しい基本ブロックを作成する
			target =
				CreateNewBlock(RISSE_WS("return_by_exception"),
					catch_branch_stmt->GetBlock()->GetLocalNamespace());

			// 例外オブジェクトから値を取り出す
			tSSAVariable * ret_var = NULL;
			AddStatement(catch_branch_stmt->GetPosition(),
				ocGetExitTryValue, &ret_var, except_value);

			// return 文を作成
			AddStatement(catch_branch_stmt->GetPosition(),
				ocReturn, NULL, ret_var);

			// 新しい基本ブロックを作成(ただしここには到達しない)
			CreateNewBlock("disconnected_by_return");
		}
		else if((*i)[0] == RISSE_WC('#'))
		{
			// すでに解決済み
			// なにもしない
		}
		else
		{
			// goto など
			// 新しい基本ブロックを作成する

			// この新しいブロックは、catch_branch_stmt の名前空間を引き継ぐ。
			// 生成順序が前後しているが、このブロックは catch_branch_stmt の
			// あとに来るためである。
			target =
				CreateNewBlock(RISSE_WS("goto_by_exception"),
					catch_branch_stmt->GetBlock()->GetLocalNamespace());

			// 例外オブジェクトから値を取り出す
			tSSAVariable * break_var = NULL;
			AddStatement(catch_branch_stmt->GetPosition(),
				ocGetExitTryValue, &break_var, except_value);

			// _ に値を書き込む
			WriteLastEvalResult(catch_branch_stmt->GetPosition(), break_var);

			Function->AddPendingLabelJump(target, *i);

			// 新しい基本ブロックを作成(ただしここには到達しない)
			CreateNewBlock("disconnected_by_goto_or_continue_or_break");
		}

		// catch_branch_stmt にジャンプ先を追加
		if(target) catch_branch_stmt->AddTarget(target);
	}

	// 保存しておいた CurrentBlock や LocalNamespace を元に戻す
	CurrentBlock = block_save;
	LocalNamespace = ns_save;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::AddCatchBranchTargets()
{
	for(tCatchBranchAndExceptionMap::iterator i =
							CatchBranchAndExceptionMap.begin();
		i != CatchBranchAndExceptionMap.end(); i++)
	{
		AddCatchBranchTargetsForOne(
			i->second->CatchBranchStatement,
			i->second->ExceptionValue,
			i->second->ExitTryBranchTargetLabels);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::AddCatchBranchAndExceptionValue(
				tSSAStatement * catch_branch_stmt,
				tSSAVariable * except_value)
{
	RISSE_ASSERT(ExitTryBranchTargetLabels);
	RISSE_ASSERT(catch_branch_stmt->GetCode() == ocCatchBranch);

	CatchBranchAndExceptionMap.insert(
		tCatchBranchAndExceptionMap::value_type(
			catch_branch_stmt->GetTryIdentifierIndex(),
			new tCatchBranchAndExceptionValue(
				catch_branch_stmt,
				except_value,
				ExitTryBranchTargetLabels)));
	ExitTryBranchTargetLabels = NULL; // このインスタンスはここではもう使わない


}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariableAccessMap * tSSAForm::CreateAccessMap(risse_size pos)
{
	// 遅延評価ブロック内で発生したtry脱出用例外に備えて ExitTryBranchTargetLabels
	// を作成する
	// ここで作成した ExitTryBranchTargetLabels は AddCatchBranchAndExceptionValue()
	// で CatchBranch とともに保存される
	ExitTryBranchTargetLabels = new tExitTryBranchTargetLabels();
	return new tSSAVariableAccessMap(this, pos);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void * tSSAForm::CreateLazyBlock(risse_size pos, const tString & basename,
	bool sharevars, tSSAVariableAccessMap * accessmap,
	tSSAForm *& new_form, tSSAVariable *& block_var)
{
	// 遅延評価ブロックの名称を決める
	tString block_name = basename + RISSE_WS(" ") +
		tString::AsString(GetFunction()->
						GetFunctionGroup()->GetCompiler()->GetUniqueNumber());

	// 遅延評価ブロックを生成
	if(sharevars)
	{
		// 関数の場合
		tCompilerFunction *child_function =
			new tCompilerFunction(Function->GetFunctionGroup(),
				Function, risse_size_max, block_name);
		new_form =
			new tSSAForm(pos, child_function, block_name, this, !sharevars);
	}
	else
	{
		// ブロックの場合
		new_form =
			new tSSAForm(pos, Function, block_name, this, !sharevars);
	}

	Children.push_back(new_form);

	// 新しく作成した遅延評価ブロックに、一番外側のローカル名前空間をpushする
	new_form->LocalNamespace->Push();

	// ローカル名前空間の親子関係を設定
	new_form->LocalNamespace->SetParent(LocalNamespace);

	if(!sharevars)
	{
		// 変数を固定しない場合はアクセスマップを設定する
		RISSE_ASSERT(accessmap != NULL);
		new_form->LocalNamespace->SetAccessMap(accessmap);
	}

	// 遅延評価ブロックを生成する文を追加する
	tSSAStatement * lazy_stmt =
		AddStatement(pos, ocDefineLazyBlock, &block_var);
	lazy_stmt->SetName(block_name);
	lazy_stmt->SetDefinedForm(new_form);

	// 情報を返す
	tLazyBlockParam * param = new tLazyBlockParam();
	param->NewForm = new_form;
	param->Position = pos;
	param->BlockVariable = block_var;
	param->AccessMap = accessmap;
	return reinterpret_cast<void *>(param);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::ListVariablesForLazyBlock(risse_size pos, tSSAVariableAccessMap * accessmap)
{
	// 遅延評価ブロックで読み込みが起こった変数を処理する
	accessmap->GenerateChildWrite(this, pos);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::CleanupLazyBlock(void * param)
{
	// param はtLazyBlockParam を表している
	// tLazyBlockParam * info_param = reinterpret_cast<tLazyBlockParam *>(param);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::CleanupAccessMap(risse_size pos, tSSAVariableAccessMap * accessmap)
{
	accessmap->GenerateChildRead(this, pos);

	// ocEndAccessMap を追加する
	accessmap->GenerateEndAccessMap(this, pos);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::AddBindingMap(risse_size pos, tSSAVariable * binding)
{
	// まず、このスコープから可視な変数をすべて共有するとしてマークする
	LocalNamespace->ShareAllVisibleVariableNames();

	// それぞれの変数のマッピングを追加する
	tSSALocalNamespace::tAliasMap names;
	LocalNamespace->ListAllVisibleVariableNumberedNames(names);
	for(tSSALocalNamespace::tAliasMap::iterator i = names.begin(); i != names.end(); i++)
	{
		const tString & name = i->first; // 装飾なしの名前
		const tString & nname = i->second; // 番号付きの名前
		tSSAVariable * name_var = AddConstantValueStatement(pos, name);
		tSSAStatement * stmt = AddStatement(pos, ocAddBindingMap, NULL, binding, name_var);
		stmt->SetName(nname);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tSSAForm::Dump() const
{
	// この form の EntryBlock から到達可能な基本ブロックをすべてダンプする
	gc_vector<tSSABlock *> blocks;
	EntryBlock->Traverse(blocks);
	tString ret;
	for(gc_vector<tSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		ret += (*i)->Dump();

	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::GenerateLastReturn(const tASTNode * root)
{
	// 最後の return; 文がない場合に備え、これを補う。
	// 実際に最後の return 文があった場合は単にこの文は実行されない物として
	// 後続の LeapDeadBlocks() で破棄される。
	// 返す値は '_' の値となる。
	risse_size pos = root->SearchEndPosition();
	tSSAVariable * ret_var = LocalNamespace->Read(pos, ss_lastEvalResultHiddenVarName);
	AddStatement(pos, ocReturn, NULL, ret_var);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::LeapDeadBlocks()
{
	// EntryBlock から到達可能なすべての基本ブロックを得る
	gc_vector<tSSABlock *> blocks;
	EntryBlock->Traverse(blocks);

	// EntryBlock から到達可能なすべての基本ブロックに 「生存」の印を付ける
	// また、そこで宣言されている変数のマークをすべてクリアする
	for(gc_vector<tSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
	{
		(*i)->SetAlive();
		(*i)->ClearVariableMarks();
	}

	// EntryBlock から到達可能なすべての基本ブロックのうち、
	// 到達できないブロックが Pred にあれば削除する
	for(gc_vector<tSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->DeleteDeadPred();

	// 各変数のうち、変数が使用されている文が所属しているブロックが死んでいる場合、
	// その文を変数の使用リストから削除する
	for(gc_vector<tSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->DeleteDeadStatementsFromVariables();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::ConvertSharedVariableAccess()
{
	// EntryBlock から到達可能なすべての基本ブロックを得る
	gc_vector<tSSABlock *> blocks;
	EntryBlock->Traverse(blocks);

	// すべてのブロックに対して処理を行う
	for(gc_vector<tSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->ConvertSharedVariableAccess();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::AnalyzeVariableBlockLiveness()
{
	// EntryBlock から到達可能なすべての基本ブロックを得て、マークをクリアする
	// また、そこで宣言されている変数のマークをすべてクリアする
	gc_vector<tSSABlock *> blocks;
	EntryBlock->Traverse(blocks);
	for(gc_vector<tSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
	{
		(*i)->SetMark(NULL);
		(*i)->ClearVariableMarks();
	}

	// LiveIn と LiveOut を作成する
	for(gc_vector<tSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->CreateLiveInAndLiveOut();

	// それぞれのブロック内にある変数に対して生存区間解析を行う
	for(gc_vector<tSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->AnalyzeVariableBlockLiveness();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::AnalyzeVariableBlockLiveness(tSSAVariable * var)
{
	// それぞれの 変数の使用位置について、変数の宣言か、この変数が使用されていると
	// マークされている箇所にまで逆順にブロックをたどる
	// (すでにたどったブロックはたどらない)

wxFprintf(stderr, wxT("for %s: "), var->GetQualifiedName().AsWxString().c_str());
	// この変数が宣言されている文
	tSSAStatement * decl_stmt = var->GetDeclared();

	// この変数が宣言されているブロック
	tSSABlock * decl_block = decl_stmt->GetBlock();

	// この変数が使用されているそれぞれの文について
	const gc_vector<tSSAStatement *> & used = var->GetUsed();
	for(gc_vector<tSSAStatement *>::const_iterator si = used.begin();
		si != used.end(); si++)
	{
		// この文
		tSSAStatement * used_stmt = *si;

		// この文のブロックは
		tSSABlock * used_block = used_stmt->GetBlock();
		RISSE_ASSERT(used_block != NULL);

		// ブロックを逆順にたどる
		// 終了条件は
		// ・たどるべきブロックがなくなった場合 -> あり得ない
		// ・変数の宣言されたブロックにたどり着いた場合
		// ・すでにこの変数が使用されているとマークされているブロックにたどり着いた場合

		gc_vector<tSSABlock *> Stack;
wxFprintf(stderr, wxT(", starting from  %s"), used_block->GetName().AsWxString().c_str());
		Stack.push_back(used_block); // 初期ノードを入れる
		do
		{
			bool stop = false;

			// スタックから値を取り出す
			tSSABlock * quest_block = Stack.back();
			Stack.pop_back();
wxFprintf(stderr, wxT(", checking for block %s"), quest_block->GetName().AsWxString().c_str());

			if(quest_block == decl_block && used_stmt->GetCode() == ocPhi)
			{
				// 変数が宣言されているブロックにたどり着いた場合は、そこでこのノード
				// の先をたどるのは辞める
				// ただし宣言された文と同じブロックのいずれかの
				// phi 関数で使われている場合
				//   要するにたとえば
				//     x1=phi(x0,x2)
				//     y1=phi(y0,x1)
				//   こんな感じの基本ブロックに再入してたりとか
				//     x1=phi(x0,x1)
				//   こんな感じの基本ブロックに再入してたりとか
				//     x0=phi(x1,x2)
				//     x2=x0+3
				//   こんな感じの基本ブロックに再入してたりとか
				// こういうときは前のブロック(=それは自分自身のブロックかもしれないが)
				// をちゃんとたどる
				if(used_stmt->GetBlock() != decl_block)
				{
					stop = true;
wxFprintf(stderr, wxT(", the block is declaring block; stop"), quest_block->GetName().AsWxString().c_str());
				}
				else
				{
wxFprintf(stderr, wxT(", using stmt and declaring stmt are both phi; continue"), quest_block->GetName().AsWxString().c_str());
				}

				// φ関数の場合は、φ関数の先が既にliveoutになっているかどうかをみる
				// φ関数のused内でvarを探す
				if(!stop)
				{
					const gc_vector<tSSAVariable *> & phi_used = used_stmt->GetUsed();
					risse_size idx = 0;
					for(gc_vector<tSSAVariable *>::const_iterator i = phi_used.begin();
						i != phi_used.end(); i++, idx++)
					{
						if(*i == var)
						{
							// var が見つかったのでその方向のブロックの liveout を見る
							tSSABlock * pred = quest_block->GetPred()[idx];
							if(pred->GetLiveness(var, true))
							{
								stop = true;
	wxFprintf(stderr, wxT(", the phi pred block %s has liveness for the variable; stop"), pred->GetName().AsWxString().c_str());
								break;
							}
						}
					}
				}

				if(!stop)
				{
					// この時点で quest_block では
					// 変数が宣言されていない→これよりも前のブロックで変数が宣言されている
					// →このブロックとpredの間では変数は生存している
					// つまり、LiveIn に変数を追加する
					quest_block->AddLiveness(var, false);
wxFprintf(stderr, wxT(", adding livein"), quest_block->GetName().AsWxString().c_str());

					// スタックに quest_block のpred を追加する
					// また、pred の LiveOut にも変数を追加する
					// ただし、この文がφ関数だった場合は、φ関数に対応した
					// 方向のブロックのみを追加する

					// φ関数のused内でvarを探す
					const gc_vector<tSSAVariable *> & phi_used = used_stmt->GetUsed();
					risse_size idx = 0;
					for(gc_vector<tSSAVariable *>::const_iterator i = phi_used.begin();
						i != phi_used.end(); i++, idx++)
					{
						if(*i == var)
						{
							// var が見つかったのでその方向へ探索を続ける
							tSSABlock * pred = quest_block->GetPred()[idx];
wxFprintf(stderr, wxT(", found phi pred %s"), pred->GetName().AsWxString().c_str());
							pred->AddLiveness(var, true);
							Stack.push_back(pred);
						}
					}
				}


			}
			else
			{
				// 変数が宣言されているブロックにたどり着いた場合は、そこでこのノード
				// の先をたどるのは辞める
				if(quest_block == decl_block)
				{
					stop = true;
wxFprintf(stderr, wxT(", the block is declaring block; stop"), quest_block->GetName().AsWxString().c_str());
				}

				// quest_block の LiveIn にこの変数が追加されているか
				// 追加されているならば そこでこのノード
				// の先をたどるのは辞める
				// また、φ関数の場合は、φ関数の先が既にliveoutになっているかどうかをみる
				if(!stop && quest_block->GetLiveness(var, false))
				{
					stop = true;
wxFprintf(stderr, wxT(", the block has liveness for the variable; stop"));
				}

				if(!stop)
				{
					// この時点で quest_block では
					// 変数が宣言されていない→これよりも前のブロックで変数が宣言されている
					// →このブロックとpredの間では変数は生存している
					// つまり、LiveIn に変数を追加する
					quest_block->AddLiveness(var, false);
	wxFprintf(stderr, wxT(", adding livein"), quest_block->GetName().AsWxString().c_str());

					// スタックに quest_block のpred を追加する
					// また、pred の LiveOut にも変数を追加する
					for(gc_vector<tSSABlock *>::const_iterator i =
										quest_block->GetPred().begin();
						i != quest_block->GetPred().end(); i++)
					{
						(*i)->AddLiveness(var, true);
						Stack.push_back(*i);
					}
				}
			}
		} while(Stack.size() > 0);

	}
wxFprintf(stderr, wxT("\n")); fflush(stderr);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::SetStatementOrder()
{
	// EntryBlock から到達可能なすべての基本ブロックを得る
	gc_vector<tSSABlock *> blocks;
	EntryBlock->Traverse(blocks);

	// すべての基本ブロック内の文に通し番号を設定する
	risse_size order = 1;
	for(gc_vector<tSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->SetOrder(order);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::CreateVariableInterferenceGraph()
{
	// EntryBlock から到達可能なすべての基本ブロックを得る
	gc_vector<tSSABlock *> blocks;
	EntryBlock->Traverse(blocks);

	// すべての基本ブロックに対して変数の干渉グラフを作成させる
	for(gc_vector<tSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->CreateVariableInterferenceGraph();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::OptimizeStatement()
{
	// 基本ブロックのリストを取得
	gc_vector<tSSABlock *> blocks;
	EntryBlock->Traverse(blocks);

	// すべての文をいったん列挙した後、それを作業リストとして作業を行う
	// すべての文を収集
	gc_map<risse_size, tSSAStatement *> statements;
	for(gc_vector<tSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->ListAllStatements(statements);

	// リストが空になるまで文ごとに処理を行う
	while(statements.size())
	{
		// どれか一つをstatementsからとってくる
		gc_map<risse_size, tSSAStatement *>::iterator i = statements.begin();
		tSSAStatement * stmt = i->second;
		statements.erase(i);

		// その文に対して最適化を行う
		stmt->OptimizeAtStatementLevel(statements);
	}

	// すべてのブロックの生存フラグを倒す
	for(gc_vector<tSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->SetAlive(false);

	// 各文で宣言された変数の型の伝播や、条件付き定数伝播を行う。
	gc_vector<tSSAVariable *> variables;

	// エントリブロックの生存フラグを立てる
	EntryBlock->SetAlive(true);

	// blocks にエントリブロックを push する
	blocks.clear();
	blocks.push_back(EntryBlock);

	// リストが空になるまで文ごとあるいはブロックごとに処理を行う
	while(variables.size() || blocks.size())
	{
		if(variables.size() > 0)
		{
			// variables から変数を pop する
			tSSAVariable * var = variables.back();
			variables.pop_back();

			// その変数が使用されている各文について解析を行う
			const gc_vector<tSSAStatement *> & used = var->GetUsed();
			for(gc_vector<tSSAStatement *>::const_iterator i = used.begin();
				i != used.end(); i++)
				const_cast<tSSAStatement*>(*i)->AnalyzeConstantPropagation(variables, blocks);
		}

		if(blocks.size() > 0)
		{
			// blocks からブロックを pop する
			tSSABlock * block = blocks.back();
			blocks.pop_back();

			// そのブロックに対して型伝播解析・定数伝播解析を行う
			block->AnalyzeConstantPropagation(variables, blocks);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::TraceCoalescable()
{
	// 基本ブロックのリストを取得
	gc_vector<tSSABlock *> blocks;
	EntryBlock->Traverse(blocks);

	// 変数の合併を行うためのトレースを行う
	// phi関数、単純代入をトレースし、関連する変数をすべて一つにまとめる
	for(gc_vector<tSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->TraceCoalescable();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::Coalesce()
{
	// 基本ブロックのリストを取得
	gc_vector<tSSABlock *> blocks;
	EntryBlock->Traverse(blocks);

	// 変数の合併を行う
	for(gc_vector<tSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->Coalesce();

	// これ以降、SSA性は破壊される
	State = ssNonSSA;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::RemovePhiStatements()
{
	// 基本ブロックのリストを取得
	gc_vector<tSSABlock *> blocks;
	EntryBlock->Traverse(blocks);

	// それぞれのブロックについて処理
	for(gc_vector<tSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->RemovePhiStatements();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::Check3AddrAssignee()
{
	// 基本ブロックのリストを取得
	gc_vector<tSSABlock *> blocks;
	EntryBlock->Traverse(blocks);

	// それぞれのブロックについて処理
	for(gc_vector<tSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->Check3AddrAssignee();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::AssignRegisters()
{
	// 変数にレジスタを割り当てる

	// 基本ブロックのリストを取得
	gc_vector<tSSABlock *> blocks;
	EntryBlock->Traverse(blocks);

	// それぞれのブロックについて処理
	gc_vector<void*> assign_work;
	for(gc_vector<tSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->AssignRegisters(assign_work);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::EnsureCodeGenerator()
{
	// code generator は既にコンストラクタ内で作成されているので
	// ここでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::GenerateCode()
{
	if(State == ssCodeGenerated) return; // 既にコードを生成した場合は何もしない

	// バイトコードを生成する
	RISSE_ASSERT(CodeGenerator != NULL);
	CodeGenerator->SetRegisterBase();

	// EntryBlock から到達可能なすべての基本ブロックを得る
	gc_vector<tSSABlock *> blocks;
	EntryBlock->Traverse(blocks);

	// すべての基本ブロックに対してコード生成を行わせる
	for(gc_vector<tSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->GenerateCode(CodeGenerator);

	// コードを確定する
	CodeGenerator->FixCode();

	// この時点で CodeGenerator は、この SSA 形式から生成されたコードが
	// どれほどのスタック領域を喰うのかを知っている。
	// 子 SSA 形式から生成されたコードは親 SSA 形式から生成されたコードの
	// スタック領域を破壊しないようにレジスタを配置する必要がある。
	// すべての子SSA形式に対してバイトコード生成を行わせる

	// TODO: スタックフレームの割り当ては最小限になっているか？
	//		try...catchはその場で実行されるので、親コードブロックの「その時点」
	//		でのレジスタの使用分の直後にtry...catchのスタックフレームが配置
	//		できればよい
	for(gc_vector<tSSAForm *>::const_iterator i = Children.begin();
		i != Children.end(); i++)
	{
		RISSE_ASSERT((*i)->CodeGenerator != NULL);
		(*i)->GenerateCode();
	}

	// CodeBlock にコードを設定する
	CodeBlock->Assign(CodeGenerator);

	// コードを生成し終わった
	State = ssCodeGenerated;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAForm::SetSharedVariableNestCount()
{
	// コードブロックに最大の共有変数のネストカウントを通知する
	risse_size nestcount = CodeGenerator->QuerySharedVariableNestCount();
	CodeBlock->SetSharedVariableNestCount(nestcount);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
