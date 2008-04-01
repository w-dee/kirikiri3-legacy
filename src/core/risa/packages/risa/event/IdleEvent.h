//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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



#include "risa/packages/risa/event/Event.h"
#include "risa/common/Singleton.h"
#include "risa/common/PointerList.h"
#include <wx/timer.h>

namespace Risa {
//---------------------------------------------------------------------------


class tIdleEventDestination;
//---------------------------------------------------------------------------
//! @brief		アイドル時に発生するイベントを管理するクラス
//---------------------------------------------------------------------------
class tIdleEventManager : public singleton_base<tIdleEventManager>, depends_on<tEventSystem>
{
	friend class tIdleEventDestination;

	pointer_list<tIdleEventDestination> Destinations; //!< イベントの配信先

protected:
	//! @brief		アイドルイベントの配信先を登録する
	//! @param		item		配信先
	void Register(tIdleEventDestination * item);

	//! @brief		アイドルイベントの配信先の登録を解除する
	//! @param		item		配信先
	void Unregister(tIdleEventDestination * item);

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
class tIdleEventDestination : protected depends_on<tIdleEventManager>,
	public tDestructee
{
	bool Receiving; //!< イベント配信が有効かどうか

public:
	//! @brief		コンストラクタ
	tIdleEventDestination();

	//! @brief		デストラクタ
	virtual ~tIdleEventDestination();

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
class tCompactEventEnum
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


class tCompactEventDestination;
//---------------------------------------------------------------------------
//! @brief		コンパクト時に発生するイベントを管理するクラス
//---------------------------------------------------------------------------
class tCompactEventManager : public singleton_base<tCompactEventManager>,
								public tCompactEventEnum,
								protected wxTimer
{
	friend class tCompactEventDestination;

	pointer_list<tCompactEventDestination> Destinations; //!< イベントの配信先

public:
	//! @brief		コンストラクタ
	tCompactEventManager();

protected:
	//! @brief		コンパクトイベントの配信先を登録する
	//! @param		item		配信先
	void Register(tCompactEventDestination * item);

	//! @brief		コンパクトイベントの配信先の登録を解除する
	//! @param		item		配信先
	void Unregister(tCompactEventDestination * item);

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
class tCompactEventDestination : protected depends_on<tCompactEventManager>,
								public tCompactEventEnum, public tDestructee
{
public:
	tCompactEventDestination()
	{ tCompactEventManager::instance()->Register(this); }
	virtual ~tCompactEventDestination()
	{ tCompactEventManager::instance()->Unregister(this); }

public:
	//! @brief		コンパクトイベントが配信されるとき
	//! @param		コンパクトレベル
	virtual void OnCompact(tCompactLevel level) = 0;
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif
