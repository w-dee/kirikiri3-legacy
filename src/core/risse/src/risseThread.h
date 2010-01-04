/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief マルチスレッド関連ユーティリティ
//---------------------------------------------------------------------------
#ifndef _RISSETHREAD_H
#define _RISSETHREAD_H

/*! @note

※ もともとスレッドサポートは Risa にあった物を Risse に持ってきた物なので
   ここの説明は少し変です。 TODO: これ直して

Risse は wxWidgets と boost という２つのライブラリのスレッド実装の恩恵を
受けられる状態にあるが、それぞれは微妙に Risse の要求からはずれている。

ここでは、スレッド関連の実装の比較を行う。

以下の「問題あり」「問題なし」は、Risse の用途で問題があるか無いかということ
であり、それぞれの実装が単純に「ダメか」「良いか」ということを言っているの
ではない。

■ スレッドの実装

	- wxThread						問題あり
	- boost::thread					問題あり

	wxThread は設計上の問題があり、Risseの用途では使うことができない
	( Wait()やDelete()でメッセージループを動かしてしまう)

	boost::thread はスレッドの優先順位を指定できない。

	ここでは、wxThread の Wait() や Delete() でメッセージループを動かして
	しまうといった問題に対し、wxThread をベースに Wait の動作を独自に実装
	することで解決を図る。


■ CriticalSection の実装

	- wxCriticalSection				問題あり
	- boost::recursive_mutex		とくに問題なし
	- Risse::tCiriticalSection		問題なし

	wxCriticalSection は、再帰的な(再入可能な)クリティカルセクションを実現できる
	保証がない。boost::recursive_mutex はその名の通り再入可能な mutex だが、
	たとえば Windows の提供する CriticalSection よりは効率が悪い。
	Risse::tCiriticalSection は、プラットフォームネイティブな
	クリティカルセクションを利用できる場合は利用するようになっているので
	もっとも効率がよい。

	ここでは、tCriticalSection を typedef したものを tCriticalSection
	として用いて使うこととする。


	CriticalSection は、プログラム終了時までに正常に破棄される保証はない
	(破棄されるよりも前にプログラムが終了する可能性がある)。通常は OS が
	mutex などを回収するし、Windows の CriticalSection にあっては特に回収
	すべき物など本来はない(←半分嘘、mutexを内部に持っている場合がある)
	ので問題は起きないと思われる。
*/

#ifdef RISSE_SUPPORT_THREADS

/*
	というわけで、現状 Risse でスレッドを使うには wxWidgets が必要になる。
	wxWidgets を使いたくない場合は新たに書き起こすことにする。
*/


#ifndef RISSE_SUPPORT_WX
	#error "wxWidgets support is missing. Currently Risse needs wxWidgets to support threads."
#endif

#include <wx/log.h>
#include <wx/thread.h>
#include "risseTypes.h"
#include "risseString.h"
#include "risseGC.h"


//---------------------------------------------------------------------------
// クリティカルセクションとアトミックカウンタ
//---------------------------------------------------------------------------

/*
	Risse のクリティカルセクションは、再突入可能 ( recursive ) であることが
	保証できなくてはならない。
	この条件を満たすのは Win32 のクリティカルセクションや boost の
	recursive_mutex で、wxWidgets の wxCriticalSection はこの保証がないので
	使えない。
*/


#if _WIN32
	// Windows プラットフォームの場合

	#ifdef _MSC_VER
	#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>

	namespace Risse
	{
	/**
	 * クリティカルセクションの実装
	 */
	class tCriticalSection : public tDestructee
	{
		CRITICAL_SECTION CS; //!< Win32 クリティカルセクションオブジェクト
#ifdef RISSE_ASSERT_ENABLED
		DWORD LockingThreadId; //!< ロックを行っているスレッドID
		DWORD LockingNestCount; //!< ロックのネストカウント
#endif
	public:
		/**
		 * コンストラクタ
		 */
		tCriticalSection(){
#ifdef RISSE_ASSERT_ENABLED
			LockingThreadId = 0;
			LockingNestCount = 0;
#endif
			InitializeCriticalSection(&CS);
		}
		~tCriticalSection() { DeleteCriticalSection(&CS); } //!< デストラクタ

	private:
		tCriticalSection(const tCriticalSection &); // non-copyable

	private:
		void Enter() { EnterCriticalSection(&CS); } //!< クリティカルセクションに入る
		void Leave() { LeaveCriticalSection(&CS); } //!< クリティカルセクションから出る

#ifdef RISSE_ASSERT_ENABLED
	public:
		DWORD GetLockingThreadId() const { return LockingThreadId; }
#endif

	public:
		/**
		 * クリティカルセクション用ロッカー
		 */
		class tLocker : public tCollectee
		{
			tCriticalSection & CS;
		public:
			tLocker(tCriticalSection & cs) : CS(cs)
			{
				CS.Enter();
#ifdef RISSE_ASSERT_ENABLED
				if(CS.LockingNestCount == 0)
					CS.LockingThreadId = GetCurrentThreadId();
				CS.LockingNestCount ++;
#endif
			}
			~tLocker()
			{
#ifdef RISSE_ASSERT_ENABLED
				CS.LockingNestCount --;
				if(CS.LockingNestCount == 0)
					CS.LockingThreadId = 0;
#endif
				CS.Leave();
			}
		private:
			tLocker(const tLocker &); // non-copyable
		};

#ifdef RISSE_ASSERT_ENABLED
	#define RISSE_ASSERT_CS_LOCKED(x) \
		RISSE_ASSERT((x).GetLockingThreadId() == GetCurrentThreadId())
#endif

		/**
		 * 「条件によってはロックを行わない」クリティカルセクション用ロッカー
		 */
		class tConditionalLocker : public tCollectee
		{
			char Locker[sizeof(tLocker)]; // !< tLockerを格納する先
			bool Locked; //!< 実際にロックが行われたかどうか
		public:
			/**
			 * コンストラクタ
			 * @param cs	クリティカルセクションオブジェクトへのポインタ(NULLの場合はロックを行わない)
			 */
			tConditionalLocker(tCriticalSection * cs)
			{
				if(cs)
				{
					Locked = true;
					new (reinterpret_cast<tLocker*>(Locker)) tLocker(*(cs));
				}
				else
				{
					Locked = false;
				}
			}

			/**
			 * デストラクタ
			 */
			~tConditionalLocker()
			{
				if(Locked)
				{
					(reinterpret_cast<tLocker*>(Locker))->~tLocker();
				}
			}
		};
	};

	} // namespace Risse
