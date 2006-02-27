//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 高精度タイマーの実装
//---------------------------------------------------------------------------
#ifndef _RISATIMER_H
#define _RISATIMER_H

#include "base/utils/Singleton.h"
#include "base/utils/Thread.h"
#include "base/event/TickCount.h"
#include "base/event/Event.h"





class tRisaTimerConsumer;

//---------------------------------------------------------------------------
//! @brief		タイマーのタイミングを管理するクラス(スケジューラ)
//---------------------------------------------------------------------------
class tRisaTimerScheduler : depends_on<tRisaTickCount>,
							public tRisaThread
{
	tRisaCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション
	tRisaThreadEvent Event; //!< イベント
	typedef std::vector<tRisaTimerConsumer*> tConsumers; //!< tRisaTimerConsumer の配列 の typedef
	tConsumers Consumers; //!< tRisaTimerConsumer の配列

	risse_uint64 NearestTick; //!< もっとも時間的に近い位置にあるTick (tRisaTickCount::InvalidTickCount の場合は無効)
	size_t NearestIndex; //!< もっちも時間的に近い位置にある Consumer のConsumers内におけるインデックス
	volatile bool NearestInfoValid; //!< Nearest* のメンバの情報が有効かどうか
	volatile bool NeedRescheduleOnPeriodChange;
		//!< tRisaTimerConsumer::SetNextTick内でReschedule()が呼ばれたときに
		//!< 本当にRescheduleする必要があるかどうか

public:
	tRisaTimerScheduler();
	~tRisaTimerScheduler();


private:
	friend class tRisaTimerConsumer;

	void Register(tRisaTimerConsumer * consumer);
	void Unregister(tRisaTimerConsumer * consumer);
	void Reschedule();
	void GetNearestInfo();
protected:
	void Execute();
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief		タイマーのタイミング発生先となるクラス
//---------------------------------------------------------------------------
class tRisaTimerConsumer
{
	tRisaTimerScheduler * Owner;
	risse_uint64 NextTick; // 次に OnPeriod を呼ぶべき絶対Tick
protected:
	tRisaTimerConsumer(tRisaTimerScheduler * owner);
	virtual ~tRisaTimerConsumer();
public:
	risse_uint64 GetNextTick() const { return NextTick; } //!< 次にOnPeriodをコールバックすべき絶対TickCountを返す
	void SetNextTick(risse_uint64 nexttick);

	//! @brief	指定されたTickCountに達したときに呼び出される
	//! @param	scheduled_tick GetNextTick() が返した Tick (本来呼ばれるべき時点の絶対TickCount)
	//! @param	current_tick この OnPeriod が呼ばれた時点でのtick カウント(実際に呼ばれた時点での絶対TickCount)
	//! @note	このメソッドはなるべくすぐに帰ること。
	//!			このメソッドの実行中はスケジューラがlockされているので
	//!			長時間の処理を行ってはならない。
	//!			また、このメソッド内でこのオブジェクトを削除しないこと。
	//! @note	OnPeriod内ではSetNextTickを使って次の絶対Tickを設定すること。
	//!			さもないと、その絶対Tickに対して何度も(不特定回数) OnPeriod が呼ばれることになる。
	//! @note	このメソッドはタイマ用スレッドから呼ばれる。必要であれば
	//!			スレッド保護を行うこと
	virtual void OnPeriod(risse_uint64 scheduled_tick, risse_uint64 current_tick) = 0;
};
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
//! @brief		イベントタイマースケジューラ
//! @note		Risaのイベントシステムと協調して動作するスケジューラ。
//!				素の tRisaTimerScheduler/tRisaTimerConsumer は マルチスレッド
//!				で動作するので少々扱いづらいが、
//!				イベントタイマーはイベントシステムを使って、イベントハンドラが
//!				メインスレッドから呼ばれることを確実にするほか、いくつかの機能
//!				のカプセル化も行う。
//---------------------------------------------------------------------------
class tRisaEventTimerScheduler :
				public tRisaTimerScheduler,
				public singleton_base<tRisaEventTimerScheduler>, // このクラスはシングルトンオブジェクト
				manual_start<tRisaEventTimerScheduler> // このクラスのインスタンスは手動起動
{
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		イベントタイマーコンシューマ
//---------------------------------------------------------------------------
class tRisaEventTimerConsumer :
	public tRisaTimerConsumer,
	public tRisaEventDestination,
	depends_on<tRisaEventTimerScheduler>, // このクラスは tRisaEventTimerScheduler に依存
	depends_on<tRisaEventSystem> // イベント管理システムに依存
{
	tRisaCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション
	bool Enabled; //!< タイマーが有効かどうか
	risse_uint64 Interval; //!< タイマー周期
	risse_uint64 ReferenceTick; //!< 周期の基準となるTick


protected:
	tRisaEventTimerConsumer();
	~tRisaEventTimerConsumer();

public:
	bool GetEnabled() const { return Enabled; } //!< 有効かどうかを得る
	void SetEnabled(bool enabled);
	risse_uint64 GetInterval() const { return Interval; } //!< タイマー周期を得る
	void SetInterval(risse_uint64 interval);
	void Reset();

private:
	void ResetInterval();

	virtual void OnPeriod(risse_uint64 scheduled_tick, risse_uint64 current_tick); // from tRisaScriptTimerConsumer
	virtual void OnEvent(tRisaEventInfo * info); // from tRisaEventDestination

public:
	virtual void OnTimer() = 0; //!< タイマイベントが発生した

};
//---------------------------------------------------------------------------



#endif

