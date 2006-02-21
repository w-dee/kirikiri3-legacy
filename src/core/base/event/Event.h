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
#include "base/utils/Singleton.h"


//---------------------------------------------------------------------------
//! @brief		イベントの基本クラス
//---------------------------------------------------------------------------
class tRisaEventBase
{
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
	ttstr Name; //!< イベント名
	iRisseDispatch2 * Source; //!< イベントの発生元 Risse オブジェクト
	iRisseDispatch2 * Destination; //!< イベントの配信先 Risse オブジェクト
	tPriority Priority; //!< イベントの優先度

public:
	//! @brief コンストラクタ
	tRisaEventBase(
		const ttstr & name,
		iRisseDispatch2 * source = NULL,
		iRisseDispatch2 * destination = NULL,
		tPriority prio = epNormal) :
			Name(name),
			Source(source),
			Destination(Destination),
			Priority(prio)
	{
		if(Source) Source->AddRef();
		if(Destination) Destination->AddRef();
	}

	//! @brief デストラクタ
	virtual ~tRisaEventBase()
	{
		if(Destination) Destination->Release();
		if(Source) Source->Release();
	}

	const ttstr & GetName() const { return Name; } //!< イベント名を得る
	iRisseDispatch2 * GetSourceNoAddRef() const { return Source; } //!< イベント発生源を得る
	iRisseDispatch2 * GetDestinationNoAddRef() const { return Destination; } //!< イベント配信先を得る
	tPriority GetPriority() const { return Priority; } //!< イベントの優先順位を得る

	virtual void Deliver() = 0; //!< イベントを配信先に配信する
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
	tRisseCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

	typedef std::deque<tRisaEventBase *> tQueue; //!< キュー用コンテナの typedef
	tQueue Queues[tRisaEventBase::epMax + 1]; //!< イベント用キュー
	bool CanDeliverEvents; //!< イベントを配信可能かどうか
	bool HasPendingEvents; //!< post してから処理されていないイベントが存在する場合に真

public:
	tRisaEventSystem();
	~tRisaEventSystem();

private:
	void DiscardQueue(tQueue & queue);
	void DeliverQueue(tRisaEventBase::tPriority prio);
	void DeliverEvents();

public:
	bool ProcessEvents();
	void PostEvent(tRisaEventBase * event, tEventType type = etDefault);
	size_t CountEventsInQueue(const ttstr & name,
		iRisseDispatch2 * source, tRisaEventBase::tPriority prio, size_t limit = 1);
	void CancelEvents(iRisseDispatch2 * source);
	bool GetCanDeliverEvents() const { return CanDeliverEvents; } //!< イベントを配信可能かどうかを返す
};
//---------------------------------------------------------------------------


#endif
