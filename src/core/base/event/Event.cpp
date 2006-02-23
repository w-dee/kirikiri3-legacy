//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief イベントシステム
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/event/Event.h"

RISSE_DEFINE_SOURCE_ID(1676,31212,48005,18878,7819,32358,49817,14499);

/*
Risa のイベントシステムについて

■ 用語

・発生元 (event source)
  イベントを発生させる物

・配信先 (event destination)
  イベントを配信する先


■ イベントの種類

・同期イベント
  発生源から直接呼び出されるイベント。

・非同期イベント
  発生源が一度イベントシステムにイベントをポストし、イベントシステムが非同期に
  (あとで)そのイベントを配信先に届ける。

・ペイントイベント
  ウィンドウの再描画時に発生するイベント。基本的に非同期イベント。


■ 非同期イベントの優先度

・排他的イベント (exclusive event)
  ほかの非同期イベントよりも優先して配信されるイベント。

・通常イベント

・ペイントイベント

・低優先度イベント (low priority event)
  ほかの非同期イベントをすべて処理し終わってから配信されるイベント。

■ 非同期イベントの属性

・破棄可能イベント (discardable event)
  システムの負荷などによっては廃棄してもかまわないようなイベント。

・シングルイベント (single event)
  同じイベント名、優先度、同じ発生元のイベントがすでにキューにあれば(まだ配信さ
  れていなければ)キューには入らないイベント。

*/



