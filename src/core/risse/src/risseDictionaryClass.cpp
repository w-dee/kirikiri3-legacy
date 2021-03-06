/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

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
tVariant tDictionaryInstance::idelete(const tVariant & key)
{
	volatile tSynchronizer sync(this); // sync

	tVariant deleted_value;
	bool deleted = HashTable.Delete(key, &deleted_value);

	// キーが削除されればその削除された値、そうでなければ default の値を返す
	return deleted ? deleted_value : GetPropertyDirect(ss_default);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tDictionaryInstance::clear()
{
	volatile tSynchronizer sync(this); // sync

	HashTable.Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tDictionaryInstance::get_count()
{
	volatile tSynchronizer sync(this); // sync

	return HashTable.GetCount();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tDictionaryInstance::eachPair(const tNativeCallInfo &info)
{
	info.args.ExpectBlockArgumentCount(1);

	const tVariant & block = info.args.GetBlockArgument(0);

	volatile tSynchronizer sync(this); // sync

	// TODO: ロックどうする
	// TODO: 途中で辞書配列の内容が変わったときどうする
	tMemberHashTable::tIterator iterator(HashTable);
	tVariant key, value;
	while(!iterator.End())
	{
		key = iterator.GetKey(); // 値をコピー
		value = iterator.GetValue(); // 値をコピー
		block.FuncCall(GetRTTI()->GetScriptEngine(), NULL, 0, tMethodArgument::New(key, value));
		++iterator;
	}
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tDictionaryClass, ss_Dictionary, engine->ObjectClass)
	RISSE_BIND_CONSTRUCTORS
	BindFunction(this, mnIGet, &tDictionaryInstance::iget);
	BindFunction(this, mnISet, &tDictionaryInstance::iset);
	BindFunction(this, mnIDelete, &tDictionaryInstance::idelete);
	BindFunction(this, ss_clear, &tDictionaryInstance::clear);
	BindProperty(this, ss_count, &tDictionaryInstance::get_count);
	BindFunction(this, ss_eachPair, &tDictionaryInstance::eachPair);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------

} /* namespace Risse */

