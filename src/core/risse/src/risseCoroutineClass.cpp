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
void tRisseCoroutineInstance::construct()
{
	// コルーチンの実装オブジェクトを作成
	Coroutine = new tRisseCoroutine(GetRTTI()->GetScriptEngine(),
		tRisseVariant::GetNullObject(), tRisseVariant(this));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCoroutineInstance::initialize(const tRisseNativeCallInfo & info)
{
	// 引数の数チェック
	info.args.ExpectBlockArgumentCount(1);

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();

	// 引数を元にコルーチンを作成する
	// TODO: ブロック引数だけでなくコンストラクタ引数としても関数オブジェクトを渡せるように
	Coroutine->SetFunction(info.args.GetBlockArgument(0));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseCoroutineInstance::run(const tRisseMethodArgument & args) const
{
#ifdef RISSE_COROUTINE_DEBUG
	fflush(stdout); fflush(stderr);
	fprintf(stdout, "in tRisseCoroutineInstance::run b: tRisseCoroutine %p: tRisseCoroutineInstance %p\n",
					Coroutine, this);
	fflush(stdout); fflush(stderr);
#endif

	return Coroutine->Run(args.HasArgument(0)?args[0]:tRisseVariant::GetVoidObject());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseCoroutineInstance::yield(const tRisseMethodArgument & args) const
{
#ifdef RISSE_COROUTINE_DEBUG
	fflush(stdout); fflush(stderr);
	fprintf(stdout, "in tRisseCoroutineInstance::yield b: tRisseCoroutine %p: tRisseCoroutineInstance %p\n",
					Coroutine, this);
	fflush(stdout); fflush(stderr);

	GC_gcollect();

	fflush(stdout); fflush(stderr);
	fprintf(stdout, "in tRisseCoroutineInstance::yield a: tRisseCoroutine %p: tRisseCoroutineInstance %p\n",
					Coroutine, this);
	fflush(stdout); fflush(stderr);
#endif

	return Coroutine->DoYield(args.HasArgument(0)?args[0]:tRisseVariant::GetVoidObject());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseCoroutineInstance::dispose() const
{
	Coroutine->Dispose();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseCoroutineInstance::get_alive() const
{
	return Coroutine->GetAlive();
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

	RisseBindFunction(this, ss_construct, &tRisseCoroutineInstance::construct);
	RisseBindFunction(this, ss_initialize, &tRisseCoroutineInstance::initialize);
	RisseBindFunction(this, ss_run, &tRisseCoroutineInstance::run);
	RisseBindFunction(this, ss_yield, &tRisseCoroutineInstance::yield);
	RisseBindFunction(this, ss_dispose, &tRisseCoroutineInstance::dispose);
	RisseBindProperty(this, ss_alive, &tRisseCoroutineInstance::get_alive);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseCoroutineClass::CreateNewObjectBase()
{
	return tRisseVariant(new tRisseCoroutineInstance());
}
//---------------------------------------------------------------------------

} /* namespace Risse */

