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
tRisseBreakInfo::tRisseBreakInfo(tRisseSSAForm * form)
{
	Form = form;
	CanReceiveValue = false;

	// ターゲットブロックのラベル名を生成
	JumpTargetLabel = 
		tRisseString(RISSE_WS("@break_or_continue_")) +
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
	Form->AddLabelMap(JumpTargetLabel, target);

	for(tPendingJumpSourceBlocks::iterator i = PendingJumpSourceBlocks.begin();
		i != PendingJumpSourceBlocks.end(); i++)
	{
		// それぞれの i について、Form の PendingLabelJumps に入れる
		Form->AddPendingLabelJump(*i, JumpTargetLabel);
	}
}
//---------------------------------------------------------------------------
















//---------------------------------------------------------------------------
tRisseSSAForm::tRisseSSAForm(tRisseCompiler * compiler,
	const tRisseString & name, tRisseSSAForm * parent, bool useparentframe)
{
	Compiler = compiler;
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
	CatchBranchStatement = NULL;
	ExceptionValue = NULL;

	// compiler に自身を登録する
	compiler->AddSSAForm(this);

	// コードジェネレータを作成する
	RISSE_ASSERT(!(Parent && Parent->CodeGenerator == NULL));
	CodeGenerator = new tRisseCodeGenerator(Parent ? Parent->CodeGenerator : NULL, UseParentFrame);
	CodeBlock = new tRisseCodeBlock();
	CodeBlockIndex = Compiler->AddCodeBlock(CodeBlock);

	// エントリー位置の基本ブロックを生成する
	EntryBlock = new tRisseSSABlock(this, RISSE_WS("entry"));
	LocalNamespace->SetBlock(EntryBlock);
	CurrentBlock = EntryBlock;

	// ラベルマップを作成する
	if(!UseParentFrame)
	{
		LabelMap = new tLabelMap();
		PendingLabelJumps = new tPendingLabelJumps();
	}
	else
	{
		// 親SSA形式の物を使う
		LabelMap = Parent->LabelMap;
		PendingLabelJumps = Parent->PendingLabelJumps;
	}
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
	return Compiler->GetScriptBlock();
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
risse_size tRisseSSAForm::AddExitTryBranchTargetLabels(const tRisseString & label)
{
	tExitTryBranchTargetLabels::iterator i = ExitTryBranchTargetLabels.find(label);
	if(i != ExitTryBranchTargetLabels.end())
		return i->second; // すでにマップにある

	// マップにないので追加する
	risse_size label_idx = ExitTryBranchTargetLabels.size();
	ExitTryBranchTargetLabels.insert(
		tExitTryBranchTargetLabels::value_type(
			label, label_idx));

	return label_idx;
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
void tRisseSSAForm::AddLabelMap(const tRisseString &labelname, tRisseSSABlock * block)
{
	tLabelMap::iterator i = LabelMap->find(labelname);
	if(i != LabelMap->end())
	{
		// すでにラベルがある
		eRisseCompileError::Throw(
			tRisseString(RISSE_WS_TR("label '%1' is already defined"), labelname),
				GetScriptBlock(), block->GetLastStatementPosition());
	}

	LabelMap->insert(tLabelMap::value_type(labelname, block));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAForm::AddPendingLabelJump(tRisseSSABlock * jump_block,
			const tRisseString & labelname)
{
	PendingLabelJumps->push_back(tPendingLabelJump(jump_block, labelname));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAForm::BindAllLabels()
{
	if(!CanReturn) return; // return できるSSA形式の場合しか実行しない

	for(tPendingLabelJumps::iterator i = PendingLabelJumps->begin();
		i != PendingLabelJumps->end(); i++)
	{
		// それぞれの i について、その基本ブロックの最後にジャンプ文を生成する

		// ジャンプ先を検索
		tLabelMap::iterator label_pair = LabelMap->find(i->LabelName);
		if(label_pair == LabelMap->end())
		{
			// ラベルは見つからなかった
			eRisseCompileError::Throw(
				tRisseString(RISSE_WS_TR("label '%1' is not defined"), i->LabelName),
					GetScriptBlock(),
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
				new tRisseSSAStatement(this,
					i->SourceBlock->GetLastStatementPosition(), ocJump);
			i->SourceBlock->AddStatement(stmt);
			stmt->SetJumpTarget(label_pair->second);
		}
		else
		{
			// 同じSSA形式インスタンスではない
			// この場合は、source_form の親や先祖に target_form が無ければ
			// ならない (浅いSSA形式から深いSSA形式へのジャンプはできない)
			tRisseSSAForm * child = source_form;
			tRisseSSAForm * form = child->Parent;
			while(form)
			{
				if(target_form == form)
				{
					// ジャンプ先のSSA形式が見つかった
					// この try id まで例外で抜けるためのコードを生成
					tRisseSSAStatement * stmt =
						new tRisseSSAStatement(this,
							i->SourceBlock->GetLastStatementPosition(), ocExitTryException);
					i->SourceBlock->AddStatement(stmt);
					risse_size label_idx = child->AddExitTryBranchTargetLabels(i->LabelName);
					stmt->SetTryIdentifierIndex(child->TryIdentifierIndex); // try id を設定
					stmt->SetIndex(label_idx + 2); // インデックスを設定
						// +2 = 最初の2つは例外が発生しなかったときと通常の例外が発生したとき
						// に割り当てられているので
					break;
				}

				// CanReturn な form を超えて goto はできない
				child = form;
				if(form->CanReturn)
					form = NULL;
				else
					form = child->Parent;
			}

			if(form == NULL)
			{
				// ジャンプ先ラベルはどうやら自分よりも深い場所にいるようだ
				// そういうことは今のところできないのでエラーにする
				eRisseCompileError::Throw(
					tRisseString(RISSE_WS_TR("cannot jump into deeper try block or callback block")),
						GetScriptBlock(),
							i->SourceBlock->GetLastStatementPosition());
			}
		}
	}

	PendingLabelJumps->clear(); // リストはクリアしておく
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAForm::AddReturnStatement(risse_size pos, tRisseSSAVariable * var)
{
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
				risse_size label_idx = child->AddExitTryBranchTargetLabels(RISSE_WS("@return"));
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
		if(var != NULL && !info->GetCanReceiveValue())
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
		tRisseSSAForm * child = this;
		tRisseSSAForm * form = child->Parent;
		do
		{
			if((info = is_break?form->GetCurrentBreakInfo():form->GetCurrentContinueInfo()) != NULL)
			{
				// break/continue できる

				// break/continue が値を伴ってできるかをチェック
				if(var != NULL && !info->GetCanReceiveValue())
					eRisseCompileError::Throw(
						tRisseString(
							is_break?
								RISSE_WS_TR("cannot break here with a value"):
								RISSE_WS_TR("cannot continue here with a value")),
							GetScriptBlock(), pos);

				// このSSA形式はbreak/continue文を受け取ることができる
				// この try id まで例外で抜けるためのコードを生成
				tRisseSSAStatement * stmt = AddStatement(pos, ocExitTryException, NULL, var);
				risse_size label_idx = child->AddExitTryBranchTargetLabels(
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

			// CanReturn な form を超えて break/continue はできない
			child = form;
			if(form->CanReturn)
				form = NULL;
			else
				form = child->Parent;
		} while(form);

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
void tRisseSSAForm::AddCatchBranchTargets()
{
	if(!CatchBranchStatement) return; // CatchBranchの情報が無い場合は何もしない

	RISSE_ASSERT(CatchBranchStatement->GetCode() == ocCatchBranch);
	RISSE_ASSERT(CatchBranchStatement->GetTargetCount() == 2);
	RISSE_ASSERT(Parent != NULL);

	tRisseSSABlock * block_save = Parent->CurrentBlock;

	// すべての分岐先をリストアップ
	gc_vector<tRisseString> targets;
	targets.resize(ExitTryBranchTargetLabels.size(), tRisseString::GetEmptyString());
	for(tExitTryBranchTargetLabels::iterator i = ExitTryBranchTargetLabels.begin();
		i != ExitTryBranchTargetLabels.end(); i++)
		targets[i->second] = i->first;

	// それぞれの分岐先に対してコードを生成
	for(gc_vector<tRisseString>::iterator i = targets.begin(); i != targets.end(); i++)
	{
		RISSE_ASSERT(!i->IsEmpty());
		tRisseSSABlock * target = NULL;

		enum { r_goto, r_return, r_break, r_continue } type;

		if(*i == RISSE_WS("@return"))
			type = r_return;
		else if(*i == RISSE_WS("@break"))
			type = r_break;
		else if(*i == RISSE_WS("@continue"))
			type = r_continue;
		else
			type = r_goto;

		switch(type)
		{
		case r_return:
			{
				// return 文の作成
				// 新しい基本ブロックを作成する
				target =
					Parent->CreateNewBlock(RISSE_WS("return_by_exception"));

				// 例外オブジェクトから値を取り出す
				tRisseSSAVariable * ret_var = NULL;
				Parent->AddStatement(CatchBranchStatement->GetPosition(),
					ocGetExitTryValue, &ret_var, ExceptionValue);

				// return 文を作成
				Parent->AddStatement(CatchBranchStatement->GetPosition(),
					ocReturn, NULL, ret_var);

				// 新しい基本ブロックを作成(ただしここには到達しない)
				Parent->CreateNewBlock("disconnected_by_return");
			}
			break;

		case r_break:
		case r_continue:
		case r_goto:
			// goto
			// 新しい基本ブロックを作成する
			target =
				Parent->CreateNewBlock(RISSE_WS("goto_by_exception"));

			Parent->AddPendingLabelJump(target, *i);

			// 新しい基本ブロックを作成(ただしここには到達しない)
			Parent->CreateNewBlock("disconnected_by_goto_or_continue_or_break");

			// target の名前空間を CatchBranchStatement の名前空間で上書きする
			// 上記 CreateNewBlock はそのSSA形式が最後に保持していた
			// 基本ブロックの名前空間を最新の名前空間で上書きしてしまうが、
			// この target は CatchBranchStatement がある名前空間の続きなので
			// CatchBranchStatement と同じ名前空間にしなければならない。
			target->TakeLocalNamespaceSnapshot(
				CatchBranchStatement->GetBlock()->GetLocalNamespace());
			break;
		}

		// CatchBranchStatement にジャンプ先を追加
		CatchBranchStatement->AddTarget(target);
	}

	Parent->CurrentBlock = block_save;
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
		new tRisseSSAForm(Compiler, block_name, this, !sharevars);
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
