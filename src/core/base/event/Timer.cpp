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

RISSE_DEFINE_SOURCE_ID(3263,62535,57591,17893,921,30607,15180,25430);


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
tRisaTimerScheduler::~tRisaTimerScheduler()
{
	// スレッドの削除
	Terminate(); // 終了フラグをたてる
	Event.Signal(); // スレッドをたたき起こす
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaTimerScheduler::Register(tRisaTimerConsumer * consumer)
{
	volatile tRisaCriticalSection::tLocker cs_holder(CS);

	Consumers.push_back(consumer);
	Reschedule(); // スケジュールし直す
}
//---------------------------------------------------------------------------


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
void tRisaTimerScheduler::Reschedule()
{
	NearestInfoValid = false; // NearestInfo をもう一度検索するように
	Event.Signal(); // スレッドをたたき起こす
}
//---------------------------------------------------------------------------


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
void tRisaTimerScheduler::Execute()
{
	while(!ShouldTerminate())
	{
		risse_int64 sleep_ms; // どれぐらい sleep すればよいか
		while(true)
		{
			risse_uint64 current_tick = depends_on<tRisaTickCount>::locked_instance()->Get();

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
tRisaTimerConsumer::tRisaTimerConsumer(tRisaTimerScheduler * owner) : Owner(owner)
{
	// フィールドの初期化
	NextTick = tRisaTickCount::InvalidTickCount;

	// スケジューラにコンシューマを登録
	Owner->Register(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaTimerConsumer::~tRisaTimerConsumer()
{
	// スケジューラからコンシューマを削除
	Owner->Unregister(this);
}
//---------------------------------------------------------------------------


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
tRisaEventTimerConsumer::tRisaEventTimerConsumer() :
		tRisaTimerConsumer(tRisaEventTimerScheduler::instance().get())
{
	// フィールドの初期化
	Enabled = false;
	Capacity = DefaultCapacity;
	Interval = 0;
	ReferenceTick = tRisaTickCount::InvalidTickCount;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaEventTimerConsumer::~tRisaEventTimerConsumer()
{
}
//---------------------------------------------------------------------------


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
void tRisaEventTimerConsumer::Reset()
{
	volatile tRisaCriticalSection::tLocker cs_holder(CS);

	ResetInterval();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaEventTimerConsumer::SetCapacity(size_t capa)
{
	volatile tRisaCriticalSection::tLocker cs_holder(CS);

	Capacity = capa;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaEventTimerConsumer::ResetInterval()
{
	// 時間原点をこのメソッドが呼ばれた時点に設定する

	if(Enabled && Interval != tRisaTickCount::InvalidTickCount)
	{
		// 有効の場合
		ReferenceTick = depends_on<tRisaTickCount>::locked_instance()->Get() + Interval;
		SetNextTick(ReferenceTick);
	}
	else
	{
		// 無効の場合
		SetNextTick(tRisaTickCount::InvalidTickCount);
	}

	depends_on<tRisaEventSystem>::locked_instance()->CancelEvents(this); // pending なイベントはすべてキャンセル
	QueueCount = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaEventTimerConsumer::OnPeriod(risse_uint64 scheduled_tick, risse_uint64 current_tick)
{
	volatile tRisaCriticalSection::tLocker cs_holder(CS);

	if(Enabled && Interval != tRisaTickCount::InvalidTickCount)
	{
		// キュー中に入るべきイベントの量は問題ないか？
		if(Capacity == 0 || QueueCount < Capacity)
		{
			// イベント管理システムにイベントをPostする
			depends_on<tRisaEventSystem>::locked_instance()->PostEvent(
				new tRisaEventInfo(
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
		SetNextTick(tRisaTickCount::InvalidTickCount);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaEventTimerConsumer::OnEvent(tRisaEventInfo * info)
{
	{
		// キューカウンタを減らす
		volatile tRisaCriticalSection::tLocker cs_holder(CS);
		QueueCount --;
	}

	if(Enabled && Interval != tRisaTickCount::InvalidTickCount)
		OnTimer();
}
//---------------------------------------------------------------------------



