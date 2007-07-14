/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
tArrayClass::tArrayClass(tScriptEngine * engine) :
	tClassBase(ss_Array, engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tArrayClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

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
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tArrayClass::ovulate()
{
	return tVariant(new tArrayInstance());
}
//---------------------------------------------------------------------------

} /* namespace Risse */

