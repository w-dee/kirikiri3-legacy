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
#include "prec.h"
#include "base/log/Log.h"
#include "risse/include/risseError.h"

RISSE_DEFINE_SOURCE_ID(53503,8125,25269,17586,20367,40881,26023,16793);


//---------------------------------------------------------------------------
tRisaLogger::tRisaLogger() : Buffer(MaxLogItems)
{
	LogSending = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaLogger::~tRisaLogger()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaLogger::SendPreservedLogs(tRisaLogReceiver *target)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	// ログを送信中にログの記録は行わない
	// 捨てない方がいいのかもしれないが...
	if(LogSending) return;

	// target に送る
	LogSending = true;
	try
	{
		for(std::vector<tItem>::iterator i = PreserveBuffer.begin();
			i != PreserveBuffer.end(); i++)
		{
			target->OnLog(*i);
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
void tRisaLogger::SendLogs(tRisaLogReceiver *target, size_t maxitems)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

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
void tRisaLogger::RegisterReceiver(tRisaLogReceiver * receiver)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	if(std::find(Receivers.begin(), Receivers.end(), receiver) == Receivers.end())
		Receivers.push_back(receiver);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaLogger::UnregisterReceiver(tRisaLogReceiver * receiver)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	std::vector<tRisaLogReceiver*>::iterator i;
	i = std::find(Receivers.begin(), Receivers.end(), receiver);
	if(i != Receivers.end())
		Receivers.erase(i);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaLogger::InternalLog(const ttstr & content,
	tRisaLogger::tLevel level,
	const ttstr & linkinfo)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	// ログを送信中にログの記録は行わない
	// 捨てない方がいいのかもしれないが...
	if(LogSending) return;

	// レベルが llError 以上の場合は stderr にも出力する
	if(level >= llError)
	{
		wxFprintf(stderr, wxT("%s "), wxDateTime::Now().Format().c_str());
		switch(level)
		{
		case llError   : wxFprintf(stderr, wxT("(Error) ")); break;
		case llRecord  : wxFprintf(stderr, wxT("(Record) ")); break;
		case llCritical: wxFprintf(stderr, wxT("(Critical) ")); break;
		default: break;
		}
		wxFprintf(stderr, wxT("%s\n"), content.AsWxString().c_str());
	}

	// バッファがあふれそうな場合は古いログを捨てる
	if(Buffer.GetDataSize() == Buffer.GetSize())
	{
		tItem & last = Buffer.GetLast();
		if(last.Level >= LogPreserveMinLevel)
		{
			// レベルが LogPreserveMinLevel なので、捨てないで PreserveBuffer
			// に入れる
			PreserveBuffer.push_back(last);
		}
	}

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


































//---------------------------------------------------------------------------
tRisaWxLogProxy::tRisaWxLogProxy()
{
	OldLog = wxLog::SetActiveTarget(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaWxLogProxy::~tRisaWxLogProxy()
{
	wxLog::SetActiveTarget(OldLog);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaWxLogProxy::DoLog(wxLogLevel level, const wxChar *szString, time_t t)
{
	// wxWidgets のログレベルを Risa のログレベルにマッピングする
	tRisaLogger::tLevel risa_level = tRisaLogger::llDebug; // default notice ??
	switch(level)
	{
	case wxLOG_FatalError: // program can't continue, abort immediately
		risa_level = tRisaLogger::llCritical;
		break;
	case wxLOG_Error:      // a serious error, user must be informed about it
		risa_level = tRisaLogger::llError;
		break;
	case wxLOG_Warning:    // user is normally informed about it but may be ignored
		risa_level = tRisaLogger::llWarning;
		break;
	case wxLOG_Message:    // normal message (i.e. normal output of a non GUI app)
		risa_level = tRisaLogger::llNotice;
		break;
	case wxLOG_Status:     // informational: might go to the status line of GUI app
	case wxLOG_Info:       // informational message (a.k.a. 'Verbose')
		risa_level = tRisaLogger::llInfo;
		break;
	case wxLOG_Debug:      // never shown to the user, disabled in release mode
	case wxLOG_Trace:      // trace messages are also only enabled in debug mode
	case wxLOG_Progress:   // used for progress indicator (not yet)
		risa_level = tRisaLogger::llDebug;
		break;
	default:
		;
	}

	// Risa のログ機構に流し込む
	tRisaLogger::Log(RISSE_WS("(wx) ") + ttstr(szString), risa_level);

	// OldLog に流し込む
	if(OldLog)
	{
		// 気持ち悪いキャストだが wxLogChain の実装がこうなってるので倣う
		((tRisaWxLogProxy *)OldLog)->DoLog(level, szString, t);
	}
}
//---------------------------------------------------------------------------