#else

	// boost の recursive_mutex をつかう

	#include <boost/thread.hpp>
	#include "risseGC.h"

	namespace Risse
	{
	/**
	 * クリティカルセクションの実装
	 */
	class tCriticalSection : public tDestructee
	{
		boost::recursive_mutex mutex; //!< boost::recursive_mutex mutexオブジェクト
#ifdef RISSE_ASSERT_ENABLED
		pthread_t LockingThreadId; //!< ロックを行っているスレッドID
		unsigned long LockingNestCount; //!< ロックのネストカウント
#endif
	public:
		/**
		 * コンストラクタ
		 */
		tCriticalSection()
		{
#ifdef RISSE_ASSERT_ENABLED
			LockingThreadId = 0;
			LockingNestCount = 0;
#endif
		}
		/**
		 * デストラクタ
		 */
		~tCriticalSection()
		{
#ifdef RISSE_ASSERT_ENABLED
			RISSE_ASSERT(LockingNestCount == 0);
#endif
		}

	private:
		tCriticalSection(const tCriticalSection &); // non-copyable

#ifdef RISSE_ASSERT_ENABLED
	public:
		pthread_t GetLockingThreadId() const { return LockingThreadId; }
#endif

#ifdef RISSE_ASSERT_ENABLED
	#define RISSE_ASSERT_CS_LOCKED(x) \
		RISSE_ASSERT((x).GetLockingThreadId() == pthread_self())
#endif

	public:
		/**
		 * クリティカルセクション用ロッカー
		 */
		class tLocker : public tCollectee
		{
#ifdef RISSE_ASSERT_ENABLED
			tCriticalSection & CS;
#endif
			boost::recursive_mutex::scoped_lock lock;
		public:
			tLocker(tCriticalSection & cs) :
#ifdef RISSE_ASSERT_ENABLED
				CS(cs),
#endif
				lock(cs.mutex)
			{
#ifdef RISSE_ASSERT_ENABLED
				if(CS.LockingNestCount == 0)
					CS.LockingThreadId = pthread_self();
				CS.LockingNestCount ++;
#endif
			}

			~tLocker()
			{
#ifdef RISSE_ASSERT_ENABLED
				CS.LockingNestCount --;
				if(CS.LockingNestCount == 0)
					CS.LockingThreadId = 0;
#endif
			}
		private:
			tLocker(const tLocker &); // non-copyable
		};

		/**
		 * 「条件によってはロックを行わない」クリティカルセクション用ロッカー
		 */
		class tConditionalLocker : public tCollectee
		{
			char Locker[sizeof(tLocker)]; // !< tLockerを格納する先
			bool Locked; //!< 実際にロックが行われたかどうか
		public:
			/**
			 * コンストラクタ
			 * @param cs	クリティカルセクションオブジェクトへのポインタ(NULLの場合はロックを行わない)
			 */
			tConditionalLocker(tCriticalSection * cs)
			{
				if(cs)
				{
					Locked = true;
					new (reinterpret_cast<tLocker*>(Locker)) tLocker(*(cs));
				}
				else
				{
					Locked = false;
				}
			}

			/**
			 * デストラクタ
			 */
			~tConditionalLocker()
			{
				if(Locked)
				{
					(reinterpret_cast<tLocker*>(Locker))->~tLocker();
				}
			}
		};
	};

	} // namespace Risse
#endif


	namespace Risse
	{
	//---------------------------------------------------------------------------
	// アトミックカウンタ
	//---------------------------------------------------------------------------
	// boost::detail::atomic_count を参考にした
	#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
		// Windows 用実装
		// TODO: Win64は？

		/**
		 * アトミックカウンタ
		 */
		class tAtomicCounter : public tAtomicCollectee
		{
			mutable long v; //!< 値
			tAtomicCounter(const tAtomicCounter &); //!< non-copyable
			tAtomicCounter & operator = (const tAtomicCounter &); //!< non copyable

		public:
			/**
			 * デフォルトコンストラクタ
			 * @param v	初期値
			 */
			explicit tAtomicCounter() : v(0) { }

			/**
			 * コンストラクタ
			 * @param v	初期値
			 */
			explicit tAtomicCounter(long v_) : v(v_) { }

			/**
			 * インクリメントを行う(前置インクリメント演算子)
			 */
			long operator ++() { return ::InterlockedIncrement(&v); }

			/**
			 * デクリメントを行う(前置デクリメント演算子)
			 */
			long operator --() { return ::InterlockedDecrement(&v); }

			/**
			 * long へのキャスト
			 */
			operator long() const { return ::InterlockedExchangeAdd(&v, 0); }

			/**
			 * 0 にする (注意: non-atomic)
			 */
			void reset() { static_cast<long volatile &>(v) = 0; }
		};

	#elif defined(__GLIBCPP__) || defined(__GLIBCXX__)
		// GCC (GLIBCPP) 版

		#ifdef __GLIBCXX__
			using __gnu_cxx::__atomic_add;
			using __gnu_cxx::__exchange_and_add;
		#endif

		/**
		 * アトミックカウンタ
		 */
		class tAtomicCounter : public tAtomicCollectee
		{
			mutable _Atomic_word v; //!< 値
			tAtomicCounter(const tAtomicCounter &); //!< non-copyable
			tAtomicCounter & operator = (const tAtomicCounter &); //!< non copyable

		public:
			/**
			 * デフォルトコンストラクタ
			 * @param v	初期値
			 */
			explicit tAtomicCounter() : v(0) { }

			/**
			 * コンストラクタ
			 * @param v	初期値
			 */
			explicit tAtomicCounter(long v_) : v(v_) { }

			/**
			 * インクリメントを行う(前置インクリメント演算子)
			 */
			long operator ++() { return __exchange_and_add(&v, 1) + 1; }

			/**
			 * デクリメントを行う(前置デクリメント演算子)
			 */
			long operator --() { return __exchange_and_add(&v, -1) - 1; }

			/**
			 * long へのキャスト
			 */
			operator long() const { return __exchange_and_add(&v, 0); }

			/**
			 * 0 にする (注意: non-atomic)
			 */
			void reset() { static_cast<_Atomic_word volatile &>(v) = 0; }
		};

	#else
		#error "non-supported platform; write your own atomic-counter implementation here"
		/*
			実装に当たっては:
			デストラクタによる後処理が必要なクラスにしないでください。
			(boost の pthread 版 atomic_count がそのような困る例)
		*/
	#endif
	//---------------------------------------------------------------------------

	// デフォルトの RISSE_ASSERT_CS_LOCKED
	#ifndef RISSE_ASSERT_CS_LOCKED
		#define RISSE_ASSERT_CS_LOCKED(x)
	#endif


	} // namespace Risse


