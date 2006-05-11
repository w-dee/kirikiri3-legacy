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
RISSE_DEFINE_SOURCE_ID();

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
tRisseSSAVariable * tRisseLocalNamespace::Find(const tRisseString & name)
{
	// Scopes を頭から見ていき、最初に見つかった変数を返す
	// (スコープによる変数の可視・不可視のルールに従う)
	for(tScopes::reverse_iterator ri = Scopes.rbegin(); ri != Scopes.rend(); ri++)
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
	const tRisseVariant & name = tRisseVariant())
{
	// フィールドの初期化
	Form = form;
	Declared = NULL;
	Value = NULL;
	ValueType = tRisseVariant::vtVoid;

	// 名前を生成する
	// 名前は一時変数は . で始める。
	// そのあと、name 引数で渡された変数名が続き (一時変数の場合は "tmp")、
	// ドットに続いて通し番号がつく。

	// 通し番号の準備
	risse_char uniq[40];
	Risse_int_to_str(form->GetUniqueNumber(), uniq);

	if(name.IsEmpty())
	{
		// 一時変数
		Name = tRisseString(RISSE_WS(".tmp.")) + uniq;
	}
	else
	{
		// 普通の変数
		Name = name + RISSE_WC('.') + uniq;
	}
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tRisseSSAForm::tRisseSSAForm(tRisseASTNode * root)
{
	UniqueNumber = 1;
	Root = root;
	LocalNamespace = new tRisseLocalNameSpace();
	EntryBlock = NULL;
	CurrentBlock = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAForm::Generate()
{
	// AST をたどり、それに対応する SSA 形式を作成する

	// エントリー位置のブロックを生成する
	EntryBlock = new tRisseSSABlock();
	CurrentBlock = EntryBlock;

	// ルートノードを処理する
	Root->GenerateSSA(this);
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
} // namespace Risse
