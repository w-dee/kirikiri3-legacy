//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief コンパイラ用名前空間
//---------------------------------------------------------------------------
#include "../prec.h"

#include "risseCompiler.h"
#include "risseCompilerNS.h"
#include "risseSSAForm.h"
#include "risseSSAVariable.h"
#include "risseSSAStatement.h"
#include "risseSSABlock.h"
#include "../risseExceptionClass.h"
#include "../risseScriptBlockClass.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(49117,34809,57976,16815,63634,40614,38058,11038);





//---------------------------------------------------------------------------
tSSAVariableAccessMap::tSSAVariableAccessMap(tSSAForm * form, risse_size pos)
{
	tSSAVariable * var = NULL;
	form->AddStatement(pos, ocDefineAccessMap, &var);
	Variable = var;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void tSSAVariableAccessMap::SetUsed(const tString & name, const tString & q_name, bool write)
{
	tMap::iterator i = Map.find(name);
	if(i == Map.end()) i = Map.insert(tMap::value_type(name, tInfo())).first;

	if(write)
		i->second.Write = true;
	else
		i->second.Read = true;

	i->second.Id = q_name;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAVariableAccessMap::GenerateChildWrite(tSSAForm * form, risse_size pos)
{
	for(tMap::iterator i = Map.begin(); i != Map.end(); i++)
	{
		if(i->second.Read || i->second.Write)
		{
			// 読み込みまたは書き込みが発生している
			// 「書き込み」は「読み込み」も伴うと見なす。
			// このようにしないと、かりに(条件分岐などで)実際に書き込みが行われなかった場合に
			// 元の値を保持できない (いったん親名前空間から変数をもってきて子名前空間に書き、
			// 処理の後、子名前空間から親名前空間に書き戻すため)。
			// そもそも読み込みと書き込みを区別して記録しているのは、読み込みのみしか
			// 行われなかった変数は、親の名前空間内に書き戻す必要がないため。
			tSSAVariable * var =
				form->GetLocalNamespace()->Read(pos, i->first);
			RISSE_ASSERT(var != NULL);
			tSSAVariable *temp = NULL;
			form->AddStatement(pos, ocChildWrite, &temp, Variable, var)->SetName(i->second.Id);
			i->second.TempVariable = temp; // 一時変数を記録
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAVariableAccessMap::GenerateChildRead(tSSAForm * form, risse_size pos)
{
	for(tMap::iterator i = Map.begin(); i != Map.end(); i++)
	{
		if(i->second.Write)
		{
			// 書き込みが発生している
			tSSAVariable * var = NULL;
			form->AddStatement(pos, ocChildRead, &var, Variable)->SetName(i->second.Id);
			form->GetLocalNamespace()->Write(pos, i->first, var);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSAVariableAccessMap::GenerateEndAccessMap(tSSAForm * form, risse_size pos)
{
	// ocEndAccessMap を追加する
	tSSAStatement * stmt = form->AddStatement(pos, ocEndAccessMap, NULL, Variable);

	// 暫定実装
	// ocEndAccessMap の Used に、アクセスマップで作成された一時変数を登録する
	// (変数の生存期間がここまであることを確実にする)
	for(tMap::iterator i = Map.begin(); i != Map.end(); i++)
	{
		stmt->AddUsed(i->second.TempVariable);
	}
}
//---------------------------------------------------------------------------
















//---------------------------------------------------------------------------
tSSALocalNamespace::tSSALocalNamespace()
{
	Compiler = NULL;
	Block = NULL;
	AccessMap = NULL;
	Parent = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSALocalNamespace::tSSALocalNamespace(const tSSALocalNamespace &ref)
{
	Block = ref.Block;
	AccessMap = ref.AccessMap;
	Parent = ref.Parent;
	Scopes.reserve(ref.Scopes.size());
	for(tScopes::const_iterator i = ref.Scopes.begin();
		i != ref.Scopes.end(); i++)
	{
		Scopes.push_back(new tScope(**i));
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tSSALocalNamespace::GetNumberedName(
				const tString & name, risse_int num)
{
	// num を文字列化
	return name + RISSE_WC('#') + tString::AsString(num);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSALocalNamespace::SetBlock(tSSABlock * block)
{
	Block = block;
	Compiler = block->GetForm()->GetFunction()->GetFunctionGroup()->GetCompiler();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSALocalNamespace::SetCompiler(tCompiler * compiler)
{
	Compiler = compiler;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSALocalNamespace::Push()
{
	Scopes.push_back(new tScope());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSALocalNamespace::Pop()
{
	Scopes.pop_back();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSALocalNamespace::Add(const tString & name, tSSAVariable * where)
{
	RISSE_ASSERT(Scopes.size() != 0); // スコープが一つもない場合は何もできない(このメソッドは呼ばれてはいけない)

	// 番号を決める
	risse_int num = Compiler->GetUniqueNumber(); // ここで Compiler を使う
	tString n_name = GetNumberedName(name, num);

	// 一番深いレベルのスコープにエイリアスを追加/上書きする
	tAliasMap::iterator i = Scopes.back()->AliasMap.find(name);
	if(i != Scopes.back()->AliasMap.end())
		i->second = n_name; // 上書き
	else
		Scopes.back()->AliasMap.insert(
			tAliasMap::value_type(name, n_name)); // 新規に挿入

	// 番号付きの名前を登録する
	Scopes.back()->VariableMap.insert(tVariableMap::value_type(
				n_name, where ? where : tSSAVariable::GetUninitialized()));
		// where が NULL の場合は tSSAVariable::GetUninitialized() が入るので注意する

	// 名前と番号付きの名前を where に設定する
	if(where)
	{
		where->SetName(name);
		where->SetNumberedName(n_name);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tSSALocalNamespace::Find(const tString & name,
	bool is_num, tString *n_name,
	tSSAVariable *** var) const
{
	// Scopes を頭から見ていき、最初に見つかった変数を返す
	// (スコープによる変数の可視・不可視のルールに従う)
	for(tScopes::const_reverse_iterator ri = Scopes.rbegin(); ri != Scopes.rend(); ri++)
	{
		tString name_to_find;
		if(!is_num)
		{
			// 番号付きの名前が分からない場合はエイリアスを検索する
			tAliasMap::iterator i = (*ri)->AliasMap.find(name);
			if(i == (*ri)->AliasMap.end()) continue; // 見つからなかった

			name_to_find = i->second;

			if(n_name) *n_name = i->second;
		}
		else
		{
			name_to_find = name;
		}

		tVariableMap::iterator vi = (*ri)->VariableMap.find(name_to_find);
		if(vi != (*ri)->VariableMap.end())
		{
			// 見つかった
			if(var) *var = &(vi->second);
			return true;
		}
	}
	// 見つからなかった
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tSSALocalNamespace::IsAvailable(const tString & name) const
{
	if(Find(name)) return true;

	//「チェーンされた」名前空間も探す
	tSSALocalNamespace * parent = Parent;
	while(parent)
	{
		if(parent->Find(name)) return true;
		parent = parent->Parent;
	}

	// 見つからなかった
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tSSALocalNamespace::Delete(const tString & name)
{
	for(tScopes::reverse_iterator ri = Scopes.rbegin(); ri != Scopes.rend(); ri++)
	{
		tAliasMap::iterator i = (*ri)->AliasMap.find(name);
		if(i != (*ri)->AliasMap.end())
		{
			// 見つかった
			(*ri)->AliasMap.erase(i); // 削除

			// ここでは AliasMap から削除を行うのみとなる

			return true;
		}
	}

	// 見つからなかった
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tSSALocalNamespace::MakePhiFunction(
					risse_size pos, const tString & name, const tString & n_name)
{
	RISSE_ASSERT(Block != NULL);
	tSSAVariable ** var = NULL;
	tString n_name_found;
	bool found;
	if(n_name.IsEmpty())
	{
		found = Find(name, false, &n_name_found, &var);
	}
	else
	{
		n_name_found = n_name;
		found = Find(n_name, true, &n_name_found, &var);
	}

	if(!found) return NULL; // 見つからなかった

	if(*var == tSSAVariable::GetUninitialized())
	{
		// 値が初期化されていない(不定な)変数から読み込もうとした
		tCompileExceptionClass::Throw(
			Compiler->GetScriptBlockInstance()->GetScriptEngine(),
			tString(
				RISSE_WS_TR("attempt to read uninitialized variable '%1'"),
				name),
				Block->GetForm()->GetScriptBlockInstance(), pos);
	}

	if(*var != NULL) return *var; // 見つかった、かつφ関数の作成の必要はない

	// 見つかったがφ関数を作成する必要がある場合
	Block->AddPhiFunctionToBlocks(pos, name, n_name_found, *var);

	return *var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSALocalNamespace::MarkToCreatePhi()
{
	for(tScopes::reverse_iterator ri = Scopes.rbegin(); ri != Scopes.rend(); ri++)
	{
		for(tVariableMap::iterator i = (*ri)->VariableMap.begin();
					i != (*ri)->VariableMap.end(); i++)
			i->second = NULL;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tSSAVariable * tSSALocalNamespace::Read(risse_size pos, const tString & name)
{
	tSSAVariable * ret = MakePhiFunction(pos, name);
	if(ret)
	{
		// 文の作成
		tSSAVariable * ret_var = NULL;
		tSSAStatement *stmt = Block->AddStatement(pos, ocReadVar, &ret_var, ret);
		stmt->SetName(ret->GetNumberedName());
		ret = ret_var;
		return ret;
	}
	else
	{
		// 見つからないので親名前空間を見る
		if(Parent)
		{
			// AccessMap が NULL の場合は親名前空間内で共有する
			tString n_name;
			bool is_shared = false;;
			if(Parent->AccessFromChild(name, false, AccessMap == NULL, this, &n_name, &is_shared))
			{
				// この時点でn_name は番号付きの名前になっている
				// 共有変数領域からの読み込み文または
				// 親名前空間からの読み込み文を生成する
				tSSAVariable * ret_var = NULL;
				tSSAStatement *stmt = Block->AddStatement(pos,
								is_shared ? ocRead : ocParentRead, &ret_var);
				stmt->SetName(n_name);
				ret = ret_var;
				return ret;
			}
		}

		return NULL;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tSSALocalNamespace::Write(risse_size pos,
					const tString & name, tSSAVariable * value)
{
	// ローカル変数に上書きする
	RISSE_ASSERT(value != NULL);

	// 名前空間を探す
	for(tScopes::reverse_iterator ri = Scopes.rbegin(); ri != Scopes.rend(); ri++)
	{
		tAliasMap::iterator i = (*ri)->AliasMap.find(name);
		if(i != (*ri)->AliasMap.end())
		{
			// 見つかった
			tString n_name = i->second; // 番号付き変数名

			// 同じスコープ内に番号付きの名前があるはず
			tVariableMap::iterator vi = (*ri)->VariableMap.find(n_name);
			RISSE_ASSERT(vi != (*ri)->VariableMap.end()); // 同じスコープ内にあるよね

			// 文の作成
			tSSAVariable * ret_var = NULL;
			tSSAStatement *stmt = Block->AddStatement(pos, ocWriteVar, &ret_var, value);
			stmt->SetName(n_name);

			// 変数の上書き
			vi->second = ret_var; // 上書き

			// 番号付きの名前を ret_var に設定する
			ret_var->SetName(name);
			ret_var->SetNumberedName(n_name);

			return true; // 戻る
		}
	}

	// 見つからないので親名前空間を見る
	// AccessMap が NULL の場合は親名前空間内で共有する
	if(Parent)
	{
		tString n_name;
		bool is_shared = false;;
		if(Parent->AccessFromChild(name, true, AccessMap == NULL, this, &n_name, &is_shared))
		{
			// 親名前空間で見つかった
			// この時点でn_name は番号付きの名前になっている
			// 共有変数領域への書き込み文または
			// 親名前空間への書き込み文を生成する
			tSSAStatement *stmt = Block->AddStatement(pos,
							is_shared ? ocWrite : ocParentWrite, NULL, value);
			stmt->SetName(n_name);
			return true;
		}
	}

	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tSSALocalNamespace::AccessFromChild(const tString & name,
	bool access, bool should_share, tSSALocalNamespace * child, 
	tString * ret_n_name, bool * is_shared)
{
	tString n_name;
	if(Find(name, false, &n_name))
	{
		// 変数が見つかった
		if(should_share) if(Block) Block->GetForm()->GetFunction()->ShareVariable(n_name);
		if(is_shared)
		{
			if(should_share)
			{
/*
				FPrint(stderr,
					((access ? RISSE_WS("W name:") : RISSE_WS("R name:")) + name +
					 " nestlevel:" + tString::AsString(risse_int64(Block->GetForm()->GetFunction()->GetNestLevel())) +
					 RISSE_WS("\n")).c_str());
*/
				*is_shared = true;
			}
			else
			{
				*is_shared = false;
			}
		}

		// 子のAccessMap に記録
		if(!should_share && child->AccessMap)
		{
			n_name += RISSE_WS(":") + child->AccessMap->GetIdString();
			child->AccessMap->SetUsed(name, n_name, access);
		}

		if(ret_n_name) *ret_n_name = n_name;
		return true;
	}
	else
	{
		// 変数が見つからなかった
		if(!Parent) return false; // 親名前空間がない

		// この名前空間で AccessMap がないということは親空間で共有しなければならないと言うこと
		should_share = should_share || AccessMap == NULL;

		// 親名前空間内で探す
		bool found = Parent->AccessFromChild(name, access, should_share, this, &n_name, is_shared);

		// 子がAccessMapを持っていればそれに記録
		if(!should_share && child->AccessMap)
		{
			n_name += RISSE_WS(":") + child->AccessMap->GetIdString();
			child->AccessMap->SetUsed(name, n_name, access);
		}

		if(ret_n_name) *ret_n_name = n_name;
		return found;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSALocalNamespace::InternalListAllVisibleVariableNumberedNames(tAliasMap & map) const
{
	// 親に再帰
	if(Parent) Parent->InternalListAllVisibleVariableNumberedNames(map);

	// Scopes を頭から見ていき、変数名を片っ端から追加する
	// 通常はスコープの深いところから見ていくところだが、ここでは
	// スコープのより深いところの alias で map を上書きするために
	// スコープの浅いところから見ていく。
	// 処理の都合上 InternalShareAllVisibleVariableNames() とは処理の方向が逆なのに
	// 注意
	for(tScopes::const_iterator fi = Scopes.begin(); fi != Scopes.end(); fi++)
	{
		tAliasMap & aliasmap = (*fi)->AliasMap;
		for(tAliasMap::iterator i = aliasmap.begin(); i != aliasmap.end(); i++)
		{
			if(i->first[0] != RISSE_WC('.'))
			{
				// ただし、先頭が '.' で始まっている変数は隠し変数なのでリストアップしない
				map.insert(tAliasMap::value_type(i->first, i->second));
			}
		}
	}

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSALocalNamespace::ListAllVisibleVariableNumberedNames(tAliasMap & dest) const
{
	// InternalListAllVisibleVariableNames を呼び出す
	dest.clear();
	InternalListAllVisibleVariableNumberedNames(dest);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSALocalNamespace::InternalShareAllVisibleVariableNames(tAliasMap & names) const
{
	if(!Block) return;
		// ↑ Block がない名前空間の場合は何もしない。
		// これは今のところバインディング情報に記録されていた変数用の名前空間の場合。

	// Scopes を深いところから見ていき、見つかった変数をすべて共有する。
	// 一度共有した番号なしの変数名は names に入れ、より浅いスコープにそれが
	// みつかっても浅いスコープのそれは共有しないようにする。
	for(tScopes::const_reverse_iterator fi = Scopes.rbegin(); fi != Scopes.rend(); fi++)
	{
		tAliasMap & aliasmap = (*fi)->AliasMap;
		for(tAliasMap::iterator i = aliasmap.begin(); i != aliasmap.end(); i++)
		{
			if(i->first[0] != RISSE_WC('.'))
			{
				// 先頭が '.' で始まっている変数は隠し変数なのでリストアップしない
				if(names.find(i->first) == names.end())
				{
					// 一度共有をした変数はもう共有しない
					names.insert(tAliasMap::value_type(i->first, i->second));

					// 関数に共有を指示
					Block->GetForm()->GetFunction()->ShareVariable(i->second);
				}
			}
		}
	}

	// 親に再帰
	if(Parent) Parent->InternalShareAllVisibleVariableNames(names);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tSSALocalNamespace::ShareAllVisibleVariableNames() const
{
	tAliasMap names;
	InternalShareAllVisibleVariableNames(names);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tSSALocalNamespace::Dump() const
{
	tString str;
	risse_size scope_idx = 0;
	for(tScopes::const_iterator i = Scopes.begin(); i != Scopes.end(); i++, scope_idx++)
	{
		str += RISSE_WS("=== scope #") + tString::AsString((risse_int64)scope_idx) +
			RISSE_WS(" ===\n");
		const tVariableMap & vmap = (*i)->VariableMap;
		const tAliasMap & amap = (*i)->AliasMap;

		str += RISSE_WS("- - alias - -\n");
		for(tAliasMap::const_iterator ai = amap.begin(); ai != amap.end(); ai++)
		{
			str += ai->first + RISSE_WS(" => ") + ai->second +
				RISSE_WS("\n");
		}

		str += RISSE_WS("- - numbered - -\n");
		for(tVariableMap::const_iterator vi = vmap.begin(); vi != vmap.end(); vi++)
		{
			str += vi->first + RISSE_WS("\n");
		}
	}
	return str;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risse
