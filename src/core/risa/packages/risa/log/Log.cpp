//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ログ管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "risa/packages/risa/log/Log.h"
#include "base/script/RisseEngine.h"

namespace Risa {
RISSE_DEFINE_SOURCE_ID(53503,8125,25269,17586,20367,40881,26023,16793);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tLogger::tLogger() : Buffer(MaxLogItems)
{
	LogSending = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tLogger::~tLogger()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tLogger::SendPreservedLogs(tLogReceiver *target)
{
	volatile tCriticalSection::tLocker holder(CS);

	// ログを送信中にログの記録は行わない
	// 捨てない方がいいのかもしれないが...
	if(LogSending) return;

	// target に送る
	LogSending = true;
	try
	{
		for(gc_vector<tItem>::iterator i = PreserveBuffer.begin();
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
void tLogger::SendLogs(tLogReceiver *target, size_t maxitems)
{
	volatile tCriticalSection::tLocker holder(CS);

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
void tLogger::RegisterReceiver(tLogReceiver * receiver)
{
	volatile tCriticalSection::tLocker holder(CS);

	if(std::find(Receivers.begin(), Receivers.end(), receiver) == Receivers.end())
		Receivers.push_back(receiver);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tLogger::UnregisterReceiver(tLogReceiver * receiver)
{
	volatile tCriticalSection::tLocker holder(CS);

	gc_vector<tLogReceiver*>::iterator i;
	i = std::find(Receivers.begin(), Receivers.end(), receiver);
	if(i != Receivers.end())
		Receivers.erase(i);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tLogger::InternalLog(const tString & content,
	tLogger::tLevel level,
	const tString & linkinfo)
{
	volatile tCriticalSection::tLocker holder(CS);

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
		for(gc_vector<tLogReceiver*>::iterator i = Receivers.begin();
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
tWxLogProxy::tWxLogProxy()
{
	OldLog = wxLog::SetActiveTarget(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tWxLogProxy::~tWxLogProxy()
{
	wxLog::SetActiveTarget(OldLog);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWxLogProxy::DoLog(wxLogLevel level, const wxChar *szString, time_t t)
{
	// wxWidgets のログレベルを Risa のログレベルにマッピングする
	tLogger::tLevel risa_level = tLogger::llDebug; // default notice ??
	switch(level)
	{
	case wxLOG_FatalError: // program can't continue, abort immediately
		risa_level = tLogger::llCritical;
		break;
	case wxLOG_Error:      // a serious error, user must be informed about it
		risa_level = tLogger::llError;
		break;
	case wxLOG_Warning:    // user is normally informed about it but may be ignored
		risa_level = tLogger::llWarning;
		break;
	case wxLOG_Message:    // normal message (i.e. normal output of a non GUI app)
		risa_level = tLogger::llNotice;
		break;
	case wxLOG_Status:     // informational: might go to the status line of GUI app
	case wxLOG_Info:       // informational message (a.k.a. 'Verbose')
		risa_level = tLogger::llInfo;
		break;
	case wxLOG_Debug:      // never shown to the user, disabled in release mode
	case wxLOG_Trace:      // trace messages are also only enabled in debug mode
	case wxLOG_Progress:   // used for progress indicator (not yet)
		risa_level = tLogger::llDebug;
		break;
	default:
		;
	}

	// Risa のログ機構に流し込む
	tLogger::Log(RISSE_WS("(wx) ") + tString(szString), risa_level);

/*
	TOOD: ここうまくうごいてないので直すこと (いまのままだと終了時にSEGVする)
	// OldLog に流し込む
	if(OldLog)
	{
		// 気持ち悪いキャストだが wxLogChain の実装がこうなってるので倣う
		((tWxLogProxy *)OldLog)->DoLog(level, szString, t);
	}
*/
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
//! @brief		risa.log パッケージイニシャライザ
//---------------------------------------------------------------------------
class tRisaLogPackageInitializer : public tBuiltinPackageInitializer
{
public:
	static void debug(const tString & content) //!< debug メッセージ出力
	{
		tLogger::instance()->Log(content, tLogger::llDebug);
	}

	static void info(const tString & content) //!< info メッセージ出力
	{
		tLogger::instance()->Log(content, tLogger::llInfo);
	}

	static void notice(const tString & content) //!< notice メッセージ出力
	{
		tLogger::instance()->Log(content, tLogger::llNotice);
	}

	static void warning(const tString & content) //!< warning メッセージ出力
	{
		tLogger::instance()->Log(content, tLogger::llWarning);
	}

	static void error(const tString & content) //!< error メッセージ出力
	{
		tLogger::instance()->Log(content, tLogger::llError);
	}

	static void record(const tString & content) //!< record メッセージ出力
	{
		tLogger::instance()->Log(content, tLogger::llRecord);
	}

	static void critical(const tString & content) //!< critical メッセージ出力
	{
		tLogger::instance()->Log(content, tLogger::llCritical);
	}

	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisaLogPackageInitializer(tScriptEngine * engine) :
		tBuiltinPackageInitializer(
			tSS<'r','i','s','a','.','l','o','g'>())
	{
	}

	//! @brief		パッケージを初期化する
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		パッケージ名
	//! @param		global		パッケージグローバル
	void Initialize(tScriptEngine * engine, const tString & name,
		const tVariant & global)
	{
		tLogger::ensure();

		tObjectBase * g = static_cast<tObjectBase *>(global.GetObjectInterface());

		BindFunction(g, tSS<'d','e','b','u','g'>(), &tRisaLogPackageInitializer::debug);
		BindFunction(g, tSS<'i','n','f','o'>(), &tRisaLogPackageInitializer::info);
		BindFunction(g, tSS<'n','o','t','i','c','e'>(), &tRisaLogPackageInitializer::notice);
		BindFunction(g, tSS<'w','a','r','n','i','n','g'>(), &tRisaLogPackageInitializer::warning);
		BindFunction(g, tSS<'e','r','r','o','r'>(), &tRisaLogPackageInitializer::error);
		BindFunction(g, tSS<'r','e','c','o','r','d'>(), &tRisaLogPackageInitializer::record);
		BindFunction(g, tSS<'c','r','i','t','i','c','a','l'>(), &tRisaLogPackageInitializer::critical);
	}
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief		risa.log パッケージイニシャライザレジストラ
template class tPackageInitializerRegisterer<tRisaLogPackageInitializer>;
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
} // namespace Risa


