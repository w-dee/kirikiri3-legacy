//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

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
	ValueState = vsUnknown;
	Mark = NULL;
	AssignedRegister = risse_size_max;

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
void tSSAVariable::SetInterferenceWithAll(tSSAVariable * with)
{
	SetInterferenceWith(with);
	for(tInterferenceEdgeMap::iterator ii = with->InterferenceEdgeMap->begin();
		ii != with->InterferenceEdgeMap->end(); ii++)
	{
		if(ii->first != this && ii->first != with)
			const_cast<tSSAVariable *>(ii->first)->SetInterferenceWith(this);
	}
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

		// InterferenceEdgeMap の合併
		if(var->InterferenceEdgeMap)
		{
			if(InterferenceEdgeMap == NULL) InterferenceEdgeMap = new tInterferenceEdgeMap();
			for(tInterferenceEdgeMap::iterator ii = var->InterferenceEdgeMap->begin();
				ii != var->InterferenceEdgeMap->end(); ii++)
			{
				// ii を this に付け替え
				if(ii->first != this)
					InterferenceEdgeMap->insert(tInterferenceEdgeMap::value_type(ii->first, risse_size_max));
				// ii->first のうち、var を向いていたエッジを this を向くように修正
				tInterferenceEdgeMap * ii_map = ii->first->InterferenceEdgeMap;
				RISSE_ASSERT(ii_map != NULL);
				tInterferenceEdgeMap::iterator var_edge = ii_map->find(var);
				RISSE_ASSERT(var_edge != ii_map->end());
				ii_map->erase(var_edge);
				ii_map->insert(tInterferenceEdgeMap::value_type(this, risse_size_max));
			}
		}
	}

	RISSE_ASSERT(CoalescableList == NULL);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAVariable::AssignRegister(gc_vector<void*> & assign_work)
{
	// TODO: ここの部分の高速化

	if(AssignedRegister != risse_size_max) return; // 既に割り当たっている

	if(!InterferenceEdgeMap)
	{
//wxFprintf(stderr, wxT("no interference map for %s\n"), GetQualifiedName().AsWxString().c_str());
		AssignedRegister = 0;
		return; // 干渉マップがない; そもそも干渉がないということなので遠慮無く 0 番を割り当てる
	}

	// InterferenceEdgeMap に列挙されている各変数とは異なるレジスタを割り当てる。
	// 具体的には、InterferenceEdgeMap に列挙されている変数のうち、
	// レジスタが既に割り当てられている変数のレジスタを除くレジスタのどれかを割り当てる。

//wxFprintf(stderr, wxT("with %s"), GetQualifiedName().AsWxString().c_str());
	// 今までに割り当たっているうちの最大の番号のレジスタを得る
	risse_size max_assigned_reg_num = risse_size_max;
	for(tInterferenceEdgeMap::iterator i = InterferenceEdgeMap->begin();
		i != InterferenceEdgeMap->end(); i++)
	{
		risse_size assigned_reg_num = i->first->AssignedRegister;
		if(assigned_reg_num != risse_size_max)
		{
			// assign_work の該当部分を this に設定
			if(assign_work.size() <= assigned_reg_num)
				assign_work.resize(assigned_reg_num + 1);
			assign_work[assigned_reg_num] = this;
//wxFprintf(stderr, wxT(", %s is at %d"), i->first->GetQualifiedName().AsWxString().c_str(), (int)assigned_reg_num);

			if(max_assigned_reg_num == risse_size_max || max_assigned_reg_num < assigned_reg_num)
				max_assigned_reg_num = assigned_reg_num;
		}
	}
//wxFprintf(stderr, wxT("\n"));

	// assign_work を見ていき、最初に this で無かったレジスタを割り当てる。
	if(max_assigned_reg_num != risse_size_max)
	{
		for(risse_size i = 0; i <= max_assigned_reg_num; i++)
		{
			if(assign_work[i] != this)
			{
				AssignedRegister = i;
				return;
			}
		}
	}

	// 最後まで探したけど無かったというときは max_assigned_reg_num + 1 を割り当てる
	AssignedRegister = max_assigned_reg_num + 1;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAVariable::SuggestValue(tVariant::tType type)
{
	switch(type)
	{
	case tVariant::vtVoid:		if(ValueState <= vsConstant)
								{ Value.SetTypeTag(tVariant::vtVoid); ValueState = vsConstant; }
								return;
	case tVariant::vtInteger:	break;
	case tVariant::vtReal:		break;
	case tVariant::vtNull:		if(ValueState <= vsConstant)
								{ Value.SetTypeTag(tVariant::vtNull); ValueState = vsConstant; }
								return;
	case tVariant::vtBoolean:	break;
	case tVariant::vtString:	break;
	case tVariant::vtOctet:		break;
	case tVariant::vtData:		break;
	case tVariant::vtObject:	break;
	}

	if(ValueState <= vsTypeConstant)
	{
		Value.SetTypeTag(type);
		ValueState = vsTypeConstant;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
int tSSAVariable::GetValueAsBoolean() const
{
	if(ValueState == vsConstant)
	{
		// Used[0] が定数ならばどっちになるかわかる
		if((bool)Value)
			return 1;
		else
			return 0;
	}
	else if(ValueState == vsTypeConstant)
	{
		// タイプによっては必ずfalseとして評価される物がある
		switch(Value.GetType())
		{
		case tVariant::vtVoid:
		case tVariant::vtNull:
			// この二つは必ず偽になる
			return 0;

		default:
			// どっちになるかわからない
			return 2;
		}
	}
	else if(ValueState == vsVarying)
	{
		return 2; // どっちになるかわからない
	}

	return 3; // 未定義
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant::tGuessType tSSAVariable::GetGuessType() const
{
	switch(ValueState)
	{
	case vsUnknown:
		return tVariant::gtAny;
	case vsTypeConstant:
	case vsConstant:
		return (tVariant::tGuessType) Value.GetType();
	case vsVarying:
		return tVariant::gtAny;
	}
	RISSE_ASSERT(!"unhandled type here!");
	return tVariant::gtAny;
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
tString tSSAVariable::Dump() const
{
	return GetQualifiedName();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tSSAVariable::GetTypeComment() const
{
	tString ret;
	switch(ValueState)
	{
	case vsUnknown:
		break;
	case vsConstant:
		ret += tString(RISSE_WS("constant ")) + Value.AsHumanReadable();
		break;
	case vsTypeConstant:
		ret += tString(RISSE_WS("constant type ")) + Value.GetTypeString();
		break;
	case vsVarying:
		ret += tString(RISSE_WS("varying"));
		break;
	}

	return ret;
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
	if(AssignedRegister != risse_size_max)
	{
		if(comment.IsEmpty())
			comment = RISSE_WS(" //");
		else
			comment += RISSE_WS(",");
		comment += tString(RISSE_WS(" register %")) + tString::AsString((risse_int64)AssignedRegister);
	}
	return comment;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risse
