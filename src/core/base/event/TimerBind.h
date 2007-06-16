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
#ifndef _RISATIMERBIND_H
#define _RISATIMERBIND_H


#include "risseObject.h"
#include "risseClass.h"
#include "risseGC.h"
#include "risseNativeBinder.h"


class tRisaTimerBindingConsumer;
//---------------------------------------------------------------------------
//! @brief		"Timer" クラスのインスタンス用 C++クラス
//---------------------------------------------------------------------------
class tRisaTimerInstance : public tRisseObjectBase
{
	tRisaTimerBindingConsumer * Consumer; //!< Timer のイベントを受け取るインスタンス

public:
	//! @brief		コンストラクタ
	tRisaTimerInstance();

	//! @brief		ダミーのデストラクタ(おそらく呼ばれない)
	virtual ~tRisaTimerInstance() {;}

public: // Risse用メソッドなど
	void construct();
	void initialize(const tRisseNativeCallInfo &info);
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
//! @brief		"Timer" クラス
//---------------------------------------------------------------------------
class tRisaTimerClass : public tRisseClassBase
{
	typedef tRisseClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisaTimerClass(tRisseScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	tRisseVariant CreateNewObjectBase();

public:
};
//---------------------------------------------------------------------------

#endif

