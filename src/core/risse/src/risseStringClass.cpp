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
#include "prec.h"
#include "risseTypes.h"
#include "risseStringClass.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "risseScriptEngine.h"

/*
	Risseスクリプトから見える"String" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(44706,36741,55501,19515,15528,60571,63357,21717);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tRisseStringClass::tRisseStringClass(tRisseScriptEngine * engine) :
	tRissePrimitiveClassBase(ss_String, engine->PrimitiveClass)
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
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	// construct は tRissePrimitiveClass 内ですでに登録されている

	RisseBindFunction(this, ss_ovulate, &tRisseStringClass::ovulate,
		tRisseMemberAttribute().Set(tRisseMemberAttribute::vcConst).Set(tRisseMemberAttribute::ocFinal));
	RisseBindFunction(this, ss_initialize, &tRisseStringClass::initialize,
		tRisseMemberAttribute().Set(tRisseMemberAttribute::vcConst).Set(tRisseMemberAttribute::ocFinal));
	RisseBindFunction(this, ss_charAt, &tRisseStringClass::charAt,
		tRisseMemberAttribute().Set(tRisseMemberAttribute::vcConst).Set(tRisseMemberAttribute::ocFinal));
	RisseBindProperty(this, ss_length, &tRisseStringClass::get_length,
		tRisseMemberAttribute().Set(tRisseMemberAttribute::vcConst).Set(tRisseMemberAttribute::ocFinal));

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseStringClass::ovulate()
{
	return tRisseVariant(tRisseString());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseStringClass::initialize(const tRisseNativeCallInfo & info)
{
	// 親クラスの同名メソッドは「呼び出されない」

	// 引数をすべて連結した物を初期値に使う
	// 注意: いったん ovulate で作成されたオブジェクトの中身
	//       を変更するため、const_cast を用いる
	for(risse_size i = 0; i < info.args.GetArgumentCount(); i++)
		*const_cast<tRisseVariant*>(&info.This) += info.args[i];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseStringClass::charAt(const tRisseNativeCallInfo & info, risse_offset index)
{
	if(info.result)
	{
		const tRisseString & str = info.This.operator tRisseString();
		if(index < 0) index += str.GetLength();
		if(index < 0 || static_cast<risse_size>(index) >= str.GetLength())
			info.result->Clear(); // 値が範囲外なので void を返す
		else
			*info.result = tRisseString(str, static_cast<risse_size>(index), 1);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseStringClass::get_length(const tRisseNativePropGetInfo & info)
{
	if(info.result) *info.result = (risse_int64)info.This.operator tRisseString().GetLength();
}
//---------------------------------------------------------------------------

} /* namespace Risse */

