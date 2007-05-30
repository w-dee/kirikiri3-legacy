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
tRisseObjectClass::tRisseObjectClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseObjectClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisseObjectClass::construct);
	RisseBindFunction(this, ss_initialize, &tRisseObjectClass::initialize);
	RisseBindFunction(this, ss_isA, &tRisseObjectClass::isA, 
		tRisseMemberAttribute().Set(tRisseMemberAttribute::vcConst).Set(tRisseMemberAttribute::ocFinal));
	RisseBindFunction(this, ss_eval, &tRisseObjectClass::eval);
	RisseBindFunction(this, ss_getInstanceMember, &tRisseObjectClass::getInstanceMember);
	RisseBindFunction(this, ss_setInstanceMember, &tRisseObjectClass::setInstanceMember);
	RisseBindFunction(this, ss_toException, &tRisseObjectClass::toException);
	RisseBindFunction(this, ss_p, &tRisseObjectClass::p);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseObjectClass::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseObjectClass::initialize()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseObjectClass::isA(const tRisseVariant & Class,
						const tRisseNativeBindFunctionCallingInfo & info)
{
	// 自身が引数(=クラス) のインスタンスかどうかを得る
	return info.This.InstanceOf(info.engine, Class);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseObjectClass::eval(const tRisseString & script,
							const tRisseNativeBindFunctionCallingInfo & info)
{
	// eval (式やスクリプトの評価)
	tRisseString name = info.args.HasArgument(1) ?
					tRisseString(info.args[1]) : tRisseString(RISSE_WS("(anonymous)"));
	risse_size lineofs = info.args.HasArgument(2) ? (risse_size)(risse_int64)info.args[2] : (risse_size)0;

	info.engine->Evaluate(script, name, lineofs, info.result,
							new tRisseBindingInfo(info.This), true);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseObjectClass::getInstanceMember(
	const tRisseNativeBindFunctionCallingInfo & info,
	const tRisseString & membername)
{
	// This のインスタンスメンバを取得する
	return info.This.GetPropertyDirect(info.engine, membername,
			tRisseMemberAttribute::GetDefault()|
			tRisseOperateFlags::ofInstanceMemberOnly,
			info.This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseObjectClass::setInstanceMember(
	const tRisseNativeBindFunctionCallingInfo & info,
	const tRisseString & membername, const tRisseVariant & value)
{
	// This のインスタンスメンバを設定する
	info.This.SetPropertyDirect(info.engine, membername,
		tRisseMemberAttribute::GetDefault()|
		tRisseOperateFlags::ofInstanceMemberOnly|
		tRisseOperateFlags::ofMemberEnsure,
					value, info.This);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseObjectClass::toException(const tRisseNativeBindFunctionCallingInfo & info)
{
	// デフォルトの動作は、This を文字列化してそれを RuntimeException.new 
	// に渡し、その結果を返す
	// TODO: global.RuntimeException を見に行かずに直接クラスを見に行っちゃっていいの？
	return tRisseVariant(info.engine->RuntimeExceptionClass).
			New(0, tRisseMethodArgument::New((tRisseString)info.This));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseObjectClass::p(const tRisseMethodArgument & args)
{
	for(risse_size i = 0; i < args.GetArgumentCount(); i++)
	{
		const tRisseVariant & v = args[i];

		if(v.GetType() == tRisseVariant::vtObject)
		{
			risse_char buf[40];
			Risse_pointer_to_str(v.GetObjectInterface(), buf);
			RisseFPrint(stdout, (tRisseString(RISSE_WS("Object@")) + buf).c_str());
			const tRisseVariant * context = v.GetContext();
			if(context)
			{
				if(context->GetType() == tRisseVariant::vtObject)
				{
					if(context == tRisseVariant::GetDynamicContext())
					{
						RisseFPrint(stdout, RISSE_WS(":dynamic"));
					}
					else
					{
						Risse_pointer_to_str(context->GetObjectInterface(), buf);
						RisseFPrint(stdout, (tRisseString(RISSE_WS(":")) + buf).c_str());
					}
				}
				else
				{
					RisseFPrint(stdout, (context->AsHumanReadable()).c_str());
				}
			}
		}
		else
		{
			RisseFPrint(stdout, (v.operator tRisseString()).c_str());
		}
	}
	RisseFPrint(stdout, RISSE_WS("\n"));
}
//---------------------------------------------------------------------------

} /* namespace Risse */

