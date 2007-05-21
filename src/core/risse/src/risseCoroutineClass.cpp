/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Coroutine" クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "risseCoroutineClass.h"
#include "risseCoroutine.h"
#include "risseNativeFunction.h"
#include "risseNativeProperty.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "risseScriptEngine.h"

/*
	Risseスクリプトから見える"Coroutine" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(25338,19169,41250,19084,53674,59044,24121,36918);
//---------------------------------------------------------------------------
tRisseCoroutineInstance::tRisseCoroutineInstance()
{
	Coroutine = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCoroutineInstance::MakeCoroutine(const tRisseVariant & function)
{
	Coroutine = new tRisseCoroutine(GetRTTI()->GetScriptEngine(), function, tRisseVariant(this));
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tRisseCoroutineClass::tRisseCoroutineClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCoroutineClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_construct)
	{
		// 特にやること無し
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_initialize)
	{
		// 引数の数チェック
		args.ExpectBlockArgumentCount(1);

		// 親クラスの同名メソッドを呼び出す
		engine->CoroutineClass->CallSuperClassMethod(NULL, ss_initialize, 0, args, This);

		// 引数を元にコルーチンを作成する
		// TODO: ブロック引数だけでなくコンストラクタ引数としても関数オブジェクトを渡せるように
		tRisseCoroutineInstance * obj = This.CheckAndGetObjectInterafce<tRisseCoroutineInstance, tRisseClassBase>(engine->CoroutineClass);
		obj->MakeCoroutine(args.GetBlockArgument(0));
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_run)
	{
		tRisseCoroutineInstance * obj = This.CheckAndGetObjectInterafce<tRisseCoroutineInstance, tRisseClassBase>(engine->CoroutineClass);

		obj->GetCoroutine().Run(args.HasArgument(0)?tRisseVariant::GetVoidObject():args[0]);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_yield)
	{
		tRisseCoroutineInstance * obj = This.CheckAndGetObjectInterafce<tRisseCoroutineInstance, tRisseClassBase>(engine->CoroutineClass);

		obj->GetCoroutine().DoYield(args.HasArgument(0)?tRisseVariant::GetVoidObject():args[0]);
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_METHOD(ss_exit)
	{
		tRisseCoroutineInstance * obj = This.CheckAndGetObjectInterafce<tRisseCoroutineInstance, tRisseClassBase>(engine->CoroutineClass);

		obj->GetCoroutine().Exit();
	}
	RISSE_END_NATIVE_METHOD

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	RISSE_BEGIN_NATIVE_PROPERTY(ss_alive)
	{
		RISSE_BEGINE_NATIVE_PROPERTY_GETTER
		{
			tRisseCoroutineInstance * obj = This.CheckAndGetObjectInterafce<tRisseCoroutineInstance, tRisseClassBase>(engine->CoroutineClass);

			if(result) *result = obj->GetCoroutine().GetAlive();
		}
		RISSE_END_NATIVE_PROPERTY_GETTER
	}
	RISSE_END_NATIVE_PROPERTY

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseCoroutineClass::CreateNewObjectBase()
{
	return tRisseVariant(new tRisseCoroutineInstance());
}
//---------------------------------------------------------------------------

} /* namespace Risse */

