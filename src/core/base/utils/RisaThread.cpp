//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief マルチスレッド関連ユーティリティ
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/utils/RisaThread.h"
#include "base/log/Log.h"
#include <exception>

RISSE_DEFINE_SOURCE_ID(24795,6838,687,16805,21894,40786,6545,48673);


//---------------------------------------------------------------------------
//! @brief		tRisaThread の内部的な実装
//---------------------------------------------------------------------------
class tRisaThreadInternal : public wxThread
{
	tRisaThread * Owner;
public:
	tRisaThreadInternal(tRisaThread * owner);
	~tRisaThreadInternal(); // virtual

private:
	ExitCode Entry();

private:
	void RecordUnhandledException(const risse_char *message_risse, const char * message_char);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaThreadInternal::tRisaThreadInternal(tRisaThread * owner) :
	wxThread(wxTHREAD_DETACHED)
{
	// フィールドの初期化
	Owner = owner;

	// スレッドを作成する
	Create();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaThreadInternal::~tRisaThreadInternal()
{
	// ミューテックスのロックを解除する
	Owner->Internal = NULL; // 自分自身への参照を消す
	Owner->ThreadMutex.Unlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スレッドのエントリポイント
//---------------------------------------------------------------------------
wxThread::ExitCode tRisaThreadInternal::Entry()
{
	// ミューテックスをロックする
	Owner->ThreadMutex.Lock();

	// スレッドのメイン関数を実行する
	try
	{
		Owner->Execute();
	}
/*
TODO: handle exceptions
	catch(const eRisse &e)
	{
		// !!!!!!! 例外が投げられた
		RecordUnhandledException(e.GetMessageString().c_str(), NULL);
	}
	catch(const std::exception &e)
	{
		// !!!!!!! 例外が投げられた
		RecordUnhandledException(NULL, e.what());
	}
*/
	catch(...)
	{
		throw;
	}
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		捕捉されない例外の表示・ログ
//! @param		message_risse		risse_char * 形式のメッセージ
//! @param		message_char		char * 形式のメッセージ
//---------------------------------------------------------------------------
void tRisaThreadInternal::RecordUnhandledException(
				const risse_char *message_risse, const char * message_char)
{
	tRisaLogger::Log(RISSE_WS("Unhandled exception caught in sub-thread."), tRisaLogger::llCritical);
	tRisaLogger::Log(RISSE_WS("This should be considered as a fatal error!"), tRisaLogger::llCritical);
	tRisaLogger::Log(RISSE_WS("Thread: ") + tRisseString(Owner->GetName()), tRisaLogger::llCritical);
	if(message_risse)
		tRisaLogger::Log(RISSE_WS("Message: ") + tRisseString(message_risse), tRisaLogger::llCritical);
	if(message_char)
		tRisaLogger::Log(RISSE_WS("Message: ") + tRisseString(message_char), tRisaLogger::llCritical);
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tRisaThread::tRisaThread(wxString name) : Name(name)
{
	_Terminated = false;
	Internal = new tRisaThreadInternal(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaThread::~tRisaThread()
{
	Wait();

	// Internal は自分で自分自身を解放するのでここでは解放しない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaThread::Run()
{
	Internal->Run();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaThread::Wait()
{
	Terminate();
	wxMutexLocker lock(ThreadMutex); // スレッドが終了するまで待つ
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
volatile bool tRisaThread::ShouldTerminate()
{
	return Get_Terminated() || Internal->TestDestroy();
}
//---------------------------------------------------------------------------


