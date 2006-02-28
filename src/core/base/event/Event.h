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
#ifndef _EVENT_H
#define _EVENT_H

#include <deque>
#include <boost/array.hpp>
#include "base/utils/RisaThread.h"
#include "base/utils/Singleton.h"



class tRisaEventInfo;
//---------------------------------------------------------------------------
//! @brief		イベントの発生先インターフェース
//---------------------------------------------------------------------------
class tRisaEventDestination
{
public:
	virtual void OnEvent(tRisaEventInfo * info) = 0; //!< イベントが配信されるとき
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		イベント情報クラス
//---------------------------------------------------------------------------
class tRisaEventInfo
{
	friend class tRisaEventSystem;
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
	tRisaEventDestination * Destination; //!< イベントの配信先 Risse オブジェクト
	tPriority Priority; //!< イベントの優先度

protected:
	virtual void Deliver() { if(Destination) Destination->OnEvent(this); } //!< イベントを配信先に配信する

public:
	//! @brief コンストラクタ
	tRisaEventInfo(
		int id,
		void * source = NULL,
		tRisaEventDestination * destination = NULL,
		tPriority prio = epNormal) :
			Id(id),
			Source(source),
			Destination(destination),
			Priority(prio)
	{
	}

	//! @brief デストラクタ
	virtual ~tRisaEventInfo()
	{
	}

	int GetId() const { return Id; } //!< IDを得る
	void * GetSource() { return Source; } //!< イベント発生源を得る
	tRisaEventDestination * GetDestination() { return Destination; } //!< イベント配信先を得る
	tPriority GetPriority() const { return Priority; } //!< イベントの優先順位を得る

};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		イベント管理システム
//---------------------------------------------------------------------------
class tRisaEventSystem : public singleton_base<tRisaEventSystem>
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
	tRisaCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

	typedef std::deque<tRisaEventInfo *> tQueue; //!< キュー用コンテナの typedef
	tQueue Queues[tRisaEventInfo::epMax + 1]; //!< イベント用キュー
	bool CanDeliverEvents; //!< イベントを配信可能かどうか
	bool HasPendingEvents; //!< post してから処理されていないイベントが存在する場合に真

public:
	tRisaEventSystem();
	~tRisaEventSystem();

private:
	void DiscardQueue(tQueue & queue);
	void DeliverQueue(tRisaEventInfo::tPriority prio);
	void DeliverEvents();

public:
	bool ProcessEvents();
	void PostEvent(tRisaEventInfo * event, tEventType type = etDefault);
	size_t CountEventsInQueue(int id,
		void * source, tRisaEventInfo::tPriority prio, size_t limit = 1);
	void CancelEvents(void * source);
	bool GetCanDeliverEvents() const { return CanDeliverEvents; } //!< イベントを配信可能かどうかを返す
	void SetCanDeliverEvents(bool b) { CanDeliverEvents = b; } //!< イベントを配信可能かどうかを設定する
};
//---------------------------------------------------------------------------


#endif
