/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief マルチスレッド関連ユーティリティ
//---------------------------------------------------------------------------
#ifndef _RISSETHREAD_H
#define _RISSETHREAD_H

/*! @note

※ もともとスレッドサポートは Risse にあった物を Risse に持ってきた物なので
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
	- Risse::tRisseCiriticalSection	問題なし

	wxCriticalSection は、再帰的な(再入可能な)クリティカルセクションを実現できる
	保証がない。boost::recursive_mutex はその名の通り再入可能な mutex だが、
	たとえば Windows の提供する CriticalSection よりは効率が悪い。
	Risse::tRisseCiriticalSection は、プラットフォームネイティブな
	クリティカルセクションを利用できる場合は利用するようになっているので
	もっとも効率がよい。

	ここでは、tRisseCriticalSection を typedef したものを tRisseCriticalSection
	として用いて使うこととする。


	CriticalSection は、プログラム終了時までに正常に破棄される保証はない
	(破棄されるよりも前にプログラムが終了する可能性がある)。通常は OS が
	mutex などを回収するし、Windows の CriticalSection にあっては特に回収
	すべき物など本来はないので問題は起きないと思われる。
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



//---------------------------------------------------------------------------
// クリティカルセクション
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
	//! @brief クリティカルセクションの実装
	class tRisseCriticalSection : public tRisseDestructee
	{
		CRITICAL_SECTION CS; //!< Win32 クリティカルセクションオブジェクト
	public:
		tRisseCriticalSection() { InitializeCriticalSection(&CS); } //!< コンストラクタ
		~tRisseCriticalSection() { DeleteCriticalSection(&CS); } //!< デストラクタ

	private:
		tRisseCriticalSection(const tRisseCriticalSection &); // non-copyable

	private:
		void Enter() { EnterCriticalSection(&CS); } //!< クリティカルセクションに入る
		void Leave() { LeaveCriticalSection(&CS); } //!< クリティカルセクションから出る

	public:
		//! @brief  クリティカルセクション用ロッカー
		class tLocker : public tRisseCollectee
		{
			tRisseCriticalSection & CS;
		public:
			tLocker(tRisseCriticalSection & cs) : CS(cs)
			{
				CS.Enter();
			}
			~tLocker()
			{
				CS.Leave();
			}
		private:
			tLocker(const tLocker &); // non-copyable
		};

		//! @brief  「条件によってはロックを行わない」クリティカルセクション用ロッカー
		class tConditionalLocker : public tRisseCollectee
		{
			char Locker[sizeof(tLocker)]; // !< tLockerを格納する先
			bool Locked; //!< 実際にロックが行われたかどうか
		public:
			//! @brief	コンストラクタ
			//! @param	cs		クリティカルセクションオブジェクトへのポインタ(NULLの場合はロックを行わない)
			tConditionalLocker(tRisseCriticalSection * cs)
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

			//! @brief	デストラクタ
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
	//! @brief クリティカルセクションの実装
	class tRisseCriticalSection : public tRisseCollectee
	{
		boost::recursive_mutex mutex; //!< boost::recursive_mutex mutexオブジェクト
	public:
		tRisseCriticalSection() { ; } //!< コンストラクタ
		~tRisseCriticalSection() { ; } //!< デストラクタ

	private:
		tRisseCriticalSection(const tRisseCriticalSection &); // non-copyable

	public:
		//! @brief  クリティカルセクション用ロッカー
		class tLocker : public tRisseCollectee
		{
			boost::recursive_mutex::scoped_lock lock;
		public:
			tLocker(tRisseCriticalSection & cs) : lock(cs.mutex) {;}
		private:
			tLocker(const tLocker &); // non-copyable
		};

		//! @brief  「条件によってはロックを行わない」クリティカルセクション用ロッカー
		class tConditionalLocker : public tRisseCollectee
		{
			char Storage[sizeof(tRisseCriticalSection)]; // !< tLockerを格納する先
			bool Locked; //!< 実際にロックが行われたかどうか
		public:
			//! @brief	コンストラクタ
			//! @param	cs		クリティカルセクションオブジェクトへのポインタ(NULLの場合はロックを行わない)
			tConditionalLocker(tRisseCriticalSection * cs)
			{
				if(cs)
				{
					Locked = true;
					new (reinterpret_cast<tLocker*>(Locker)) tLocker(*(cs));
				}
			}

			//! @brief	デストラクタ
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



//---------------------------------------------------------------------------


#else // #ifdef RISSE_SUPPORT_THREADS

//---------------------------------------------------------------------------
namespace Risse
{
//---------------------------------------------------------------------------

// スレッドをサポートしない場合は何もしない tRisseCriticalSection を定義する

	//! @brief クリティカルセクションの実装
	class tRisseCriticalSection : public tRisseDestructee
	{
	public:
		tRisseCriticalSection() { ; } //!< コンストラクタ
		~tRisseCriticalSection() { ; } //!< デストラクタ

	private:
		tRisseCriticalSection(const tRisseCriticalSection &); // non-copyable

	private:
		void Enter() { ; } //!< クリティカルセクションに入る
		void Leave() { ; } //!< クリティカルセクションから出る

	public:
		class tLocker : public tRisseCollectee
		{
		public:
			tLocker(tRisseCriticalSection & cs)
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
}  // namespace Risse

#endif // #ifdef RISSE_SUPPORT_THREADS










#ifdef RISSE_SUPPORT_THREADS
//---------------------------------------------------------------------------
namespace Risse 
{
//---------------------------------------------------------------------------


class tRisseThreadInternal;
//---------------------------------------------------------------------------
//! @brief		スレッドの基本クラス
//---------------------------------------------------------------------------
class tRisseThread : public tRisseDestructee
{
	friend class tRisseThreadInternal;

	tRisseCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

	volatile bool Started; //!< スレッドが実際に開始したかどうか
	volatile bool _Terminated; //!< スレッドが終了すべきかどうか
	volatile bool Get_Terminated() const { return _Terminated; }
	volatile void Set_Terminated(bool b) { _Terminated = b; }
	wxMutex ThreadMutex; //!< スレッドが終了するまで保持されるロック
	tRisseThreadInternal * Internal; //!< 内部スレッドの実装
	tRisseString Name; //!< スレッドの名前

public:
	//! @brief		コンストラクタ
	tRisseThread();

	//! @brief		デストラクタ
	virtual ~tRisseThread();

	const tRisseString GetName() const { return Name; } //!< スレッドの名前を得る

	//! @brief		スレッドの実行を開始する
	void Run();

	//! @brief		スレッドが終了するまで待つ
	//! @note		呼び出し時点でまだスレッドが実行されていない場合は実行・終了するまで待つ
	void Wait();

	void Terminate() { Set_Terminated(true); } //!< スレッドに終了を通知する

protected:
	//! @brief		スレッドが終了すべきかどうかを得る
	//! @return		スレッドが終了すべきであれば真
	volatile bool ShouldTerminate();

	//! @brief		スレッドのメイン関数(サブクラスで実装する)
	//! @note		以下のような形式で記述する
	//! <code>
	//! void Execute() {
	//!		// スレッドが終了すべき場合はループを抜ける
	//!		while(!ShouldTerminate()) {
	//!			何か処理
	//!		}
	//!	}
	//! </code>
	//! スレッドが終了する場合はかならずこの関数を抜けるようにし、
	//! Exit() は呼ばないこと。
	//! スレッド外からスレッドを終了させたい場合は Terminate を呼ぶ。
	virtual void Execute() = 0;
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		スレッドイベントクラス
/*! @note
 tRisseThreadEvent はカウント上限が1のセマフォをカプセル化したもの。はっきり
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
class tRisseThreadEvent : public tRisseDestructee
{
	wxSemaphore Semaphore; //!< セマフォオブジェクト
public:
	//! @brief コンストラクタ
	tRisseThreadEvent() : Semaphore(0, 1) {;}

	//! @brief デストラクタ
	~tRisseThreadEvent() {;}

	//! @brief どれか一つの待ちスレッドを解放する
	void Signal()
	{
	    wxLogNull logNo; // セマフォがオーバーフローするとエラーを吐くので抑止
		Semaphore.Post();
	}

	//! @brief 他のスレッドがSignalするのを待つ
	//! @param	timeout タイムアウト時間(ミリ秒) 0 = タイムアウトなし
	//! @return タイムアウトしたかどうか
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
