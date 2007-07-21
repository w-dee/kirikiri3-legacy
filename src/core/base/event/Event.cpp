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
#include "base/event/TickCount.h"
#include "base/exception/RisaException.h"
#include <wx/app.h>

namespace Risa {
RISSE_DEFINE_SOURCE_ID(1676,31212,48005,18878,7819,32358,49817,14499);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief	イベントキュー用例外クラス
//---------------------------------------------------------------------------
RISA_DEFINE_EXCEPTION_SUBCLASS(tEventQueueExceptionClass,
	(tSS<'E','v','e','n','t','Q','u','e','u','e','E','x','c','e','p','t','i','o','n'>()),
	tRisseScriptEngine::instance()->GetScriptEngine()->RuntimeExceptionClass)
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
tEventQueueInstance::tEventQueueInstance()
{
	// フィールドの初期化
	HasPendingEvents = false;
	QuitEventFound = false;
	IsMainThreadQueue = false;
	EventNotifier = new tThreadEvent();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventQueueInstance::DeliverQueue(tEventInfo::tPriority prio, risse_uint64 mastertick)
{
	tQueue & queue = Queues[prio];
	while(true)
	{
		tEventInfo * event;

		// キューからイベントを一つ拾ってくる
		// ここは複数スレッドからのアクセスから保護する
		{
			volatile tSynchronizer sync(this); // sync

			if(queue.size() == 0) break;
			event = queue.front();
			if(!event) break; // イベントがNULLなのでpopせずに戻る
			queue.pop_front(); // pop
			if(event->GetId() == 0 && event->GetSource() == this)
			{
				QuitEventFound = true;
				break; // これは quit イベントなのでpopしてから戻る
			}
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
			volatile tSynchronizer sync(this); // sync

			// キューに epExclusive のイベントがpostされたら戻る
			if(prio != tEventInfo::epExclusive &&
				Queues[tEventInfo::epExclusive].size() != 0) break;

			// CanDeliverEvents が偽になったら戻る
			if(!tEventSystem::instance()->GetCanDeliverEvents()) break;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventQueueInstance::DeliverEvents(risse_uint64 mastertick)
{
	// 一回で配信するイベントの量を、現時点でのイベントまでに
	// 制限するため、イベントのセンチネルとして null を各キュー(ただしepExclusive以外)
	// に入れる
	// ここは複数スレッドからのアクセスから保護する
	{
		volatile tSynchronizer sync(this); // sync

		if(!tEventSystem::instance()->GetCanDeliverEvents()) return; // イベントを配信できない

		if(!HasPendingEvents) return; // 未配信のイベントはない
		HasPendingEvents = false; // いったんこのフラグはここで偽に
		QuitEventFound = false; // このフラグもここで偽に

		for(int i = tEventInfo::epMin; i <= tEventInfo::epMax; i++)
		{
			if(i != tEventInfo::epExclusive)
				Queues[i].push_back(NULL);
		}
	}

	// イベントを配信する
	bool events_in_exclusive = false;
	do
	{
		for(int i = tEventInfo::epMin;
			i <= tEventInfo::epMax; i++)
		{
			DeliverQueue(static_cast<tEventInfo::tPriority>(i), mastertick);

			// ここは複数スレッドからのアクセスから保護する
			{
				volatile tSynchronizer sync(this); // sync

				// QuitEventFound だったら中断
				if(QuitEventFound) break;

				// キューに epExclusive のイベントがpostされたら
				// もう一度最初から
				if(Queues[tEventInfo::epExclusive].size() != 0)
				{
					events_in_exclusive = true;
					break;
				}
				else
				{
					events_in_exclusive = false;
				}

				// CanDeliverEvents が偽になったら戻る
				if(!tEventSystem::instance()->GetCanDeliverEvents()) break;
			}
		}
	} while(tEventSystem::instance()->GetCanDeliverEvents() && events_in_exclusive && !QuitEventFound);

	// 最初に挿入したセンチネルを取り除く
	// ここは複数スレッドからのアクセスから保護する
	{
		volatile tSynchronizer sync(this); // sync
		for(int i = tEventInfo::epMin;
			i <= tEventInfo::epMax; i++)
		{
			if(i != tEventInfo::epExclusive)
			{
				// NULL を探す
				tQueue & queue = Queues[i];
				tQueue::iterator i = std::find(queue.begin(), queue.end(),
								reinterpret_cast<tEventInfo*>(NULL));
				// NULL を除去
				if(i != queue.end())
				{
					do
					{
						i = queue.erase(i);
						if(i == queue.end()) break;
						if(*i != reinterpret_cast<tEventInfo*>(NULL)) break;
					} while(true);
				}
			}
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tEventQueueInstance::ProcessEvents(risse_uint64 mastertick)
{
	DeliverEvents(mastertick);
	return HasPendingEvents;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventQueueInstance::DiscardQueue(tQueue & queue)
{
	volatile tSynchronizer sync(this); // sync

	// キュー中のイベントをすべて削除する
	for(tQueue::iterator i = queue.begin(); i != queue.end(); i++)
	{
		tEventInfo * event = *i;
		*i = NULL; // 二度と解放しないように
		delete event;
	}

	// キューを空にする
	queue.clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventQueueInstance::PostEvent(tEventInfo * event, tEventType type)
{
	if(!tEventInfo::IsPriorityValid(event->GetPriority())) return; // 優先度が無効

	volatile tSynchronizer sync(this); // sync

	// type をチェック
	if((type & etDiscardable) && !tEventSystem::instance()->GetCanDeliverEvents())
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
		if(IsMainThreadQueue)
			::wxWakeUpIdle();
		else
			EventNotifier->Signal();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tEventQueueInstance::CountEventsInQueue(
	int id, void * source, tEventInfo::tPriority prio, size_t limit)
{
	if(!tEventInfo::IsPriorityValid(prio)) return false; // 優先度が無効

	volatile tSynchronizer sync(this); // sync

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
void tEventQueueInstance::CancelEvents(void * source)
{
	volatile tSynchronizer sync(this); // sync

	for(int n = tEventInfo::epMin; n <= tEventInfo::epMax; n++)
	{
		tQueue & queue = Queues[n];
		for(tQueue::iterator i = queue.begin(); i != queue.end(); )
		{
			if((*i) && (*i)->GetSource() == source)
			{
				tEventInfo * ev = *i;
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
void tEventQueueInstance::Loop()
{
	if(IsMainThreadQueue)
		tEventQueueExceptionClass::Throw(
					RISSE_WS_TR("Use this method for non-main queue"));
	for(;;)
	{
		risse_uint64 tick = tTickCount::instance()->Get();
		bool has_pending = ProcessEvents(tick);
		if(QuitEventFound) break;
		if(!has_pending)
		{
			// イベントが来るまで待機
			EventNotifier->Wait();
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventQueueInstance::QuitLoop()
{
	if(IsMainThreadQueue)
		tEventQueueExceptionClass::Throw(
					RISSE_WS_TR("Use this method for non-main queue"));
	PostEvent(new tEventInfo(0, this), etDefault); // this が source のイベントは特殊なイベント
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventQueueInstance::construct()
{
	// デフォルトではなにもしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventQueueInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tEventQueueClass::tEventQueueClass(tScriptEngine * engine) :
	tClassBase(tSS<'E','v','e','n','t','Q','u','e','u','e'>(), engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventQueueClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tEventQueueClass::ovulate);
	BindFunction(this, ss_construct, &tEventQueueInstance::construct);
	BindFunction(this, ss_initialize, &tEventQueueInstance::initialize);
	BindFunction(this, tSS<'l','o','o','p'>(), &tEventQueueInstance::loop);
	BindFunction(this, tSS<'q','u','i','t'>(), &tEventQueueInstance::quit);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tEventQueueClass::ovulate()
{
	return tVariant(new tEventQueueInstance());
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		EventQueue クラスレジストラ
template class tRisseClassRegisterer<tEventQueueClass>;
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
tMainEventQueue::tMainEventQueue()
{
	// イベントキューインスタンスを作成する
	// スクリプトエンジンの取得、グローバルオブジェクトの取得、イベントキュークラスの
	// 取得、イベントキューインスタンスの作成を順に行う
	tScriptEngine * engine = tRisseScriptEngine::instance()->GetScriptEngine();
	tVariant global_object = engine->GetGlobalObject();
	tVariant eventqueue_class = global_object.GetPropertyDirect(engine,
						tSS<'E','v','e','n','t','Q','u','e','u','e'>());
	tVariant instance_v = eventqueue_class.New();
	RISSE_ASSERT(instance_v.GetType() == tVariant::vtObject);
	EventQueue = instance_v;
	((tEventQueueInstance*)EventQueue.GetObjectInterface())->SetIsMainThreadQueue(true);
		// メインのイベントキューはメインスレッド(GUIスレッド)にて
		// アイドルイベントを利用して配信されるため、
		// イベントキューにイベントが入ったらアイドルイベントを起動するように設定する

	// EventQueue::mainQueue にインスタンスを登録する
	eventqueue_class.SetPropertyDirect_Object(tSS<'m','a','i','n','Q','u','e','u','e'>(),
								tOperateFlags::ofUseClassMembersRule|
								tOperateFlags::ofMemberEnsure,
								instance_v);
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tEventSourceInstance::tEventSourceInstance()
{
	// デフォルトのイベントの配信先をメインのイベントキューに設定する
	DestEventQueue = tMainEventQueue::instance()->GetEventQueue();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventSourceInstance::construct()
{
	// デフォルトではなにもしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventSourceInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventSourceInstance::SetDestEventQueue(const tVariant & queue)
{
	DestEventQueue.AssertClass(tRisseClassRegisterer<tEventQueueClass>::instance()->GetClassInstance());
	DestEventQueue = queue;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
tEventSourceClass::tEventSourceClass(tScriptEngine * engine) :
	tClassBase(tSS<'E','v','e','n','t','S','o','u','r','c','e'>(), engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tEventSourceClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tEventSourceClass::ovulate);
	BindFunction(this, ss_construct, &tEventSourceInstance::construct);
	BindFunction(this, ss_initialize, &tEventSourceInstance::initialize);
	BindProperty(this, tSS<'q','u','e','u','e'>(),
			&tEventSourceInstance::get_queue, &tEventSourceInstance::set_queue);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tEventSourceClass::ovulate()
{
	// このクラスのインスタンスは作成できないので例外を投げる
	tInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
	return tVariant();
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		EventSource クラスレジストラ
template class tRisseClassRegisterer<tEventSourceClass>;
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
} // namespace Risa

