//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief アイドル時に発生するイベントを提供するモジュール
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/event/IdleEvent.h"


/*
	pointer_list はすでにそれ自身でスレッド保護を行っているので
	pointer_list にさわっている分には特にスレッド保護を考えなくてもよい
*/

namespace Risa {
RISSE_DEFINE_SOURCE_ID(58504,55707,27606,20246,35200,16274,28002,46284);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIdleEventManager::Register(tIdleEventDestination * item)
{
	Destinations.add(item);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIdleEventManager::Unregister(tIdleEventDestination * item)
{
	Destinations.remove(item);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tIdleEventManager::Deliver(risse_uint64 mastertick)
{
	bool need_more = false;

	tEventSystem * r = tEventSystem::instance();
	if(r->GetCanDeliverEvents())
	{
		// イベントを配信する
		pointer_list<tIdleEventDestination>::scoped_lock lock(Destinations);
		for(size_t i = 0; i < Destinations.get_locked_count(); i++)
		{
			if(Destinations.get_locked(i)->OnIdle(mastertick))
				need_more = true;
		}
	}
	return need_more;
}
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
tIdleEventDestination::tIdleEventDestination()
{
	Receiving = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tIdleEventDestination::~tIdleEventDestination()
{
	if(Receiving) tIdleEventManager::instance()->Unregister(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIdleEventDestination::StartReceiveIdle()
{
	// TODO: ここのスレッド保護
	if(!Receiving)
	{
		tIdleEventManager::instance()->Register(this);
		Receiving = true;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tIdleEventDestination::EndReceiveIdle()
{
	// TODO: ここのスレッド保護
	if(Receiving)
	{
		tIdleEventManager::instance()->Unregister(this);
		Receiving = false;
	}
}
//---------------------------------------------------------------------------
































//---------------------------------------------------------------------------
tCompactEventManager::tCompactEventManager()
{
	wxTimer::Start(5000); // 5秒周期のタイマーをスタートする
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompactEventManager::Register(tCompactEventDestination * item)
{
	Destinations.add(item);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompactEventManager::Unregister(tCompactEventDestination * item)
{
	Destinations.remove(item);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompactEventManager::Deliver(tCompactLevel level)
{
	pointer_list<tCompactEventDestination>::scoped_lock lock(Destinations);
	for(size_t i = 0; i < Destinations.get_locked_count(); i++)
		Destinations.get_locked(i)->OnCompact(level);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompactEventManager::Notify()
{
	Deliver(clSlowBeat);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompactEventManager::OnDeactivate()
{
	Deliver(clDeactivate);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tCompactEventManager::OnDeactivateApp()
{
	Deliver(clDeactivateApp);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risa




