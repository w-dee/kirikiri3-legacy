//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 高精度タイマEの実裁E//---------------------------------------------------------------------------
#include "prec.h"
#include "base/event/Timer.h"

RISSE_DEFINE_SOURCE_ID(3263,62535,57591,17893,921,30607,15180,25430);

/*! @note

	OS の提供する「普通E」タイマEは
ぁEぁEE解能が低く、使ぁEにならなぁE	(侁E Windowsでは刁E能は50msが限度)、E	刁E能の高いタイマEを汎用皁Eどのように実現するかE難しいところであるが、E	ここではタイミングを制御するスレチEを一つ作Eし、そのスレチE冁Eセマフォ
	征Eがタイ
アウトするEと、tRisaTickCount クラスからのチEチEカウントを
	リファレンスとしてタイマEを実現する、E	これはおおよそOSのコンチEストスイチEの精度に依存するが、E0ms程度の
	刁E能を提供できるはずである、E	精度につぁEは TickCount.cpp の説明も参Eのこと、E
*/


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaTimerScheduler::tRisaTimerScheduler()
{
	// フィールドE初期匁E	NearestTick = tRisaTickCount::InvalidTickCount;
	NearestInfoValid = false;
	NeedRescheduleOnPeriodChange = true;

	// スレチEの実衁E	Run();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		チEトラクタ
//---------------------------------------------------------------------------
tRisaTimerScheduler::~tRisaTimerScheduler()
{
	// スレチEの削除
	Terminate(); // 終亁EラグをたてめE	Event.Signal(); // スレチEをたたき起こす
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		consumerを登録する
//! @param		consumer コールバックを発生させるコンシューマオブジェクチE//! @note		すでにそEconsumerが登録されてるかどぁEなどはチェチEしなぁE//---------------------------------------------------------------------------
void tRisaTimerScheduler::Register(tRisaTimerConsumer * consumer)
{
	volatile tRisaCriticalSection::tLocker cs_holder(CS);

	Consumers.push_back(consumer);
	Reschedule(); // スケジュールし直ぁE}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		consumerの登録を解除する
//! @param		consumer コールバックを発生させるコンシューマオブジェクチE//! @note		そEconsumerが見つからなぁE
合E何もしなぁE//---------------------------------------------------------------------------
void tRisaTimerScheduler::Unregister(tRisaTimerConsumer * consumer)
{
	volatile tRisaCriticalSection::tLocker cs_holder(CS);

	tConsumers::iterator i = std::find(Consumers.begin(), Consumers.end(), consumer);
	if(i != Consumers.end())
	{
		Consumers.erase(i);
		Reschedule(); // スケジュールし直ぁE	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スケジュールをやり直ぁE//---------------------------------------------------------------------------
void tRisaTimerScheduler::Reschedule()
{
	NearestInfoValid = false; // NearestInfo をもぁE度検索するように
	Event.Signal(); // スレチEをたたき起こす
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		もっとも近い位置にあるTickをもつConsumerを探ぁE//---------------------------------------------------------------------------
void tRisaTimerScheduler::GetNearestInfo()
{
	size_t nearest_index = static_cast<size_t>(-1L);
	risse_uint64 nearest_tick = tRisaTickCount::InvalidTickCount;

	// もっちも近い位置にある (もっとも値の小さいtickをもつ)Consumerを探ぁE	size_t index = 0;
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
//! @brief		スレチEのエントリーポインチE//! @return		スレチEの終亁EーチE//---------------------------------------------------------------------------
void tRisaTimerScheduler::Execute()
{
	while(!ShouldTerminate())
	{
		risse_int64 sleep_ms; // どれぐらい sleep すればよいぁE		while(true)
		{
			risse_uint64 current_tick = tRisaTickCount::instance()->Get();

			// 直近ETickを持つConsumerを探し、何ms後に起きれば良ぁEEかを計算すめE			{
				volatile tRisaCriticalSection::tLocker cs_holder(CS);
				if(!NearestInfoValid) GetNearestInfo();

				if(NearestTick == tRisaTickCount::InvalidTickCount)
				{
					sleep_ms = -1L;
					break; // 直近ETickを持つConsumerが屁EぁE				}

				sleep_ms = NearestTick - current_tick;
				if(sleep_ms <= 0)
				{
					// すでに時間をすぎてぁEのでOnPeriodを呼ぶ
					// OnPeriod 冁E Reschedule を呼ばれても、結局の所
					// またこのループE先
に戻って
					// リスケジュールするのでそうぁE無駁EことをしなぁEぁE
					// NeedRescheduleOnPeriodChange めEfalse に設定すめE					NeedRescheduleOnPeriodChange = false;
					Consumers[NearestIndex]->OnPeriod(NearestTick, current_tick);
					NeedRescheduleOnPeriodChange = true;
					NearestInfoValid = false; // NearestInfo を無効に
				}
				else
				{
					// ま
時間があるEで sleep する
					break;
				}
			}
		}

		// 実際に征E時間を決宁E		// 実際に征E時間は、本当に征Eべき時間よりも少なくてもかまわなぁE		// (そE場合E再スケジュールされるだけなので)
		// ここでは最長60秒征Eことにする
		if(sleep_ms < 0 || sleep_ms > 60*1000)
			sleep_ms = 60*1000;

		// 征E
		if(sleep_ms != 0) Event.Wait(static_cast<unsigned long>(sleep_ms));
	}
}
//---------------------------------------------------------------------------



















//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaTimerConsumer::tRisaTimerConsumer(tRisaTimerScheduler * owner) : Owner(owner)
{
	// フィールドE初期匁E	NextTick = tRisaTickCount::InvalidTickCount;

	// スケジューラにコンシューマを登録
	Owner->Register(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		チEトラクタ
//---------------------------------------------------------------------------
tRisaTimerConsumer::~tRisaTimerConsumer()
{
	// スケジューラからコンシューマを削除
	Owner->Unregister(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		次にOnPeriodをコールバックすべき絶対tickを設定すめE//! @param		nexttick 絶対TickCount (呼んで欲しくなぁE
合EtRisaTickCount::InvalidTickCount)
//! @note		nexttick が現在あるぁEEすでに過去
った
合E即座に OnPeriodが呼ばれる、E//! @note		ここでぁE「絶対TickCount」とは tRisaTickCount が返すようなTickCountのことである、E//!				たとえE5秒後にOnPeriodを呼びたいならE、E//!				SetNextTick(tRisaTickCount::instance()->Get() + 5000) とする、E//---------------------------------------------------------------------------
void tRisaTimerConsumer::SetNextTick(risse_uint64 nexttick)
{
	if(Owner->NeedRescheduleOnPeriodChange)
	{
		volatile tRisaCriticalSection::tLocker cs_holder(Owner->CS);

		NextTick = nexttick;
		Owner->Reschedule(); // スケジュールをやり直すよぁE
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
	// フィールドE初期匁E	Enabled = false;
	Interval = 0;
	ReferenceTick = tRisaTickCount::InvalidTickCount;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		チEトラクタ
//---------------------------------------------------------------------------
tRisaEventTimerConsumer::~tRisaEventTimerConsumer()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		有効か無効かを設定すめE//! @param		enabled 有効か無効ぁE//---------------------------------------------------------------------------
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
//! @brief		タイマE周期を設定すめE//! @param		interval タイマE周朁E//---------------------------------------------------------------------------
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
//! @brief		タイマEをリセチEする
//---------------------------------------------------------------------------
void tRisaEventTimerConsumer::Reset()
{
	volatile tRisaCriticalSection::tLocker cs_holder(CS);

	ResetInterval();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		タイマE周期をリセチEする
//---------------------------------------------------------------------------
void tRisaEventTimerConsumer::ResetInterval()
{
	// 時間原点をこのメソチEが呼ばれた時点に設定すめE
	if(Enabled && Interval != tRisaTickCount::InvalidTickCount)
	{
		// 有効の場吁E		ReferenceTick = tRisaTickCount::instance()->Get() + Interval;
		SetNextTick(ReferenceTick);
	}
	else
	{
		// 無効の場吁E		SetNextTick(tRisaTickCount::InvalidTickCount);
	}

	tRisaEventSystem::instance()->CancelEvents(this); // pending なイベントEすべてキャンセル
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		持EされたTickCountに達したときに呼び出されめE//! @param	scheduled_tick GetNextTick() が返しぁETick (本来呼ばれるべき時点の絶対TickCount)
//! @param	current_tick こE OnPeriod が呼ばれた時点でのtick カウンチE実際に呼ばれた時点での絶対TickCount)
//---------------------------------------------------------------------------
void tRisaEventTimerConsumer::OnPeriod(risse_uint64 scheduled_tick, risse_uint64 current_tick)
{
	volatile tRisaCriticalSection::tLocker cs_holder(CS);

	if(Enabled && Interval != tRisaTickCount::InvalidTickCount)
	{
		// イベント管琁EスチEにイベントをPostする
		tRisaEventSystem::instance()->PostEvent(
			new tRisaEventInfo(
				0, // id
				this, // source
				this // destination
				) );

		// 次にこEイベントを発生すべぁEtick を設定すめE		ReferenceTick += Interval;
		SetNextTick(ReferenceTick);
	}
	else
	{
		SetNextTick(tRisaTickCount::InvalidTickCount);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	イベントが配信されるとぁE//! @param	info イベント情報
//---------------------------------------------------------------------------
void tRisaEventTimerConsumer::OnEvent(tRisaEventInfo * info)
{
	if(Enabled && Interval != tRisaTickCount::InvalidTickCount)
	{
		OnTimer();
	}
}
//---------------------------------------------------------------------------



