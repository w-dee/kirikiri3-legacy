//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(49117,34809,57976,16815,63634,40614,38058,11038);





//---------------------------------------------------------------------------
tRisseSSAVariableAccessMap::tRisseSSAVariableAccessMap(tRisseSSAForm * form, risse_size pos)
{
	tRisseSSAVariable * var = NULL;
	form->AddStatement(pos, ocDefineAccessMap, &var);
	Variable = var;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void tRisseSSAVariableAccessMap::SetUsed(const tRisseString & name, bool write)
{
	tMap::iterator i = Map.find(name);
	if(i == Map.end()) i = Map.insert(tMap::value_type(name, tInfo())).first;

	if(write)
		i->second.Write = true;
	else
		i->second.Read = true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAVariableAccessMap::GenerateChildWrite(tRisseSSAForm * form, risse_size pos)
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
			tRisseSSAVariable * var =
				form->GetLocalNamespace()->Read(form, pos, i->first);
			form->AddStatement(pos, ocChildWrite, NULL, Variable, var)->SetName(i->first);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSAVariableAccessMap::GenerateChildRead(tRisseSSAForm * form, risse_size pos)
{
	for(tMap::iterator i = Map.begin(); i != Map.end(); i++)
	{
		if(i->second.Write)
		{
			// 書き込みが発生している
			tRisseSSAVariable * var = NULL;
			form->AddStatement(pos, ocChildRead, &var, Variable)->SetName(i->first);
			form->GetLocalNamespace()->Write(form, pos, i->first, var);
		}
	}
}
//---------------------------------------------------------------------------


















//---------------------------------------------------------------------------
tRisseSSALocalNamespace::tRisseSSALocalNamespace()
{
	Block = NULL;
	AccessMap = NULL;
	Parent = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseSSALocalNamespace::tRisseSSALocalNamespace(const tRisseSSALocalNamespace &ref)
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
tRisseString tRisseSSALocalNamespace::GetNumberedName(
				const tRisseString & name, risse_int num)
{
	// num を文字列化
	return name + RISSE_WC('#') + tRisseString::AsString(num);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSALocalNamespace::Push()
{
	Scopes.push_back(new tScope());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSALocalNamespace::Pop()
{
	Scopes.pop_back();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSALocalNamespace::Add(const tRisseString & name, tRisseSSAVariable * where)
{
	RISSE_ASSERT(Scopes.size() != 0); // スコープが一つもない場合は何もできない(このメソッドは呼ばれてはいけない)

	// 番号を決める
	risse_int num = Block->GetForm()->GetUniqueNumber();
	tRisseString n_name = GetNumberedName(name, num);

	// 一番深いレベルのスコープにエイリアスを追加/上書きする
	tAliasMap::iterator i = Scopes.back()->AliasMap.find(name);
	if(i != Scopes.back()->AliasMap.end())
		i->second = n_name; // 上書き
	else
		Scopes.back()->AliasMap.insert(
			tAliasMap::value_type(name, n_name)); // 新規に挿入

	// 番号付きの名前を登録する
	Scopes.back()->VariableMap.insert(tVariableMap::value_type(
				n_name, where ? where : tRisseSSAVariable::GetUninitialized()));
		// where が NULL の場合は tRisseSSAVariable::GetUninitialized() が入るので注意する

	// 名前と番号付きの名前を where に設定する
	if(where)
	{
		where->SetName(name);
		where->SetNumberedName(n_name);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseSSALocalNamespace::Find(const tRisseString & name,
	bool is_num, tRisseString *n_name,
	tRisseSSAVariable *** var) const
{
	// Scopes を頭から見ていき、最初に見つかった変数を返す
	// (スコープによる変数の可視・不可視のルールに従う)
	for(tScopes::const_reverse_iterator ri = Scopes.rbegin(); ri != Scopes.rend(); ri++)
	{
		tRisseString name_to_find;
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
bool tRisseSSALocalNamespace::IsAvailable(const tRisseString & name) const
{
	if(Find(name)) return true;

	//「チェーンされた」名前空間も探す
	tRisseSSALocalNamespace * parent = Parent;
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
bool tRisseSSALocalNamespace::Delete(const tRisseString & name)
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
tRisseSSAVariable * tRisseSSALocalNamespace::MakePhiFunction(
					risse_size pos, const tRisseString & name, const tRisseString & n_name)
{
	tRisseSSAVariable ** var = NULL;
	tRisseString n_name_found;
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

	if(*var == tRisseSSAVariable::GetUninitialized())
	{
		// 値が初期化されていない(不定な)変数から読み込もうとした
		tRisseCompileExceptionClass::Throw(
			tRisseString(
				RISSE_WS_TR("attempt to read uninitialized variable '%1'"),
				name),
				Block->GetForm()->GetScriptBlock(), pos);
	}

	if(*var != NULL) return *var; // 見つかった、かつφ関数の作成の必要はない

	// 見つかったがφ関数を作成する必要がある場合
	Block->AddPhiFunctionToBlocks(pos, name, n_name_found, *var);

	return *var;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSALocalNamespace::MarkToCreatePhi()
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
tRisseSSAVariable * tRisseSSALocalNamespace::Read(tRisseSSAForm * form,
							risse_size pos, const tRisseString & name)
{
	tRisseSSAVariable * ret = MakePhiFunction(pos, name);
	if(ret)
	{
		// 文の作成
		tRisseSSAVariable * ret_var = NULL;
		tRisseSSAStatement *stmt = form->AddStatement(pos, ocReadVar, &ret_var, ret);
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
			tRisseString n_name;
			risse_size shared_nest_level = risse_size_max;
			if(Parent->AccessFromChild(name, false, AccessMap == NULL, this, &n_name, &shared_nest_level))
			{
				// この時点でn_name は番号付きの名前になっている
				// 共有変数領域からの読み込み文または
				// 親名前空間からの読み込み文を生成する
				tRisseSSAVariable * ret_var = NULL;
				tRisseSSAStatement *stmt = form->AddStatement(pos,
								shared_nest_level != risse_size_max ? ocRead : ocParentRead, &ret_var);
				stmt->SetName(shared_nest_level != risse_size_max ? n_name : name);
				if(shared_nest_level != risse_size_max) stmt->SetNestLevel(shared_nest_level);
				ret = ret_var;
				return ret;
			}
		}

		return NULL;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseSSALocalNamespace::Write(tRisseSSAForm * form, risse_size pos,
					const tRisseString & name, tRisseSSAVariable * value)
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
			tRisseString n_name = i->second; // 番号付き変数名

			// 同じスコープ内に番号付きの名前があるはず
			tVariableMap::iterator vi = (*ri)->VariableMap.find(n_name);
			RISSE_ASSERT(vi != (*ri)->VariableMap.end()); // 同じスコープ内にあるよね

			// 文の作成
			tRisseSSAVariable * ret_var = NULL;
			tRisseSSAStatement *stmt = form->AddStatement(pos, ocWriteVar, &ret_var, value);
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
		tRisseString n_name;
		risse_size shared_nest_level = risse_size_max;
		if(Parent->AccessFromChild(name, true, AccessMap == NULL, this, &n_name, &shared_nest_level))
		{
			// 親名前空間で見つかった
			// この時点でn_name は番号付きの名前になっている
			// 共有変数領域への書き込み文または
			// 親名前空間への書き込み文を生成する
			tRisseSSAStatement *stmt = form->AddStatement(pos,
							shared_nest_level != risse_size_max ? ocWrite : ocParentWrite, NULL, value);
			stmt->SetName(shared_nest_level != risse_size_max ? n_name : name);
			if(shared_nest_level != risse_size_max) stmt->SetNestLevel(shared_nest_level);
			return true;
		}
	}

	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseSSALocalNamespace::AccessFromChild(const tRisseString & name,
	bool access, bool should_share, tRisseSSALocalNamespace * child, 
	tRisseString * ret_n_name, risse_size * shared_nest_level)
{
	tRisseString n_name;
	if(Find(name, false, &n_name))
	{
		// 変数が見つかった
		if(ret_n_name) *ret_n_name = n_name;
		if(should_share) Block->GetForm()->GetFunction()->ShareVariable(n_name);
		if(shared_nest_level)
		{
			if(should_share)
			{
/*
				RisseFPrint(stderr,
					((access ? RISSE_WS("W name:") : RISSE_WS("R name:")) + name +
					 " nestlevel:" + tRisseString::AsString(risse_int64(Block->GetForm()->GetFunction()->GetNestLevel())) +
					 RISSE_WS("\n")).c_str());
*/
				*shared_nest_level = Block->GetForm()->GetFunction()->GetNestLevel();
			}
			else
			{
				*shared_nest_level = risse_size_max;
			}
		}

		// 子のAccessMap に記録 (AccessMap に記録するのは「番号なし」の名前
		if(!should_share && child->AccessMap) child->AccessMap->SetUsed(name, access);

		return true;
	}
	else
	{
		// 変数が見つからなかった
		if(!Parent) return false; // 親名前空間がない

		// この名前空間で AccessMap がないということは親空間で共有しなければならないと言うこと
		should_share = should_share || AccessMap == NULL;

		// 子がAccessMapを持っていればそれに記録
		if(!should_share && child->AccessMap) child->AccessMap->SetUsed(name, access);

		// 親名前空間内で探す
		return Parent->AccessFromChild(name, access, should_share, this, ret_n_name, shared_nest_level);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSALocalNamespace::InternalListAllVisibleVariableNumberedNames(tAliasMap & map) const
{
	// 親に再帰
	if(Parent) Parent->InternalListAllVisibleVariableNumberedNames(map);

	// Scopes を頭から見ていき、変数名を片っ端から追加する
	// 通常はスコープの深いところから見ていくところだが、ここでは
	// スコープのより深いところの alias で map を上書きするために
	// スコープの浅いところから見ていく。
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
void tRisseSSALocalNamespace::ListAllVisibleVariableNumberedNames(gc_vector<tRisseString> & dest) const
{
	// InternalListAllVisibleVariableNames を呼び出し、map に変数名を追加させる
	// map は first が番号無しの名前、secondのほうが番号付きの名前になる
	tAliasMap map;
	InternalListAllVisibleVariableNumberedNames(map);

	// map から dest に内容を移す
	dest.clear();
	dest.reserve(map.size());
	for(tAliasMap::iterator i = map.begin(); i != map.end(); i++)
		dest.push_back(i->second);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseSSALocalNamespace::ShareAllVisibleVariableNames()
{
	// すべての可視な変数名をリストアップ
	gc_vector<tRisseString> list;
	ListAllVisibleVariableNumberedNames(list);

	// すべての変数を共有としてマーク
	for(gc_vector<tRisseString>::iterator i = list.begin(); i != list.end(); i++)
	{
		RisseFPrint(stderr,
			(RISSE_WS("marking : ") + *i + RISSE_WS("\n")).c_str());

		Block->GetForm()->GetFunction()->ShareVariable(*i);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseString tRisseSSALocalNamespace::Dump() const
{
	tRisseString str;
	risse_size scope_idx = 0;
	for(tScopes::const_iterator i = Scopes.begin(); i != Scopes.end(); i++, scope_idx++)
	{
		str += RISSE_WS("=== scope #") + tRisseString::AsString((risse_int64)scope_idx) +
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