//---------------------------------------------------------------------------


#else // #ifdef RISSE_SUPPORT_THREADS

//---------------------------------------------------------------------------
namespace Risse
{
//---------------------------------------------------------------------------

// スレッドをサポートしない場合は何もしない tCriticalSection を定義する

	/**
	 * クリティカルセクションの実装
	 */
	class tCriticalSection : public tDestructee
	{
	public:
		tCriticalSection() { ; } //!< コンストラクタ
		~tCriticalSection() { ; } //!< デストラクタ

	private:
		tCriticalSection(const tCriticalSection &); // non-copyable

	private:
		void Enter() { ; } //!< クリティカルセクションに入る
		void Leave() { ; } //!< クリティカルセクションから出る

	public:
		class tLocker : public tCollectee
		{
		public:
			tLocker(tCriticalSection & cs)
			{
			}
			~tLocker()
			{
			}
		private:
			tLocker(const tLocker &); // non-copyable
		};
	};

//---------------------------------------------------------------------------


// スレッドをサポートしない場合は何もしない tAtomicCounter を定義する
	typedef long tAtomicCounter;


//---------------------------------------------------------------------------

}  // namespace Risse

#endif // #ifdef RISSE_SUPPORT_THREADS










#ifdef RISSE_SUPPORT_THREADS
//---------------------------------------------------------------------------
namespace Risse 
{
//---------------------------------------------------------------------------


class tThreadInternal;
//---------------------------------------------------------------------------
/**
 * スレッドの基本クラス
 */
class tThread : public tDestructee
{
	friend class tThreadInternal;

	tCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

	tAtomicCounter StartInitiated; //!< スレッドの開始指示をしたかどうか
	tAtomicCounter Started; //!< スレッドが実際に開始したかどうか
	tAtomicCounter _Terminated; //!< スレッドが終了すべきかどうか
	bool Get_Terminated() const { return (long)_Terminated != 0; }
	void Set_Terminated() { if(++_Terminated >= 2) --_Terminated; }
	wxMutex ThreadMutex; //!< スレッドが終了するまで保持されるロック
	tThreadInternal * Internal; //!< 内部スレッドの実装
	tString Name; //!< スレッドの名前

public:
	/**
	 * コンストラクタ
	 */
	tThread();

	/**
	 * デストラクタ
	 */
	virtual ~tThread();

	const tString GetName() const { return Name; } //!< スレッドの名前を得る

	/**
	 * スレッドの実行を開始する
	 */
	void Run();

	/**
	 * スレッドが終了するまで待つ
	 * @note	呼び出し時点でまだスレッドが実行されていない場合は実行・終了するまで待つ。
	 *			スレッドの実行が開始されてない場合は開始されるまで永遠に待つことになるので
	 *			注意。
	 */
	void Wait();

