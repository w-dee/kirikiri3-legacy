//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 高精度タイマーの実装
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/event/timer/Timer.h"
#include "risa/packages/risa/event/TickCount.h"


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
	Enabled = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTimerConsumer::SetEnabled(bool b)
{
	volatile tCriticalSection::tLocker cs_holder(Owner->GetCS());

	if(Enabled != b)
	{
		if(b)
			Owner->Register(this); // owner に登録
		else
			Owner->Unregister(this); // owner から削除
		Enabled = b;
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void tTimerConsumer::SetNextTick(risse_uint64 nexttick)
{
	volatile tCriticalSection::tLocker cs_holder(Owner->GetCS());

	if(Enabled && Owner->NeedRescheduleOnPeriodChange)
	{

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
tPeriodicTimerConsumer::tPeriodicTimerConsumer(tTimerScheduler * owner) :
		tTimerConsumer(owner)
{
	// フィールドの初期化
	Interval = 0;
	ReferenceTick = tTickCount::InvalidTickCount;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPeriodicTimerConsumer::SetEnabled(bool enabled)
{
	// 親クラスのsetenabled をオーバーライド
	volatile tCriticalSection::tLocker cs_holder(GetOwner()->GetCS());

	if(enabled) ResetInterval(true);

	tTimerConsumer::SetEnabled(enabled);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPeriodicTimerConsumer::SetInterval(risse_uint64 interval)
{
	volatile tCriticalSection::tLocker cs_holder(GetOwner()->GetCS());

	if(Interval != interval)
	{
		Interval = interval;
		ResetInterval();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPeriodicTimerConsumer::Reset()
{
	volatile tCriticalSection::tLocker cs_holder(GetOwner()->GetCS());

	ResetInterval();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPeriodicTimerConsumer::ResetInterval(bool force)
{
	// 時間原点をこのメソッドが呼ばれた時点に設定する

	if(force || (GetEnabled() && Interval != tTickCount::InvalidTickCount))
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
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tPeriodicTimerConsumer::OnPeriod(risse_uint64 scheduled_tick, risse_uint64 current_tick)
{
	volatile tCriticalSection::tLocker cs_holder(GetOwner()->GetCS());

	if(GetEnabled() && Interval != tTickCount::InvalidTickCount)
	{
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
tEventTimerConsumer::tEventTimerConsumer() :
		tPeriodicTimerConsumer(tEventTimerScheduler::instance())
{
	// フィールドの初期化
	Capacity = DefaultCapacity;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventTimerConsumer::SetEnabled(bool enabled)
{
	// 親クラスのsetenabled をオーバーライド
	volatile tSynchronizer sync(this); // sync

	ClearQueue();

	tPeriodicTimerConsumer::SetEnabled(enabled);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventTimerConsumer::SetInterval(risse_uint64 interval)
{
	volatile tSynchronizer sync(this); // sync

	if(interval != GetInterval()) ClearQueue();

	tPeriodicTimerConsumer::SetInterval(interval);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventTimerConsumer::SetCapacity(size_t capa)
{
	volatile tSynchronizer sync(this); // sync

	Capacity = capa;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventTimerConsumer::ClearQueue()
{
	volatile tSynchronizer sync(this); // sync

	GetDestEventQueueInstance()->CancelEvents(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventTimerConsumer::OnPeriod(risse_uint64 scheduled_tick, risse_uint64 current_tick)
{
	volatile tSynchronizer sync(this); // sync

	if(GetEnabled() && GetInterval() != tTickCount::InvalidTickCount)
	{
		// キュー中に入るべきイベントの量は問題ないか？
		if(Capacity == 0 || QueueCount < Capacity)
		{
			// イベント管理システムにイベントをPostする
			GetDestEventQueueInstance()->PostEvent(
				new tEventInfo(
					0, // id
					this, // source
					this // destination
					) );
			QueueCount ++;
		}
	}

	tPeriodicTimerConsumer::OnPeriod(scheduled_tick, current_tick);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventTimerConsumer::OnEvent(tEventInfo * info)
{
	{
		// キューカウンタを減らす
		volatile tSynchronizer sync(this); // sync
		QueueCount --;
	}

	if(GetEnabled() && GetInterval() != tTickCount::InvalidTickCount)
		OnTimer(info->GetTick());
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
/**
 * "Timer" クラスのインスタンス用 C++クラス
 */
class tTimerInstance : public tEventTimerConsumer
{
public:
	/**
	 * コンストラクタ
	 */
	tTimerInstance();

	/**
	 * ダミーのデストラクタ(おそらく呼ばれない)
	 */
	virtual ~tTimerInstance() {;}

	// tEventTimerConsumer::OnTimer 実装
	void OnTimer(risse_uint64 tick)
	{
		// onTimer を呼び出す
		Operate(ocFuncCall, NULL, tSS<'o','n','T','i','m','e','r'>(),
				0, tMethodArgument::New((risse_int64)tick));
	}

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);
	void set_enabled(bool b) { SetEnabled(b); }
	bool get_enabled() const { return GetEnabled(); }
	void set_interval(risse_uint64 interval) { SetInterval(interval); }
	risse_uint64 get_interval() const { return GetInterval(); }
	void set_capacity(risse_size capa) { SetCapacity(capa); }
	risse_size get_capacity() const { return GetCapacity(); }
	void reset() { ClearQueue(); }
	void onTimer(risse_uint64 tick) { (void)tick;/* デフォルトでは何もしない */ }

};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tTimerInstance::tTimerInstance()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTimerInstance::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTimerInstance::initialize(const tNativeCallInfo &info)
{
	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
/**
 * "Timer" クラス
 */
class tTimerClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	/**
	 * コンストラクタ
	 * @param engine	スクリプトエンジンインスタンス
	 */
	tTimerClass(tScriptEngine * engine);

	/**
	 * 各メンバをインスタンスに追加する
	 */
	void RegisterMembers();

	/**
	 * newの際の新しいオブジェクトを作成して返す
	 */
	static tVariant ovulate();

public: // Risse 用メソッドとか
	static risse_uint64 getTickCount() { return tTickCount::instance()->Get(); }
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tTimerClass::tTimerClass(tScriptEngine * engine) :
	tClassBase(tSS<'T','i','m','e','r'>(),
		tClassHolder<tEventSourceClass>::instance()->GetClass())
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTimerClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tTimerClass::ovulate);
	BindFunction(this, ss_construct, &tTimerInstance::construct);
	BindFunction(this, ss_initialize, &tTimerInstance::initialize);
	BindProperty(this, tSS<'e','n','a','b','l','e','d'>(), &tTimerInstance::get_enabled, &tTimerInstance::set_enabled);
	BindProperty(this, tSS<'i','n','t','e','r','v','a','l'>(), &tTimerInstance::get_interval, &tTimerInstance::set_interval);
	BindProperty(this, tSS<'c','a','p','a','c','i','t','y'>(), &tTimerInstance::get_capacity, &tTimerInstance::set_capacity);
	BindFunction(this, tSS<'r','e','s','e','t'>(), &tTimerInstance::reset);
	BindFunction(this, tSS<'o','n','T','i','m','e','r'>(), &tTimerInstance::onTimer);

	BindFunction(this, tSS<'g','e','t','T','i','c','k','C','o','u','n','t'>(), &tTimerClass::getTickCount);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tTimerClass::ovulate()
{
	return tVariant(new tTimerInstance());
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
/**
 * Timer クラスレジストラ
 */
template class tClassRegisterer<
	tSS<'r','i','s','a','.','e','v','e','n','t'>,
	tTimerClass>;
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace Risa


