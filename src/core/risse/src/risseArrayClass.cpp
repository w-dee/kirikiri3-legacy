/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Array" クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "risseArrayClass.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "risseScriptEngine.h"

/*
	Risseスクリプトから見える"Array" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(65360,34010,1527,19914,27817,35057,17111,22724);
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
void tArrayInstance::construct()
{
	// default メンバを追加 (デフォルトではvoid)
	// TODO: default ではなくて missing メソッドみたいなのを実装しようよ
	RegisterNormalMember(ss_default, tVariant::GetVoidObject());

	// filler メンバを追加 (デフォルトではvoid)
	RegisterNormalMember(ss_filler, tVariant::GetVoidObject());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tArrayInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();

	// 引数を元に配列を構成する
	// ここの動作は push と同じ
	for(risse_size i = 0; i < info.args.GetArgumentCount(); i++)
		Array.push_back(info.args[i]);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tArrayInstance::iget(risse_offset ofs_index)
{
	volatile tSynchronizer sync(this); // sync

	if(ofs_index < 0) ofs_index += Array.size(); // 折り返す

	risse_size index = static_cast<risse_size>(ofs_index);
	if(ofs_index < 0 || index >= Array.size())
	{
		// 範囲外
		// default の値を得て、それを返す
		return GetPropertyDirect(ss_default);
	}

	// 値を返す
	return Array[index];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tArrayInstance::iset(const tVariant & value, risse_offset ofs_index)
{
	volatile tSynchronizer sync(this); // sync

	if(ofs_index < 0) ofs_index += Array.size(); // 折り返す

	if(ofs_index < 0)  { /* それでもまだ負: TOOD: out of bound 例外 */ return; }

	risse_size index = static_cast<risse_size>(ofs_index);
	if(index >= Array.size())
	{
		// 配列を拡張する
		// もし、拡張する際に値を埋める必要があるならば
		// 値を埋める
		if(index > Array.size())
		{
			// filler で埋める
			Array.resize(index+1, GetPropertyDirect(ss_filler));

			// 値の上書き
			Array[index] = value;
		}
		else /* if index == Array.size() */
		{
			Array.push_back(value);
		}
	}
	else
	{
		// 既存の値の上書き
		Array[index] = value;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tArrayInstance::push(const tMethodArgument & args)
{
	volatile tSynchronizer sync(this); // sync

	for(risse_size i = 0; i < args.GetArgumentCount(); i++)
		Array.push_back(args[i]);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tArrayInstance::pop()
{
	volatile tSynchronizer sync(this); // sync

	if(Array.size() > 0)
	{
		tVariant val;
		val = Array.back();
		Array.pop_back();
		return val;
	}
	else
	{
		return GetPropertyDirect(ss_default);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tArrayInstance::unshift(const tMethodArgument & args)
{
	volatile tSynchronizer sync(this); // sync

	risse_size i = args.GetArgumentCount();
	while(i--) Array.push_front(args[i]);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tArrayInstance::shift()
{
	volatile tSynchronizer sync(this); // sync

	if(Array.size() > 0)
	{
		tVariant val;
		val = Array.front();
		Array.pop_front();
		return val;
	}
	else
	{
		return GetPropertyDirect(ss_default);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tArrayInstance::get_length() const
{
	volatile tSynchronizer sync(this); // sync

	return Array.size();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tArrayInstance::set_length(size_t new_size)
{
	volatile tSynchronizer sync(this); // sync

	if(Array.size() < new_size)
	{
		// 拡張
		Array.resize(new_size, GetPropertyDirect(ss_filler));
	}
	else
	{
		// 縮小
		Array.resize(new_size);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tArrayInstance::join(const tMethodArgument & args)
{
	// 文字列を、指定したデリミタで連結して返す
	volatile tSynchronizer sync(this); // sync

	tString ret;
	for(tArray::iterator i = Array.begin(); i != Array.end(); i++)
	{
		if(i != Array.begin() && args.HasArgument(0))
			ret += args[0].operator tString();
		ret += (*i).operator tString();
	}
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tArrayInstance::remove(const tVariant & value, const tMethodArgument & args)
{
	// 配列中から value を === 演算子で探し、見つかれば削除し、valueを返す
	// 見つからなければ default の値を返す
	// 第２引数 = すべて削除するか (デフォルト = true)
	bool remove_all = args.HasArgument(1) ? args[1].operator bool() : true;
	bool any_removed = false;

	volatile tSynchronizer sync(this); // sync

	for(tArray::iterator i = Array.begin(); i != Array.end(); /**/)
	{
		if(i->DiscEqual(value))
		{
			i = Array.erase(i);
			any_removed = true;
			if(!remove_all) break;
		}
		else
		{
			i++;
		}
	}

	return any_removed ? value : GetPropertyDirect(ss_default);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tArrayInstance::erase(risse_offset ofs_index)
{
	// 配列中の idx にある要素を削除する
	volatile tSynchronizer sync(this); // sync

	if(ofs_index < 0) ofs_index += Array.size(); // 折り返す

	risse_size index = static_cast<risse_size>(ofs_index);
	if(index < Array.size() && ofs_index >= 0)
	{
		// 範囲内であればそこの要素を削除し、そこにあった値を返す
		tVariant val = Array[index];
		Array.erase(Array.begin() + index);
		return val;
	}
	else
	{
		// 範囲外ならば default を返す
		return GetPropertyDirect(ss_default);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tArrayInstance::has(const tVariant & value)
{
	// 配列中に value があるかどうかを返す
	// 比較には === 演算子を用いる
	volatile tSynchronizer sync(this); // sync

	for(tArray::iterator i = Array.begin(); i != Array.end(); i++)
	{
		if(i->DiscEqual(value))
			return true;
	}

	return false;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tArrayClass, ss_Array, engine->ObjectClass)
	BindFunction(this, ss_ovulate, &tArrayClass::ovulate);
	BindFunction(this, ss_construct, &tArrayInstance::construct);
	BindFunction(this, ss_initialize, &tArrayInstance::initialize);
	BindFunction(this, mnIGet, &tArrayInstance::iget);
	BindFunction(this, mnISet, &tArrayInstance::iset);
	BindFunction(this, ss_push, &tArrayInstance::push);
	BindFunction(this, ss_pop, &tArrayInstance::pop);
	BindFunction(this, ss_unshift, &tArrayInstance::unshift);
	BindFunction(this, ss_shift, &tArrayInstance::shift);
	BindProperty(this, ss_length, &tArrayInstance::get_length, &tArrayInstance::set_length);
	BindFunction(this, ss_join, &tArrayInstance::join);
	BindFunction(this, ss_remove, &tArrayInstance::remove);
	BindFunction(this, ss_erase, &tArrayInstance::erase);
	BindFunction(this, ss_has, &tArrayInstance::has);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tEnumerableIterator::tEnumerableIterator(const tVariant & array) : Array(array)
{
	Count = (risse_int64)Array.GetPropertyDirect(Array.GetScriptEngine(), ss_length);
	Index = (risse_size)(risse_offset)-1;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tEnumerableIterator::Next()
{
	Index ++;
	if(Index >= Count) return false;
	Value = Array.Invoke(Array.GetScriptEngine(), mnIGet, tVariant((risse_int64)Index));
	return true;
}
//---------------------------------------------------------------------------






} /* namespace Risse */