//---------------------------------------------------------------------------
//! @brief		イベントの優先順位を比較するための関数オブジェクトクラス
//---------------------------------------------------------------------------
struct tRisaEventPriorityCompare :
	std::binary_function<tRisaEventBase*, tRisaEventBase*, bool>
{
    bool operator()(const tRisaEventBase*& X, const tRisaEventBase*& Y) const
    {
        return (X->GetPriority() < Y->GetPriority());
    }
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaEventSystem::tRisaEventSystem()
{
	// フィールドの初期化
	CanDeliverEvents = true;
	HasPendingEvents = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaEventSystem::~tRisaEventSystem()
{
	// キューをすべて破棄する
	for(int i = tRisaEventBase::epMin; i <= tRisaEventBase::epMax; i++)
		DiscardQueue(Queues[i]);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定された優先度のキューの中のイベントを配信する
//! @param		prio		優先度
//! @note		prio!=tRisaEventBase::epExclusiveの場合、epExclusiveのイベントが
//!				ポストされたり、CanDeliverEvents が偽になった場合は即座に戻る。
//---------------------------------------------------------------------------
void tRisaEventSystem::DeliverQueue(tRisaEventBase::tPriority prio)
{
	tQueue & queue = Queues[prio];
	while(true)
	{
		tRisaEventBase * event;

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
				event->Deliver();
			}
			catch(...)
			{
				delete event;
				throw;
			}
			delete event;
		}

		// ここは複数スレッドからのアクセスから保護する
		{
			volatile tRisaCriticalSection::tLocker holder(CS);

			// キューに epExclusive のイベントがpostされたら戻る
			if(prio != tRisaEventBase::epExclusive &&
				Queues[tRisaEventBase::epExclusive].size() != 0) break;

			// CanDeliverEvents が偽になったら戻る
			if(!CanDeliverEvents) break;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		すべてのイベントを配信する
//---------------------------------------------------------------------------
void tRisaEventSystem::DeliverEvents()
{
	// 一回で配信するイベントの量を、現時点でのイベントまでに
	// 制限するため、イベントのセンチネルとして null を各キュー(ただしepExclusive以外)
	// に入れる
	// ここは複数スレッドからのアクセスから保護する
	{
		volatile tRisaCriticalSection::tLocker holder(CS);

		if(!HasPendingEvents) return; // 未配信のイベントはない
		HasPendingEvents = false; // いったんこのフラグはここで偽に

		for(int i = tRisaEventBase::epMin; i <= tRisaEventBase::epMax; i++)
		{
			if(i != tRisaEventBase::epExclusive)
				Queues[i].push_back(NULL);
		}
	}

	// イベントを配信する
	bool events_in_exclusive = false;
	do
	{
		for(int i = tRisaEventBase::epMin;
			i <= tRisaEventBase::epMax; i++)
		{
			DeliverQueue(static_cast<tRisaEventBase::tPriority>(i));

			// ここは複数スレッドからのアクセスから保護する
			{
				volatile tRisaCriticalSection::tLocker holder(CS);

				// キューに epExclusive のイベントがpostされたら
				// もう一度最初から
				if(Queues[tRisaEventBase::epExclusive].size() != 0)
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
		for(int i = tRisaEventBase::epMin;
			i <= tRisaEventBase::epMax; i++)
		{
			if(i != tRisaEventBase::epExclusive)
			{
				while(Queues[i].size() > 0 && Queues[i].front() == NULL)
					Queues[i].pop_front();
			}
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		イベントの配信処理を行う
//! @return		まだ処理すべきイベントがあるかどうか
//! @note		wxApp::ProcessIdle から呼ばれる
//---------------------------------------------------------------------------
bool tRisaEventSystem::ProcessEvents()
{
	DeliverEvents();
	return HasPendingEvents;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定キュー中のイベントをすべて破棄する
//! @param		queue		キュー
//---------------------------------------------------------------------------
void tRisaEventSystem::DiscardQueue(tQueue & queue)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	// キュー中のイベントをすべて削除する
	for(tQueue::iterator i = queue.begin(); i != queue.end(); i++)
	{
		tRisaEventBase * event = *i;
		*i = NULL; // 二度と解放しないように
		delete event;
	}

	// キューを空にする
	queue.clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		イベントをポストする
//---------------------------------------------------------------------------
void tRisaEventSystem::PostEvent(tRisaEventBase * event, tEventType type)
{
	if(!tRisaEventBase::IsPriorityValid(event->GetPriority())) return; // 優先度が無効

	volatile tRisaCriticalSection::tLocker holder(CS);

	// type をチェック
	if((type & etDiscardable) && !CanDeliverEvents)
	{
		// イベントを破棄可能で、かつイベントを配信できない状態の場合
		delete event; // イベントを削除して
		return; // 戻る
	}

	if((type & etSingle) && CountEventsInQueue(
		event->GetName(), event->GetSourceNoAddRef(), event->GetPriority()))
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
//! @brief		指定されたイベントがすでにキューに入っているかどうかを調べる
//! @param		name		イベント名
//! @param		source		イベント発生元
//! @param		prio		優先度
//! @param		limit		数え上げる最大値(0=全部数える)
//! @return		name と source と prio が一致するイベントがすでにキューにあるかどうか
//---------------------------------------------------------------------------
size_t tRisaEventSystem::CountEventsInQueue(
	const ttstr & name,
	iRisseDispatch2 * source, tRisaEventBase::tPriority prio, size_t limit)
{
	if(!tRisaEventBase::IsPriorityValid(prio)) return false; // 優先度が無効

	volatile tRisaCriticalSection::tLocker holder(CS);

	// キューを検索する
	size_t count = 0;
	tQueue queue = Queues[prio];
	for(tQueue::iterator i = queue.begin(); i != queue.end(); i++)
	{
		if(*i && (*i)->GetSourceNoAddRef() == source && (*i)->GetName() == name)
		{
			count ++;
			if(limit && count >= limit) return count;
		}
	}

	return count;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		イベントをキャンセルする
//! @param		キャンセルしたいイベントの発生元
//---------------------------------------------------------------------------
void tRisaEventSystem::CancelEvents(iRisseDispatch2 * source)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	for(int n = tRisaEventBase::epMin; n <= tRisaEventBase::epMax; n++)
	{
		tQueue & queue = Queues[n];
		for(tQueue::iterator i = queue.begin(); i != queue.end(); )
		{
			if((*i) && (*i)->GetSourceNoAddRef() == source)
			{
				tRisaEventBase * ev = *i;
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

