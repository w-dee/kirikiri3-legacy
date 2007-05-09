/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Binding" クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "risseBindingClass.h"
#include "risseNativeFunction.h"
#include "risseNativeProperty.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"

/*
	"Binding" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(26427,5348,61020,19015,38284,853,37508,24976);
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
void tRisseBindingInstance::AddMap(tRisseVariant &This, const tRisseString &name, risse_uint32 reg)
{
	tRisseBindingInstance * obj =
		This.CheckAndGetObjectInterafce<tRisseBindingInstance, tRisseBindingClass>();
	obj->GetBindingMap().insert(tBindingMap::value_type(name, reg));
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tRisseBindingClass::tRisseBindingClass() :
	tRisseClassBase(tRisseObjectClass::GetPointer())
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBindingClass::RegisterMembers()
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
		// デフォルトでは何もしない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドを呼び出す
		tRisseBindingClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD_OPTION(mnIGet,attribute.Set(tRisseMemberAttribute::vcConst).Set(tRisseMemberAttribute::ocFinal))
	{
		// ローカル変数の値を得る
		// TODO: Dictionary 互換の実装
		tRisseBindingInstance * obj = This.CheckAndGetObjectInterafce<tRisseBindingInstance, tRisseBindingClass>();
		tRisseBindingInstance::tBindingMap & map = obj->GetBindingMap();


		args.ExpectArgumentCount(1);
		tRisseBindingInstance::tBindingMap::iterator i = map.find((tRisseString)args[1]);
		if(i == map.end())
		{
			// 見つからなかった
			// とりあえず void を返す
			if(result) result->Clear();
		}
		else
		{
			// 見つかった
			if(result)
			{
				risse_size nestlevel = (i->second >> 16) & 0xffff;
				risse_size regnum = i->second & 0xffff;
				*result = obj->GetFrames()->At(nestlevel, regnum);
			}
		}
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD_OPTION(mnISet,attribute.Set(tRisseMemberAttribute::vcConst).Set(tRisseMemberAttribute::ocFinal))
	{
#if 0
		tRisseArrayInstance * obj = This.CheckAndGetObjectInterafce<tRisseArrayInstance, tRisseArrayClass>();
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
				tRisseVariant filler = This.GetPropertyDirect(ss_filler);
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
#endif
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_push)
	{
/*
		tRisseArrayInstance * obj = This.CheckAndGetObjectInterafce<tRisseArrayInstance, tRisseArrayClass>();
		tRisseArrayInstance::tArray & array = obj->GetArray();

		for(risse_size i = 0; i < args.GetArgumentCount(); i++)
			array.push_back(args[i]);
*/
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_pop)
	{
/*
		tRisseArrayInstance * obj = This.CheckAndGetObjectInterafce<tRisseArrayInstance, tRisseArrayClass>();
		tRisseArrayInstance::tArray & array = obj->GetArray();

		tRisseVariant val;
		if(array.size() > 0)
		{
			val = array.back();
			array.pop_back();
		}
		else
		{
			val = This.GetPropertyDirect(ss_default);
		}
		if(result) *result = val;
*/
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_unshift)
	{
/*
		tRisseArrayInstance * obj = This.CheckAndGetObjectInterafce<tRisseArrayInstance, tRisseArrayClass>();
		tRisseArrayInstance::tArray & array = obj->GetArray();

		risse_size i = args.GetArgumentCount();
		while(i--) array.push_front(args[i]);
*/
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_shift)
	{
/*
		tRisseArrayInstance * obj = This.CheckAndGetObjectInterafce<tRisseArrayInstance, tRisseArrayClass>();
		tRisseArrayInstance::tArray & array = obj->GetArray();

		tRisseVariant val;
		if(array.size() > 0)
		{
			val = array.front();
			array.pop_front();
		}
		else
		{
			val = This.GetPropertyDirect(ss_default);
		}
		if(result) *result = val;
*/
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_PROPERTY(ss_length)
	{
/*
		RISSE_BEGINE_NATIVE_PROPERTY_GETTER
		{
			tRisseArrayInstance * obj = This.CheckAndGetObjectInterafce<tRisseArrayInstance, tRisseArrayClass>();
			tRisseArrayInstance::tArray & array = obj->GetArray();

			if(result) *result = (risse_int64)array.size();
		}
		RISSE_END_NATIVE_PROPERTY_GETTER

		RISSE_BEGINE_NATIVE_PROPERTY_SETTER
		{
			tRisseArrayInstance * obj = This.CheckAndGetObjectInterafce<tRisseArrayInstance, tRisseArrayClass>();
			tRisseArrayInstance::tArray & array = obj->GetArray();

			risse_size new_size = (risse_size)(risse_int64)(value);
			if(array.size() < new_size)
			{
				// 拡張
				// filler の値を得る
				tRisseVariant filler = This.GetPropertyDirect(ss_filler);
				array.resize(new_size, filler);
			}
			else
			{
				// 縮小
				array.resize(new_size);
			}
		}
		RISSE_END_NATIVE_PROPERTY_SETTER
*/
	}
	RISSE_END_NATIVE_PROPERTY

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseBindingClass::CreateNewObjectBase()
{
	return tRisseVariant(new tRisseBindingInstance());
}
//---------------------------------------------------------------------------

} /* namespace Risse */

