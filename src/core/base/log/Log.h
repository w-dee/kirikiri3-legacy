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

#include "risse.h"
#include "Singleton.h"
#include "RingBuffer.h"
#include <deque>
#include <wx/datetime.h>

//---------------------------------------------------------------------------
//! @brief		ロガークラス(シングルトン)
//---------------------------------------------------------------------------
class tRisaLogger
{
	const static size_t MaxLogItems = 4096; //!< 最大のログ行数
public:

	//! @brief ログの１アイテム(１行) を表す構造体
	struct tItem
	{
		//! @brief ログアイテムのエラーの種別
		enum tType
		{
			itDebug,	//!< デバッグに関する物
			itInfo,		//!< 通知
			itNotice,	//!< 通常状態だが大事な情報
			itWarning,	//!< 警告状態
			itError,	//!< 通常のエラー
			itCritical	//!< 致命的なエラー
		};

		wxDateTime Timestamp; //!< ログが行われた日付時刻
		ttstr Content; //!< ログの内容
		ttstr Link; //!< リンク情報
		tType Type; //!< エラーの種別
	};

	tRisaRingBuffer<tItem> Buffer; //!< ログを格納するためのリングバッファ

public:
	tRisaLogger();
	~tRisaLogger();

private:
	tRisaSingletonObjectLifeTracer<tRisaLogger> singleton_object_life_tracer;
public:
	static boost::shared_ptr<tRisaLogger> & instance() { return
		tRisaSingleton<tRisaLogger>::instance();
			} //!< このシングルトンのインスタンスを返す

	void Log(const ttstr & content, tItem::tType type = tItem::itInfo,
		const ttstr & linkinfo = RisseEmptyString);
};
//---------------------------------------------------------------------------




#endif
//---------------------------------------------------------------------------
