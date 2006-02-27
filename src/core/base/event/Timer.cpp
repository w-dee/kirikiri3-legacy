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
#include "prec.h"
#include "base/event/Timer.h"

RISSE_DEFINE_SOURCE_ID(3263,62535,57591,17893,921,30607,15180,25430);

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


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaTimerScheduler::tRisaTimerScheduler()
{
	// フィールドの初期化
	NearestTick = tRisaTickCount::InvalidTickCount;
	NearestInfoValid = false;
	NeedRescheduleOnPeriodChange = true;

	// スレッドの実行
	Run();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaTimerScheduler::~tRisaTimerScheduler()
{
	// スレッドの削除
	Terminate(); // 終了フラグをたてる
	Event.Signal(); // スレッドをたたき起こす
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		consumerを登録する
//! @param		consumer コールバックを発生させるコンシューマオブジェクト
//! @note		すでにそのconsumerが登録されてるかどうかなどはチェックしない
//---------------------------------------------------------------------------
void tRisaTimerScheduler::Register(tRisaTimerConsumer * consumer)
{
	volatile tRisaCriticalSection::tLocker cs_holder(CS);

	Consumers.push_back(consumer);
	Reschedule(); // スケジュールし直す
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		consumerの登録を解除する
//! @param		consumer コールバックを発生させるコンシューマオブジェクト
//! @note		そのconsumerが見つからない場合は何もしない
//---------------------------------------------------------------------------
void tRisaTimerScheduler::Unregister(tRisaTimerConsumer * consumer)
{
	volatile tRisaCriticalSection::tLocker cs_holder(CS);

	tConsumers::iterator i = std::find(Consumers.begin(), Consumers.end(), consumer);
	if(i != Consumers.end())
	{
		Consumers.erase(i);
		Reschedule(); // スケジュールし直す
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スケジュールをやり直す
//---------------------------------------------------------------------------
void tRisaTimerScheduler::Reschedule()
{
	NearestInfoValid = false; // NearestInfo をもう一度検索するように
	Event.Signal(); // スレッドをたたき起こす
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		もっとも近い位置にあるTickをもつConsumerを探す
//---------------------------------------------------------------------------
void tRisaTimerScheduler::GetNearestInfo()
{
	size_t nearest_index = static_cast<size_t>(-1L);
	risse_uint64 nearest_tick = tRisaTickCount::InvalidTickCount;

	// もっちも近い位置にある (もっとも値の小さいtickをもつ)Consumerを探す
	size_t index = 0;
	for(tConsumers::iterator i = Consumers.begin();
		i != Consumers.end(); i++, index++)
	{
		risse_uint64 tick = (*i)->GetNextTick();
		if(tick != tRisaTickCount::InvalidTickCount)
		{
			if(nearest_tick == tRisaTickCount::InvalidTickCount ||
				nearest_tick > tick)
			{
				nearest_index = index;
				nearest_tick = tick;
			}
		}
	}

	NearestTick = nearest_tick;
	NearestIndex = nearest_index;
	NearestInfoValid = true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スレッドのエントリーポイント
//! @return		スレッドの終了コード
//---------------------------------------------------------------------------
void tRisaTimerScheduler::Execute()
{
	while(!ShouldTerminate())
	{
		risse_int64 sleep_ms; // どれぐらい sleep すればよいか
		while(true)
		{
			risse_uint64 current_tick = tRisaTickCount::instance()->Get();

			// 直近のTickを持つConsumerを探し、何ms後に起きれば良いのかを計算する
			{
				volatile tRisaCriticalSection::tLocker cs_holder(CS);
				if(!NearestInfoValid) GetNearestInfo();

				if(NearestTick == tRisaTickCount::InvalidTickCount)
				{
					sleep_ms = -1L;
					break; // 直近のTickを持つConsumerが居ない
				}

				sleep_ms = NearestTick - current_tick;
				if(sleep_ms <= 0)
				{
					// すでに時間をすぎているのでOnPeriodを呼ぶ
					// OnPeriod 内で Reschedule を呼ばれても、結局の所
					// またこのループの先頭に戻って
					// リスケジュールするのでそういう無駄なことをしないように
					// NeedRescheduleOnPeriodChange を false に設定する
					NeedRescheduleOnPeriodChange = false;
					Consumers[NearestIndex]->OnPeriod(NearestTick, current_tick);
					NeedRescheduleOnPeriodChange = true;
					NearestInfoValid = false; // NearestInfo を無効に
				}
				else
				{
					// まだ時間があるので sleep する
					break;
				}
			}
		}

		// 実際に待つ時間を決定
		// 実際に待つ時間は、本当に待つべき時間よりも少なくてもかまわない
		// (その場合は再スケジュールされるだけなので)
		// ここでは最長60秒待つことにする
		if(sleep_ms < 0 || sleep_ms > 60*1000)
			sleep_ms = 60*1000;

		// 待つ
		if(sleep_ms != 0) Event.Wait(static_cast<unsigned long>(sleep_ms));
	}
}
//---------------------------------------------------------------------------



















//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaTimerConsumer::tRisaTimerConsumer(tRisaTimerScheduler * owner) : Owner(owner)
{
	// フィールドの初期化
	NextTick = tRisaTickCount::InvalidTickCount;

	// スケジューラにコンシューマを登録
	Owner->Register(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaTimerConsumer::~tRisaTimerConsumer()
{
	// スケジューラからコンシューマを削除
	Owner->Unregister(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		次にOnPeriodをコールバックすべき絶対tickを設定する
//! @param		nexttick 絶対TickCount (呼んで欲しくない場合はtRisaTickCount::InvalidTickCount)
//! @note		nexttick が現在あるいはすでに過去だった場合は即座に OnPeriodが呼ばれる。
//! @note		ここでいう「絶対TickCount」とは tRisaTickCount が返すようなTickCountのことである。
//!				たとえば5秒後にOnPeriodを呼びたいならば、
//!				SetNextTick(tRisaTickCount::instance()->Get() + 5000) とする。
//---------------------------------------------------------------------------
void tRisaTimerConsumer::SetNextTick(risse_uint64 nexttick)
{
	if(Owner->NeedRescheduleOnPeriodChange)
	{
		volatile tRisaCriticalSection::tLocker cs_holder(Owner->CS);

		NextTick = nexttick;
		Owner->Reschedule(); // スケジュールをやり直すように
	}
	else
	{
		NextTick = nexttick;
	}
}
//---------------------------------------------------------------------------


















//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaEventTimerConsumer::tRisaEventTimerConsumer() :
		tRisaTimerConsumer(tRisaEventTimerScheduler::instance().get())
{
	// フィールドの初期化
	Enabled = false;
	Interval = 0;
	ReferenceTick = tRisaTickCount::InvalidTickCount;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaEventTimerConsumer::~tRisaEventTimerConsumer()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		有効か無効かを設定する
//! @param		enabled 有効か無効か
//---------------------------------------------------------------------------
void tRisaEventTimerConsumer::SetEnabled(bool enabled)
{
	volatile tRisaCriticalSection::tLocker cs_holder(CS);

	if(Enabled != enabled)
	{
		Enabled = enabled;

		Reset();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		タイマー周期を設定する
//! @param		interval タイマー周期
//---------------------------------------------------------------------------
void tRisaEventTimerConsumer::SetInterval(risse_uint64 interval)
{
	volatile tRisaCriticalSection::tLocker cs_holder(CS);

	if(Interval != interval)
	{
		Interval = interval;
		Reset();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		タイマーをリセットする
//---------------------------------------------------------------------------
void tRisaEventTimerConsumer::Reset()
{
	volatile tRisaCriticalSection::tLocker cs_holder(CS);

	ResetInterval();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		タイマー周期をリセットする
//---------------------------------------------------------------------------
void tRisaEventTimerConsumer::ResetInterval()
{
	// 時間原点をこのメソッドが呼ばれた時点に設定する

	if(Enabled && Interval != tRisaTickCount::InvalidTickCount)
	{
		// 有効の場合
		ReferenceTick = tRisaTickCount::instance()->Get() + Interval;
		SetNextTick(ReferenceTick);
	}
	else
	{
		// 無効の場合
		SetNextTick(tRisaTickCount::InvalidTickCount);
	}

	tRisaEventSystem::instance()->CancelEvents(this); // pending なイベントはすべてキャンセル
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定されたTickCountに達したときに呼び出される
//! @param	scheduled_tick GetNextTick() が返した Tick (本来呼ばれるべき時点の絶対TickCount)
//! @param	current_tick この OnPeriod が呼ばれた時点でのtick カウント(実際に呼ばれた時点での絶対TickCount)
//---------------------------------------------------------------------------
void tRisaEventTimerConsumer::OnPeriod(risse_uint64 scheduled_tick, risse_uint64 current_tick)
{
	volatile tRisaCriticalSection::tLocker cs_holder(CS);

	if(Enabled && Interval != tRisaTickCount::InvalidTickCount)
	{
		// イベント管理システムにイベントをPostする
		tRisaEventSystem::instance()->PostEvent(
			new tRisaEventInfo(
				0, // id
				this, // source
				this // destination
				) );

		// 次にこのイベントを発生すべき tick を設定する
		ReferenceTick += Interval;
		SetNextTick(ReferenceTick);
	}
	else
	{
		SetNextTick(tRisaTickCount::InvalidTickCount);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	イベントが配信されるとき
//! @param	info イベント情報
//---------------------------------------------------------------------------
void tRisaEventTimerConsumer::OnEvent(tRisaEventInfo * info)
{
	if(Enabled && Interval != tRisaTickCount::InvalidTickCount)
	{
		OnTimer();
	}
}
//---------------------------------------------------------------------------



