//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief "Module" (モジュール) の実装
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseModuleClass.h"
#include "risseObjectClass.h"
#include "risseOpCodes.h"
#include "risseStaticStrings.h"
#include "risseScriptEngine.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(63047,20109,44050,17555,30336,10949,23175,16849);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tRisseModuleClass::tRisseModuleClass(tRisseScriptEngine * engine) :
	tRisseClassBase(ss_Module, engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseModuleClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_ovulate, &tRisseModuleClass::ovulate);
	RisseBindFunction(this, ss_construct, &tRisseModuleClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseModuleClass::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseModuleClass::ovulate()
{
	return tRisseVariant(new tRisseObjectBase());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseModuleClass::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseModuleClass::initialize(const tRisseVariant & name, const tRisseNativeCallInfo &info)
{
	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();

	// name はクラス名
	// This に name という名前で値を登録し、書き込み禁止にする
	info.This.SetPropertyDirect(info.engine, ss_name,
		tRisseMemberAttribute::GetDefault() |
		tRisseOperateFlags::ofMemberEnsure|tRisseOperateFlags::ofInstanceMemberOnly,
		name);
	(*const_cast<tRisseVariant*>(&info.This)).
		Do(info.engine, ocDSetAttrib, NULL, ss_name,
			tRisseMemberAttribute(tRisseMemberAttribute::vcConst));
}
//---------------------------------------------------------------------------



} // namespace Risse
