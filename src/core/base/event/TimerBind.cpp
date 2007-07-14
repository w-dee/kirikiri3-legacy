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
	tScriptEngine * ScriptEngine; //!< スクリプトエンジンインスタンス
	tObjectInterface * Target; //!< イベントの送り先

public:
	//! @brief		コンストラクタ
	tRisaTimerBindingConsumer() { ScriptEngine = NULL; }

	//! @brief		スクリプトエンジンとターゲットを設定する
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		target		イベントの送り先
	void SetTarget(tScriptEngine * engine, 
		tObjectInterface * target)  { ScriptEngine = engine; Target = target; }

protected:
	void OnTimer()
	{
		// Target の onTimer を呼び出す
		if(ScriptEngine)
			Target->Operate(ocFuncCall, NULL, tSS<'o','n','T','i','m','e','r'>());
	}
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		"Timer" クラスのインスタンス用 C++クラス
//---------------------------------------------------------------------------
class tRisaTimerInstance : public tObjectBase
{
	tRisaTimerBindingConsumer * Consumer; //!< Timer のイベントを受け取るインスタンス

public:
	//! @brief		コンストラクタ
	tRisaTimerInstance();

	//! @brief		ダミーのデストラクタ(おそらく呼ばれない)
	virtual ~tRisaTimerInstance() {;}

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);
	void set_enabled(bool b);
	bool get_enabled() const;
	void set_interval(risse_uint64 interval);
	risse_uint64 get_interval() const;
	void set_capacity(risse_size capa);
	risse_size get_capacity() const;
	void reset();
	void onTimer();
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
void tRisaTimerInstance::initialize(const tNativeCallInfo &info)
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
//! @brief		"Timer" クラス
//---------------------------------------------------------------------------
class tRisaTimerClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisaTimerClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public:
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tRisaTimerClass::tRisaTimerClass(tScriptEngine * engine) :
	tClassBase(tSS<'T','i','m','e','r'>(), engine->ObjectClass)
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

	BindFunction(this, ss_ovulate, &tRisaTimerClass::ovulate);
	BindFunction(this, ss_construct, &tRisaTimerInstance::construct);
	BindFunction(this, ss_initialize, &tRisaTimerInstance::initialize);
	BindProperty(this, tSS<'e','n','a','b','l','e','d'>(), &tRisaTimerInstance::get_enabled, &tRisaTimerInstance::set_enabled);
	BindProperty(this, tSS<'i','n','t','e','r','v','a','l'>(), &tRisaTimerInstance::get_interval, &tRisaTimerInstance::set_interval);
	BindProperty(this, tSS<'c','a','p','a','c','i','t','y'>(), &tRisaTimerInstance::get_capacity, &tRisaTimerInstance::set_capacity);
	BindFunction(this, tSS<'r','e','s','e','t'>(), &tRisaTimerInstance::reset);
	BindFunction(this, tSS<'o','n','T','i','m','e','r'>(), &tRisaTimerInstance::onTimer);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tRisaTimerClass::ovulate()
{
	return tVariant(new tRisaTimerInstance());
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		Timer クラスレジストラ
template class tRisaRisseClassRegisterer<tRisaTimerClass>;
//---------------------------------------------------------------------------
