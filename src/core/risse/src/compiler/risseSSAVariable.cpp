//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief SSA形式における「変数」
//---------------------------------------------------------------------------
#include "../prec.h"

#include "risseSSAVariable.h"
#include "risseSSAStatement.h"
#include "risseSSAForm.h"
#include "risseSSABlock.h"
#include "risseCompilerNS.h"
#include "risseCompiler.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(10364,52326,29794,19229,46468,33376,32183,16216);




//---------------------------------------------------------------------------
tSSAVariable::tSSAVariable(tSSAForm * form, 
	tSSAStatement *stmt, const tString & name)
{
	// フィールドの初期化
	Form = form;
	Declared = stmt;
	CoalescableList = NULL;
	InterferenceEdgeMap = NULL;
	FirstUsedStatement = NULL;
	LastUsedStatement = NULL;
	Value = NULL;
	ValueType = tVariant::vtVoid;
	Mark = NULL;

	// この変数が定義された文の登録
	if(Declared) Declared->SetDeclared(this);

	// 名前と番号を設定する
	SetName(name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAVariable::SetName(const tString & name)
{
	// 名前を設定する
	Name = name;

	// 通し番号の準備
	Version = Form->GetFunction()->GetFunctionGroup()->GetCompiler()->GetUniqueNumber();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tSSAVariable::GetQualifiedName() const
{
	if(Name.IsEmpty())
	{
		// 一時変数の場合は _tmp@ の次にバージョン文字列
		return tString(RISSE_WS("_tmp@")) + tString::AsString(Version);
	}
	else
	{
		// NumberedName がある場合はそれを、Nameを使う
		tString n;
		if(!NumberedName.IsEmpty()) n = NumberedName; else n = Name;
		// 普通の変数の場合は 変数名@バージョン文字列
		return n + RISSE_WC('@') + tString::AsString(Version);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAVariable::DeleteUsed(tSSAStatement * stmt)
{
	gc_vector<tSSAStatement*>::iterator it =
		std::find(Used.begin(), Used.end(), stmt);
	if(it != Used.end()) Used.erase(it);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAVariable::SetInterferenceWith(tSSAVariable * with)
{
	// 作成しようとしているグラフは無向グラフ
	// this と with に相互にエッジを作る
	RISSE_ASSERT(this != with);

	if(!this->InterferenceEdgeMap) this->InterferenceEdgeMap = new tInterferenceEdgeMap();
	if(!with->InterferenceEdgeMap) with->InterferenceEdgeMap = new tInterferenceEdgeMap();

	this->InterferenceEdgeMap->insert(tInterferenceEdgeMap::value_type(with, risse_size_max));
	with->InterferenceEdgeMap->insert(tInterferenceEdgeMap::value_type(this, risse_size_max));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAVariable::EnsureCoalescableList()
{
	if(!CoalescableList)
	{
		CoalescableList = new gc_vector<tSSAVariable*>();
		CoalescableList->push_back(this);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAVariable::CoalesceCoalescableList(tSSAVariable * with)
{
	EnsureCoalescableList();
	with->EnsureCoalescableList();

	if(CoalescableList == with->CoalescableList) return; // すでに合併済み

	// with の CoalescableList に登録されている変数の CoalescableList を
	// 自分の CoalescableList に変更し、
	// 自分の CoalescableList に with->CoalescableList を追加する
	gc_vector<tSSAVariable *> * list = with->CoalescableList;
	for(gc_vector<tSSAVariable *>::iterator i = list->begin();
		i != list->end(); i++)
		(*i)->CoalescableList = CoalescableList;
	CoalescableList->insert(CoalescableList->end(),
		list->begin(), list->end());

	// 自分と with の両方に同じ CoalescableList を設定する
	with->CoalescableList = CoalescableList;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tSSAVariable::CheckInterferenceWith(tSSAVariable * with)
{
	if(!InterferenceEdgeMap) return false; // この変数はどれとも干渉していない
	return InterferenceEdgeMap->find(with) != InterferenceEdgeMap->end();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAVariable::Coalesce()
{
	// この変数を合併する
	if(!CoalescableList) return; // 情報を持っていないので何もしない

	gc_vector<tSSAVariable *> * list = CoalescableList;

	for(gc_vector<tSSAVariable *>::iterator i = list->begin();
		i != list->end(); i++)
	{
		// グループに属するすべての変数の宣言と使用を this に置き換える
		tSSAVariable * var = *i;
		(*i)->CoalescableList = NULL; // CoalescableList は NULL にする (もう処理し終えたよということ)

		// 以降は自分自身を除外
		if(var == this) continue; // 自分自身は除外

		tSSABlock * last_block;
		tSSABlock * current_block;

		// Declared の置き換え
		tSSAStatement * decl_stmt = var->Declared;
		decl_stmt->SetDeclared(this);

		// Declared の含まれるブロックの LiveIn, LiveOut も更新
		current_block = decl_stmt->GetBlock();
		current_block->CoalesceLiveness(var, this);
		last_block = current_block;

		// Used の置き換え
		gc_vector<tSSAStatement *> & used = var->Used;

		for(gc_vector<tSSAStatement *>::iterator i = used.begin();
			i != used.end(); i++)
		{
			(*i)->OverwriteUsed(var, this);
			AddUsed(*i); // SSA性は保持しないので必要ないのかもしれないが、念のため。

			// (*i) の含まれるブロックの LiveIn, LiveOut も更新
			current_block = (*i)->GetBlock();
			if(current_block != last_block)
				current_block->CoalesceLiveness(var, this);
			last_block = current_block;
		}
	}

	RISSE_ASSERT(CoalescableList == NULL);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tSSAVariable::GenerateFuncCall(risse_size pos, const tString & name,
			tSSAVariable * param1,
			tSSAVariable * param2,
			tSSAVariable * param3)
{
	tSSAVariable * func_var = NULL;

	if(!name.IsEmpty())
	{
		// 関数名が指定されている場合

		// メソッド名を置く
		tSSAVariable * method_name_var =
			Form->AddConstantValueStatement(pos, name);

		// メソッドオブジェクトを得る
		Form->AddStatement(pos, ocDGet, &func_var,
								this, method_name_var);
	}
	else
	{
		func_var = this;
	}

	// 関数呼び出し文を生成する
	tSSAVariable * ret_var = NULL;
	tSSAStatement * call_stmt =
		Form->AddStatement(pos, ocFuncCall, &ret_var, func_var, param1, param2, param3);
	call_stmt->SetFuncExpandFlags(0);

	// 戻る
	return ret_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAVariable::DeleteDeadStatements()
{
	for(gc_vector<tSSAStatement*>::iterator it = Used.begin(); it != Used.end();)
	{
		// この文が死んだ基本ブロックに属しているならば削除
		if((*it)->GetBlock()->GetAlive())
		{
			// 生きている
			it ++;
		}
		else
		{
			// 死んでいる
			it = Used.erase(it);
		}
	}

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAVariable::AnalyzeVariableStatementLiveness(tSSAStatement * stmt)
{
	// ここでは命令の生成順(通し番号順)に比較を行うため、通し番号が
	// すでに文についていなければならない
	RISSE_ASSERT(stmt->GetOrder() != risse_size_max);
	if(!FirstUsedStatement)
	{
		FirstUsedStatement = LastUsedStatement = stmt;
	}
	else
	{
		if(FirstUsedStatement->GetOrder() > stmt->GetOrder())
			FirstUsedStatement = stmt;
		if(LastUsedStatement->GetOrder() < stmt->GetOrder())
			LastUsedStatement = stmt;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tSSAVariable::Dump() const
{
	return GetQualifiedName();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tSSAVariable::GetTypeComment() const
{
	if(Value)
	{
		// 定数である
		return tString(RISSE_WS("constant ")) +
			Value->AsHumanReadable();
	}
	else if(ValueType != tVariant::vtVoid)
	{
		// 型が決まっている
		return tString(RISSE_WS("always type ")) +
			tVariant::GetTypeString(ValueType);
	}
	else return tString();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tSSAVariable::GetComment() const
{
	tString comment = GetTypeComment();
	if(!comment.IsEmpty())
		comment = RISSE_WS(" // ") +Dump() + RISSE_WS(" = ") + comment;
	if(CoalescableList)
	{
		if(comment.IsEmpty())
			comment = RISSE_WS(" //");
		else
			comment += RISSE_WS(",");
		risse_char tmp[25];
		pointer_to_str(CoalescableList, tmp);
		comment += tString(RISSE_WS(" coalescable to id 0x")) + tmp;
	}
	return comment;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