	void Terminate() { Set_Terminated(); } //!< スレッドに終了を通知する

	/**
	 * 現在実行しているスレッドがメインスレッドかどうかを帰す
	 */
	static bool IsMainThread() { return ::wxIsMainThread(); }

protected:
	/**
	 * スレッドが終了すべきかどうかを得る
	 * @return	スレッドが終了すべきであれば真
	 */
	volatile bool ShouldTerminate();

	/**
	 * スレッドのメイン関数を呼ぶ
	 * @note	Execute() メソッドを呼ぶこと。
	 *			下位クラスでオーバーライドする事も出来る。
	 */
	virtual void CallExecute() { Execute(); }

	/**
	 * スレッドのメイン関数(サブクラスで実装する)
	 * @note	以下のような形式で記述する
	 *			 <code>
	 *			void Execute() {
	 *			// スレッドが終了すべき場合はループを抜ける
	 *			while(!ShouldTerminate()) {
	 *			何か処理
	 *			}
	 *			}
	 *			 </code>
	 *			スレッドが終了する場合はかならずこの関数を抜けるようにし、
	 *			Exit() は呼ばないこと。
	 *			スレッド外からスレッドを終了させたい場合は Terminate を呼ぶ。
	 */
	virtual void Execute() = 0;
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
/**
 * スレッドイベントクラス
 */
/*! @note
 tThreadEvent はカウント上限が1のセマフォをカプセル化したもの。はっきり
 言ってこれぐらいではカプセル化する意味は無いかもしれないが、吉里吉里２が
 このタイプのスレッドイベントを多用していた影響でこのクラスを提供する。
 セマフォではなく Mutexでも良いのかもしれないが wxMutex はタイムアウト付きの
 「待ち」ができない。
 Wait() を呼ぶとSignalが呼ばれるか、タイムアウトするまで待つ。
 Signal() を呼ぶと、どれか一つの待ちスレッドを解放する。(スレッドが解放される
 までに何度Signal() を呼んでも解放されるのは１つだけ)。どのスレッドも待っていない
 場合は、次に別のスレッドがWaitを呼ぶと、そのWaitはすぐに戻るようになる。
 このクラスは待ちスレッドはいくつあってもよいが、Signal を実行するスレッドは
 かならず一つに限定する。Signal を実行するスレッドが複数の可能性がある場合は、
 同時には必ず一つのスレッドのみが Signal を実行するように CriticalSectionなど
 で保護すること。

 同時に解放できるスレッドの数を複数にしたり、どのスレッドも待っていない場合は
 どれかのスレッドが待ち始めるまで実行をブロックしたいといった場合は wxCondition
 を使うこと。
*/
//---------------------------------------------------------------------------
class tThreadEvent : public tDestructee
{
	wxSemaphore Semaphore; //!< セマフォオブジェクト
public:
	/**
	 * コンストラクタ
	 */
	tThreadEvent() : Semaphore(0, 1) {;}

	/**
	 * デストラクタ
	 */
	~tThreadEvent() {;}

	/**
	 * どれか一つの待ちスレッドを解放する
	 */
	void Signal()
	{
	    wxLogNull logNo; // セマフォがオーバーフローするとエラーを吐くので抑止
		Semaphore.Post();
	}

	/**
	 * 他のスレッドがSignalするのを待つ
	 * @param timeout	タイムアウト時間(ミリ秒) 0 = タイムアウトなし
	 * @return	タイムアウトしたかどうか
	 */
	bool Wait(unsigned long timeout = 0)
	{
		if(timeout == 0)
		{
			Semaphore.Wait();
			return false;
		}
		else
		{
			return wxSEMA_TIMEOUT == Semaphore.WaitTimeout(timeout);
		}
	}
};
//---------------------------------------------------------------------------

} // namespace Risse


#endif // #ifdef RISSE_SUPPORT_THREADS

#endif
