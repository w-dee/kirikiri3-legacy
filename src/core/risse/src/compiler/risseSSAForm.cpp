//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

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
#include "../risseException.h"
#include "../risseScriptBlockBase.h"
#include "../risseCodeBlock.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(7246,55563,12549,16650,3991,47135,2967,64968);



//---------------------------------------------------------------------------
tRisseBreakInfo::tRisseBreakInfo(tRisseSSAForm * form,
	const tRisseString & label_prefix)
{
	Form = form;
	IsBlock = false;

	// ターゲットブロックのラベル名を生成
	JumpTargetLabel = 
				(label_prefix.IsEmpty() ?
					tRisseString(RISSE_WS("@break_or_continue")):label_prefix) +
				RISSE_WS("_") +
				tRisseString::AsString(Form->GetUniqueNumber());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBreakInfo::AddJump(tRisseSSABlock * jump_block)
{
	PendingJumpSourceBlocks.push_back(jump_block);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBreakInfo::BindAll(tRisseSSABlock * target)
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
tRisseSSAForm::tRisseSSAForm(tRisseCompilerFunction * function,
	const tRisseString & name, tRisseSSAForm * parent, bool useparentframe)
{
	Function = function;
	Parent = parent;
	UseParentFrame = useparentframe;
	Name = name;
	UniqueNumber = 0;
	LocalNamespace = new tRisseSSALocalNamespace();
	EntryBlock = NULL;
	CurrentBlock = NULL;
	CurrentSwitchInfo = NULL;
	CurrentBreakInfo = NULL;
	CurrentContinueInfo = NULL;
	FunctionCollapseArgumentVariable = NULL;
	TryIdentifierIndex = risse_size_max;
	CanReturn = !UseParentFrame; // いまのところ CanReturn はこの式の通りで決定される
	ExitTryBranchTargetLabels = NULL;

	// 関数インスタンスに自身を登録する
	Function->AddSSAForm(this);

	// コードジェネレータを作成する
	RISSE_ASSERT(!(Parent && Parent->CodeGenerator == NULL));
	CodeGenerator = new tRisseCodeGenerator(Parent ? Parent->CodeGenerator : NULL, UseParentFrame);
	CodeBlock = new tRisseCodeBlock();
	CodeBlockIndex = Function->GetFunctionGroup()->GetCompiler()->AddCodeBlock(CodeBlock);

	// エントリー位置の基本ブロックを生成する
	EntryBlock = new tRisseSSABlock(this, RISSE_WS("entry"));
	LocalNamespace->SetBlock(EntryBlock);
	CurrentBlock = EntryBlock;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAForm::Generate(const tRisseASTNode * root)
{
	// AST をたどり、それに対応する SSA 形式を作成する

	// ルートノードを処理する
	root->GenerateReadSSA(this);

	// 実行ブロックの最後の return 文を生成する
	GenerateLastReturn(root);

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAForm::OptimizeAndUnSSA()
{
	// 到達しない基本ブロックからのパスを削除
	LeapDeadBlocks();

	// 共有変数へのアクセスを別形式の文に変換
	ConvertSharedVariableAccess();

	// 変数の有効範囲をブロック単位で解析
	AnalyzeVariableBlockLiveness();

	// φ関数を除去
	RemovePhiStatements();

	// 変数の有効範囲を文単位で解析
	AnalyzeVariableStatementLiveness();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseScriptBlockBase * tRisseSSAForm::GetScriptBlock() const
{
	return Function->GetFunctionGroup()->GetCompiler()->GetScriptBlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSABlock * tRisseSSAForm::CreateNewBlock(const tRisseString & name)
{
	// 今までの (Current) の基本ブロックに名前空間のスナップショットを作る
	CurrentBlock->TakeLocalNamespaceSnapshot(LocalNamespace);

	// ローカル変数名前空間をいったんすべてφ関数を見るようにマークする
	LocalNamespace->MarkToCreatePhi();

	// 新しい基本ブロックを作成する
	tRisseSSABlock * new_block = new tRisseSSABlock(this, name);

	LocalNamespace->SetBlock(new_block);

	// 新しい「現在の」基本ブロックを設定し、それを返す
	CurrentBlock = new_block;
	return CurrentBlock;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tRisseSSAForm::InternalAddExitTryBranchTargetLabel(
						tExitTryBranchTargetLabels * target_label, 
						const tRisseString & label)
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
risse_size tRisseSSAForm::AddExitTryBranchTargetLabel(risse_size try_id,
													const tRisseString & label)
{
	tCatchBranchAndExceptionMap::iterator i =
		CatchBranchAndExceptionMap.find(try_id);
	RISSE_ASSERT(i != CatchBranchAndExceptionMap.end());
	return InternalAddExitTryBranchTargetLabel(i->second->ExitTryBranchTargetLabels, label);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tRisseSSAForm::AddExitTryBranchTargetLabel(const tRisseString & label)
{
	RISSE_ASSERT(ExitTryBranchTargetLabels != NULL);
	return InternalAddExitTryBranchTargetLabel(ExitTryBranchTargetLabels, label);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseSSAForm::AddConstantValueStatement(
										risse_size pos,
										const tRisseVariant & val)
{
	// 文の作成
	tRisseSSAStatement * stmt =
		new tRisseSSAStatement(this, pos, ocAssignConstant);
	// 変数の作成
	tRisseSSAVariable * var = new tRisseSSAVariable(this, stmt);
	var->SetValue(new tRisseVariant(val));
	// 文の追加
	CurrentBlock->AddStatement(stmt);
	// 戻る
	return var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAStatement * tRisseSSAForm::AddStatement(risse_size pos, tRisseOpCode code,
		tRisseSSAVariable ** ret_var,
			tRisseSSAVariable *using1,
			tRisseSSAVariable *using2,
			tRisseSSAVariable *using3,
			tRisseSSAVariable *using4
			)
{
	// 文の作成
	tRisseSSAStatement * stmt =
		new tRisseSSAStatement(this, pos, code);

	if(ret_var)
	{
		// 戻りの変数の作成
		*ret_var = new tRisseSSAVariable(this, stmt);
	}

	// 文のSSAブロックへの追加
	GetCurrentBlock()->AddStatement(stmt);

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
void tRisseSSAForm::AddReturnStatement(risse_size pos, tRisseSSAVariable * var)
{
	if(var == NULL)
	{
		// 値が無い場合はvoidにする
		var = AddConstantValueStatement(pos, tRisseVariant()); // void
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
		tRisseSSAForm * child = this;
		tRisseSSAForm * form = child->Parent;
		do
		{
			if(form->CanReturn)
			{
				// このSSA形式は return文で抜けることができる
				// この try id まで例外で抜けるためのコードを生成
				tRisseSSAStatement * stmt = AddStatement(pos, ocExitTryException, NULL, var);
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
void tRisseSSAForm::AddBreakOrContinueStatement(bool is_break, risse_size pos,
											tRisseSSAVariable * var)
{
	// このSSA形式インスタンスはbreakまたはcontinueできるか？
	tRisseBreakInfo * info;
	if((info = is_break?GetCurrentBreakInfo():GetCurrentContinueInfo()) != NULL)
	{
		// この SSA 形式は break/continue できる

		// break/continue が値を伴ってできるかをチェック
		if(var != NULL && !info->GetIsBlock())
			eRisseCompileError::Throw(
				tRisseString(
					is_break?
						RISSE_WS_TR("cannot break here with a value"):
						RISSE_WS_TR("cannot continue here with a value")),
					GetScriptBlock(), pos);

		// ジャンプ文を info に登録
		info->AddJump(GetCurrentBlock());

		// 新しい基本ブロックを作成
		CreateNewBlock(is_break?
						RISSE_WS("disconnected_by_break"):
						RISSE_WS("disconnected_by_continue"));
	}
	else
	{
		// break/continue できる SSA 形式を探す
		tRisseSSAForm * child = NULL;
		tRisseSSAForm * form = this;
		do
		{
			if(form->CanReturn) { form = NULL; break; } // CanReturn な form を超えて break/continue はできない
			child = form;
			form = child->Parent;
			if(!form) break;

			if((info = is_break?form->GetCurrentBreakInfo():form->GetCurrentContinueInfo()) != NULL)
			{
				// break/continue が値を伴ってできるかをチェック
				if(var != NULL && !info->GetIsBlock())
					eRisseCompileError::Throw(
						tRisseString(
							is_break?
								RISSE_WS_TR("cannot break here with a value"):
								RISSE_WS_TR("cannot continue here with a value")),
							GetScriptBlock(), pos);

				if(var == NULL && info->GetIsBlock())
				{
					// 値を受け取ることができるのに値が無い場合はvoidにする
					var = AddConstantValueStatement(pos, tRisseVariant()); // void
				}

				// continue が値を伴える場合は return と同じ動作になる
				if(!is_break && info->GetIsBlock())
				{
					// 単純に return 文を作成可能
					// return 文を作成
					AddStatement(pos, ocReturn, NULL, var);

					// 新しい基本ブロックを作成(ただしここには到達しない)
					CreateNewBlock("disconnected_by_return");
					return;
				}

				// このSSA形式はbreak/continue文を受け取ることができる
				// この try id まで例外で抜けるためのコードを生成
				tRisseSSAStatement * stmt = AddStatement(pos, ocExitTryException, NULL, var);
				risse_size label_idx = form->AddExitTryBranchTargetLabel(
													info->GetJumpTargetLabel());
				stmt->SetTryIdentifierIndex(child->TryIdentifierIndex); // try id を設定
				stmt->SetIndex(label_idx + 2); // インデックスを設定
					// +2 = 最初の2つは例外が発生しなかったときと通常の例外が発生したとき
					// に割り当てられているので

				CreateNewBlock(is_break?
					RISSE_WS("disconnected_by_break_by_exception"):
					RISSE_WS("disconnected_by_continue_by_exception"));
				break;
			}
		} while(true);

		if(form = NULL)
		{
			// break/continue できないようだ
			eRisseCompileError::Throw(
				tRisseString(is_break?
					RISSE_WS_TR("cannot place 'break' here"):
					RISSE_WS_TR("cannot place 'continue' here")),
					GetScriptBlock(), pos);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAForm::AddCatchBranchTargetsForOne(tRisseSSAStatement * catch_branch_stmt,
				tRisseSSAVariable * except_value, tExitTryBranchTargetLabels * target_labels)
{
	RISSE_ASSERT(catch_branch_stmt->GetCode() == ocCatchBranch);
	RISSE_ASSERT(catch_branch_stmt->GetTargetCount() >= 2);

	tRisseSSABlock * block_save = CurrentBlock;

	// すべての分岐先をリストアップ
	gc_vector<tRisseString> targets;
	targets.resize(target_labels->size(), tRisseString::GetEmptyString());
	for(tExitTryBranchTargetLabels::iterator i = target_labels->begin();
		i != target_labels->end(); i++)
		targets[i->second] = i->first;

	// それぞれの分岐先に対してコードを生成
	for(gc_vector<tRisseString>::iterator i = targets.begin(); i != targets.end(); i++)
	{
		RISSE_ASSERT(!i->IsEmpty());
		tRisseSSABlock * target = NULL;

		if(*i == RISSE_WS("@return"))
		{
			// return 文の作成
			// 新しい基本ブロックを作成する
			target =
				CreateNewBlock(RISSE_WS("return_by_exception"));

			// 例外オブジェクトから値を取り出す
			tRisseSSAVariable * ret_var = NULL;
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
			target =
				CreateNewBlock(RISSE_WS("goto_by_exception"));

			Function->AddPendingLabelJump(target, *i);

			// 新しい基本ブロックを作成(ただしここには到達しない)
			CreateNewBlock("disconnected_by_goto_or_continue_or_break");

			// target の名前空間を catch_branch_stmt の名前空間で上書きする
			// 上記 CreateNewBlock はそのSSA形式が最後に保持していた
			// 基本ブロックの名前空間を最新の名前空間で上書きしてしまうが、
			// この target は catch_branch_stmt がある名前空間の続きなので
			// catch_branch_stmt と同じ名前空間にしなければならない。
			target->TakeLocalNamespaceSnapshot(
				catch_branch_stmt->GetBlock()->GetLocalNamespace());
		}

		// catch_branch_stmt にジャンプ先を追加
		if(target) catch_branch_stmt->AddTarget(target);
	}

	CurrentBlock = block_save;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAForm::AddCatchBranchTargets()
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
void tRisseSSAForm::AddCatchBranchAndExceptionValue(
				tRisseSSAStatement * catch_branch_stmt,
				tRisseSSAVariable * except_value)
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
void tRisseSSAForm::ShareVariable(const tRisseString & name)
{
	SharedVariableMap.insert(tSharedVariableMap::value_type(name, NULL));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseSSAForm::GetShared(const tRisseString & name)
{
	return SharedVariableMap.find(name) != SharedVariableMap.end();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariableAccessMap * tRisseSSAForm::CreateAccessMap(risse_size pos)
{
	// 遅延評価ブロック内で発生したtry脱出用例外に備えて ExitTryBranchTargetLabels
	// を作成する
	// ここで作成した ExitTryBranchTargetLabels は AddCatchBranchAndExceptionValue()
	// で CatchBranch とともに保存される
	ExitTryBranchTargetLabels = new tExitTryBranchTargetLabels();

	return new tRisseSSAVariableAccessMap(this, pos);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void * tRisseSSAForm::CreateLazyBlock(risse_size pos, const tRisseString & basename,
	bool sharevars, tRisseSSAVariableAccessMap * accessmap,
	tRisseSSAForm *& new_form, tRisseSSAVariable *& block_var)
{
	// 遅延評価ブロックの名称を決める
	tRisseString block_name = basename + RISSE_WS(" ") + tRisseString::AsString(GetUniqueNumber());

	// 遅延評価ブロックを生成
	new_form =
		new tRisseSSAForm(Function, block_name, this, !sharevars);
	Children.push_back(new_form);

	// ローカル名前空間の親子関係を設定
	new_form->LocalNamespace->SetParent(LocalNamespace);

	if(!sharevars)
	{
		// 変数を固定しない場合はアクセスマップを設定する
		RISSE_ASSERT(accessmap != NULL);
		new_form->LocalNamespace->SetAccessMap(accessmap);
	}

	// 遅延評価ブロックを生成する文を追加する
	tRisseSSAStatement * lazy_stmt =
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
void tRisseSSAForm::ListVariablesForLazyBlock(risse_size pos, tRisseSSAVariableAccessMap * accessmap)
{
	// 遅延評価ブロックで読み込みが起こった変数を処理する
	accessmap->GenerateChildWrite(this, pos);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAForm::CleanupLazyBlock(void * param)
{
	// param はtLazyBlockParam を表している
	// tLazyBlockParam * info_param = reinterpret_cast<tLazyBlockParam *>(param);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAForm::CleanupAccessMap(risse_size pos, tRisseSSAVariableAccessMap * accessmap)
{
	accessmap->GenerateChildRead(this, pos);

	// ocEndAccessMap を追加する
	AddStatement(pos, ocEndAccessMap, NULL, accessmap->GetVariable());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_int tRisseSSAForm::GetUniqueNumber()
{
	if(Parent) return Parent->GetUniqueNumber(); // 親がある場合は親のを使う
	UniqueNumber++;
	// int のサイズにもよるが、32bit integer では 2^30 ぐらいで元に戻る
	// もちろんこれはそれほど変数が使われることは無いだろうという推測の元なので
	// 周回が起こったらここで例外を吐いて止まってしまった方がいいかもしれない
	if(UniqueNumber >= 1 << (sizeof(risse_int) * 8 - 2)) UniqueNumber = 0;
	return UniqueNumber;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseSSAForm::Dump() const
{
	// この form の EntryBlock から到達可能な基本ブロックをすべてダンプする
	gc_vector<tRisseSSABlock *> blocks;
	EntryBlock->Traverse(blocks);
	tRisseString ret;
	for(gc_vector<tRisseSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		ret += (*i)->Dump();

	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAForm::GenerateLastReturn(const tRisseASTNode * root)
{
	// 最後の return; 文がない場合に備え、これを補う。
	// 実際に最後の return 文があった場合は単にこの文は実行されない物として
	// 後続の LeapDeadBlocks() で破棄される。
	risse_size pos = root->SearchEndPosition();
	tRisseSSAVariable * void_var = AddConstantValueStatement(pos, tRisseVariant()); // void
	AddStatement(pos, ocReturn, NULL, void_var);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAForm::LeapDeadBlocks()
{
	// EntryBlock から到達可能なすべての基本ブロックのマークを解除する
	// TODO: 正確な動作は「(到達可能である・なしに関わらず)すべての基本ブロックのマークを解除する」
	gc_vector<tRisseSSABlock *> blocks;
	EntryBlock->Traverse(blocks);
	for(gc_vector<tRisseSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->SetMark(NULL);

	// EntryBlock から到達可能なすべての基本ブロックをマークする
	for(gc_vector<tRisseSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->SetMark();

	// EntryBlock から到達可能なすべての基本ブロックのうち、
	// 到達できないブロックが Pred にあれば削除する
	for(gc_vector<tRisseSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->DeleteUnmarkedPred();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAForm::ConvertSharedVariableAccess()
{
	// EntryBlock から到達可能なすべての基本ブロックを得る
	gc_vector<tRisseSSABlock *> blocks;
	EntryBlock->Traverse(blocks);

	// すべてのブロックに対して処理を行う
	for(gc_vector<tRisseSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->ConvertSharedVariableAccess();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAForm::AnalyzeVariableBlockLiveness()
{
	// EntryBlock から到達可能なすべての基本ブロックを得て、マークをクリアする
	gc_vector<tRisseSSABlock *> blocks;
	EntryBlock->Traverse(blocks);
	for(gc_vector<tRisseSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->SetMark(NULL);

	// LiveIn と LiveOut を作成する
	for(gc_vector<tRisseSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->CreateLiveInAndLiveOut();

	// それぞれのブロック内にある変数に対して生存区間解析を行う
	for(gc_vector<tRisseSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->AnalyzeVariableBlockLiveness();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAForm::AnalyzeVariableBlockLiveness(tRisseSSAVariable * var)
{
	// それぞれの 変数の使用位置について、変数の宣言か、この変数が使用されていると
	// マークされている箇所にまで逆順にブロックをたどる
	// (すでにたどったブロックはたどらない)

	// この変数が宣言されている文
	tRisseSSAStatement * decl_stmt = var->GetDeclared();

	// この変数が宣言されているブロック
	tRisseSSABlock * decl_block = decl_stmt->GetBlock();

	// この変数が使用されているそれぞれの文について
	const gc_vector<tRisseSSAStatement *> & used = var->GetUsed();
	for(gc_vector<tRisseSSAStatement *>::const_iterator si = used.begin();
		si != used.end(); si++)
	{
		// この文
		tRisseSSAStatement * used_stmt = *si;

		// この文のブロックは
		tRisseSSABlock * used_block = used_stmt->GetBlock();
		RISSE_ASSERT(used_block != NULL);

		// ブロックを逆順にたどる
		// 終了条件は
		// ・たどるべきブロックがなくなった場合 -> あり得ない
		// ・変数の宣言されたブロックにたどり着いた場合
		// ・すでにこの変数が使用されているとマークされているブロックにたどり着いた場合

		gc_vector<tRisseSSABlock *> Stack;
		Stack.push_back(used_block); // 初期ノードを入れる
		do
		{
			bool stop = false;

			// スタックから値を取り出す
			tRisseSSABlock * quest_block = Stack.back();
			Stack.pop_back();

			// 変数が宣言されているブロックにたどり着いた場合は、そこでこのノード
			// の先をたどるのは辞める
			if(quest_block == decl_block)
				stop = true;

			// quest_block の LiveOut または LiveIn にこの変数が追加されているか
			// 追加されているならば そこでこのノード
			// の先をたどるのは辞める
			if(!stop &&(
					quest_block->GetLiveness(var, true) ||
					quest_block->GetLiveness(var, false)))
				stop = true;

			if(!stop)
			{
				// この時点で quest_block では
				// 変数が宣言されていない→これよりも前のブロックで変数が宣言されている
				// →このブロックとpredの間では変数は生存している
				// つまり、LiveIn に変数を追加する
				quest_block->AddLiveness(var, false);

				// スタックに quest_block のpred を追加する
				// また、pred の LiveOut にも変数を追加する
				// ただし、この文がφ関数だった場合は、φ関数に対応した
				// 方向のブロックのみを追加する
				if(used_stmt->GetCode() == ocPhi && used_block == quest_block)
				{
					// φ関数のused内でvarを探す
					const gc_vector<tRisseSSAVariable *> & phi_used = used_stmt->GetUsed();
					risse_size idx = 0;
					for(gc_vector<tRisseSSAVariable *>::const_iterator i = phi_used.begin();
						i != phi_used.end(); i++, idx++)
					{
						if(*i == var)
						{
							// var が見つかったのでその方向へ探索を続ける
							tRisseSSABlock * pred = quest_block->GetPred()[idx];
							pred->AddLiveness(var, true);
							Stack.push_back(pred);
							break;
						}
					}
				}
				else
				{
					for(gc_vector<tRisseSSABlock *>::const_iterator i =
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
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAForm::RemovePhiStatements()
{
	// 基本ブロックのリストを取得
	gc_vector<tRisseSSABlock *> blocks;
	EntryBlock->Traverse(blocks);

	// それぞれのブロックにつき処理
	for(gc_vector<tRisseSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->RemovePhiStatements();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAForm::AnalyzeVariableStatementLiveness()
{
	// EntryBlock から到達可能なすべての基本ブロックを得る
	gc_vector<tRisseSSABlock *> blocks;
	EntryBlock->Traverse(blocks);

	// すべての基本ブロック内の文に通し番号を設定する
	risse_size order = 0;
	for(gc_vector<tRisseSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->SetOrder(order);

	// 変数の詳細な生存範囲解析を行う
	for(gc_vector<tRisseSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
		(*i)->AnalyzeVariableStatementLiveness();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAForm::EnsureCodeGenerator()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAForm::GenerateCode() const
{
	// バイトコードを生成する
	RISSE_ASSERT(CodeGenerator != NULL);

	// EntryBlock から到達可能なすべての基本ブロックを得る
	gc_vector<tRisseSSABlock *> blocks;
	EntryBlock->Traverse(blocks);

	// すべての共有されている変数を登録する
	CodeGenerator->SetRegisterBase();
	for(tSharedVariableMap::const_iterator i = SharedVariableMap.begin();
		i != SharedVariableMap.end(); i++)
	{
		CodeGenerator->AddSharedRegNameMap(i->first);
	}

	// すべての基本ブロックに対してコード生成を行わせる
	for(gc_vector<tRisseSSABlock *>::iterator i = blocks.begin(); i != blocks.end(); i++)
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
	for(gc_vector<tRisseSSAForm *>::const_iterator i = Children.begin();
		i != Children.end(); i++)
	{
		RISSE_ASSERT((*i)->CodeGenerator != NULL);
		(*i)->GenerateCode();
	}

	// CodeBlock にコードを設定する
	CodeBlock->Assign(CodeGenerator);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risse
