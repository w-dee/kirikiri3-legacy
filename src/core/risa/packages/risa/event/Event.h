//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief イベントシステム
//---------------------------------------------------------------------------
#ifndef _EVENT_H
#define _EVENT_H

#include <deque>
#include <boost/array.hpp>
#include "risa/common/RisaThread.h"
#include "risa/common/Singleton.h"
#include "risa/common/PointerList.h"
#include "risa/common/RisseEngine.h"
#include "risa/common/RisaException.h"


/* @note
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
namespace Risa {
//---------------------------------------------------------------------------






class tEventInfo;
//---------------------------------------------------------------------------
/**
 * イベントの発生先インターフェース
 */
class tEventDestination : public tCollectee
{
public:
	virtual ~tEventDestination() {} //!< デストラクタ(おそらく呼ばれない)

	virtual void OnEvent(tEventInfo * info) = 0; //!< イベントが配信されるとき
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
/**
 * イベント情報クラス
 */
class tEventInfo : public tCollectee
{
	friend class tEventQueueInstance;
public:
	/**
	 * イベントの優先度
	 */
	enum tPriority
	{
		epExclusive /*!< 排他的イベント */,
		epNormal /*!< 通常イベント */,
		epPaint /*!< ウィンドウペイントオブジェクト */,
		epLow /*!< 低優先度イベント */,
		epMin = epExclusive /*!< イベント優先度の最小値 */,
		epMax = epLow /*!< イベント優先度の最大値 */
	};
	static bool IsPriorityValid(tPriority prio) { return prio >= epMin && prio <= epMax; }
		//!< 優先度が有効な値の範囲であるかどうか

private:
	int Id; //!< イベントID (Source固有)
	void * Source; //!< イベントの発生元
	tEventDestination * Destination; //!< イベントの配信先 Risse オブジェクト
	tPriority Priority; //!< イベントの優先度
	risse_uint64 Tick; //!< イベントが配信される tick

	void SetTick(risse_uint64 tick)  { Tick = tick; } // Tick を設定する

protected:
	virtual void Deliver() { if(Destination) Destination->OnEvent(this); } //!< イベントを配信先に配信する

public:
	/**
	 * コンストラクタ
	 */
	tEventInfo(
		int id,
		void * source = NULL,
		tEventDestination * destination = NULL,
		tPriority prio = epNormal) :
			Id(id),
			Source(source),
			Destination(destination),
			Priority(prio)
	{
	}

	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	virtual ~tEventInfo() {}

	int GetId() const { return Id; } //!< IDを得る
	void * GetSource() { return Source; } //!< イベント発生源を得る
	risse_uint64 GetTick() const { return Tick; } //!< Tick を得る
	tEventDestination * GetDestination() { return Destination; } //!< イベント配信先を得る
	tPriority GetPriority() const { return Priority; } //!< イベントの優先順位を得る

};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
/**
 * イベントシステム
 * @note	イベントシステム共通の設定を保持したり、共通の設定を行ったりする
 */
class tEventSystem : public singleton_base<tEventSystem>
{
	bool CanDeliverEvents; //!< イベントを配信可能かどうか
	mutable tCriticalSection CS; //!< このインスタンスを保護するためのCS

public:
	/**
	 * イベントシステムの状態が変わった場合に呼び出されるコールバック
	 */
	class tStateListener
	{
	public:
		/**
		 * デストラクタ(おそらく呼ばれない)
		 */
		virtual ~tStateListener() {}

		/**
		 * イベントを配信可能かどうかのステータスが変わった際に
		 * 呼ばれる
		 * @param b	イベントを配信可能かどうか
		 * @note	様々なスレッドから呼ばれる可能性があるので注意すること
		 */
		virtual void OnCanDeliverEventsChanged(bool b) {;}
	};

private:
	pointer_list<tStateListener> StateListeners; //!< イベント状態リスナ

public:
	/**
	 * コンストラクタ
	 */
	tEventSystem() { CanDeliverEvents = true; }

	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	virtual ~tEventSystem() {}

	bool GetCanDeliverEvents() const
	{ volatile tCriticalSection::tLocker cs_holder(CS);
	  return CanDeliverEvents; } //!< イベントを配信可能かどうかを返す
	void SetCanDeliverEvents(bool b); //!< イベントを配信可能かどうかを設定する

	pointer_list<tStateListener> & GetStateListeners() { return StateListeners; } //!< イベント状態リスナを得る
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
/**
 * "EventQueue" クラスのインスタンス用 C++クラス
 */
class tEventQueueInstance : public tObjectBase, depends_on<tEventSystem>
{
public:
	/**
	 * イベントのタイプ
	 */
	enum tEventType
	{
		etDefault = 0 /*!< デフォルトのイベントタイプ */,
		etDiscardable = 1 /*!< 破棄可能イベント */,
		etSingle = 2 /*!< シングルイベント */
	};

	typedef gc_deque<tEventInfo *> tQueue; //!< キュー用コンテナの typedef

private:
	tQueue Queues[tEventInfo::epMax + 1]; //!< イベント用キュー
	bool HasPendingEvents; //!< post してから処理されていないイベントが存在する場合に真
	bool QuitEventFound; //!< Quit イベントが見つかった場合に真
	bool IsMainThreadQueue; //!< メインスレッド用のキューかどうか
	tThreadEvent * EventNotifier; //!< イベントが到着したことを知らせる ThreadEvent (メインスレッド用でないキュー用)

public:
	/**
	 * コンストラクタ
	 */
	tEventQueueInstance();

