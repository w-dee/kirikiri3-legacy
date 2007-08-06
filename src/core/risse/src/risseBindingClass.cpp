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
void tBindingInstance::AddMap(tVariant &This, const tString &name, risse_uint32 reg)
{
	RISSE_ASSERT(This.GetType() == tVariant::vtObject);
	tScriptEngine * engine = This.GetObjectInterface()->GetRTTI()->GetScriptEngine();
	tBindingInstance * obj =
		This.AssertAndGetObjectInterafce<tBindingInstance>(engine->BindingClass);
	obj->GetBindingMap().insert(tBindingInfo::tBindingMap::value_type(name, reg));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tBindingInstance::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tBindingInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tBindingInstance::eval(const tString & script,
			const tNativeCallInfo &info) const
{
	// ロックしない

	tString name = info.args.HasArgument(1) ?
					tString(info.args[1]) : tString(RISSE_WS("(anonymous)"));
	risse_size lineofs = info.args.HasArgument(2) ? (risse_size)(risse_int64)info.args[2] : (risse_size)0;

	GetRTTI()->GetScriptEngine()->Evaluate(script, name, lineofs, info.result, GetInfo(), true);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tBindingInstance::iget(const tString & name) const
{
	volatile tSynchronizer sync(this); // sync

	tBindingInfo::tBindingMap & map = GetBindingMap();
	tBindingInfo::tBindingMap::iterator i = map.find(name);
	if(i == map.end())
	{
		// 見つからなかった
		// とりあえず void を返す
		return tVariant::GetVoidObject();
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
void tBindingInstance::iset(const tVariant & value, const tString & name)
{
	volatile tSynchronizer sync(this); // sync

	tBindingInfo::tBindingMap & map = GetBindingMap();
	tBindingInfo::tBindingMap::iterator i = map.find(name);
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
tBindingClass::tBindingClass(tScriptEngine * engine) :
	tClassBase(ss_Binding, engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tBindingClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tBindingClass::ovulate);
	BindFunction(this, ss_construct, &tBindingInstance::construct);
	BindFunction(this, ss_initialize, &tBindingInstance::initialize);
	BindFunction(this, ss_eval, &tBindingInstance::eval);
	BindFunction(this, mnIGet, &tBindingInstance::iget);
	BindFunction(this, mnISet, &tBindingInstance::iset);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tBindingClass::ovulate()
{
	return tVariant(new tBindingInstance());
}
//---------------------------------------------------------------------------

} /* namespace Risse */

