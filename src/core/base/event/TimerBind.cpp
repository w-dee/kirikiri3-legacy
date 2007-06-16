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
#include "risse/include/risseStringTemplate.h"
#include "risse/include/risseStaticStrings.h"
#include "risse/include/risseObjectClass.h"

RISSE_DEFINE_SOURCE_ID(25021,49177,9141,20257,35249,61240,44766,27087);

//---------------------------------------------------------------------------
//! @brief		Timerのイベントを受け取るクラス
//---------------------------------------------------------------------------
class tRisaTimerBindingConsumer : public tRisaEventTimerConsumer
{
	tRisseScriptEngine * ScriptEngine; //!< スクリプトエンジンインスタンス
	tRisseObjectInterface * Target; //!< イベントの送り先

public:
	//! @brief		コンストラクタ
	tRisaTimerBindingConsumer() { ScriptEngine = NULL; }

	//! @brief		スクリプトエンジンとターゲットを設定する
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		target		イベントの送り先
	void SetTarget(tRisseScriptEngine * engine, 
		tRisseObjectInterface * target)  { ScriptEngine = engine; Target = target; }

protected:
	void OnTimer()
	{
		// Target の onTimer を呼び出す
		if(ScriptEngine)
			Target->Operate(ocFuncCall, NULL, tRisseSS<'o','n','T','i','m','e','r'>());
	}
};
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
tRisaTimerInstance::tRisaTimerInstance()
{
	Consumer = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaTimerInstance::construct()
{
	Consumer = new tRisaTimerBindingConsumer();
	Consumer->SetTarget(GetRTTI()->GetScriptEngine(), this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaTimerInstance::initialize(const tRisseNativeCallInfo &info)
{
	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaTimerInstance::set_enabled(bool b)
{
	Consumer->SetEnabled(b);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisaTimerInstance::get_enabled() const
{
	return Consumer->GetEnabled();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaTimerInstance::set_interval(risse_uint64 interval)
{
	Consumer->SetInterval(interval);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tRisaTimerInstance::get_interval() const
{
	return Consumer->GetInterval();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaTimerInstance::set_capacity(risse_size capa)
{
	Consumer->SetCapacity(capa);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tRisaTimerInstance::get_capacity() const
{
	return Consumer->GetCapacity();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaTimerInstance::reset()
{
	Consumer->Reset();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaTimerInstance::onTimer()
{
	// Timer.onTimer は何もしない
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tRisaTimerClass::tRisaTimerClass(tRisseScriptEngine * engine) :
	tRisseClassBase(engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaTimerClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	RisseBindFunction(this, ss_construct, &tRisaTimerInstance::construct);
	RisseBindFunction(this, ss_initialize, &tRisaTimerInstance::initialize);
	RisseBindProperty(this, tRisseSS<'e','n','a','b','l','e','d'>(), &tRisaTimerInstance::get_enabled, &tRisaTimerInstance::set_enabled);
	RisseBindProperty(this, tRisseSS<'i','n','t','e','r','v','a','l'>(), &tRisaTimerInstance::get_interval, &tRisaTimerInstance::set_interval);
	RisseBindProperty(this, tRisseSS<'c','a','p','a','c','i','t','y'>(), &tRisaTimerInstance::get_capacity, &tRisaTimerInstance::set_capacity);
	RisseBindFunction(this, tRisseSS<'r','e','s','e','t'>(), &tRisaTimerInstance::reset);
	RisseBindFunction(this, tRisseSS<'o','n','T','i','m','e','r'>(), &tRisaTimerInstance::onTimer);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseVariant tRisaTimerClass::CreateNewObjectBase()
{
	return tRisseVariant(new tRisaTimerInstance());
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		Timer クラスレジストラ
template class tRisaRisseClassRegisterer<tRisaTimerClass, tRisseSS<'T','i','m','e','r'> >;
//---------------------------------------------------------------------------
