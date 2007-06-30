/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Thread" クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risseTypes.h"
#include "risseThreadClass.h"
#include "risseThread.h"
#include "risseStaticStrings.h"
#include "risseObjectClass.h"
#include "risseScriptEngine.h"
#include "risseExceptionClass.h"

/*
	Risseスクリプトから見える"Thread" クラスの実装
*/

namespace Risse
{
RISSE_DEFINE_SOURCE_ID(18367,25403,3163,16422,1923,3893,57628,424);


//---------------------------------------------------------------------------
//! @brief		Risseスクリプト用スレッドを実装するクラス
//---------------------------------------------------------------------------
class tRisseScriptThread : protected tRisseThread
{
	//TODO: 正しくスレッドの状態をハンドリングすること
	tRisseThreadInstance * Owner; //!< オーナー
#if 0
	tRisseThreadEvent Event; //!< イベントオブジェクト
#endif

protected:
	//! @brief		スレッドの実行ルーチン
	void Execute()
	{
		if(Owner)
		{
			tRisseScriptEngine * engine = Owner->GetRTTI()->GetScriptEngine();
			try
			{
				try
				{
					// Method を呼び出す
					// TODO: メソッドのコンテキストを正しく
					if(!Owner->Method.IsVoid())
						Owner->Method.FuncCall(engine, &Owner->Ret,
							tRisseString::GetEmptyString(), 0,
							tRisseMethodArgument::Empty(), Owner->Context);
				}
				catch(const tRisseTemporaryException * te)
				{
					te->ThrowConverted(engine);
				}
			}
			catch(const tRisseVariant * e)
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
	//! @brief	コンストラクタ
	tRisseScriptThread()
	{
		Owner = NULL;
	}

	//! @brief	スレッドの実行を開始する
	//! @param	owner		オーナーとなる tRisseThreadInstance のインスタンス
	void Start(tRisseThreadInstance * owner)
	{
		Owner = owner;
		Run(); // tRisseThread のメソッドのネーミングは Risse スクリプト上の物と違うので注意
	}

	//! @brief	スレッドの終了を待つ
	//! @return	スレッドメソッドの戻り値
	void Join()
	{
		Wait(); // tRisseThread のメソッドのネーミングは Risse スクリプト上の物と違うので注意
	}
#if 0
	//! @brief	スレッドを一定時間停止する
	//! @param	timeout		タイムアウト(-1=無限に待つ)
	//! @return	タイムアウトした場合は真、Wakeup により Sleep が中断された場合は偽
	//! @note	他のスレッドからこのメソッドを呼び出した場合、呼び出したスレッドが待つ事になるが
	//!			想定外の使い方である (将来的にはそのようなことを行った場合は例外を吐くように
	//!			するかもしれない)
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

	//! @brief	Sleep しているスレッドの Sleep を解除する
	//! @note	スレッドが Sleep していない場合は、スレッドが次回 Sleep した
	//!			場合はその Sleep はすぐに false を伴って戻るので注意。
	//! 		呼び出し側でこれのCriticalSectionによる保護を行うこと。
	void Wakeup()
	{
		Event.Signal();
	}
#endif
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tRisseThreadInstance::tRisseThreadInstance()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseThreadInstance::construct()
{
	// 何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseThreadInstance::initialize(const tRisseNativeCallInfo & info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();

	// ブロック引数があればそれを実行、無ければ run メソッドを実行するように設定する
	if(info.args.GetBlockArgumentCount() >= 1)
		Method = info.args.GetBlockArgument(0);
	else
		Method = info.This.GetPropertyDirect_Object(ss_run);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseThreadInstance::run(void) const
{
	// デフォルトの run は何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseThreadInstance::start()
{
	// スレッドの実行を開始する
	{
		volatile tSynchronizer sync(this); // sync

		if(Thread) return; // already started
		Thread = new tRisseScriptThread();
	}

	Context = (tRisseObjectInterface*)this;

	Thread->Start(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseThreadInstance::join() const
{
	// スレッドの終了を待機する
	if(!Thread) return Ret; // no thread
	Thread->Join();
	if(Exception) throw Exception; // 例外が発生していた場合はそれを再び投げる
	return Ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisseThreadInstance::sleep(risse_int64 timeout)
{
#if 0
	if(!Thread) return false;
	return Thread->Sleep(timeout);
#endif
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseThreadInstance::wakeup()
{
#if 0
	volatile tSynchronizer sync(this); // sync
	if(!Thread) return;
	Thread->Wakeup();
#endif
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
tRisseThreadClass::tRisseThreadClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseThreadClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_ovulate, &tRisseThreadClass::ovulate);
	RisseBindFunction(this, ss_construct, &tRisseThreadInstance::construct);
	RisseBindFunction(this, ss_initialize, &tRisseThreadInstance::initialize);
	RisseBindFunction(this, ss_run, &tRisseThreadInstance::run);
	RisseBindFunction(this, ss_start, &tRisseThreadInstance::start);
	RisseBindFunction(this, ss_join, &tRisseThreadInstance::join);
#if 0
	RisseBindFunction(this, ss_sleep, &tRisseThreadInstance::sleep);
	RisseBindFunction(this, ss_wakeup, &tRisseThreadInstance::wakeup);
#endif
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisseThreadClass::ovulate()
{
	return tRisseVariant(new tRisseThreadInstance());
}
//---------------------------------------------------------------------------

} /* namespace Risse */

