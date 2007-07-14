//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief アイドル時に発生するイベントを提供するモジュール
//---------------------------------------------------------------------------
#ifndef _IDLEEVENT_H
#define _IDLEEVENT_H

/*! @note
	アイドルイベントは、アプリケーションで他に処理すべきイベントが無くなった
	場合に発生するイベントである (実際のところこれは嘘で、wxWidgets の Idle
	イベント時に確かに発生するイベントではあるが、Risa のイベントシステムの
	キュー上にイベントが残っていても、一回で処理すべきイベントの処理が終われば
	呼び出される)。
	アイドルイベントの戻り値を真にすると連続してアイドルイベントが発生するように
	なる (吉里吉里２における Continous Event と同等)。

	コンパクトイベントは、アプリケーションのウィンドウが非アクティブになったり
	アプリケーションが非アクティブになったり、あるいは５秒おきに発生するイベン
	ト。なにか未処理の終了処理があればそれを終了させたり、ガベージコレクション
	を実行したりする。
*/



#include "base/event/Event.h"
#include "base/utils/Singleton.h"
#include "base/utils/PointerList.h"
#include <wx/timer.h>

class tRisaIdleEventDestination;
//---------------------------------------------------------------------------
//! @brief		アイドル時に発生するイベントを管理するクラス
//---------------------------------------------------------------------------
class tRisaIdleEventManager : public singleton_base<tRisaIdleEventManager>, depends_on<tRisaEventSystem>
{
	friend class tRisaIdleEventDestination;

	pointer_list<tRisaIdleEventDestination> Destinations; //!< イベントの配信先

protected:
	//! @brief		アイドルイベントの配信先を登録する
	//! @param		item		配信先
	void Register(tRisaIdleEventDestination * item);

	//! @brief		アイドルイベントの配信先の登録を解除する
	//! @param		item		配信先
	void Unregister(tRisaIdleEventDestination * item);

public:
	//! @brief		アイドルイベントを配信する
	//! @param		mastertick		このイベントが配信される際の TickCount (マスタ・ティック)
	//! @return		まだアイドルイベントが必要かどうか
	bool Deliver(risse_uint64 mastertick);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		アイドル時に発生するイベントの送り先となるクラス
//! @note		StartReceiveIdle を呼ばない限りはイベントは発生しない
//!				このクラスのインスタンスのデストラクタは、メインスレッド以外から非同期に
//!				呼ばれる可能性があるので注意すること (シングルトンクラスならば問題ない)
//---------------------------------------------------------------------------
class tRisaIdleEventDestination : protected depends_on<tRisaIdleEventManager>,
	public tDestructee
{
	bool Receiving; //!< イベント配信が有効かどうか

public:
	//! @brief		コンストラクタ
	tRisaIdleEventDestination();

	//! @brief		デストラクタ
	virtual ~tRisaIdleEventDestination();

	//! @brief		アイドルイベントの受信を開始する
	void StartReceiveIdle();

	//! @brief		アイドルイベントの受信を停止する
	void EndReceiveIdle();

public:
	//! @brief		アイドルイベントが配信されるとき
	//! @param		tick  イベントが配信されたときの TickCount
	//! @return		もっとアイドルイベントが欲しいときに真
	virtual bool OnIdle(risse_uint64 tick) = 0;
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		列挙型の名前空間用のためのクラス
//---------------------------------------------------------------------------
class tRisaCompactEventEnum
{
public:
	enum tCompactLevel
	{
		clSlowBeat, // on every 5 sec.
		clDeactivate, // on application/window deactivate
		clDeactivateApp // on application deactivate
	};
};
//---------------------------------------------------------------------------


class tRisaCompactEventDestination;
//---------------------------------------------------------------------------
//! @brief		コンパクト時に発生するイベントを管理するクラス
//---------------------------------------------------------------------------
class tRisaCompactEventManager : public singleton_base<tRisaCompactEventManager>,
								public tRisaCompactEventEnum,
								protected wxTimer
{
	friend class tRisaCompactEventDestination;

	pointer_list<tRisaCompactEventDestination> Destinations; //!< イベントの配信先

public:
	//! @brief		コンストラクタ
	tRisaCompactEventManager();

protected:
	//! @brief		コンパクトイベントの配信先を登録する
	//! @param		item		配信先
	void Register(tRisaCompactEventDestination * item);

	//! @brief		コンパクトイベントの配信先の登録を解除する
	//! @param		item		配信先
	void Unregister(tRisaCompactEventDestination * item);

	//! @brief		コンパクトイベントを配信する
	//! @param		level		レベル
	void Deliver(tCompactLevel level);

	//! @brief		タイマー周期が来た時(wxTimer::Notify 実装)
	void Notify(); // wxTimer::Notify

public:
	//! @brief		ウィンドウが非アクティブになった場合
	void OnDeactivate();

	//! @brief		アプリケーションが非アクティブになった場合
	void OnDeactivateApp();
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		コンパクト時に発生するイベントの送り先となるクラス
//!				このクラスのインスタンスのデストラクタは、メインスレッド以外から非同期に
//!				呼ばれる可能性があるので注意すること (シングルトンクラスならば問題ない)
//---------------------------------------------------------------------------
class tRisaCompactEventDestination : protected depends_on<tRisaCompactEventManager>,
								public tRisaCompactEventEnum, public tDestructee
{
public:
	tRisaCompactEventDestination()
	{ tRisaCompactEventManager::instance()->Register(this); }
	virtual ~tRisaCompactEventDestination()
	{ tRisaCompactEventManager::instance()->Unregister(this); }

public:
	//! @brief		コンパクトイベントが配信されるとき
	//! @param		コンパクトレベル
	virtual void OnCompact(tCompactLevel level) = 0;
};
//---------------------------------------------------------------------------

#endif
