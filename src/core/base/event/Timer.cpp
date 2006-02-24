//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief タイマークラス
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
		if(sleep_ms == -1 || sleep_ms > 60*1000)
			sleep_ms = 60*1000;
		if(sleep_ms == 0) sleep_ms = 1; // 0 ms 待ちはしない(最低でも1ms)

		// 待つ
		Event.Wait(static_cast<unsigned long>(sleep_ms));
	}
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		次にOnPeriodをコールバックすべき絶対tickを設定する
//! @param		nexttick 絶対TickCount (呼んで欲しくない場合はtRisaTickCount::InvalidTickCount)
//! @note		nexttick が現在あるいはすでに過去だった場合は即座に OnPeriodが呼ばれる。
//! @note		ここでいう「絶対TickCount」とは tRisaTickCount が返すようなTickCountのことである。
//!				たとえば５秒後にOnPeriodを呼びたいならば、
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




