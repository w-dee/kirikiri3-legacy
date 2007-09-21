/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Object" クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "risseObjectClass.h"
#include "risseStaticStrings.h"
#include "risseExceptionClass.h"
#include "risseScriptEngine.h"
#include "risseBindingInfo.h"

/*
	Risseスクリプトから見える"Object" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(41134,45186,9497,17812,19604,2796,36426,21671);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tObjectClass::tObjectClass(tScriptEngine * engine) :
	tClassBase(engine)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tObjectClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_construct, &tObjectClass::construct);
	BindFunction(this, ss_initialize, &tObjectClass::initialize);
	BindFunction(this, mnDiscEqual, &tObjectClass::DiscEqual);
	BindFunction(this, ss_isA, &tObjectClass::isA, 
		tMemberAttribute().Set(tMemberAttribute::vcConst).Set(tMemberAttribute::ocFinal));
	BindFunction(this, ss_eval, &tObjectClass::eval);
	BindFunction(this, ss_getInstanceMember, &tObjectClass::getInstanceMember);
	BindFunction(this, ss_setInstanceMember, &tObjectClass::setInstanceMember);
	BindFunction(this, ss_toException, &tObjectClass::toException);
	BindFunction(this, ss_p, &tObjectClass::p);
	BindFunction(this, mnBoolean, &tObjectClass::toBoolean);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tObjectClass::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tObjectClass::initialize()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tObjectClass::DiscEqual(const tNativeCallInfo & info, const tVariant &rhs)
{
	// === 演算子
	if(info.This.GetType() == tVariant::vtObject)
	{
		if(rhs.GetType() != tVariant::vtObject) return false;
		return info.This.GetObjectInterface() == rhs.GetObjectInterface();
	}
	// プリミティブ型
	return info.This.DiscEqual(rhs);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool tObjectClass::isA(const tVariant & Class,
						const tNativeCallInfo & info)
{
	// 自身が引数(=クラス) のインスタンスかどうかを得る
	return info.This.InstanceOf(info.engine, Class);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tObjectClass::eval(const tString & script,
							const tNativeCallInfo & info)
{
	// eval (式やスクリプトの評価)
	tString name = info.args.HasArgument(1) ?
					tString(info.args[1]) : tString(RISSE_WS("(anonymous)"));
	risse_size lineofs = info.args.HasArgument(2) ? (risse_size)(risse_int64)info.args[2] : (risse_size)0;

	info.engine->Evaluate(script, name, lineofs, info.result,
							new tBindingInfo(info.This), true);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tObjectClass::getInstanceMember(
	const tNativeCallInfo & info,
	const tString & membername)
{
	// This のインスタンスメンバを取得する
	return info.This.GetPropertyDirect(info.engine, membername,
			tMemberAttribute::GetDefault()|
			tOperateFlags::ofInstanceMemberOnly,
			info.This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tObjectClass::setInstanceMember(
	const tNativeCallInfo & info,
	const tString & membername, const tVariant & value)
{
	// This のインスタンスメンバを設定する
	info.This.SetPropertyDirect(info.engine, membername,
		tMemberAttribute::GetDefault()|
		tOperateFlags::ofInstanceMemberOnly|
		tOperateFlags::ofMemberEnsure,
					value, info.This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tObjectClass::toException(const tNativeCallInfo & info)
{
	// デフォルトの動作は、This を文字列化してそれを RuntimeException.new 
	// に渡し、その結果を返す
	// TODO: global.RuntimeException を見に行かずに直接クラスを見に行っちゃっていいの？
	return tVariant(info.engine->RuntimeExceptionClass).
			New(0, tMethodArgument::New((tString)info.This));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tObjectClass::p(const tMethodArgument & args)
{
	for(risse_size i = 0; i < args.GetArgumentCount(); i++)
	{
		const tVariant & v = args[i];

		if(v.GetType() == tVariant::vtObject)
		{
			risse_char buf[40];
			::Risse::pointer_to_str(v.GetObjectInterface(), buf);
			FPrint(stdout, (tString(RISSE_WS("Object@")) + buf).c_str());
			const tVariant * context = v.GetContext();
			if(context)
			{
				if(context->GetType() == tVariant::vtObject)
				{
					if(context == tVariant::GetDynamicContext())
					{
						FPrint(stdout, RISSE_WS(":dynamic"));
					}
					else
					{
						::Risse::pointer_to_str(context->GetObjectInterface(), buf);
						FPrint(stdout, (tString(RISSE_WS(":")) + buf).c_str());
					}
				}
				else
				{
					FPrint(stdout, (context->AsHumanReadable()).c_str());
				}
			}
		}
		else
		{
			FPrint(stdout, (v.operator tString()).c_str());
		}
	}
	FPrint(stdout, RISSE_WS("\n"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tObjectClass::Bless(tObjectInterface * intf)
{
	// intf の RTTI にこのクラスの ClassRTTI を設定する
	intf->SetRTTI(&GetClassRTTI());

	// intf に class を登録し、Object クラスを指すようにする
	intf->Do(ocDSet, NULL, ss_class,
		tOperateFlags(tMemberAttribute::GetDefault()) |
		tOperateFlags::ofMemberEnsure|tOperateFlags::ofInstanceMemberOnly,
		tMethodArgument::New(tVariant((tObjectInterface *)this)) );
}
//---------------------------------------------------------------------------

} /* namespace Risse */

