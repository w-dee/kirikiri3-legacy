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
tRisseSSAVariable::tRisseSSAVariable(tRisseSSAForm * form, 
	tRisseSSAStatement *stmt, const tRisseString & name)
{
	// フィールドの初期化
	Form = form;
	Declared = stmt;
	FirstUsedStatement = NULL;
	LastUsedStatement = NULL;
	Value = NULL;
	ValueType = tRisseVariant::vtVoid;
	Mark = NULL;

	// この変数が定義された文の登録
	if(Declared) Declared->SetDeclared(this);

	// 名前と番号を設定する
	SetName(name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAVariable::SetName(const tRisseString & name)
{
	// 名前を設定する
	Name = name;

	// 通し番号の準備
	Version = Form->GetFunction()->GetFunctionGroup()->GetCompiler()->GetUniqueNumber();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseSSAVariable::GetQualifiedName() const
{
	if(Name.IsEmpty())
	{
		// 一時変数の場合は _tmp@ の次にバージョン文字列
		return tRisseString(RISSE_WS("_tmp@")) + tRisseString::AsString(Version);
	}
	else
	{
		// NumberedName がある場合はそれを、Nameを使う
		tRisseString n;
		if(!NumberedName.IsEmpty()) n = NumberedName; else n = Name;
		// 普通の変数の場合は 変数名@バージョン文字列
		return n + RISSE_WC('@') + tRisseString::AsString(Version);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAVariable::DeleteUsed(tRisseSSAStatement * stmt)
{
	gc_vector<tRisseSSAStatement*>::iterator it =
		std::find(Used.begin(), Used.end(), stmt);
	if(it != Used.end()) Used.erase(it);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseSSAVariable::GenerateFuncCall(risse_size pos, const tRisseString & name,
			tRisseSSAVariable * param1,
			tRisseSSAVariable * param2,
			tRisseSSAVariable * param3)
{
	tRisseSSAVariable * func_var = NULL;

	if(!name.IsEmpty())
	{
		// 関数名が指定されている場合

		// メソッド名を置く
		tRisseSSAVariable * method_name_var =
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
	tRisseSSAVariable * ret_var = NULL;
	tRisseSSAStatement * call_stmt =
		Form->AddStatement(pos, ocFuncCall, &ret_var, func_var, param1, param2, param3);
	call_stmt->SetFuncExpandFlags(0);

	// 戻る
	return ret_var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAVariable::DeleteDeadStatements()
{
	for(gc_vector<tRisseSSAStatement*>::iterator it = Used.begin(); it != Used.end();)
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
void tRisseSSAVariable::AnalyzeVariableStatementLiveness(tRisseSSAStatement * stmt)
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
tRisseString tRisseSSAVariable::Dump() const
{
	return GetQualifiedName();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseSSAVariable::GetTypeComment() const
{
	if(Value)
	{
		// 定数である
		return tRisseString(RISSE_WS("constant ")) +
			Value->AsHumanReadable();
	}
	else if(ValueType != tRisseVariant::vtVoid)
	{
		// 型が決まっている
		return tRisseString(RISSE_WS("always type ")) +
			tRisseVariant::GetTypeString(ValueType);
	}
	else return tRisseString();
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risse
