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
void tRisseArrayInstance::construct()
{
	// default メンバを追加 (デフォルトではvoid)
	RegisterNormalMember(ss_default, tRisseVariant::GetVoidObject());

	// filler メンバを追加 (デフォルトではvoid)
	RegisterNormalMember(ss_filler, tRisseVariant::GetVoidObject());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseArrayInstance::initialize(const tRisseNativeCallInfo &info)
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
tRisseVariant tRisseArrayInstance::iget(risse_offset ofs_index) const
{
	volatile tSynchronizer sync(this); // sync

	if(ofs_index < 0) ofs_index += Array.size(); // 折り返す

	risse_size index = static_cast<risse_size>(ofs_index);
	if(ofs_index < 0 || index >= Array.size())
	{
		// 範囲外
		// default の値を得て、それを返す
		return ReadMember(ss_default);
	}

	// 値を返す
	return Array[index];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseArrayInstance::iset(const tRisseVariant & value, risse_offset ofs_index)
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
			Array.resize(index+1, ReadMember(ss_filler));

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
void tRisseArrayInstance::push(const tRisseMethodArgument & args)
{
	volatile tSynchronizer sync(this); // sync

	for(risse_size i = 0; i < args.GetArgumentCount(); i++)
		Array.push_back(args[i]);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseArrayInstance::pop()
{
	volatile tSynchronizer sync(this); // sync

	if(Array.size() > 0)
	{
		tRisseVariant val;
		val = Array.back();
		Array.pop_back();
		return val;
	}
	else
	{
		return ReadMember(ss_default);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseArrayInstance::unshift(const tRisseMethodArgument & args)
{
	volatile tSynchronizer sync(this); // sync

	risse_size i = args.GetArgumentCount();
	while(i--) Array.push_front(args[i]);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseArrayInstance::shift()
{
	volatile tSynchronizer sync(this); // sync

	if(Array.size() > 0)
	{
		tRisseVariant val;
		val = Array.front();
		Array.pop_front();
		return val;
	}
	else
	{
		return ReadMember(ss_default);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tRisseArrayInstance::get_length() const
{
	volatile tSynchronizer sync(this); // sync

	return Array.size();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseArrayInstance::set_length(size_t new_size)
{
	volatile tSynchronizer sync(this); // sync

	if(Array.size() < new_size)
	{
		// 拡張
		Array.resize(new_size, ReadMember(ss_filler));
	}
	else
	{
		// 縮小
		Array.resize(new_size);
	}
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tRisseArrayClass::tRisseArrayClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseArrayClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_ovulate, &tRisseArrayClass::ovulate);
	RisseBindFunction(this, ss_construct, &tRisseArrayInstance::construct);
	RisseBindFunction(this, ss_initialize, &tRisseArrayInstance::initialize);
	RisseBindFunction(this, mnIGet, &tRisseArrayInstance::iget);
	RisseBindFunction(this, mnISet, &tRisseArrayInstance::iset);
	RisseBindFunction(this, ss_push, &tRisseArrayInstance::push);
	RisseBindFunction(this, ss_pop, &tRisseArrayInstance::pop);
	RisseBindFunction(this, ss_unshift, &tRisseArrayInstance::unshift);
	RisseBindFunction(this, ss_shift, &tRisseArrayInstance::shift);
	RisseBindProperty(this, ss_length, &tRisseArrayInstance::get_length, &tRisseArrayInstance::set_length);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseArrayClass::ovulate()
{
	return tRisseVariant(new tRisseArrayInstance());
}
//---------------------------------------------------------------------------

} /* namespace Risse */

