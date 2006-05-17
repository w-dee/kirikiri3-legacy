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
tRisseLocalNamespace::tRisseLocalNamespace(const tRisseLocalNamespace &ref)
{
	Scopes.reserve(ref.Scopes.size());
	for(gc_vector<tMap *>::const_iterator i = ref.Scopes.begin();
		i != ref.Scopes.end(); i++)
	{
		Scopes.push_back(new tMap(**i));
	}
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
bool tRisseLocalNamespace::Find(const tRisseString & name, tRisseSSAVariable ** var) const
{
	// Scopes を頭から見ていき、最初に見つかった変数を返す
	// (スコープによる変数の可視・不可視のルールに従う)
	for(tScopes::const_reverse_iterator ri = Scopes.rbegin(); ri != Scopes.rend(); ri++)
	{
		tMap::iterator i = (*ri)->find(name);
		if(i != (*ri)->end())
		{
			// 見つかった
			if(var) *var = i->second;
			return true;
		}
	}
	// 見つからなかった
	return false;
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
tRisseSSAVariable * tRisseLocalNamespace::MakePhiFunction(
						tRisseSSAForm * form, risse_size pos, const tRisseString & name)
{
	for(tScopes::reverse_iterator ri = Scopes.rbegin(); ri != Scopes.rend(); ri++)
	{
		tMap::iterator i = (*ri)->find(name);
		if(i != (*ri)->end())
		{
			// 見つかった
			if(i->second != NULL) return i->second; // 見つかったがφ関数を作成する必要はない
			// 見つかったがφ関数を作成する必要がある
			form->GetCurrentBlock()->AddPhiFunction(pos, name, i->second);
			return i->second;
		}
	}

	// 見つからなかった
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseLocalNamespace::MarkToCreatePhi()
{
	for(tScopes::reverse_iterator ri = Scopes.rbegin(); ri != Scopes.rend(); ri++)
	{
		for(tMap::iterator i = (*ri)->begin(); i != (*ri)->end(); i++)
			i->second = NULL;
	}
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
	TrueBranch = FalseBranch = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseSSAStatement::Dump() const
{
	switch(Code)
	{
	case ocNoOperation:			// なにもしない
		return RISSE_WS("nop");
	case ocPhi:		// φ関数
		{
			tRisseString ret;

			RISSE_ASSERT(Declared != NULL);
			ret += Declared->Dump() + RISSE_WS(" = PHI(");

			// φ関数の引数を追加
			tRisseString used;
			for(gc_vector<tRisseSSAVariable*>::const_iterator i = Used.begin();
					i != Used.end(); i++)
			{
				if(!used.IsEmpty()) used += RISSE_WS(", ");
				used += (*i)->Dump();
			}

			ret += used + RISSE_WS(")");

			// 変数のコメントを追加
			tRisseString comment = Declared->GetTypeComment();
			if(!comment.IsEmpty())
				ret += RISSE_WS(" // ") + Declared->Dump() + RISSE_WS(" = ") + comment;

			return ret;
		}

	case ocJump:
		{
			RISSE_ASSERT(JumpTarget != NULL);
			return RISSE_WS("goto ") + JumpTarget->GetName();
		}

	case ocBranch:
		{
			RISSE_ASSERT(TrueBranch != NULL);
			RISSE_ASSERT(FalseBranch != NULL);
			RISSE_ASSERT(Used.size() == 1);
			return
					RISSE_WS("if ") + (*Used.begin())->Dump() + 
					RISSE_WS(" then ") + TrueBranch->GetName() +
					RISSE_WS(" else ") + FalseBranch->GetName();
		}

	default:
		{
			tRisseString ret;

			// 変数の宣言
			if(Declared)
				ret += Declared->Dump() + RISSE_WS(" = "); // この文で宣言された変数がある

			if(Code == ocAssign)
			{
				// 単純代入
				RISSE_ASSERT(Used.size() == 1);
				ret += (*Used.begin())->Dump();
			}
			else
			{
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
tRisseSSABlock::tRisseSSABlock(tRisseSSAForm * form, const tRisseString & name)
{
	Form = form;
	FirstStatement = LastStatement = NULL;
	LocalNamespace = NULL;
	InDump = Dumped = false;

	// 通し番号の準備
	risse_char uniq[40];
	Risse_int_to_str(form->GetUniqueNumber(), uniq);

	Name = name + RISSE_WC('_') + uniq;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSABlock::AddPhiFunction(risse_size pos, const tRisseString & name,
	tRisseSSAVariable *& var)
{
	// φ関数を追加する
	tRisseSSAStatement * stmt = new tRisseSSAStatement(Form, pos, ocPhi);

	// 戻りの変数を作成する
	// この AddPhiFunction メソッドを呼び出す tRisseLocalNamespace::MakePhiFunction
	// は、見つかった変数のmap のsecondをvar引数に渡してくる。このため、
	// var に代入した時点でこの変数の新しいSSA変数が可視になる。
	var = new tRisseSSAVariable(Form, stmt, name);

	// φ関数は必ずブロックの先頭に追加される
	if(!FirstStatement)
	{
		// 最初の文
		FirstStatement = LastStatement = stmt;
	}
	else
	{
		// ２つ目以降の文
		FirstStatement->SetPred(stmt);
		stmt->SetSucc(FirstStatement);
		FirstStatement = stmt;
	}

	// 関数の引数を調べる
	// 関数の引数は、直前のブロックのローカル名前空間のスナップショットから
	// 変数名を探すことで得る
	for(gc_vector<tRisseSSABlock *>::iterator i = Pred.begin(); i != Pred.end(); i ++)
	{
		RISSE_ASSERT((*i)->LocalNamespace != NULL);
		tRisseSSAVariable * found_var =
			(*i)->LocalNamespace->MakePhiFunction(Form, pos, var->GetOriginalName());
		if(!found_var)
		{
			// エラー
		}
		stmt->AddUsed(found_var);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSABlock::AddPred(tRisseSSABlock * block)
{
	// 直前の基本ブロックを追加する
	Pred.push_back(block);

	// block の直後基本ブロックとして this を追加する
	block->AddSucc(this);

	// 既存のφ関数は、すべて再調整しなければならない
	tRisseSSAStatement *stmt = FirstStatement;
	while(stmt)
	{
		if(stmt->GetCode() != ocPhi) break;
		RISSE_ASSERT(!stmt->GetDeclared()->GetName().IsEmpty());
		RISSE_ASSERT(block->LocalNamespace != NULL);

		tRisseSSAVariable * decl_var = stmt->GetDeclared();

		tRisseSSAVariable * var =
			block->LocalNamespace->MakePhiFunction(Form, stmt->GetPosition(), decl_var->GetOriginalName());
		if(!var)
		{
			// エラー
		}
		stmt->AddUsed(var);

		stmt = stmt->GetSucc();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSABlock::AddSucc(tRisseSSABlock * block)
{
	Succ.push_back(block);
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
void tRisseSSABlock::TakeLocalNamespaceSnapshot(tRisseLocalNamespace * ref)
{
	LocalNamespace = new tRisseLocalNamespace(*ref);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSABlock::ClearDumpFlags() const
{
	InDump = true;
	Dumped = false;
	for(gc_vector<tRisseSSABlock *>::const_iterator i = Succ.begin();
									i != Succ.end(); i ++)
	{
		if(!(*i)->InDump)
			(*i)->ClearDumpFlags();
	}
	InDump = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseSSABlock::Dump() const
{
	tRisseString ret;

	ret += Name + RISSE_WS(":\n");

	InDump = true; // 再入しないように
	Dumped = true; // ２回以上ダンプしないように

	try
	{
		if(!FirstStatement)
		{
			// 一つも文を含んでいない？？
			ret += RISSE_WS("This SSA basic block does not contain any statements\n\n");
		}
		else
		{
			// すべての文をダンプ
			tRisseSSAStatement * stmt = FirstStatement;
			do 
			{
				ret += stmt->Dump() + RISSE_WS("\n");
				stmt = stmt->GetSucc();
			} while(stmt != NULL);

			ret += RISSE_WS("\n");
		}

		// 直後のブロックをダンプ
		for(gc_vector<tRisseSSABlock *>::const_iterator i = Succ.begin();
										i != Succ.end(); i ++)
		{
			if(!(*i)->InDump && !(*i)->Dumped)
				ret += (*i)->Dump();
		}
	}
	catch(...)
	{
		InDump = false;
		throw;
	}
	InDump = false;

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

	// エントリー位置の基本ブロックを生成する
	EntryBlock = new tRisseSSABlock(this, RISSE_WS("entry"));
	CurrentBlock = EntryBlock;

	// ルートノードを処理する
	Root->GenerateSSA(ScriptBlock, this);
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
	new_block->AddPred(pred?pred:CurrentBlock);

	// 新しい「現在の」基本ブロックを設定し、それを返す
	CurrentBlock = new_block;
	return CurrentBlock;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseSSAForm::Dump() const
{
	// この form から到達可能な基本ブロックをすべてダンプする
	return EntryBlock->Dump();
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace Risse
