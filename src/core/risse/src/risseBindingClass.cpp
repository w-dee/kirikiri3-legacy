/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Binding" クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "risseBindingClass.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "risseScriptEngine.h"

/*
	"Binding" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(26427,5348,61020,19015,38284,853,37508,24976);
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
void tRisseBindingInstance::AddMap(tRisseVariant &This, const tRisseString &name, risse_uint32 reg)
{
	RISSE_ASSERT(This.GetType() == tRisseVariant::vtObject);
	tRisseScriptEngine * engine = This.GetObjectInterface()->GetRTTI()->GetScriptEngine();
	tRisseBindingInstance * obj =
		This.CheckAndGetObjectInterafce<tRisseBindingInstance, tRisseClassBase>(engine->BindingClass);
	obj->GetBindingMap().insert(tRisseBindingInfo::tBindingMap::value_type(name, reg));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBindingInstance::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBindingInstance::initialize(const tRisseNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBindingInstance::eval(const tRisseString & script,
			const tRisseNativeCallInfo &info) const
{
	// ロックしない

	tRisseString name = info.args.HasArgument(1) ?
					tRisseString(info.args[1]) : tRisseString(RISSE_WS("(anonymous)"));
	risse_size lineofs = info.args.HasArgument(2) ? (risse_size)(risse_int64)info.args[2] : (risse_size)0;

	GetRTTI()->GetScriptEngine()->Evaluate(script, name, lineofs, info.result, GetInfo(), true);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseBindingInstance::iget(const tRisseString & name) const
{
	volatile tSynchronizer sync(this); // sync

	tRisseBindingInfo::tBindingMap & map = GetBindingMap();
	tRisseBindingInfo::tBindingMap::iterator i = map.find(name);
	if(i == map.end())
	{
		// 見つからなかった
		// とりあえず void を返す
		return tRisseVariant::GetVoidObject();
	}
	else
	{
		// 見つかった
		risse_size nestlevel = (i->second >> 16) & 0xffff;
		risse_size regnum = i->second & 0xffff;
		return GetFrames()->Get(nestlevel, regnum);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBindingInstance::iset(const tRisseVariant & value, const tRisseString & name)
{
	volatile tSynchronizer sync(this); // sync

	tRisseBindingInfo::tBindingMap & map = GetBindingMap();
	tRisseBindingInfo::tBindingMap::iterator i = map.find(name);
	if(i == map.end())
	{
		// 見つからなかった
		// TODO: Dictionary 互換の例外
	}
	else
	{
		// 見つかった
		risse_size nestlevel = (i->second >> 16) & 0xffff;
		risse_size regnum = i->second & 0xffff;
		GetFrames()->Set(nestlevel, regnum, value);
	}
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tRisseBindingClass::tRisseBindingClass(tRisseScriptEngine * engine) :
	tRisseClassBase(ss_Binding, engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseBindingClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_ovulate, &tRisseBindingClass::ovulate);
	RisseBindFunction(this, ss_construct, &tRisseBindingInstance::construct);
	RisseBindFunction(this, ss_initialize, &tRisseBindingInstance::initialize);
	RisseBindFunction(this, ss_eval, &tRisseBindingInstance::eval);
	RisseBindFunction(this, mnIGet, &tRisseBindingInstance::iget);
	RisseBindFunction(this, mnISet, &tRisseBindingInstance::iset);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseBindingClass::ovulate()
{
	return tRisseVariant(new tRisseBindingInstance());
}
//---------------------------------------------------------------------------

} /* namespace Risse */

