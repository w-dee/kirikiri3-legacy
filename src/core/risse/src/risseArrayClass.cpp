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
#include "risseNativeFunction.h"
#include "risseNativeProperty.h"
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// default メンバを追加 (デフォルトではvoid)
		This.SetPropertyDirect_Object(ss_default, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant::GetVoidObject(), This);

		// filler メンバを追加 (デフォルトではvoid)
		This.SetPropertyDirect_Object(ss_filler, tRisseOperateFlags::ofMemberEnsure,
			tRisseVariant::GetVoidObject(), This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す
		engine->ArrayClass->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);

		// 引数を元に配列を構成する
		// ここの動作は push と同じ
		tRisseArrayInstance * obj = This.CheckAndGetObjectInterafce<tRisseArrayInstance, tRisseClassBase>(engine->ArrayClass);
		tRisseArrayInstance::tArray & array = obj->GetArray();

		for(risse_size i = 0; i < args.GetArgumentCount(); i++)
			array.push_back(args[i]);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD_OPTION(mnIGet,attribute.Set(tRisseMemberAttribute::vcConst).Set(tRisseMemberAttribute::ocFinal))
	{
		tRisseArrayInstance * obj = This.CheckAndGetObjectInterafce<tRisseArrayInstance, tRisseClassBase>(engine->ArrayClass);
		tRisseArrayInstance::tArray & array = obj->GetArray();

		args.ExpectArgumentCount(1);

		risse_offset ofs_index = static_cast<risse_size>((risse_int64)args[0]);
		if(ofs_index < 0) ofs_index += array.size(); // 折り返す

		risse_size index = static_cast<risse_size>(ofs_index);
		if(ofs_index < 0 || index >= array.size())
		{
			// 範囲外
			// default の値を得て、それを返す
			tRisseVariant default_value = This.GetPropertyDirect_Object(ss_default);
			if(result) *result = default_value;
			return;
		}

		// 値を返す
		if(result) *result = array[index];
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD_OPTION(mnISet,attribute.Set(tRisseMemberAttribute::vcConst).Set(tRisseMemberAttribute::ocFinal))
	{
		tRisseArrayInstance * obj = This.CheckAndGetObjectInterafce<tRisseArrayInstance, tRisseClassBase>(engine->ArrayClass);
		tRisseArrayInstance::tArray & array = obj->GetArray();

		args.ExpectArgumentCount(2);

		risse_offset ofs_index = static_cast<risse_size>((risse_int64)args[1]);
		if(ofs_index < 0) ofs_index += array.size(); // 折り返す

		if(ofs_index < 0)  { /* それでもまだ負: TOOD: out of bound 例外 */ return; }

		risse_size index = static_cast<risse_size>(ofs_index);
		if(index >= array.size())
		{
			// 配列を拡張する
			// もし、拡張する際に値を埋める必要があるならば
			// 値を埋める
			if(index > array.size())
			{
				// filler の値を得る
				tRisseVariant filler = This.GetPropertyDirect_Object(ss_filler);
				// filler で埋める
				array.resize(index+1, filler);

				// 値の上書き
				array[index] = args[0];
			}
			else /* if index == array.size() */
			{
				array.push_back(args[0]);
			}
		}
		else
		{
			// 既存の値の上書き
			array[index] = args[0];
		}
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_push)
	{
		tRisseArrayInstance * obj = This.CheckAndGetObjectInterafce<tRisseArrayInstance, tRisseClassBase>(engine->ArrayClass);
		tRisseArrayInstance::tArray & array = obj->GetArray();

		for(risse_size i = 0; i < args.GetArgumentCount(); i++)
			array.push_back(args[i]);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_pop)
	{
		tRisseArrayInstance * obj = This.CheckAndGetObjectInterafce<tRisseArrayInstance, tRisseClassBase>(engine->ArrayClass);
		tRisseArrayInstance::tArray & array = obj->GetArray();

		tRisseVariant val;
		if(array.size() > 0)
		{
			val = array.back();
			array.pop_back();
		}
		else
		{
			val = This.GetPropertyDirect_Object(ss_default);
		}
		if(result) *result = val;
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_unshift)
	{
		tRisseArrayInstance * obj = This.CheckAndGetObjectInterafce<tRisseArrayInstance, tRisseClassBase>(engine->ArrayClass);
		tRisseArrayInstance::tArray & array = obj->GetArray();

		risse_size i = args.GetArgumentCount();
		while(i--) array.push_front(args[i]);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_shift)
	{
		tRisseArrayInstance * obj = This.CheckAndGetObjectInterafce<tRisseArrayInstance, tRisseClassBase>(engine->ArrayClass);
		tRisseArrayInstance::tArray & array = obj->GetArray();

		tRisseVariant val;
		if(array.size() > 0)
		{
			val = array.front();
			array.pop_front();
		}
		else
		{
			val = This.GetPropertyDirect_Object(ss_default);
		}
		if(result) *result = val;
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_PROPERTY(ss_length)
	{
		RISSE_BEGINE_NATIVE_PROPERTY_GETTER
		{
			tRisseArrayInstance * obj = This.CheckAndGetObjectInterafce<tRisseArrayInstance, tRisseClassBase>(engine->ArrayClass);
			tRisseArrayInstance::tArray & array = obj->GetArray();

			if(result) *result = (risse_int64)array.size();
		}
		RISSE_END_NATIVE_PROPERTY_GETTER

		RISSE_BEGINE_NATIVE_PROPERTY_SETTER
		{
			tRisseArrayInstance * obj = This.CheckAndGetObjectInterafce<tRisseArrayInstance, tRisseClassBase>(engine->ArrayClass);
			tRisseArrayInstance::tArray & array = obj->GetArray();

			risse_size new_size = (risse_size)(risse_int64)(value);
			if(array.size() < new_size)
			{
				// 拡張
				// filler の値を得る
				tRisseVariant filler = This.GetPropertyDirect_Object(ss_filler);
				array.resize(new_size, filler);
			}
			else
			{
				// 縮小
				array.resize(new_size);
			}
		}
		RISSE_END_NATIVE_PROPERTY_SETTER
	}
	RISSE_END_NATIVE_PROPERTY

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseArrayClass::CreateNewObjectBase()
{
	return tRisseVariant(new tRisseArrayInstance());
}
//---------------------------------------------------------------------------

} /* namespace Risse */

