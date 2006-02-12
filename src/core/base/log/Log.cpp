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
#include "prec.h"
#include "base/log/Log.h"
#include "risse/include/risseError.h"

RISSE_DEFINE_SOURCE_ID(53503,8125,25269,17586,20367,40881,26023,16793);


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaLogger::tRisaLogger() : Buffer(MaxLogItems)
{
	LogSending = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaLogger::~tRisaLogger()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定行分のログを指定のtRisaLogReceiverに送る
//! @param		target		ログの送り先となるレシーバオブジェクト
//! @param		maxitems	送るログの最大行数 (これよりもtRisaLoggerが保持している
//!							ログのサイズが大きい場合は、最後の maxitems 個が送られる)
//---------------------------------------------------------------------------
void tRisaLogger::SendLogs(tRisaLogReceiver *target, size_t maxitems)
{
	volatile tRisseCriticalSection::tLocker holder(CS);

	// ログを送信中にログの記録は行わない
	// 捨てない方がいいのかもしれないが...
	if(LogSending) return;

	// maxitems の調整
	size_t buffer_datasize = Buffer.GetDataSize();
	if(maxitems == static_cast<size_t>(-1L))
		maxitems = buffer_datasize;
	else if(maxitems > buffer_datasize)
		maxitems = buffer_datasize;

	// maxitems 個を target に送る
	LogSending = true;
	try
	{
		for(size_t n = buffer_datasize - maxitems; n < buffer_datasize; n++)
		{
			target->OnLog(Buffer.GetAt(n));
		}
	}
	catch(...)
	{
		LogSending = false;
		throw;
	}
	LogSending = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ログを受信するための tRisaLogReceiver を登録する
//! @param		receiver	レシーバオブジェクト
//---------------------------------------------------------------------------
void tRisaLogger::RegisterReceiver(tRisaLogReceiver * receiver)
{
	volatile tRisseCriticalSection::tLocker holder(CS);

	if(std::find(Receivers.begin(), Receivers.end(), receiver) == Receivers.end())
		Receivers.push_back(receiver);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ログを受信するための tRisaLogReceiver の登録を解除する
//! @param		receiver	レシーバオブジェクト
//---------------------------------------------------------------------------
void tRisaLogger::UnregisterReceiver(tRisaLogReceiver * receiver)
{
	volatile tRisseCriticalSection::tLocker holder(CS);

	std::vector<tRisaLogReceiver*>::iterator i;
	i = std::find(Receivers.begin(), Receivers.end(), receiver);
	if(i != Receivers.end())
		Receivers.erase(i);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ログを記録する
//! @param		content		ログの内容
//! @param		level		ログレベル
//! @param		linkinfo	リンク情報
//---------------------------------------------------------------------------
void tRisaLogger::Log(const ttstr & content,
	tRisaLogger::tLevel level,
	const ttstr & linkinfo)
{
	volatile tRisseCriticalSection::tLocker holder(CS);

	// ログを送信中にログの記録は行わない
	// 捨てない方がいいのかもしれないが...
	if(LogSending) return;

	// Buffer の書き込み位置に記録
	tItem & item = Buffer.GetLast();
	item.Timestamp = wxDateTime::Now();
	item.Content = content;
	item.Link = linkinfo;
	item.Level = level;

	// Buffer の書き込みポインタを進める
	// (バッファがあふれた場合は古いログを捨てる)
	Buffer.AdvanceWritePosWithDiscard();

	// 全てのレシーバにログの情報を伝える
	LogSending = true;
	try
	{
		for(std::vector<tRisaLogReceiver*>::iterator i = Receivers.begin();
			i != Receivers.end(); i++)
		{
			(*i)->OnLog(item);
		}
	}
	catch(...)
	{
		LogSending = false;
		throw;
	}
	LogSending = false;
}
//---------------------------------------------------------------------------


