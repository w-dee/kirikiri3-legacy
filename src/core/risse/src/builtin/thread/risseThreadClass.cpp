/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Thread" クラスの実装
//---------------------------------------------------------------------------
#include "../../prec.h"
#include "../../risseTypes.h"
#include "risseThreadClass.h"
#include "../../risseThread.h"
#include "../../risseStaticStrings.h"
#include "../../risseObjectClass.h"
#include "../../risseScriptEngine.h"
#include "../../risseExceptionClass.h"

/*
	Risseスクリプトから見える"Thread" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(18367,25403,3163,16422,1923,3893,57628,424);


//---------------------------------------------------------------------------
/**
 * Risseスクリプト用スレッドを実装するクラス
 */
class tScriptThread : protected tThread
{
	//TODO: 正しくスレッドの状態をハンドリングすること
	tThreadInstance * Owner; //!< オーナー
#if 0
	tThreadEvent Event; //!< イベントオブジェクト
#endif

protected:
	/**
	 * スレッドの実行ルーチン
	 */
	void Execute()
	{
		if(Owner)
		{
			tScriptEngine * engine = Owner->GetRTTI()->GetScriptEngine();
			try
			{
				try
				{
					// Method を呼び出す
					// TODO: メソッドのコンテキストを正しく
					if(!Owner->Method.IsVoid())
						Owner->Method.FuncCall(engine, &Owner->Ret,
							tString::GetEmptyString(), 0,
							tMethodArgument::Empty(), Owner->Context);
				}
				catch(const tTemporaryException * te)
				{
					te->ThrowConverted(engine);
				}
			}
			catch(const tVariant * e)
			{
				// 例外を受け取った
				// Owner が例外を参照できるように例外オブジェクトを設定する
				Owner->Exception = e;
			}
		}

		// スレッドが存在している間は、これらはスタック上などから参照可能な位置にあるが、
		// スレッドが無くなったとき (GCからも見えなくなったとき)、また他のポインタも
		// とぎれたとき、これらのポインタが残っていると、finalization が
		// 必要なオブジェクトを巻き込んだ循環参照に
		// はまることになる。それを避けるため、このオブジェクトが持っているポインタの類を
		// すべてここで無効にする。
		Owner = NULL;
	}

public:
	/**
	 * コンストラクタ
	 */
	tScriptThread()
	{
		Owner = NULL;
	}

	/**
	 * スレッドの実行を開始する
	 * @param owner	オーナーとなる tThreadInstance のインスタンス
	 */
	void Start(tThreadInstance * owner)
	{
		Owner = owner;
		Run(); // tThread のメソッドのネーミングは Risse スクリプト上の物と違うので注意
	}

	/**
	 * スレッドの終了を待つ
	 * @return	スレッドメソッドの戻り値
	 */
	void Join()
	{
		Wait(); // tThread のメソッドのネーミングは Risse スクリプト上の物と違うので注意
	}
#if 0
	/**
	 * スレッドを一定時間停止する
	 * @param timeout	タイムアウト(-1=無限に待つ)
	 * @return	タイムアウトした場合は真、Wakeup により Sleep が中断された場合は偽
	 * @note	他のスレッドからこのメソッドを呼び出した場合、呼び出したスレッドが待つ事になるが
	 *			想定外の使い方である (将来的にはそのようなことを行った場合は例外を吐くように
	 *			するかもしれない)
	 */
	bool Sleep(risse_int64 timeout)
	{
		if(timeout == 0) return true; // すぐに戻る
		if(timeout < 0)
		{
			return Event.Wait(0); // 無限に待つ
		}
		// Event.Wait は unsigned long 型の引数を取る。
		// これは risse_int64 よりもサイズが小さい場合がある
		// そのため、一度に max_one_wait ずつ待つことにする。
		const unsigned long max_one_wait = 1<<(sizeof(unsigned long)*8-2);
		while(timeout > max_one_wait)
		{
			if(!Event.Wait(max_one_wait)) return false;
				// タイムアウトしなかった場合はすぐに戻る
			timeout -= max_one_wait;
		}
		// この時点で timeout は 1 以上 max_one_wait 以下
		return Event.Wait(static_cast<unsigned long>(timeout));
	}

	/**
	 * Sleep しているスレッドの Sleep を解除する
	 * @note	スレッドが Sleep していない場合は、スレッドが次回 Sleep した
	 *			場合はその Sleep はすぐに false を伴って戻るので注意。
	 *			呼び出し側でこれのCriticalSectionによる保護を行うこと。
	 */
	void Wakeup()
	{
		Event.Signal();
	}
#endif
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tThreadInstance::tThreadInstance()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tThreadInstance::construct()
{
	volatile tSynchronizer sync(this); // sync

	// デフォルトでは run メソッドを実行するように設定する
	Method = GetPropertyDirect(ss_run);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tThreadInstance::initialize(const tNativeCallInfo & info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();

	// ブロック引数があればそれを実行するようにする
	if(info.args.GetBlockArgumentCount() >= 1)
		Method = info.args.GetBlockArgument(0);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tThreadInstance::run(void) const
{
	// デフォルトの run は何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tThreadInstance::start()
{
	// スレッドの実行を開始する
	{
		volatile tSynchronizer sync(this); // sync

		if(Thread) return; // already started
		Thread = new tScriptThread();
	}

	Context = (tObjectInterface*)this;

	Thread->Start(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tThreadInstance::join() const
{
	// スレッドの終了を待機する
	if(!Thread) return Ret; // no thread
	Thread->Join();
	if(Exception) throw Exception; // 例外が発生していた場合はそれを再び投げる
	return Ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tThreadInstance::sleep(risse_int64 timeout)
{
#if 0
	if(!Thread) return false;
	return Thread->Sleep(timeout);
#endif
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tThreadInstance::wakeup()
{
#if 0
	volatile tSynchronizer sync(this); // sync
	if(!Thread) return;
	Thread->Wakeup();
#endif
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tThreadClass, ss_Thread, engine->ObjectClass, new tThreadInstance())
	BindFunction(this, ss_ovulate, &tThreadClass::ovulate);
	BindFunction(this, ss_construct, &tThreadInstance::construct);
	BindFunction(this, ss_initialize, &tThreadInstance::initialize);
	BindFunction(this, ss_run, &tThreadInstance::run);
	BindFunction(this, ss_start, &tThreadInstance::start);
	BindFunction(this, ss_join, &tThreadInstance::join);
#if 0
	BindFunction(this, ss_sleep, &tThreadInstance::sleep);
	BindFunction(this, ss_wakeup, &tThreadInstance::wakeup);
#endif
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
tThreadPackageInitializer::tThreadPackageInitializer() :
	tBuiltinPackageInitializer(ss_thread)
{
	ThreadClass = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tThreadPackageInitializer::Initialize(tScriptEngine * engine, const tString & name,
		const tVariant & global)
{
	ThreadClass = new tThreadClass(engine);
	ThreadClass->RegisterInstance(global);
}
//---------------------------------------------------------------------------



} /* namespace Risse */

