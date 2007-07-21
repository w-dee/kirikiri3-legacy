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
#ifndef _EVENT_H
#define _EVENT_H

#include <deque>
#include <boost/array.hpp>
#include "base/utils/RisaThread.h"
#include "base/utils/Singleton.h"
#include "base/script/RisseEngine.h"
#include "base/exception/RisaException.h"


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
//! @brief		イベントの発生先インターフェース
//---------------------------------------------------------------------------
class tEventDestination
{
public:
	virtual void OnEvent(tEventInfo * info) = 0; //!< イベントが配信されるとき
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		イベント情報クラス
//---------------------------------------------------------------------------
class tEventInfo : public tCollectee
{
	friend class tEventQueueInstance;
public:
	//! @brief  イベントの優先度
	enum tPriority
	{
		epExclusive, //!< 排他的イベント
		epNormal, //!< 通常イベント
		epPaint, //!< ウィンドウペイントオブジェクト
		epLow, //!< 低優先度イベント
		epMin = epExclusive, //!< イベント優先度の最小値
		epMax = epLow //!< イベント優先度の最大値
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
	//! @brief コンストラクタ
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

	int GetId() const { return Id; } //!< IDを得る
	void * GetSource() { return Source; } //!< イベント発生源を得る
	risse_uint64 GetTick() const { return Tick; } //!< Tick を得る
	tEventDestination * GetDestination() { return Destination; } //!< イベント配信先を得る
	tPriority GetPriority() const { return Priority; } //!< イベントの優先順位を得る

};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		イベントシステム
//! @note		イベントシステム共通の設定を保持したり、共通の設定を行ったりする
//---------------------------------------------------------------------------
class tEventSystem : public singleton_base<tEventSystem>
{
	bool CanDeliverEvents; //!< イベントを配信可能かどうか

public:
	//! @brief		コンストラクタ
	tEventSystem() { CanDeliverEvents = true; }

	bool GetCanDeliverEvents() const { return CanDeliverEvents; } //!< イベントを配信可能かどうかを返す
	void SetCanDeliverEvents(bool b) { CanDeliverEvents = b; } //!< イベントを配信可能かどうかを設定する
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		"EventQueue" クラスのインスタンス用 C++クラス
//---------------------------------------------------------------------------
class tEventQueueInstance : public tObjectBase, depends_on<tEventSystem>
{
public:
	//! @brief イベントのタイプ
	enum tEventType
	{
		etDefault = 0, //!< デフォルトのイベントタイプ
		etDiscardable = 1, //!< 破棄可能イベント
		etSingle = 2 //!< シングルイベント
	};

private:
	typedef gc_deque<tEventInfo *> tQueue; //!< キュー用コンテナの typedef
	tQueue Queues[tEventInfo::epMax + 1]; //!< イベント用キュー
	bool HasPendingEvents; //!< post してから処理されていないイベントが存在する場合に真
	bool QuitEventFound; //!< Quit イベントが見つかった場合に真
	bool IsMainThreadQueue; //!< メインスレッド用のキューかどうか
	tThreadEvent * EventNotifier; //!< イベントが到着したことを知らせる ThreadEvent (メインスレッド用でないキュー用)

public:
	//! @brief		コンストラクタ
	tEventQueueInstance();

	//! @brief		デストラクタ(呼ばれることはない)
	virtual ~tEventQueueInstance() {;}

private:
	//! @brief		指定された優先度のキューの中のイベントを配信する
	//! @param		prio		優先度
	//! @note		prio!=tEventInfo::epExclusiveの場合、epExclusiveのイベントが
	//!				ポストされたり、CanDeliverEvents が偽になった場合は即座に戻る。
	void DeliverQueue(tEventInfo::tPriority prio, risse_uint64 mastertick);

	//! @brief		すべてのイベントを配信する
	//! @param		mastertick	マスタ・ティックカウント
	void DeliverEvents(risse_uint64 mastertick);

	//! @brief		指定キュー中のイベントをすべて破棄する
	//! @param		queue		キュー
	void DiscardQueue(tQueue & queue);

	//! @brief		メインスレッド用のキューかどうか
	//! @param		b	メインスレッド用のキューかどうか
	//! @note		メインのイベントキューにのみ用いる。通常はこのメソッドを利用しないこと。
	void SetIsMainThreadQueue(bool b) { IsMainThreadQueue = b; }

	friend class tMainEventQueue;
public:

	//! @brief		イベントの配信処理を行う
	//! @param		mastertick	マスタ・ティックカウント
	//! @return		まだ処理すべきイベントがあるかどうか
	//! @note		wxApp::ProcessIdle から呼ばれる
	bool ProcessEvents(risse_uint64 mastertick);

	//! @brief		イベントをポストする
	//! @param		event		イベント
	//! @param		type		イベントタイプ
	void PostEvent(tEventInfo * event, tEventType type = etDefault);


	//! @brief		指定されたイベントがすでにキューに入っている数を数える
	//! @param		id			イベントID
	//! @param		source		イベント発生元
	//! @param		prio		優先度
	//! @param		limit		数え上げる最大値(0=全部数える)
	//! @return		name と source と prio が一致するイベントがすでにキューにあるかどうか
	size_t CountEventsInQueue(int id,
		void * source, tEventInfo::tPriority prio, size_t limit = 1);

	//! @brief		イベントをキャンセルする
	//! @param		source		キャンセルしたいイベントの発生元
	void CancelEvents(void * source);

	//! @brief		イベントループに入る
	//! @note		メインのイベントキューの場合は例外が発生する。
	void Loop();

	//! @brief		イベントループから抜ける
	//! @note		イベントループから抜けるための特殊なイベントが自分自身に追加される。
	//!				通常、それ以前にポストされたすべてのイベントを処理し終わってから
	//!				イベントループから抜ける。それ以降にポストされたイベントは
	//!				無視される。メインのイベントキューに対して呼ばれた場合は例外が発生する。
	//!				このメソッドを実行したからと言ってすぐにイベントループから抜ける
	//!				保証はない。
	void QuitLoop();

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);
	void loop() { Loop(); }
	void quit() { QuitLoop(); }
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		"EventQueue" クラス
//---------------------------------------------------------------------------
class tEventQueueClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tEventQueueClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public:
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		メインスレッドの(デフォルトの)イベントキューを作成するシングルトンクラス
//---------------------------------------------------------------------------
class tMainEventQueue : public singleton_base<tMainEventQueue>,
	depends_on<tRisseScriptEngine>,
	depends_on<tRisseClassRegisterer<tEventQueueClass> >
{
	tVariant EventQueue; //!< Risseインスタンス

public:
	//! @brief		コンストラクタ
	tMainEventQueue();

	//! @brief		メインのイベントキューを取得する
	tVariant & GetEventQueue() { return EventQueue; }

	//! @brief		メインのイベントキューインスタンスを得る
	tEventQueueInstance * GetEventQueueInstance() const
	{
		return (tEventQueueInstance*)EventQueue.GetObjectInterface();
	}
};
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief		イベントの発生となるRisseインスタンスの共通クラス
//---------------------------------------------------------------------------
class tEventSourceInstance : public tObjectBase
{
	tVariant DestEventQueue; //!< 配信先のイベントキュー
public:
	tEventSourceInstance(); // コンストラクタ
	virtual ~tEventSourceInstance() {;} // デストラクタ (おそらく呼ばれない)

	//! @brief		配送先のイベントキューを得る
	//! @return		配送先のイベントキュー
	tVariant & GetDestEventQueue() { return DestEventQueue; }

	//! @brief		配送先のイベントキューを設定する
	//! @param		queue		配送先のイベントキュー
	void SetDestEventQueue(const tVariant & queue);

	//! @brief		配送先のイベントキューインスタンスを得る
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
//! @brief		"EventSource" クラス
//---------------------------------------------------------------------------
class tEventSourceClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tEventSourceClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public:
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
} // namespace Risa


#endif