	/**
	 * デストラクタ(呼ばれることはない)
	 */
	virtual ~tEventQueueInstance() {;}

private:
	/**
	 * 指定された優先度のキューの中のイベントを配信する
	 * @param prio	優先度
	 * @note	prio!=tEventInfo::epExclusiveの場合、epExclusiveのイベントが
	 *			ポストされたり、CanDeliverEvents が偽になった場合は即座に戻る。
	 */
	void DeliverQueue(tEventInfo::tPriority prio, risse_uint64 mastertick);

	/**
	 * すべてのイベントを配信する
	 * @param mastertick	マスタ・ティックカウント
	 */
	void DeliverEvents(risse_uint64 mastertick);

	/**
	 * 指定キュー中のイベントをすべて破棄する
	 * @param queue	キュー
	 */
	void DiscardQueue(tQueue & queue);

	/**
	 * メインスレッド用のキューかどうかを設定する
	 * @param b	メインスレッド用のキューかどうか
	 * @note	メインのイベントキューにのみ用いる。通常はこのメソッドを利用しないこと。
	 */
	void SetIsMainThreadQueue(bool b) { IsMainThreadQueue = b; }

	friend class tMainEventQueue;
public:

	/**
	 * イベントの配信処理を行う
	 * @param mastertick	マスタ・ティックカウント
	 * @return	まだ処理すべきイベントがあるかどうか
	 * @note	wxApp::ProcessIdle から呼ばれる
	 */
	bool ProcessEvents(risse_uint64 mastertick);

	/**
	 * イベントをポストする
	 * @param event	イベント
	 * @param type	イベントタイプ
	 */
	void PostEvent(tEventInfo * event, tEventType type = etDefault);


	/**
	 * 指定されたイベントがすでにキューに入っている数を数える
	 * @param id		イベントID
	 * @param source	イベント発生元
	 * @param prio		優先度
	 * @param limit		数え上げる最大値(0=全部数える)
	 * @return	name と source と prio が一致するイベントがすでにキューにあるかどうか
	 */
	size_t CountEventsInQueue(int id,
		void * source, tEventInfo::tPriority prio, size_t limit = 1);

private:
	/**
	 * イベントをキャンセルし、かつキャンセルしたイベントを得る(内部関数)
	 * @param source	キャンセルしたいイベントの発生元
	 * @param dest		キャンセルしたイベントの格納先(内容はクリアされないので注意)
	 */
	void InternalCancelEvents(void * source, tQueue * dest);

public:
	/**
	 * イベントをキャンセルする
	 * @param source	キャンセルしたいイベントの発生元
	 */
	void CancelEvents(void * source) { InternalCancelEvents(source, NULL); }

	/**
	 * イベントをキャンセルし、かつキャンセルしたイベントを得る
	 * @param source	キャンセルしたいイベントの発生元
	 * @param dest		キャンセルしたイベントの格納先(内容はクリアされないので注意)
	 */
	void CancelEvents(void * source, tQueue & dest) { InternalCancelEvents(source, &dest); }

	/**
	 * イベントループに入る
	 * @note	メインのイベントキューの場合は例外が発生する。
	 */
	void Loop();

	/**
	 * イベントループから抜ける
	 * @note	イベントループから抜けるための特殊なイベントが自分自身に追加される。
	 *			通常、それ以前にポストされたすべてのイベントを処理し終わってから
	 *			イベントループから抜ける。それ以降にポストされたイベントは
	 *			無視される。メインのイベントキューに対して呼ばれた場合は例外が発生する。
	 *			このメソッドを実行したからと言ってすぐにイベントループから抜ける
	 *			保証はない。
	 */
	void QuitLoop();

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);
	void loop() { Loop(); }
	void quit() { QuitLoop(); }
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
/**
 * "EventQueue" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tEventQueueClass, tClassBase, tEventQueueInstance, itNormal)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
/**
 * メインスレッドの(デフォルトの)イベントキューを作成するシングルトンクラス
 */
class tMainEventQueue : public singleton_base<tMainEventQueue>,
	depends_on<tRisseScriptEngine>,
	manual_start<tMainEventQueue>
{
	tVariant EventQueue; //!< Risseインスタンス

public:
	/**
	 * コンストラクタ
	 */
	tMainEventQueue();

	/**
	 * メインのイベントキューを取得する
	 */
	tVariant & GetEventQueue() { return EventQueue; }

	/**
	 * メインのイベントキューインスタンスを得る
	 */
	tEventQueueInstance * GetEventQueueInstance() const
	{
		return (tEventQueueInstance*)EventQueue.GetObjectInterface();
	}
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
/**
 * イベントの発生となるRisseインスタンスの共通クラス
 */
class tEventSourceInstance : public tObjectBase
{
	tVariant DestEventQueue; //!< 配信先のイベントキュー
public:
	tEventSourceInstance(); // コンストラクタ
	virtual ~tEventSourceInstance() {;} // デストラクタ (おそらく呼ばれない)

	/**
	 * 配送先のイベントキューを得る
	 * @return	配送先のイベントキュー
	 */
	tVariant & GetDestEventQueue() { return DestEventQueue; }

	/**
	 * 配送先のイベントキューを設定する
	 * @param queue	配送先のイベントキュー
	 */
	void SetDestEventQueue(const tVariant & queue);

	/**
	 * 配送先のイベントキューインスタンスを得る
	 */
	tEventQueueInstance * GetDestEventQueueInstance() const
	{
		// DestEventQueue が tEventQueueInstance を表しているということは
		// SetDestEventQueue() 内で assert 済み
		return (tEventQueueInstance*)DestEventQueue.GetObjectInterface();
	}

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);
	tVariant & get_queue() { return GetDestEventQueue(); }
	void set_queue(const tVariant & v) { SetDestEventQueue(v); }
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
/**
 * "EventSource" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tEventSourceClass, tClassBase, tEventSourceInstance, itNoInstance)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace Risa


#endif
