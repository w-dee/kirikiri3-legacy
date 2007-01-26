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
//! @brief		NativeFunction: .construct
//---------------------------------------------------------------------------
static void Number_construct(RISSE_NATIVEFUNCTION_CALLEE_ARGS)
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseNumberClass::tRisseNumberClass() :
	tRisseClass(tRissePrimitiveClass::GetPointer())
{
	// クラスに必要なメソッドを登録する

	// construct, initialize などは新しいオブジェクトのコンテキスト上で実行されるので
	// コンテキストとしては null を指定する

	// construct
	RegisterNormalMember(ss_construct, tRisseVariant(new tRisseNativeFunctionBase(Number_construct)));
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

