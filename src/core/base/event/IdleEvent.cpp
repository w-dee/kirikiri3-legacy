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
#include "prec.h"
#include "base/event/IdleEvent.h"

RISSE_DEFINE_SOURCE_ID(58504,55707,27606,20246,35200,16274,28002,46284);



//---------------------------------------------------------------------------
void tRisaIdleEventManager::Register(tRisaIdleEventDestination * item)
{
	Destinations.add(item);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaIdleEventManager::Unregister(tRisaIdleEventDestination * item)
{
	Destinations.remove(item);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisaIdleEventManager::Deliver(risse_uint64 mastertick)
{
	bool need_more = false;

	// Idleイベントは tRisaEventSystem がイベントを配信可能かどうかを見る
	if(tRisaEventSystem::pointer r = tRisaEventSystem::instance())
	{
		if(r->GetCanDeliverEvents())
		{
			// イベントを配信する
			pointer_list<tRisaIdleEventDestination>::scoped_lock lock(Destinations);
			for(size_t i = 0; i < Destinations.get_locked_count(); i++)
			{
				if(Destinations.get_locked(i)->OnIdle(mastertick))
					need_more = true;
			}
		}
	}
	return need_more;
}
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
tRisaIdleEventDestination::tRisaIdleEventDestination()
{
	Receiving = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaIdleEventDestination::~tRisaIdleEventDestination()
{
	if(Receiving) depends_on<tRisaIdleEventManager>::locked_instance()->Unregister(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaIdleEventDestination::StartReceiveIdle()
{
	if(!Receiving)
	{
		depends_on<tRisaIdleEventManager>::locked_instance()->Register(this);
		Receiving = true;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaIdleEventDestination::EndReceiveIdle()
{
	if(Receiving)
	{
		depends_on<tRisaIdleEventManager>::locked_instance()->Unregister(this);
		Receiving = false;
	}
}
//---------------------------------------------------------------------------
































//---------------------------------------------------------------------------
tRisaCompactEventManager::tRisaCompactEventManager()
{
	wxTimer::Start(5000); // 5秒周期のタイマーをスタートする
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaCompactEventManager::Register(tRisaCompactEventDestination * item)
{
	Destinations.add(item);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaCompactEventManager::Unregister(tRisaCompactEventDestination * item)
{
	Destinations.remove(item);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaCompactEventManager::Deliver(tCompactLevel level)
{
	pointer_list<tRisaCompactEventDestination>::scoped_lock lock(Destinations);
	for(size_t i = 0; i < Destinations.get_locked_count(); i++)
		Destinations.get_locked(i)->OnCompact(level);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaCompactEventManager::Notify()
{
	Deliver(clSlowBeat);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaCompactEventManager::OnDeactivate()
{
	Deliver(clDeactivate);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaCompactEventManager::OnDeactivateApp()
{
	Deliver(clDeactivateApp);
}
//---------------------------------------------------------------------------






