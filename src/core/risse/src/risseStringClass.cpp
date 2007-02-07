/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "String" クラスの実装
//---------------------------------------------------------------------------

#include "risseTypes.h"
#include "risseStringClass.h"
#include "risseNativeFunction.h"
#include "risseNativeProperty.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"

/*
	Risseスクリプトから見える"String" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(44706,36741,55501,19515,15528,60571,63357,21717);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		NativeFunction: String.initialize
//---------------------------------------------------------------------------
static void String_initialize(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	// 親クラスの同名メソッドは「呼び出されない」

	// 引数をすべて連結した物を初期値に使う
	// 注意: いったん CreateNewObjectBase で作成されたオブジェクトの中身
	//       を変更するため、const_cast を用いる
	for(risse_size i = 0; i < args.GetArgumentCount(); i++)
		*const_cast<tRisseVariant*>(&This) += args[i];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: String.charAt
//---------------------------------------------------------------------------
static void String_charAt(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	if(args.GetArgumentCount() < 1) RisseThrowBadArgumentCount(args.GetArgumentCount(), 1);

	if(result)
	{
		const tRisseString & str = This.operator tRisseString();
		risse_offset index = (risse_int64)args[0];
		if(index < 0) index += str.GetLength();
		if(index < 0 || static_cast<risse_size>(index) >= str.GetLength())
			result->Clear(); // 値が範囲外なので void を返す
		else
			*result = tRisseString(str, static_cast<risse_size>(index), 1);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeProperty: String.length
//---------------------------------------------------------------------------
static void String_length_getter(RISSE_NATIVEPROPERTY_GETTER_ARGS)
{
	if(result) *result = (risse_int64)This.operator tRisseString().GetLength();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseStringClass::tRisseStringClass() : tRissePrimitiveClassBase(tRissePrimitiveClass::GetPointer())
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseStringClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する

	// construct は tRissePrimitiveClass 内ですでに登録されている

	// initialize
	RegisterNormalMember(ss_initialize, tRisseVariant(tRisseNativeFunction::New(String_initialize)));

	// charAt
	RegisterNormalMember(ss_charAt, tRisseVariant(tRisseNativeFunction::New(String_charAt)));

	// length
	RegisterNormalMember(ss_length,
		tRisseVariant(tRisseNativeProperty::New(String_length_getter, NULL)),
			tRisseMemberAttribute(tRisseMemberAttribute::pcProperty));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseStringClass::CreateNewObjectBase()
{
	return tRisseVariant(tRisseString());
}
//---------------------------------------------------------------------------

} /* namespace Risse */

