//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ログ管理
//---------------------------------------------------------------------------
#ifndef _LOG_H
#define _LOG_H

#include "risse/include/risse.h"
#include "base/utils/Singleton.h"
#include "base/utils/RingBuffer.h"
#include <deque>
#include <wx/datetime.h>


class tRisaLogReceiver;

//---------------------------------------------------------------------------
//! @brief		ロガークラス(シングルトン)
//---------------------------------------------------------------------------
class tRisaLogger
{
	tRisseCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

	const static size_t MaxLogItems = 4096; //!< 最大のログ行数
public:

	//! @brief ログの１アイテム(１行) を表す構造体
	struct tItem
	{
		//! @brief ログアイテムの種別
		enum tType
		{
			itDebug,	//!< デバッグに関する物
			itInfo,		//!< 情報通知
			itNotice,	//!< 通常状態だが大事な情報通知
			itWarning,	//!< 警告
			itError,	//!< 通常のエラー
			itCritical	//!< 致命的なエラー
		};

		wxDateTime Timestamp; //!< ログが行われた日付時刻
		ttstr Content; //!< ログの内容
		ttstr Link; //!< リンク情報
		tType Type; //!< 種別
	};

	tRisaRingBuffer<tItem> Buffer; //!< ログを格納するためのリングバッファ

	std::vector<tRisaLogReceiver*> Receivers; //!< ログを受信するオブジェクト(レシーバ)の配列

	bool LogSending; //!< レシーバにログを送っている最中に真

public:
	tRisaLogger();
	~tRisaLogger();

private:
	tRisaSingletonObjectLifeTracer<tRisaLogger> singleton_object_life_tracer;

public:
	static boost::shared_ptr<tRisaLogger> & instance() { return
		tRisaSingleton<tRisaLogger>::instance();
			} //!< このシングルトンのインスタンスを返す

public:
	const tRisaRingBuffer<tItem> & GetBuffer() const 
		{ return Buffer; } //!< Buffer への参照を得る

	void SendLogs(tRisaLogReceiver *target, size_t maxitems = static_cast<size_t>(-1L));

	void RegisterReceiver(tRisaLogReceiver * receiver);
	void UnregisterReceiver(tRisaLogReceiver * receiver);

	void Log(const ttstr & content, tItem::tType type = tItem::itInfo,
		const ttstr & linkinfo = RisseEmptyString);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ログを受け取るためのインターフェース
//---------------------------------------------------------------------------
class tRisaLogger;
class tRisaLogReceiver
{
public:
	//! @brief ログが追加されるとき
	//! @param	item  ログアイテム
	//! @note  このメソッドは複数のスレッドから呼ばれることがある。
	//!			ただし、複数のスレッドから「同時には呼ばれない」ことは
	//!			tRisaLogger が保証する。
	virtual void OnLog(const tRisaLogger::tItem & item) = 0;
};
//---------------------------------------------------------------------------





#endif
//---------------------------------------------------------------------------
