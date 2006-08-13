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
void tRisseSSALabelMap::AddMap(const tRisseString &labelname, tRisseSSABlock * block, risse_size pos)
{
	tLabelMap::iterator i = LabelMap.find(labelname);
	if(i != LabelMap.end())
	{
		// すでにラベルがある
		eRisseCompileError::Throw(
			tRisseString(RISSE_WS_TR("label '%1' is already defined"), labelname),
				Form->GetScriptBlock(), pos);
	}

	LabelMap.insert(tLabelMap::value_type(labelname, block));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSALabelMap::AddPendingLabelJump(tRisseSSABlock * block, risse_size pos,
		const tRisseString & labelname)
{
	PendingLabelJumps.push_back(tPendingLabelJump(block, pos, labelname));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSALabelMap::BindAll()
{
	for(tPendingLabelJumps::iterator i = PendingLabelJumps.begin();
		i != PendingLabelJumps.end(); i++)
	{
		// それぞれの i について、その基本ブロックの最後にジャンプ文を生成する

		// ジャンプ先を検索
		tLabelMap::iterator label_pair = LabelMap.find(i->LabelName);
		if(label_pair == LabelMap.end())
			eRisseCompileError::Throw(
				tRisseString(RISSE_WS_TR("label '%1' is not defined"), i->LabelName),
					Form->GetScriptBlock(), i->Position);

		// ジャンプ文を生成
		tRisseSSAStatement * stmt = new tRisseSSAStatement(Form, i->Position, ocJump);
		i->Block->AddStatement(stmt);
		stmt->SetJumpTarget(label_pair->second);
	}
}
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
void tRisseBreakInfo::AddJump(tRisseSSAStatement * jump_stmt)
{
	PendingJumps.push_back(jump_stmt);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBreakInfo::BindAll(tRisseSSABlock * target)
{
	for(tPendingJumps::iterator i = PendingJumps.begin(); i != PendingJumps.end(); i++)
	{
		// それぞれの i について、ジャンプ文のジャンプ先を設定する
		(*i)->SetJumpTarget(target);
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
	LabelMap = new tRisseSSALabelMap(this);
	EntryBlock = NULL;
	CurrentBlock = NULL;
	CurrentSwitchInfo = NULL;
	CurrentBreakInfo = NULL;
	CurrentContinueInfo = NULL;
	FunctionCollapseArgumentVariable = NULL;

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

	// 未バインドのラベルジャンプをすべて解決
	LabelMap->BindAll();

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
tRisseSSABlock * tRisseSSAForm::CreateNewBlock(const tRisseString & name, tRisseSSABlock * pred)
{
	// 今までの (Current) の基本ブロックに名前空間のスナップショットを作る
	CurrentBlock->TakeLocalNamespaceSnapshot(LocalNamespace);

	// ローカル変数名前空間をいったんすべてφ関数を見るようにマークする
	LocalNamespace->MarkToCreatePhi();

	// 新しい基本ブロックを作成する
	tRisseSSABlock * new_block = new tRisseSSABlock(this, name);

	if(pred) new_block->AddPred(pred);

	LocalNamespace->SetBlock(new_block);

	// 新しい「現在の」基本ブロックを設定し、それを返す
	CurrentBlock = new_block;
	return CurrentBlock;
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
void * tRisseSSAForm::CreateLazyBlock(risse_size pos, const tRisseString & basename,
	bool sharevars, tRisseSSAForm *& new_form, tRisseSSAVariable *& block_var)
{
	// 遅延評価ブロックの名称を決める
	tRisseString block_name = basename + RISSE_WS(" ") + tRisseString::AsString(GetUniqueNumber());

	// 遅延評価ブロックを生成
	new_form =
		new tRisseSSAForm(Compiler, block_name, this, !sharevars);
	Children.push_back(new_form);

	// ローカル名前空間の親子関係を設定
	new_form->LocalNamespace->SetParent(LocalNamespace);

	tRisseSSAVariableAccessMap * access_map = NULL;
	if(!sharevars)
	{
		// 変数を固定しない場合はAccessMapをnew_formの名前空間に作成する
		// (AccessMapはどの変数にアクセスがあったかを記録する)
		access_map = new_form->LocalNamespace->CreateAccessMap();
	}

	// 遅延評価ブロックを生成する文を追加する
	tRisseSSAStatement * lazy_stmt =
		AddStatement(pos, ocDefineLazyBlock, &block_var);
	lazy_stmt->SetName(block_name);
	lazy_stmt->SetDefinedForm(new_form);

	// 遅延評価ブロックで読み込みが起こった変数を処理する
	if(access_map) access_map->GenerateChildRead(this, pos, block_var);

	// 情報を返す
	tLazyBlockParam * param = new tLazyBlockParam();
	param->NewForm = new_form;
	param->Position = pos;
	param->BlockVariable = block_var;
	param->AccessMap = access_map;
	return reinterpret_cast<void *>(param);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAForm::CleanupLazyBlock(void * param)
{
	// param はtLazyBlockParam を表している
	tLazyBlockParam * info_param = reinterpret_cast<tLazyBlockParam *>(param);

	// 遅延評価ブロックで書き込みが起こった変数を処理する
	tRisseSSAVariableAccessMap * access_map = info_param->AccessMap;
	if(access_map)
		access_map->GenerateChildWrite(
			this, info_param->Position, info_param->BlockVariable);

	// ocEndLazyBlock を追加する
	AddStatement(info_param->Position, ocEndLazyBlock, NULL, info_param->BlockVariable);
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
