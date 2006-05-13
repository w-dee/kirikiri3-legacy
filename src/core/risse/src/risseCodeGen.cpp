//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief バイトコードジェネレータ
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseCodeGen.h"


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(52364,51758,14226,19534,54934,29340,32493,12680);

//---------------------------------------------------------------------------
tRisseLocalNamespace::tRisseLocalNamespace()
{
	Push(); // 最初の名前空間を push しておく
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseLocalNamespace::Push()
{
	Scopes.push_back(new tMap());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseLocalNamespace::Pop()
{
	Scopes.pop_back();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseLocalNamespace::Add(const tRisseString & name, tRisseSSAVariable * where)
{
	for(tScopes::reverse_iterator ri = Scopes.rbegin(); ri != Scopes.rend(); ri++)
	{
		tMap::iterator i = (*ri)->find(name);
		if(i != (*ri)->end())
		{
			// 見つかった
			i->second = where; // 上書き
			return;
		}
	}

	// 見つからなかったので、一番深いレベルのスコープに追加する
	Scopes.back()->insert(std::pair<tRisseString, tRisseSSAVariable *>(name, where));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseLocalNamespace::Find(const tRisseString & name) const
{
	// Scopes を頭から見ていき、最初に見つかった変数を返す
	// (スコープによる変数の可視・不可視のルールに従う)
	for(tScopes::const_reverse_iterator ri = Scopes.rbegin(); ri != Scopes.rend(); ri++)
	{
		tMap::iterator i = (*ri)->find(name);
		if(i != (*ri)->end())
		{
			// 見つかった
			return i->second;
		}
	}
	// 見つからなかった
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseLocalNamespace::Delete(const tRisseString & name)
{
	for(tScopes::reverse_iterator ri = Scopes.rbegin(); ri != Scopes.rend(); ri++)
	{
		tMap::iterator i = (*ri)->find(name);
		if(i != (*ri)->end())
		{
			// 見つかった
			(*ri)->erase(i); // 削除
			return true;
		}
	}

	// 見つからなかった
	return false;
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tRisseSSAVariable::tRisseSSAVariable(tRisseSSAForm * form, 
	tRisseSSAStatement *stmt, const tRisseString & name)
{
	// フィールドの初期化
	Form = form;
	OriginalName = name;
	Declared = stmt;
	Value = NULL;
	ValueType = tRisseVariant::vtVoid;

	// この変数が定義された文の登録
	if(Declared) Declared->SetDeclared(this);

	// 名前を生成する
	// 名前の後にはドットに続いて通し番号がつく。
	// 一時変数の場合は名前として ".tmp" が用いられる

	// 通し番号の準備
	risse_char uniq[40];
	Risse_int_to_str(form->GetUniqueNumber(), uniq);

	if(name.IsEmpty())
	{
		// 一時変数
		Name = tRisseString(RISSE_WS("@tmp_")) + uniq;
	}
	else
	{
		// 普通の変数
		Name = name + RISSE_WC('_') + uniq;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseSSAVariable::Dump() const
{
	return Name;
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
tRisseSSAStatement::tRisseSSAStatement(tRisseSSAForm * form,
	risse_size position, tRisseOpCode code)
{
	// フィールドの初期化
	Form = form;
	Position = position;
	Code = code;
	Block = NULL;
	Pred = NULL;
	Succ = NULL;
	Declared = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseSSAStatement::Dump() const
{
	switch(Code)
	{
	case ocNoOperation:			//!< なにもしない
		return RISSE_WS("nop");
	default:
		{
			tRisseString ret;

			// 変数の宣言
			if(Declared)
				ret += Declared->Dump() + RISSE_WS(" = "); // この文で宣言された変数がある

			// 使用している引数の最初の引数はメッセージの送り先なので
			// オペレーションコードよりも前に置く
			if(Used.size() != 0)
				ret += (*Used.begin())->Dump() + RISSE_WC('.');

			// オペレーションコード
			ret += tRisseString(RisseOpCodeNames[Code]);

			// 使用している引数
			if(Used.size() >= 2)
			{
				tRisseString used;
				for(gc_vector<tRisseSSAVariable*>::const_iterator i = Used.begin() + 1;
						i != Used.end(); i++)
				{
					if(!used.IsEmpty()) used += RISSE_WS(", ");
					used += (*i)->Dump();
				}
				ret += RISSE_WS("(") + used +
								RISSE_WS(")");
			}
			else
			{
				ret += RISSE_WS("()");
			}

			// 変数の宣言に関してコメントがあればそれを追加
			if(Declared)
			{
				tRisseString comment = Declared->GetTypeComment();
				if(!comment.IsEmpty())
					ret += RISSE_WS(" // ") + Declared->Dump() + RISSE_WS(" = ") + comment;
			}
			return ret;
		}
	}
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tRisseSSAVariable * tRisseSSABlock::AddConstantValueStatement(
										risse_size pos,
										const tRisseVariant & val)
{
	// 文の作成
	tRisseSSAStatement * stmt =
		new tRisseSSAStatement(Form, pos, ocAssignConstant);
	// 変数の作成
	tRisseSSAVariable * var = new tRisseSSAVariable(Form, stmt);
	var->SetValue(new tRisseVariant(val));
	// 文の追加
	AddStatement(stmt);
	// 戻る
	return var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseSSABlock::Dump() const
{
	if(!FirstStatement)
	{
		// 一つも文を含んでいない？？
		return RISSE_WS("This SSA block does not contain any statements\n");
	}

	tRisseString ret;
	tRisseSSAStatement * stmt = FirstStatement;
	do 
	{
		ret += stmt->Dump() + RISSE_WS("\n");
		stmt = stmt->GetSucc();
	} while(stmt != NULL);

	return ret;
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tRisseSSAForm::tRisseSSAForm(tRisseScriptBlockBase * scriptblock, tRisseASTNode * root)
{
	ScriptBlock = scriptblock;
	Root = root;
	UniqueNumber = 0;
	LocalNamespace = new tRisseLocalNamespace();
	EntryBlock = NULL;
	CurrentBlock = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAForm::Generate()
{
	// AST をたどり、それに対応する SSA 形式を作成する

	// エントリー位置のブロックを生成する
	EntryBlock = new tRisseSSABlock(this);
	CurrentBlock = EntryBlock;

	// ルートノードを処理する
	Root->GenerateSSA(ScriptBlock, this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseSSAForm::Dump() const
{
	// この form から到達可能なブロックをすべてダンプする
	return EntryBlock->Dump();
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace Risse
