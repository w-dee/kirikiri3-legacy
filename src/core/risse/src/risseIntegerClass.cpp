/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Integer" クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "risseIntegerClass.h"
#include "risseNativeFunction.h"
#include "risseNativeProperty.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "risseNumberClass.h"
#include "risseScriptEngine.h"

/*
	Risseスクリプトから見える"String" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(39234,49682,57279,16499,28574,56016,64030,59385);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseIntegerClass::tRisseIntegerClass(tRisseScriptEngine * engine) :
	tRissePrimitiveClassBase(engine->NumberClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseIntegerClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	// construct は tRissePrimitiveClass 内ですでに登録されている

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD_OPTION(ss_initialize,attribute.Set(tRisseMemberAttribute::vcConst).Set(tRisseMemberAttribute::ocFinal))
	{
		// 親クラスの同名メソッドは「呼び出されない」

		// 引数をすべて連結した物を初期値に使う
		// 注意: いったん CreateNewObjectBase で作成されたオブジェクトの中身
		//       を変更するため、const_cast を用いる
		if(args.HasArgument(0))
			*const_cast<tRisseVariant*>(&This) = args[0].operator risse_int64();
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_times)
	{
		if(args.GetBlockArgumentCount() < 1) RisseThrowBadBlockArgumentCount(args.GetArgumentCount(), 1);

		risse_int64 count = This.operator risse_int64();
		while(count --)
			args.GetBlockArgument(0).FuncCall(engine);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseIntegerClass::CreateNewObjectBase()
{
	return tRisseVariant((risse_int64)0);
}
//---------------------------------------------------------------------------

} /* namespace Risse */

