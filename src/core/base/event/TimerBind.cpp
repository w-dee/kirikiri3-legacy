//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief タイマーのRisseバインディング
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/event/TimerBind.h"
#include "base/event/Event.h"
#include "base/event/Timer.h"
#include "base/script/RisseEngine.h"
#include "risse/include/risseNative.h"

RISSE_DEFINE_SOURCE_ID(25021,49177,9141,20257,35249,61240,44766,27087);





//---------------------------------------------------------------------------
//! @brief		スクリプト用タイマースケジューラ
//---------------------------------------------------------------------------
class tRisaScriptTimerScheduler :
				public tRisaTimerScheduler,
				public singleton_base<tRisaScriptTimerScheduler>, // このクラスはシングルトンオブジェクト
				manual_start<tRisaScriptTimerScheduler> // このクラスのインスタンスは手動起動
{
};
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
//! @brief		スクリプト用タイマーコンシューマ
//---------------------------------------------------------------------------
class tRisaScriptTimerConsumer :
	depends_on<tRisaScriptTimerScheduler>, // このクラスは tRisaScriptTimerScheduler に依存
	public tRisaTimerConsumer
{
public:
	//! @brief コンストラクタ
	tRisaScriptTimerConsumer() :
		tRisaTimerConsumer(tRisaScriptTimerScheduler::instance().get()) {;}

	//! @brief デストラクタ
	~tRisaScriptTimerConsumer() {;}
};
//---------------------------------------------------------------------------
















//---------------------------------------------------------------------------
//! @brief		Timerネイティブインスタンス
//---------------------------------------------------------------------------
class tRisseNI_Timer :
						public tRisseNativeInstance,
						public tRisaEventDestination,
						public tRisaScriptTimerConsumer,
						depends_on<tRisaEventSystem> // イベント管理システムに依存
{
	tRisaCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

	iRisseDispatch2 * Owner; //!< オーナーとなるRisseオブジェクト

	bool Enabled; //!< タイマーが有効かどうか
	risse_uint64 Interval; //!< タイマー周期
	risse_uint64 ReferenceTick; //!< 周期の基準となるTick

public:
	tRisseNI_Timer();

	risse_error Construct(risse_int numparams,
		tRisseVariant **param, iRisseDispatch2 *risse_obj);
	void Invalidate();

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

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisseNI_Timer::tRisseNI_Timer()
{
	// フィールドの初期化
	Owner = NULL;
	Enabled = false;
	Interval = 0;
	ReferenceTick = tRisaTickCount::InvalidTickCount;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Risse コンストラクタ
//---------------------------------------------------------------------------
risse_error tRisseNI_Timer::Construct(risse_int numparams,
		tRisseVariant **param, iRisseDispatch2 *risse_obj)
{
	Owner = risse_obj;
	return RISSE_S_OK;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Risse 無効化関数
//---------------------------------------------------------------------------
void tRisseNI_Timer::Invalidate()
{
	// まず、Timer からのコールバックイベントの停止
	SetNextTick(tRisaTickCount::InvalidTickCount);

	// この時点で OnPeriod はもう呼ばれることはないはず
	Owner = NULL;

	// イベントキューにたまったすべてのイベントを削除
	tRisaEventSystem::instance()->CancelEvents(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		有効か無効かを設定する
//! @param		enabled 有効か無効か
//---------------------------------------------------------------------------
void tRisseNI_Timer::SetEnabled(bool enabled)
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
void tRisseNI_Timer::SetInterval(risse_uint64 interval)
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
void tRisseNI_Timer::Reset()
{
	volatile tRisaCriticalSection::tLocker cs_holder(CS);

	ResetInterval();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		タイマー周期をリセットする
//---------------------------------------------------------------------------
void tRisseNI_Timer::ResetInterval()
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
void tRisseNI_Timer::OnPeriod(risse_uint64 scheduled_tick, risse_uint64 current_tick)
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
void tRisseNI_Timer::OnEvent(tRisaEventInfo * info)
{
	volatile tRisaCriticalSection::tLocker cs_holder(CS);

	if(Enabled && Interval != tRisaTickCount::InvalidTickCount)
	{
		static ttstr onTimer_name(RISSE_WS("onTimer"));
		Owner->FuncCall(
			0, // flag
			onTimer_name.c_str(), // name
			onTimer_name.GetHint(), // hint
			NULL, // result
			0, // numparams
			NULL, // parameters
			Owner // objthis
			);
	}
}
//---------------------------------------------------------------------------


















//---------------------------------------------------------------------------
//! @brief		Timerネイティブクラス
//---------------------------------------------------------------------------
class tRisseNC_Timer : public tRisseNativeClass
{
public:
	tRisseNC_Timer();

	static risse_uint32 ClassID;

private:
	tRisseNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		TimerクラスID
//---------------------------------------------------------------------------
risse_uint32 tRisseNC_Timer::ClassID = (risse_uint32)-1;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisseNC_Timer::tRisseNC_Timer() :
	tRisseNativeClass(RISSE_WS("Timer"))
{
	// class constructor

	RISSE_BEGIN_NATIVE_MEMBERS(/*Risse class name*/Timer)
	RISSE_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var. name*/_this, /*var. type*/tRisseNI_Timer,
	/*Risse class name*/ Timer)
{
	return RISSE_S_OK;
}
RISSE_END_NATIVE_CONSTRUCTOR_DECL(/*Risse class name*/Timer)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/reset)
{
	/*%
		@brief	タイマーをリセットする
		@note
		<p>
			タイマーをリセットします。
			タイマーが有効ならば(enabledプロパティが真ならば)、
			時間原点をこのメソッドが呼ばれた時点にし、タイマー間隔を
			スケジュールし直します。
		</p>
	*/
	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Timer);

	_this->Reset();

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/reset)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/onTimer)
{
	/*%
		@type event
		@brief	タイマー周期が来た
		@note
		<p>
			タイマー周期が来た際に呼ばれます。
		</p>
	*/
	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/onTimer)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(enabled)
{
	/*%
		@brief	有効か無効か
		@note
		<p>
			タイマーが有効か、無効化を表します。真ならば有効で onTimer イベントが
			発生し、偽ならば無効で、イベントは発生しません。
		</p>
	*/
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Timer);
		if(result) *result = (tTVInteger)_this->GetEnabled();
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_BEGIN_NATIVE_PROP_SETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Timer);
		_this->SetEnabled(*param);
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(enabled)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(interval)
{
	/*%
		@brief	タイマー周期
		@note
		<p>
			タイマー周期を ms 単位で指定します。この周期ごとに onTimer 
			イベントが発生します。
		</p>
	*/
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Timer);
		if(result) *result = (tTVInteger)_this->GetInterval();
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_BEGIN_NATIVE_PROP_SETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Timer);
		_this->SetInterval((tTVInteger)*param);
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(interval)
//----------------------------------------------------------------------

//----------------------------------------------------------------------
	RISSE_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ネイティブインスタンスを作成して返す
//! @return		ネイティブインスタンス
//---------------------------------------------------------------------------
tRisseNativeInstance *tRisseNC_Timer::CreateNativeInstance()
{
	return new tRisseNI_Timer();
}
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
//! @brief クラスレジストラ
//---------------------------------------------------------------------------
class tRisaTimerRegisterer :
			public singleton_base<tRisaTimerRegisterer>,
			depends_on<tRisaRisseScriptEngine>
{
public:
	tRisaTimerRegisterer()
	{
		iRisseDispatch2 * cls =  new tRisseNC_Timer();
		try
		{
			tRisaRisseScriptEngine::instance()->
				RegisterGlobalObject(RISSE_WS("Timer"), cls);
		}
		catch(...)
		{
			cls->Release();
			throw;
		}
		cls->Release();
	}
};
//---------------------------------------------------------------------------




