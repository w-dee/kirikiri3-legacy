/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Real" クラスの実装
//---------------------------------------------------------------------------

#include "risseTypes.h"
#include "risseRealClass.h"
#include "risseNativeFunction.h"
#include "risseNativeProperty.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "risseNumberClass.h"

/*
	Risseスクリプトから見える"String" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(5442,9100,49866,17725,24713,23464,12701,40981);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tRisseRealClass::tRisseRealClass() : tRissePrimitiveClassBase(tRisseNumberClass::GetPointer())
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseRealClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	// construct は tRissePrimitiveClass 内ですでに登録されている

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 親クラスの同名メソッドは「呼び出されない」

		// 引数をすべて連結した物を初期値に使う
		// 注意: いったん CreateNewObjectBase で作成されたオブジェクトの中身
		//       を変更するため、const_cast を用いる
		if(args.HasArgument(0))
			*const_cast<tRisseVariant*>(&This) = args[0].operator risse_real();
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseRealClass::CreateNewObjectBase()
{
	return tRisseVariant((risse_real)0);
}
//---------------------------------------------------------------------------

} /* namespace Risse */

