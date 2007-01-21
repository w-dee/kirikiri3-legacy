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

/*
	Risseスクリプトから見える"Array" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(65360,34010,1527,19914,27817,35057,17111,22724);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Array.construct
//---------------------------------------------------------------------------
class tRisseNativeFunction_Array_construct : public tRisseNativeFunctionBase
{
protected:
	//! @brief		Risseメソッド呼び出し時に呼ばれるメソッド
	void Call(
		tRisseVariant * result,
		tRisseOperateFlags flags,
		const tRisseMethodArgument & args,
		const tRisseMethodArgument & bargs,
		const tRisseVariant &This,
		const tRisseStackFrameContext &stack)
	{
		// デフォルトでは何もしない
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		NativeFunction: Array.initialize
//---------------------------------------------------------------------------
class tRisseNativeFunction_Array_initialize : public tRisseNativeFunctionBase
{
protected:
	//! @brief		Risseメソッド呼び出し時に呼ばれるメソッド
	void Call(
		tRisseVariant * result,
		tRisseOperateFlags flags,
		const tRisseMethodArgument & args,
		const tRisseMethodArgument & bargs,
		const tRisseVariant &This,
		const tRisseStackFrameContext &stack)
	{
		// 親クラスの同名メソッドを呼び出す
		// TODO: コンストラクタの正しい実装
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseArrayClass::tRisseArrayClass(const tRisseVariant & super) : tRisseClass(super)
{
	// クラスに必要なメソッドを登録する

	// construct, initialize などは新しいオブジェクトのコンテキスト上で実行されるので
	// コンテキストとしては null を指定する

	// construct
	RegisterNormalMember(ss_construct, tRisseVariant(new tRisseNativeFunction_Array_construct()));
	// initialize
	RegisterNormalMember(ss_initialize, tRisseVariant(new tRisseNativeFunction_Array_initialize()));
}
//---------------------------------------------------------------------------

} /* namespace Risse */

