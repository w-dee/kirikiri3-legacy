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
#include "prec.h"
#include "base/event/Timer.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(3263,62535,57591,17893,921,30607,15180,25430);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
tTimerScheduler::tTimerScheduler()
{
	// フィールドの初期化
	NearestTick = tTickCount::InvalidTickCount;
	NearestInfoValid = false;
	NeedRescheduleOnPeriodChange = true;

	// スレッドの実行
	Run();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTimerScheduler::~tTimerScheduler()
{
	// スレッドの削除
	Terminate(); // 終了フラグをたてる
	Event.Signal(); // スレッドをたたき起こす
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTimerScheduler::Register(tTimerConsumer * consumer)
{
	volatile tCriticalSection::tLocker cs_holder(CS);

	Consumers.push_back(consumer);
	Reschedule(); // スケジュールし直す
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTimerScheduler::Unregister(tTimerConsumer * consumer)
{
	volatile tCriticalSection::tLocker cs_holder(CS);

	tConsumers::iterator i = std::find(Consumers.begin(), Consumers.end(), consumer);
	if(i != Consumers.end())
	{
		Consumers.erase(i);
		Reschedule(); // スケジュールし直す
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTimerScheduler::Reschedule()
{
	NearestInfoValid = false; // NearestInfo をもう一度検索するように
	Event.Signal(); // スレッドをたたき起こす
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTimerScheduler::GetNearestInfo()
{
	size_t nearest_index = static_cast<size_t>(-1L);
	risse_uint64 nearest_tick = tTickCount::InvalidTickCount;

	// もっちも近い位置にある (もっとも値の小さいtickをもつ)Consumerを探す
	size_t index = 0;
	for(tConsumers::iterator i = Consumers.begin();
		i != Consumers.end(); i++, index++)
	{
		risse_uint64 tick = (*i)->GetNextTick();
		if(tick != tTickCount::InvalidTickCount)
		{
			if(nearest_tick == tTickCount::InvalidTickCount ||
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
void tTimerScheduler::Execute()
{
	while(!ShouldTerminate())
	{
		risse_int64 sleep_ms; // どれぐらい sleep すればよいか
		while(true)
		{
			risse_uint64 current_tick = tTickCount::instance()->Get();

			// 直近のTickを持つConsumerを探し、何ms後に起きれば良いのかを計算する
			{
				volatile tCriticalSection::tLocker cs_holder(CS);
				if(!NearestInfoValid) GetNearestInfo();

				if(NearestTick == tTickCount::InvalidTickCount)
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
tTimerConsumer::tTimerConsumer(tTimerScheduler * owner) : Owner(owner)
{
	// フィールドの初期化
	NextTick = tTickCount::InvalidTickCount;

	// スケジューラにコンシューマを登録
	Owner->Register(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTimerConsumer::~tTimerConsumer()
{
	// スケジューラからコンシューマを削除
	Owner->Unregister(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTimerConsumer::SetNextTick(risse_uint64 nexttick)
{
	if(Owner->NeedRescheduleOnPeriodChange)
	{
		volatile tCriticalSection::tLocker cs_holder(Owner->CS);

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
tEventTimerConsumer::tEventTimerConsumer() :
		tTimerConsumer(tEventTimerScheduler::instance())
{
	// フィールドの初期化
	Enabled = false;
	Capacity = DefaultCapacity;
	Interval = 0;
	ReferenceTick = tTickCount::InvalidTickCount;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tEventTimerConsumer::~tEventTimerConsumer()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventTimerConsumer::SetEnabled(bool enabled)
{
	volatile tCriticalSection::tLocker cs_holder(CS);

	if(Enabled != enabled)
	{
		Enabled = enabled;

		Reset();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventTimerConsumer::SetInterval(risse_uint64 interval)
{
	volatile tCriticalSection::tLocker cs_holder(CS);

	if(Interval != interval)
	{
		Interval = interval;
		Reset();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventTimerConsumer::Reset()
{
	volatile tCriticalSection::tLocker cs_holder(CS);

	ResetInterval();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventTimerConsumer::SetCapacity(size_t capa)
{
	volatile tCriticalSection::tLocker cs_holder(CS);

	Capacity = capa;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventTimerConsumer::ResetInterval()
{
	// 時間原点をこのメソッドが呼ばれた時点に設定する

	if(Enabled && Interval != tTickCount::InvalidTickCount)
	{
		// 有効の場合
		ReferenceTick = tTickCount::instance()->Get() + Interval;
		SetNextTick(ReferenceTick);
	}
	else
	{
		// 無効の場合
		SetNextTick(tTickCount::InvalidTickCount);
	}

	tEventSystem::instance()->CancelEvents(this); // pending なイベントはすべてキャンセル
	QueueCount = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventTimerConsumer::OnPeriod(risse_uint64 scheduled_tick, risse_uint64 current_tick)
{
	volatile tCriticalSection::tLocker cs_holder(CS);

	if(Enabled && Interval != tTickCount::InvalidTickCount)
	{
		// キュー中に入るべきイベントの量は問題ないか？
		if(Capacity == 0 || QueueCount < Capacity)
		{
			// イベント管理システムにイベントをPostする
			tEventSystem::instance()->PostEvent(
				new tEventInfo(
					0, // id
					this, // source
					this // destination
					) );
			QueueCount ++;
		}

		// 次にこのイベントを発生すべき tick を設定する
		ReferenceTick += Interval;
		SetNextTick(ReferenceTick);
	}
	else
	{
		SetNextTick(tTickCount::InvalidTickCount);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventTimerConsumer::OnEvent(tEventInfo * info)
{
	{
		// キューカウンタを減らす
		volatile tCriticalSection::tLocker cs_holder(CS);
		QueueCount --;
	}

	if(Enabled && Interval != tTickCount::InvalidTickCount)
		OnTimer();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa


