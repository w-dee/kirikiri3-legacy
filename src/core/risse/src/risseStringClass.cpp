/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
tStringClass::tStringClass(tScriptEngine * engine) :
	tPrimitiveClassBase(ss_String, engine->PrimitiveClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStringClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	// construct は tPrimitiveClass 内ですでに登録されている

	BindFunction(this, ss_ovulate, &tStringClass::ovulate,
		tMemberAttribute().Set(tMemberAttribute::vcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, ss_initialize, &tStringClass::initialize,
		tMemberAttribute().Set(tMemberAttribute::vcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, ss_charAt, &tStringClass::charAt,
		tMemberAttribute().Set(tMemberAttribute::vcConst).Set(tMemberAttribute::ocFinal));
	BindProperty(this, ss_length, &tStringClass::get_length,
		tMemberAttribute().Set(tMemberAttribute::vcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, ss_substr, &tStringClass::substr,
		tMemberAttribute().Set(tMemberAttribute::vcConst).Set(tMemberAttribute::ocFinal));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tStringClass::ovulate()
{
	return tVariant(tString());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStringClass::initialize(const tNativeCallInfo & info)
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
void tStringClass::charAt(const tNativeCallInfo & info, risse_offset index)
{
	if(info.result)
	{
		const tString & str = info.This.operator tString();
		if(index < 0) index += str.GetLength();
		if(index < 0 || static_cast<risse_size>(index) >= str.GetLength())
			info.result->Clear(); // 値が範囲外なので void を返す
		else
			*info.result = tString(str, static_cast<risse_size>(index), 1);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStringClass::get_length(const tNativePropGetInfo & info)
{
	if(info.result) *info.result = (risse_int64)info.This.operator tString().GetLength();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStringClass::substr(risse_offset start, const tNativeCallInfo & info)
{
	if(info.result)
	{
		const tString & str = info.This.operator tString();
		if(start < 0) start += str.GetLength();
		if(start < 0 || static_cast<risse_size>(start) >= str.GetLength())
		{
			info.result->Clear(); // 値が範囲外なので void を返す
		}
		else
		{
			// 第２引数が与えられた場合はその長さ、与えられなければ文字列の最後
			// まで切り取る
			risse_size len = info.args.HasArgument(1) ?
				(risse_size)(risse_int64)info.args[1] : risse_size_max;

			risse_size avail_len = str.GetLength() - start;
			if(len > avail_len) len = avail_len;
			*info.result = tString(str, static_cast<risse_size>(start), len);
		}
	}
}
//---------------------------------------------------------------------------

} /* namespace Risse */

