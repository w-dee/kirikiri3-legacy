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
#include "prec.h"
#include "risseTypes.h"
#include "risseNumberClass.h"
#include "risseNativeFunction.h"
#include "risseNativeProperty.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "rissePrimitiveClass.h"
#include "risseExceptionClass.h"
#include "risseScriptEngine.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(2098,51592,31991,16696,47274,13601,12452,21741);
//---------------------------------------------------------------------------
tRisseNumberClass::tRisseNumberClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->PrimitiveClass)
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
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD_OPTION(ss_construct,attribute.Set(tRisseMemberAttribute::vcConst).Set(tRisseMemberAttribute::ocFinal))
	{
		// デフォルトでは何もしない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_PROPERTY_OPTION(ss_isNaN,attribute.Set(tRisseMemberAttribute::vcConst).Set(tRisseMemberAttribute::ocFinal))
	{
		RISSE_BEGINE_NATIVE_PROPERTY_GETTER
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
		RISSE_END_NATIVE_PROPERTY_GETTER
	}
	RISSE_END_NATIVE_PROPERTY

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseNumberClass::CreateNewObjectBase()
{
	// このクラスのインスタンスは作成できないので例外を投げる
	tRisseInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
	return tRisseVariant();
}
//---------------------------------------------------------------------------

} /* namespace Risse */

