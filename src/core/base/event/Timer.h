//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 高精度タイマーの実装
//---------------------------------------------------------------------------
#ifndef _RISATIMER_H
#define _RISATIMER_H

/*! @note

	OS の提供する「普通の」タイマーはだいたい分解能が低く、使い物にならない
	(例: Windowsでは分解能は50msが限度)。
	分解能の高いタイマーを汎用的にどのように実現するかは難しいところであるが、
	ここではタイミングを制御するスレッドを一つ作成し、そのスレッド内でセマフォ
	待ちがタイムアウトするのと、tRisaTickCount クラスからのティックカウントを
	リファレンスとしてタイマーを実現する。
	これはおおよそOSのコンテキストスイッチの精度に依存するが、10ms程度の
	分解能を提供できるはずである。
	精度については TickCount.cpp の説明も参照のこと。

*/

#include "base/utils/Singleton.h"
#include "base/utils/RisaThread.h"
#include "base/event/TickCount.h"
#include "base/event/Event.h"






class tRisaTimerConsumer;

//---------------------------------------------------------------------------
//! @brief		タイマーのタイミングを管理するクラス(スケジューラ)
//---------------------------------------------------------------------------
class tRisaTimerScheduler : protected depends_on<tRisaTickCount>,
							public tRisaThread
{
	tRisaCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション
	tRisaThreadEvent Event; //!< イベント
	typedef gc_vector<tRisaTimerConsumer*> tConsumers; //!< tRisaTimerConsumer の配列 の typedef
	tConsumers Consumers; //!< tRisaTimerConsumer の配列

	risse_uint64 NearestTick; //!< もっとも時間的に近い位置にあるTick (tRisaTickCount::InvalidTickCount の場合は無効)
	size_t NearestIndex; //!< もっちも時間的に近い位置にある Consumer のConsumers内におけるインデックス
	volatile bool NearestInfoValid; //!< Nearest* のメンバの情報が有効かどうか
	volatile bool NeedRescheduleOnPeriodChange;
		//!< tRisaTimerConsumer::SetNextTick内でReschedule()が呼ばれたときに
		//!< 本当にRescheduleする必要があるかどうか

public:
	//! @brief		コンストラクタ
	tRisaTimerScheduler();

	//! @brief		デストラクタ
	~tRisaTimerScheduler();


private:
	friend class tRisaTimerConsumer;

	//! @brief		consumerを登録する
	//! @param		consumer コールバックを発生させるコンシューマオブジェクト
	//! @note		すでにそのconsumerが登録されてるかどうかなどはチェックしない
	void Register(tRisaTimerConsumer * consumer);

	//! @brief		consumerの登録を解除する
	//! @param		consumer コールバックを発生させるコンシューマオブジェクト
	//! @note		そのconsumerが見つからない場合は何もしない
	void Unregister(tRisaTimerConsumer * consumer);

	//! @brief		スケジュールをやり直す
	void Reschedule();

	//! @brief		もっとも近い位置にあるTickをもつConsumerを探す
	void GetNearestInfo();

protected:
	//! @brief		スレッドのエントリーポイント
	//! @return		スレッドの終了コード
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
	//! @brief		コンストラクタ
	tRisaTimerConsumer(tRisaTimerScheduler * owner);

	//! @brief		デストラクタ
	virtual ~tRisaTimerConsumer();
public:
	risse_uint64 GetNextTick() const { return NextTick; } //!< 次にOnPeriodをコールバックすべき絶対TickCountを返す

	//! @brief		次にOnPeriodをコールバックすべき絶対tickを設定する
	//! @param		nexttick 絶対TickCount (呼んで欲しくない場合はtRisaTickCount::InvalidTickCount)
	//! @note		nexttick が現在あるいはすでに過去だった場合は即座に OnPeriodが呼ばれる。
	//! @note		ここでいう「絶対TickCount」とは tRisaTickCount が返すようなTickCountのことである。
	//!				たとえば5秒後にOnPeriodを呼びたいならば、
	//!				SetNextTick(tRisaTickCount::instance()->Get() + 5000) とする。
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
	protected depends_on<tRisaEventTimerScheduler>, // このクラスは tRisaEventTimerScheduler に依存
	protected depends_on<tRisaEventSystem>, // イベント管理システムに依存
	protected depends_on<tRisaTickCount> // このクラスは tRisaTickCount に依存
{
	static const size_t DefaultCapacity = 6; //!< Capacity のデフォルトの値

	tRisaCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション
	bool Enabled; //!< タイマーが有効かどうか
	risse_uint64 Interval; //!< タイマー周期
	risse_uint64 ReferenceTick; //!< 周期の基準となるTick
	size_t Capacity; //!< 一度にイベントキューにためることのできる量(0=制限無し)
	size_t QueueCount; //!< キューにたまっているイベントの数

protected:
	//! @brief		コンストラクタ
	tRisaEventTimerConsumer();

	//! @brief		デストラクタ
	~tRisaEventTimerConsumer();

public:
	bool GetEnabled() const { return Enabled; } //!< 有効かどうかを得る

	//! @brief		有効か無効かを設定する
	//! @param		enabled 有効か無効か
	void SetEnabled(bool enabled);

	risse_uint64 GetInterval() const { return Interval; } //!< タイマー周期を得る

	//! @brief		タイマー周期を設定する
	//! @param		interval タイマー周期
	void SetInterval(risse_uint64 interval);

	//! @brief		タイマーをリセットする
	void Reset();

	//! @brief		容量(一度にイベントキューにためることのできるイベントの数)を設定する
	void SetCapacity(size_t capa);

	size_t GetCapacity() const { return Capacity; } //!< 容量を得る

private:
	//! @brief		タイマー周期をリセットする
	void ResetInterval();

	//! @brief		指定されたTickCountに達したときに呼び出される
	//! @param	scheduled_tick GetNextTick() が返した Tick (本来呼ばれるべき時点の絶対TickCount)
	//! @param	current_tick この OnPeriod が呼ばれた時点でのtick カウント(実際に呼ばれた時点での絶対TickCount)
	virtual void OnPeriod(risse_uint64 scheduled_tick, risse_uint64 current_tick); // from tRisaScriptTimerConsumer

	//! @brief	イベントが配信されるとき
	//! @param	info イベント情報
	virtual void OnEvent(tRisaEventInfo * info); // from tRisaEventDestination

public:
	virtual void OnTimer() = 0; //!< タイマイベントが発生した

};
//---------------------------------------------------------------------------



#endif

