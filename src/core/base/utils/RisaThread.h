//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief マルチスレッド関連ユーティリティ
//---------------------------------------------------------------------------
#ifndef _RISATHREAD_H
#define _RISATHREAD_H

#include <wx/thread.h>
#include "risse/include/risseUtils.h"


//---------------------------------------------------------------------------
//! @brief		クリティカルセクションクラス
//---------------------------------------------------------------------------
typedef tRisseCriticalSection tRisaCriticalSection;
//---------------------------------------------------------------------------


class tRisaThreadInternal;
//---------------------------------------------------------------------------
//! @brief		スレッドの基本クラス
//---------------------------------------------------------------------------
class tRisaThread
{
	friend class tRisaThreadInternal;

	tRisaCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

	volatile bool _Terminated; //!< スレッドが終了すべきかどうか
	volatile bool Get_Terminated() const { return _Terminated; }
	volatile void Set_Terminated(bool b) { _Terminated = b; }
	wxMutex ThreadMutex; //!< スレッドが終了するまで保持されるロック
	tRisaThreadInternal * Internal; //!< 内部スレッドの実装

public:
	tRisaThread();
	virtual ~tRisaThread();

	void Run(); // オーバーライド
	void Wait(); // オーバーライド

	void Terminate() { Set_Terminated(true); } //!< スレッドに終了を通知する

protected:
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
 tRisaThreadEvent はカウント上限が1のセマフォをカプセル化したもの。はっきり
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
class tRisaThreadEvent
{
	wxSemaphore Semaphore; //!< セマフォオブジェクト
public:
	//! @brief コンストラクタ
	tRisaThreadEvent() : Semaphore(0, 1) {;}

	//! @brief デストラクタ
	~tRisaThreadEvent() {;}

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



#endif
