//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief アイドル時に発生するイベントを提供するモジュール
//---------------------------------------------------------------------------
#ifndef _IDLEEVENT_H
#define _IDLEEVENT_H

#include "base/event/Event.h"
#include "base/utils/Singleton.h"
#include "base/utils/PointerList.h"
#include <wx/timer.h>

class tRisaIdleEventDestination;
//---------------------------------------------------------------------------
//! @brief		アイドル時に発生するイベントを管理するクラス
//---------------------------------------------------------------------------
class tRisaIdleEventManager : public singleton_base<tRisaIdleEventManager>
{
	friend class tRisaIdleEventDestination;

	pointer_list<tRisaIdleEventDestination> Destinations; //!< イベントの配信先

protected:
	void Register(tRisaIdleEventDestination * item);
	void Unregister(tRisaIdleEventDestination * item);

public:
	bool Deliver(risse_uint64 mastertick);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		アイドル時に発生するイベントの送り先となるクラス
//! @note		StartReceiveIdle を呼ばない限りはイベントは発生しない
//---------------------------------------------------------------------------
class tRisaIdleEventDestination : depends_on<tRisaIdleEventManager>
{
	bool Receiving; //!< イベント配信が有効かどうか

public:
	tRisaIdleEventDestination();
	virtual ~tRisaIdleEventDestination();
	void StartReceiveIdle();
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
	tRisaCompactEventManager();

protected:
	void Register(tRisaCompactEventDestination * item);
	void Unregister(tRisaCompactEventDestination * item);

	void Deliver(tCompactLevel level);
	void Notify(); // wxTimer::Notify

public:
	void OnDeactivate();
	void OnDeactivateApp();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンパクト時に発生するイベントの送り先となるクラス
//---------------------------------------------------------------------------
class tRisaCompactEventDestination : depends_on<tRisaCompactEventManager>,
								public tRisaCompactEventEnum
{
public:
	tRisaCompactEventDestination() { tRisaCompactEventManager::instance()->Register(this); }
	virtual ~tRisaCompactEventDestination() { tRisaCompactEventManager::instance()->Unregister(this); }

public:
	//! @brief		コンパクトイベントが配信されるとき
	//! @param		コンパクトレベル
	virtual void OnCompact(tCompactLevel level) = 0;
};
//---------------------------------------------------------------------------

#endif
