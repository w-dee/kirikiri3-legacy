//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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
#include "base/exception/RisaException.h"
#include "risse/include/risseNative.h"

RISSE_DEFINE_SOURCE_ID(25021,49177,9141,20257,35249,61240,44766,27087);




#if 0


//---------------------------------------------------------------------------
//! @brief		Timerネイティブインスタンス
//---------------------------------------------------------------------------
class tRisseNI_Timer :
						public tRisseNativeInstance,
						public tRisaEventTimerConsumer
{
	iRisseDispatch2 * Owner; //!< オーナーとなるRisseオブジェクト

public:
	tRisseNI_Timer();

	risse_error Construct(risse_int numparams,
		tRisseVariant **param, iRisseDispatch2 *risse_obj);
	void Invalidate();

protected:
	void OnTimer(); // tRisaEventtimerConsumerのオーバーライド
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisseNI_Timer::tRisseNI_Timer()
{
	// フィールドの初期化
	Owner = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Risse コンストラクタ
//---------------------------------------------------------------------------
risse_error tRisseNI_Timer::Construct(risse_int numparams,
		tRisseVariant **param, iRisseDispatch2 *risse_obj)
{
	/*%
		@fn		Timer.Timer
		@brief	Timerを構築する
	*/
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
	SetEnabled(false);

	// この時点で OnPeriod はもう呼ばれることはないはず
	Owner = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief	イベントが配信されるとき
//! @param	info イベント情報
//---------------------------------------------------------------------------
void tRisseNI_Timer::OnTimer()
{
	if(!Owner)
	{
		eRisaException::Throw(RISSE_WS_TR("constructor onTimer was not properly called; call the constructor from subclass constructor"));
	}

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
			タイマーが有効か、無効かを表します。真ならば有効で onTimer イベントが
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
RISSE_BEGIN_NATIVE_PROP_DECL(capacity)
{
	/*%
		@brief	イベント容量
		@note
		<p>
			イベントキューに一度に入れることのできる、このタイマーのイベントの数を表します。
		</p>
		<p>
			イベントの処理に時間がかかっている場合、次のタイマー周期が来ると、
			イベントはイベントキューに入ります。イベントの処理が終わり次第
			イベントキューに入っているイベントは処理されますが、このプロパティ
			ではそのイベントキューに一度に入ることのできるイベントの数を
			制限することが出来ます。
		</p>
		<p>
			デフォルトの値は 6 です。 0 を指定するとイベントキューに一度に入る
			ことの出来るイベントの数を無制限にすることが出来ますが、
			システムがイベントを処理しきることが出来ずにフリーズする可能性も
			あるため、注意が必要です。
		</p>
	*/
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Timer);
		if(result) *result = (tTVInteger)_this->GetCapacity();
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_BEGIN_NATIVE_PROP_SETTER
	{
		RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_Timer);
		_this->SetCapacity((tTVInteger)*param);
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(capacity)
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
			protected depends_on<tRisaRisseScriptEngine>
{
public:
	tRisaTimerRegisterer()
	{
		iRisseDispatch2 * cls =  new tRisseNC_Timer();
		try
		{
			depends_on<tRisaRisseScriptEngine>::locked_instance()->
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

#endif


