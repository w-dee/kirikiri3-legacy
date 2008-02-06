/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Dictionary" クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "risseDictionaryClass.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "risseScriptEngine.h"

/*
	Risseスクリプトから見える "Dictionary" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(6163,14610,25476,18127,15249,59042,30468,59014);
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
void tDictionaryInstance::construct()
{
	// default メンバを追加 (デフォルトではvoid)
	// TODO: default ではなくて missing メソッドみたいなのを実装しようよ
	RegisterNormalMember(ss_default, tVariant::GetVoidObject());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tDictionaryInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tDictionaryInstance::iget(const tVariant & key)
{
	volatile tSynchronizer sync(this); // sync

	tVariant * found = HashTable.Find(key);
	if(!found)
	{
		// 見つからなかった
		// default の値を得て、それを返す
		return GetPropertyDirect(ss_default);
	}

	// 値を返す
	return *found;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tDictionaryInstance::iset(const tVariant & value, const tVariant & key)
{
	volatile tSynchronizer sync(this); // sync

	HashTable.Add(key, value);
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tDictionaryClass::tDictionaryClass(tScriptEngine * engine) :
	tClassBase(ss_Dictionary, engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tDictionaryClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tDictionaryClass::ovulate);
	BindFunction(this, ss_construct, &tDictionaryInstance::construct);
	BindFunction(this, ss_initialize, &tDictionaryInstance::initialize);
	BindFunction(this, mnIGet, &tDictionaryInstance::iget);
	BindFunction(this, mnISet, &tDictionaryInstance::iset);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tDictionaryClass::ovulate()
{
	return tVariant(new tDictionaryInstance());
}
//---------------------------------------------------------------------------

} /* namespace Risse */

