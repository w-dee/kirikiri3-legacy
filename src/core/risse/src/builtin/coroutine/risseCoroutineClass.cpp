/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Coroutine" クラスの実装
//---------------------------------------------------------------------------
#include "../../prec.h"
#include "../../risseTypes.h"
#include "risseCoroutineClass.h"
#include "risseCoroutine.h"
#include "../../risseStaticStrings.h"
#include "../../risseObjectClass.h"
#include "../../risseScriptEngine.h"

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

	return Coroutine->Resume(args.Get(0, tVariant::GetVoidObject()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tCoroutineInstance::yield(const tMethodArgument & args) const
{
	volatile tSynchronizer sync(this); // sync

	GC_gcollect();
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

	return Coroutine->DoYield(args.Get(0, tVariant::GetVoidObject()));
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
RISSE_IMPL_CLASS_BEGIN(tCoroutineClass, ss_Coroutine, engine->ObjectClass)
	RISSE_BIND_CONSTRUCTORS
	BindFunction(this, ss_run, &tCoroutineInstance::run);
	BindFunction(this, ss_resume, &tCoroutineInstance::resume);
	BindFunction(this, ss_yield, &tCoroutineInstance::yield);
	BindFunction(this, ss_dispose, &tCoroutineInstance::dispose);
	BindProperty(this, ss_alive, &tCoroutineInstance::get_alive);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tCoroutineExceptionClass, ss_CoroutineException, engine->RuntimeExceptionClass)
	BindFunction(this, ss_construct, &tCoroutineExceptionClass::construct);
	BindFunction(this, ss_initialize, &tCoroutineExceptionClass::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCoroutineExceptionClass::construct()
{
	// 特にやることはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCoroutineExceptionClass::initialize(const tNativeCallInfo & info)
{
	// 親クラスの同名メソッドを呼び出す(引数はそのまま)
	info.InitializeSuperClass(info.args);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCoroutineExceptionClass::ThrowCoroutineHasAlreadyExited(tScriptEngine * engine)
{
	tTemporaryException * e =
		new tTemporaryException(ss_coroutine, ss_CoroutineException,
					tString(RISSE_WS_TR("coroutine has already exited")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCoroutineExceptionClass::ThrowCoroutineHasNotStartedYet(tScriptEngine * engine)
{
	tTemporaryException * e =
		new tTemporaryException(ss_coroutine, ss_CoroutineException,
					tString(RISSE_WS_TR("coroutine has not started yet")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCoroutineExceptionClass::ThrowCoroutineIsNotRunning(tScriptEngine * engine)
{
	tTemporaryException * e =
		new tTemporaryException(ss_coroutine, ss_CoroutineException,
					tString(RISSE_WS_TR("coroutine is not running")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCoroutineExceptionClass::ThrowCoroutineIsRunning(tScriptEngine * engine)
{
	tTemporaryException * e =
		new tTemporaryException(ss_coroutine, ss_CoroutineException,
					tString(RISSE_WS_TR("coroutine is currently running")));
	if(engine) e->ThrowConverted(engine); else throw e;
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
tCoroutinePackageInitializer::tCoroutinePackageInitializer() :
	tBuiltinPackageInitializer(ss_coroutine)
{
	CoroutineClass = NULL;
	CoroutineExceptionClass = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCoroutinePackageInitializer::Initialize(tScriptEngine * engine, const tString & name,
		const tVariant & global)
{
	InitCoroutine();
	CoroutineClass = new tCoroutineClass(engine);
	CoroutineClass->RegisterInstance(global);
	CoroutineExceptionClass = new tCoroutineExceptionClass(engine);
	CoroutineExceptionClass->RegisterInstance(global);
}
//---------------------------------------------------------------------------
} /* namespace Risse */

