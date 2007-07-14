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



namespace Risa {
RISSE_DEFINE_SOURCE_ID(25021,49177,9141,20257,35249,61240,44766,27087);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Timerのイベントを受け取るクラス
//---------------------------------------------------------------------------
class tTimerBindingConsumer : public tEventTimerConsumer
{
	tScriptEngine * ScriptEngine; //!< スクリプトエンジンインスタンス
	tObjectInterface * Target; //!< イベントの送り先

public:
	//! @brief		コンストラクタ
	tTimerBindingConsumer() { ScriptEngine = NULL; }

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
class tTimerInstance : public tObjectBase
{
	tTimerBindingConsumer * Consumer; //!< Timer のイベントを受け取るインスタンス

public:
	//! @brief		コンストラクタ
	tTimerInstance();

	//! @brief		ダミーのデストラクタ(おそらく呼ばれない)
	virtual ~tTimerInstance() {;}

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
tTimerInstance::tTimerInstance()
{
	Consumer = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTimerInstance::construct()
{
	Consumer = new tTimerBindingConsumer();
	Consumer->SetTarget(GetRTTI()->GetScriptEngine(), this);
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
void tTimerInstance::set_enabled(bool b)
{
	Consumer->SetEnabled(b);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tTimerInstance::get_enabled() const
{
	return Consumer->GetEnabled();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTimerInstance::set_interval(risse_uint64 interval)
{
	Consumer->SetInterval(interval);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tTimerInstance::get_interval() const
{
	return Consumer->GetInterval();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTimerInstance::set_capacity(risse_size capa)
{
	Consumer->SetCapacity(capa);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tTimerInstance::get_capacity() const
{
	return Consumer->GetCapacity();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTimerInstance::reset()
{
	Consumer->Reset();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTimerInstance::onTimer()
{
	// Timer.onTimer は何もしない
}
//---------------------------------------------------------------------------
















//---------------------------------------------------------------------------
//! @brief		"Timer" クラス
//---------------------------------------------------------------------------
class tTimerClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tTimerClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public:
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tTimerClass::tTimerClass(tScriptEngine * engine) :
	tClassBase(tSS<'T','i','m','e','r'>(), engine->ObjectClass)
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
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tTimerClass::ovulate()
{
	return tVariant(new tTimerInstance());
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		Timer クラスレジストラ
template class tRisseClassRegisterer<tTimerClass>;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa

