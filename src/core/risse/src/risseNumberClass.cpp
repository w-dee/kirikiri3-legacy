/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risseの数値型用クラス関連の処理の実装
//---------------------------------------------------------------------------

#include "risseTypes.h"
#include "risseNumberClass.h"
#include "risseNativeFunction.h"
#include "risseNativeProperty.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "rissePrimitiveClass.h"


namespace Risse
{
RISSE_DEFINE_SOURCE_ID(2098,51592,31991,16696,47274,13601,12452,21741);
//---------------------------------------------------------------------------
//! @brief		NativeFunction: Number.construct
//---------------------------------------------------------------------------
static void Number_construct(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Number.isNaN
//---------------------------------------------------------------------------
static void Number_isNaN_getter(RISSE_NATIVEPROPERTY_GETTER_ARGS)
{
	tRisseVariant num = This.Plus();
	if(result)
	{
		switch(num.GetType())
		{
		case tRisseVariant::vtReal:
			*result = (bool)RISSE_FC_IS_NAN(RisseGetFPClass(num.operator risse_real()));
			break;

		case tRisseVariant::vtInteger:
		default:
			// 整数の場合やそのほかの場合は偽を返す
			*result = false;
			break;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseNumberClass::tRisseNumberClass() :
	tRisseClassBase(tRissePrimitiveClass::GetPointer())
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseNumberClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する

	// construct, initialize などは新しいオブジェクトのコンテキスト上で実行されるので
	// コンテキストとしては null を指定する

	// construct
	RegisterNormalMember(ss_construct, tRisseVariant(tRisseNativeFunction::New(Number_construct)));
	// isNaN
	RegisterNormalMember(ss_isNaN,
		tRisseVariant(tRisseNativeProperty::New(Number_isNaN_getter, NULL)),
			tRisseMemberAttribute(tRisseMemberAttribute::pcProperty));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseNumberClass::CreateNewObjectBase()
{
	// このクラスのインスタンスは作成できないので例外を投げる
	RisseThrowCannotCreateInstanceFromThisClass();
	return tRisseVariant();
}
//---------------------------------------------------------------------------

} /* namespace Risse */

