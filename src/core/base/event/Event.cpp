//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief イベントシステム
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/event/Event.h"
#include "base/exception/RisaException.h"
#include <wx/app.h>

namespace Risa {
RISSE_DEFINE_SOURCE_ID(1676,31212,48005,18878,7819,32358,49817,14499);
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
tRisaEventSystem::tRisaEventSystem()
{
	// フィールドの初期化
	CanDeliverEvents = true;
	HasPendingEvents = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaEventSystem::~tRisaEventSystem()
{
	// キューをすべて破棄する
	for(int i = tRisaEventInfo::epMin; i <= tRisaEventInfo::epMax; i++)
		DiscardQueue(Queues[i]);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaEventSystem::DeliverQueue(tRisaEventInfo::tPriority prio, risse_uint64 mastertick)
{
	tQueue & queue = Queues[prio];
	while(true)
	{
		tRisaEventInfo * event;

		// キューからイベントを一つ拾ってくる
		// ここは複数スレッドからのアクセスから保護する
		{
			volatile tRisaCriticalSection::tLocker holder(CS);
			if(queue.size() == 0) break;
			event = queue.front();
			if(!event) break; // イベントがNULLなのでpopせずに戻る
			queue.pop_front();
		}

		// イベントを配信する
		if(event)
		{
			try
			{
				try
				{
					event->SetTick(mastertick); // tick の設定
					event->Deliver();
				}
				catch(...)
				{
					delete event;
					throw;
				}
				delete event;
			}
			RISA_CATCH_AND_SHOW_SCRIPT_EXCEPTION(RISSE_WS("Asynchronous Event"))
		}

		// ここは複数スレッドからのアクセスから保護する
		{
			volatile tRisaCriticalSection::tLocker holder(CS);

			// キューに epExclusive のイベントがpostされたら戻る
			if(prio != tRisaEventInfo::epExclusive &&
				Queues[tRisaEventInfo::epExclusive].size() != 0) break;

			// CanDeliverEvents が偽になったら戻る
			if(!CanDeliverEvents) break;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaEventSystem::DeliverEvents(risse_uint64 mastertick)
{
	// 一回で配信するイベントの量を、現時点でのイベントまでに
	// 制限するため、イベントのセンチネルとして null を各キュー(ただしepExclusive以外)
	// に入れる
	// ここは複数スレッドからのアクセスから保護する
	{
		volatile tRisaCriticalSection::tLocker holder(CS);

		if(!CanDeliverEvents) return; // イベントを配信できない

		if(!HasPendingEvents) return; // 未配信のイベントはない
		HasPendingEvents = false; // いったんこのフラグはここで偽に

		for(int i = tRisaEventInfo::epMin; i <= tRisaEventInfo::epMax; i++)
		{
			if(i != tRisaEventInfo::epExclusive)
				Queues[i].push_back(NULL);
		}
	}

	// イベントを配信する
	bool events_in_exclusive = false;
	do
	{
		for(int i = tRisaEventInfo::epMin;
			i <= tRisaEventInfo::epMax; i++)
		{
			DeliverQueue(static_cast<tRisaEventInfo::tPriority>(i), mastertick);

			// ここは複数スレッドからのアクセスから保護する
			{
				volatile tRisaCriticalSection::tLocker holder(CS);

				// キューに epExclusive のイベントがpostされたら
				// もう一度最初から
				if(Queues[tRisaEventInfo::epExclusive].size() != 0)
				{
					events_in_exclusive = true;
					break;
				}
				else
				{
					events_in_exclusive = false;
				}

				// CanDeliverEvents が偽になったら戻る
				if(!CanDeliverEvents) break;
			}
		}
	} while(CanDeliverEvents && events_in_exclusive);

	// 最初に挿入したセンチネルを取り除く
	// ここは複数スレッドからのアクセスから保護する
	{
		volatile tRisaCriticalSection::tLocker holder(CS);
		for(int i = tRisaEventInfo::epMin;
			i <= tRisaEventInfo::epMax; i++)
		{
			if(i != tRisaEventInfo::epExclusive)
			{
				// NULL を探す
				tQueue & queue = Queues[i];
				tQueue::iterator i = std::find(queue.begin(), queue.end(),
								reinterpret_cast<tRisaEventInfo*>(NULL));
				// NULL を除去
				if(i != queue.end())
				{
					do
					{
						i = queue.erase(i);
						if(i == queue.end()) break;
						if(*i != reinterpret_cast<tRisaEventInfo*>(NULL)) break;
					} while(true);
				}
			}
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisaEventSystem::ProcessEvents(risse_uint64 mastertick)
{
	DeliverEvents(mastertick);
	return HasPendingEvents;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaEventSystem::DiscardQueue(tQueue & queue)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	// キュー中のイベントをすべて削除する
	for(tQueue::iterator i = queue.begin(); i != queue.end(); i++)
	{
		tRisaEventInfo * event = *i;
		*i = NULL; // 二度と解放しないように
		delete event;
	}

	// キューを空にする
	queue.clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaEventSystem::PostEvent(tRisaEventInfo * event, tEventType type)
{
	if(!tRisaEventInfo::IsPriorityValid(event->GetPriority())) return; // 優先度が無効

	volatile tRisaCriticalSection::tLocker holder(CS);

	// type をチェック
	if((type & etDiscardable) && !CanDeliverEvents)
	{
		// イベントを破棄可能で、かつイベントを配信できない状態の場合
		delete event; // イベントを削除して
		return; // 戻る
	}

	if((type & etSingle) && CountEventsInQueue(
		event->GetId(), event->GetSource(), event->GetPriority()))
	{
		// イベントがすでにキュー内にある場合
		delete event; // イベントを削除して
		return; // 戻る
	}

	// イベントをキューに入れる
	try
	{
		Queues[event->GetPriority()].push_back(event);
	}
	catch(...)
	{
		delete event;
		throw;
	}

	// イベント配信をたたき起こす
	if(!HasPendingEvents)
	{
		HasPendingEvents = true;
		::wxWakeUpIdle();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tRisaEventSystem::CountEventsInQueue(
	int id, void * source, tRisaEventInfo::tPriority prio, size_t limit)
{
	if(!tRisaEventInfo::IsPriorityValid(prio)) return false; // 優先度が無効

	volatile tRisaCriticalSection::tLocker holder(CS);

	// キューを検索する
	size_t count = 0;
	tQueue queue = Queues[prio];
	for(tQueue::iterator i = queue.begin(); i != queue.end(); i++)
	{
		if(*i && (*i)->GetSource() == source && (*i)->GetId() == id)
		{
			count ++;
			if(limit && count >= limit) return count;
		}
	}

	return count;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaEventSystem::CancelEvents(void * source)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	for(int n = tRisaEventInfo::epMin; n <= tRisaEventInfo::epMax; n++)
	{
		tQueue & queue = Queues[n];
		for(tQueue::iterator i = queue.begin(); i != queue.end(); )
		{
			if((*i) && (*i)->GetSource() == source)
			{
				tRisaEventInfo * ev = *i;
				i = queue.erase(i);
				delete ev;
			}
			else
			{
				i++;
			}
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa

