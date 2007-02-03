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

#include "risseTypes.h"
#include "risseArrayClass.h"
#include "risseNativeFunction.h"
#include "risseNativeProperty.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"

/*
	Risseスクリプトから見える"Array" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(65360,34010,1527,19914,27817,35057,17111,22724);
//---------------------------------------------------------------------------
static void Array_push(RISSE_NATIVEFUNCTION_CALLEE_ARGS);


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Array.construct
//---------------------------------------------------------------------------
static void Array_construct(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	// default メンバを追加 (デフォルトではvoid)
	This.SetPropertyDirect(ss_default, tRisseOperateFlags::ofInstanceMemberOnly|tRisseOperateFlags::ofMemberEnsure,
		tRisseVariant::GetVoidObject(), This);

	// filler メンバを追加 (デフォルトではvoid)
	This.SetPropertyDirect(ss_filler, tRisseOperateFlags::ofInstanceMemberOnly|tRisseOperateFlags::ofMemberEnsure,
		tRisseVariant::GetVoidObject(), This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Array.initialize
//---------------------------------------------------------------------------
static void Array_initialize(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	// 親クラスの同名メソッドを呼び出す
	tRisseArrayClass::GetPointer()->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);

	// 引数を元に配列を構成する
	Array_push(result, flags, args, This); // push を呼ぶ
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Array.[]
//---------------------------------------------------------------------------
static void Array_IGet(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	tRisseArrayObject * obj = This.CheckAndGetObjectInterafce<tRisseArrayObject, tRisseArrayClass>();
	tRisseArrayObject::tArray & array = obj->GetArray();

	if(args.GetArgumentCount() < 1) RisseThrowBadArgumentCount(args.GetArgumentCount(), 1);

	risse_offset ofs_index = static_cast<risse_size>((risse_int64)args[0]);
	if(ofs_index < 0) ofs_index += array.size(); // 折り返す

	risse_size index = static_cast<risse_size>(ofs_index);
	if(ofs_index < 0 || index >= array.size())
	{
		// 範囲外
		// default の値を得て、それを返す
		tRisseVariant default_value = This.GetPropertyDirect(ss_default);
		if(result) *result = default_value;
		return;
	}

	// 値を返す
	if(result) *result = array[index];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Array.[]=
//---------------------------------------------------------------------------
static void Array_ISet(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	tRisseArrayObject * obj = This.CheckAndGetObjectInterafce<tRisseArrayObject, tRisseArrayClass>();
	tRisseArrayObject::tArray & array = obj->GetArray();

	if(args.GetArgumentCount() < 2) RisseThrowBadArgumentCount(args.GetArgumentCount(), 2);

	risse_offset ofs_index = static_cast<risse_size>((risse_int64)args[0]);
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
			array[index] = args[1];
		}
		else /* if index == array.size() */
		{
			array.push_back(args[1]);
		}
	}
	else
	{
		// 既存の値の上書き
		array[index] = args[1];
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Array.push
//---------------------------------------------------------------------------
static void Array_push(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	tRisseArrayObject * obj = This.CheckAndGetObjectInterafce<tRisseArrayObject, tRisseArrayClass>();
	tRisseArrayObject::tArray & array = obj->GetArray();

	for(risse_size i = 0; i < args.GetArgumentCount(); i++)
		array.push_back(args[i]);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Array.pop
//---------------------------------------------------------------------------
static void Array_pop(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	tRisseArrayObject * obj = This.CheckAndGetObjectInterafce<tRisseArrayObject, tRisseArrayClass>();
	tRisseArrayObject::tArray & array = obj->GetArray();

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
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Array.unshift
//---------------------------------------------------------------------------
static void Array_unshift(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	tRisseArrayObject * obj = This.CheckAndGetObjectInterafce<tRisseArrayObject, tRisseArrayClass>();
	tRisseArrayObject::tArray & array = obj->GetArray();

	risse_size i = args.GetArgumentCount();
	while(i--) array.push_front(args[i]);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Array.shift
//---------------------------------------------------------------------------
static void Array_shift(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	tRisseArrayObject * obj = This.CheckAndGetObjectInterafce<tRisseArrayObject, tRisseArrayClass>();
	tRisseArrayObject::tArray & array = obj->GetArray();

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
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeProperty: Array.length
//---------------------------------------------------------------------------
static void Array_length_getter(RISSE_NATIVEPROPERTY_GETTER_ARGS)
{
	tRisseArrayObject * obj = This.CheckAndGetObjectInterafce<tRisseArrayObject, tRisseArrayClass>();
	tRisseArrayObject::tArray & array = obj->GetArray();

	if(result) *result = (risse_int64)array.size();
}
static void Array_length_setter(RISSE_NATIVEPROPERTY_SETTER_ARGS)
{
	tRisseArrayObject * obj = This.CheckAndGetObjectInterafce<tRisseArrayObject, tRisseArrayClass>();
	tRisseArrayObject::tArray & array = obj->GetArray();

	// TODO: デフォルトの値の扱い
	array.resize((risse_size)(risse_int64)(value));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseArrayClass::tRisseArrayClass() :
	tRisseClassBase(tRisseObjectClass::GetPointer())
{
	// クラスに必要なメソッドを登録する

	// construct, initialize などは新しいオブジェクトのコンテキスト上で実行されるので
	// コンテキストとしては null を指定する

	// construct
	RegisterNormalMember(ss_construct, tRisseVariant(new tRisseNativeFunctionBase(Array_construct)));
	// initialize
	RegisterNormalMember(ss_initialize, tRisseVariant(new tRisseNativeFunctionBase(Array_initialize)));

	// []
	RegisterNormalMember(mnIGet, tRisseVariant(new tRisseNativeFunctionBase(Array_IGet)));
	// []=
	RegisterNormalMember(mnISet, tRisseVariant(new tRisseNativeFunctionBase(Array_ISet)));

	// push
	RegisterNormalMember(ss_push, tRisseVariant(new tRisseNativeFunctionBase(Array_push)));
	// pop
	RegisterNormalMember(ss_pop, tRisseVariant(new tRisseNativeFunctionBase(Array_pop)));
	// unshift
	RegisterNormalMember(ss_unshift, tRisseVariant(new tRisseNativeFunctionBase(Array_unshift)));
	// shift
	RegisterNormalMember(ss_shift, tRisseVariant(new tRisseNativeFunctionBase(Array_shift)));
	// length
	RegisterNormalMember(ss_length,
		tRisseVariant(new tRisseNativePropertyBase(Array_length_getter, Array_length_setter)),
			tRisseMemberAttribute(tRisseMemberAttribute::pcProperty));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseArrayClass::CreateNewObjectBase()
{
	return tRisseVariant(new tRisseArrayObject());
}
//---------------------------------------------------------------------------

} /* namespace Risse */

