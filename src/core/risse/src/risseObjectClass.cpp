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
#include "risseNativeFunction.h"
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

	// construct, initialize などは新しいオブジェクトのコンテキスト上で実行されるので
	// コンテキストとしては null を指定する

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// デフォルトでは何もしない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// デフォルトでは何もしない
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD_OPTION(ss_isA, (attribute.Set(tRisseMemberAttribute::vcConst).Set(tRisseMemberAttribute::ocFinal)))
	{
		// 自身が引数(=クラス) のインスタンスかどうかを得る
		// 引数チェック
		args.ExpectArgumentCount(1);

		tRisseVariant ret = This.InstanceOf(engine, args[0]);
		if(result) *result = ret;
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_eval)
	{
		// eval (式やスクリプトの評価)
		args.ExpectArgumentCount(1);

		tRisseString script = args[0];
		tRisseString name = args.HasArgument(1) ?
						tRisseString(args[1]) : tRisseString(RISSE_WS("(anonymous)"));
		risse_size lineofs = args.HasArgument(2) ? (risse_size)(risse_int64)args[2] : (risse_size)0;

		engine->Evaluate(script, name, lineofs, result, new tRisseBindingInfo(This), true);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_getInstanceMember)
	{
		// 引数チェック
		args.ExpectArgumentCount(1);

		// This のインスタンスメンバを取得する
		tRisseVariant ret = 
			This.GetPropertyDirect(engine, args[0],
				tRisseMemberAttribute::GetDefault()|
				tRisseOperateFlags::ofInstanceMemberOnly,
				This);
		if(result) *result = ret;
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_setInstanceMember)
	{
		// 引数チェック
		args.ExpectArgumentCount(2);

		// This のインスタンスメンバを設定する
		This.SetPropertyDirect(engine, args[0],
			tRisseMemberAttribute::GetDefault()|
			tRisseOperateFlags::ofInstanceMemberOnly|
			tRisseOperateFlags::ofMemberEnsure,
						args[1], This);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_toException)
	{
		// デフォルトの動作は、This を文字列化してそれを RuntimeException.new 
		// に渡し、その結果を返す
		// TODO: global.RuntimeException を見に行かずに直接シングルトンインスタンスに
		//		バインドしちゃっていいの？
		tRisseVariant ret = tRisseVariant(engine->RuntimeExceptionClass).
				New(0, tRisseMethodArgument::New((tRisseString)This));
		if(result) *result = ret;
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_p)
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
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*

	RISSE_BEGIN_NATIVE_METHOD(ss_puts)
	{
		// This を標準出力に出力する
		RisseFPrint(stdout, This.operator tRisseString().c_str());
		// 改行する
		RisseFPrint(stdout, RISSE_WS("\n"));
	}
	RISSE_END_NATIVE_METHOD
*/
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}
//---------------------------------------------------------------------------

} /* namespace Risse */

