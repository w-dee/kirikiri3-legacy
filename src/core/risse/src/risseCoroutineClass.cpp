/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
tCoroutineInstance::tCoroutineInstance()
{
	Coroutine = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCoroutineInstance::construct()
{
	volatile tSynchronizer sync(this); // sync

	// コルーチンの実装オブジェクトを作成
	Coroutine = new tCoroutine(GetRTTI()->GetScriptEngine(),
		tVariant::GetNullObject(), tVariant(this));

	// デフォルトでは run メソッドを実行するようにする
	Coroutine->SetFunction(GetPropertyDirect(ss_run));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCoroutineInstance::initialize(const tNativeCallInfo & info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();

	// ブロック引数があればそれを実行、無ければ run メソッドを実行するように設定する
	if(info.args.GetBlockArgumentCount() >= 1)
		Coroutine->SetFunction(info.args.GetBlockArgument(0));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCoroutineInstance::run() const
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tCoroutineInstance::resume(const tMethodArgument & args) const
{
	volatile tSynchronizer sync(this); // sync

#ifdef RISSE_CORO_DEBUG
	fflush(stdout); fflush(stderr);
	fprintf(stdout, "in tCoroutineInstance::resume b: tCoroutine %p: tCoroutineInstance %p\n",
					Coroutine, this);
	fflush(stdout); fflush(stderr);
#endif

	return Coroutine->Resume(args.HasArgument(0)?args[0]:tVariant::GetVoidObject());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tCoroutineInstance::yield(const tMethodArgument & args) const
{
	volatile tSynchronizer sync(this); // sync

#ifdef RISSE_CORO_DEBUG
	fflush(stdout); fflush(stderr);
	fprintf(stdout, "in tCoroutineInstance::yield b: tCoroutine %p: tCoroutineInstance %p\n",
					Coroutine, this);
	fflush(stdout); fflush(stderr);

	GC_gcollect();

	fflush(stdout); fflush(stderr);
	fprintf(stdout, "in tCoroutineInstance::yield a: tCoroutine %p: tCoroutineInstance %p\n",
					Coroutine, this);
	fflush(stdout); fflush(stderr);
#endif

	return Coroutine->DoYield(args.HasArgument(0)?args[0]:tVariant::GetVoidObject());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCoroutineInstance::dispose() const
{
	volatile tSynchronizer sync(this); // sync

	Coroutine->Dispose();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tCoroutineInstance::get_alive() const
{
	volatile tSynchronizer sync(this); // sync

	return Coroutine->GetAlive();
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
tCoroutineClass::tCoroutineClass(tScriptEngine * engine) :
	tClassBase(ss_Coroutine, engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCoroutineClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tCoroutineClass::ovulate);
	BindFunction(this, ss_construct, &tCoroutineInstance::construct);
	BindFunction(this, ss_initialize, &tCoroutineInstance::initialize);
	BindFunction(this, ss_run, &tCoroutineInstance::run);
	BindFunction(this, ss_resume, &tCoroutineInstance::resume);
	BindFunction(this, ss_yield, &tCoroutineInstance::yield);
	BindFunction(this, ss_dispose, &tCoroutineInstance::dispose);
	BindProperty(this, ss_alive, &tCoroutineInstance::get_alive);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tCoroutineClass::ovulate()
{
	return tVariant(new tCoroutineInstance());
}
//---------------------------------------------------------------------------

} /* namespace Risse */

