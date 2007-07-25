//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief C++ でモジュールを簡単に実装できるようにするためのヘルパークラス
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseModuleClass.h"
#include "risseModule.h"
#include "risseObjectClass.h"
#include "risseStaticStrings.h"
#include "risseScriptEngine.h"

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(52148,41054,64996,18248,26505,21142,29180,62811);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tModuleBase::tModuleBase(const tString & name, tScriptEngine * engine)
{
	tVariant instance_v;
	engine->ModuleClass->Do(ocFuncCall, &instance_v, ss_new, 0, tMethodArgument::New(name));
	RISSE_ASSERT(dynamic_cast<tObjectBase *>(instance_v.GetObjectInterface()) != NULL);
	Instance = reinterpret_cast<tObjectBase *>(instance_v.GetObjectInterface());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tModuleBase::RegisterMembers()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tModuleBase::RegisterInstance(tVariant & target)
{
	tString name = Instance->GetPropertyDirect(ss_name);
	target.SetPropertyDirect_Object(name,
		tOperateFlags(tMemberAttribute::GetDefault()) |
		tOperateFlags::ofMemberEnsure |
		tOperateFlags::ofInstanceMemberOnly |
		tOperateFlags::ofUseClassMembersRule,
				tVariant(Instance));
}
//---------------------------------------------------------------------------

} // namespace Risse
