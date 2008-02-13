/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Octet" クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "risseOctetClass.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "risseScriptEngine.h"

/*
	Risseスクリプトから見える"Octet" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(19296,56052,4218,18194,45952,40158,60560,61630);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tOctetClass::tOctetClass(tScriptEngine * engine) :
	tPrimitiveClassBase(ss_Octet, engine->PrimitiveClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOctetClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	// construct は tPrimitiveClass 内ですでに登録されている

	BindFunction(this, ss_ovulate, &tOctetClass::ovulate,
		tMemberAttribute().Set(tMemberAttribute::vcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, ss_initialize, &tOctetClass::initialize,
		tMemberAttribute().Set(tMemberAttribute::vcConst).Set(tMemberAttribute::ocFinal));
	BindProperty(this, ss_length, &tOctetClass::get_length,
		tMemberAttribute().Set(tMemberAttribute::vcConst).Set(tMemberAttribute::ocFinal));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tOctetClass::ovulate()
{
	return tVariant(tOctet());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOctetClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドは「呼び出されない」

	// 引数をすべて連結した物を初期値に使う
	// 注意: いったん ovulate で作成されたオブジェクトの中身
	//       を変更するため、const_cast を用いる
	for(risse_size i = 0; i < info.args.GetArgumentCount(); i++)
		*const_cast<tVariant*>(&info.This) += info.args[i];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOctetClass::get_length(const tNativePropGetInfo & info)
{
	if(info.result) *info.result = (risse_int64)info.This.operator tOctet().GetLength();
}
//---------------------------------------------------------------------------


} /* namespace Risse */
