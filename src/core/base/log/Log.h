//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ログ管理
//---------------------------------------------------------------------------
#ifndef _LOG_H
#define _LOG_H

#include "base/utils/Singleton.h"
#include "base/utils/RingBuffer.h"
#include "base/utils/RisaThread.h"
#include <deque>
#include <wx/datetime.h>

namespace Risa {
//---------------------------------------------------------------------------

class tLogReceiver;
//---------------------------------------------------------------------------
//! @brief		ロガークラス(シングルトン)
//---------------------------------------------------------------------------
class tLogger : public singleton_base<tLogger>
{
public:
	//! @brief ログアイテムのレベル(ログレベル)
	enum tLevel
	{
		llDebug,	//!< デバッグに関する物
		llInfo,		//!< 情報通知
		llNotice,	//!< 通常状態だが大事な情報通知
		llWarning,	//!< 警告
		llError,	//!< 通常のエラー
		llRecord,	//!< 記録すべき重要な情報
		llCritical	//!< 致命的なエラー
	};

	//! @brief ログの１アイテム(１行) を表す構造体
	struct tItem : public tCollectee
	{
		wxDateTime Timestamp; //!< ログが行われた日付時刻
		tString Content; //!< ログの内容
		tString Link; //!< リンク情報
		tLevel Level; //!< ログレベル
	};

private:
	tCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション
	const static size_t MaxLogItems = 2048; //!< 最大のログ行数
	const static tLevel LogPreserveMinLevel = llError; //!< PreserveBuffer に入れる最小のログレベル


	tRingBuffer<tItem> Buffer; //!< ログを格納するためのリングバッファ
	gc_vector<tItem> PreserveBuffer; //!< ログからあふれたアイテムを保持し続けるバッファ
	gc_vector<tLogReceiver*> Receivers; //!< ログを受信するオブジェクト(レシーバ)の配列
	bool LogSending; //!< レシーバにログを送っている最中に真

public:
	//! @brief		コンストラクタ
	tLogger();

	//! @brief		デストラクタ
	~tLogger();

public:
	const tRingBuffer<tItem> & GetBuffer() const 
		{ return Buffer; } //!< Buffer への参照を得る

	//! @brief		PreserveBuffer の内容を指定のtLogReceiverに送る
	//! @param		target		ログの送り先となるレシーバオブジェクト
	void SendPreservedLogs(tLogReceiver *target);

	//! @brief		指定行分のログを指定のtLogReceiverに送る
	//! @param		target		ログの送り先となるレシーバオブジェクト
	//! @param		maxitems	送るログの最大行数 (これよりもtLoggerが保持している
	//!							ログのサイズが大きい場合は、最後の maxitems 個が送られる)
	void SendLogs(tLogReceiver *target, size_t maxitems = static_cast<size_t>(-1L));

	//! @brief		ログを受信するための tLogReceiver を登録する
	//! @param		receiver	レシーバオブジェクト
	void RegisterReceiver(tLogReceiver * receiver);

	//! @brief		ログを受信するための tLogReceiver の登録を解除する
	//! @param		receiver	レシーバオブジェクト
	void UnregisterReceiver(tLogReceiver * receiver);

private:
	//! @brief		ログを記録する
	//! @param		content		ログの内容
	//! @param		level		ログレベル
	//! @param		linkinfo	リンク情報
	void InternalLog(const tString & content, tLevel level = llInfo,
		const tString & linkinfo = tString::GetEmptyString());

public:
	//! @brief ログを行う
	//! @note 通常のログ記録にはこちらを使うこと
	static void Log(const tString & content, tLevel level = llInfo,
		const tString & linkinfo = tString::GetEmptyString())
	{
		if(tLogger::alive())
			tLogger::instance()->InternalLog(content, level, linkinfo);
	}
};
//---------------------------------------------------------------------------


class tLogger;
//---------------------------------------------------------------------------
//! @brief		ログを受け取るためのインターフェース
//---------------------------------------------------------------------------
class tLogReceiver
{
public:
	//! @brief ログが追加されるとき
	//! @param	item  ログアイテム
	//! @note  このメソッドは複数のスレッドから呼ばれることがある。
	//!			ただし、このメソッドが複数のスレッドから
	//!			「同時には呼ばれない」ことは tLogger が保証する。
	//!			(もっとも他のメソッドは複数のスレッドから同時に呼ばれる
	//!			可能性はあるので、どっちにしろ厳重なスレッド保護を行った
	//!			ほうがよい)
	virtual void OnLog(const tLogger::tItem & item) = 0;
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		wxWidgets のログを Risa のログ機構に流し込むためのクラス
//---------------------------------------------------------------------------
class tWxLogProxy :
	public wxLog,
	public singleton_base<tWxLogProxy>,
	protected depends_on<tLogger>
{
	wxLog * OldLog; //!< このオブジェクトが作成される前に存在していたActiveなログ

public:
	//! @brief		コンストラクタ
	tWxLogProxy();

	//! @brief		デストラクタ
	~tWxLogProxy();

protected:
	//! @brief		ログを行う
	//! @note		wxWidgets の wxLog クラスの説明を参照のこと
	virtual void DoLog(wxLogLevel level, const wxChar *szString, time_t t);

	DECLARE_NO_COPY_CLASS(tWxLogProxy)
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


#endif
