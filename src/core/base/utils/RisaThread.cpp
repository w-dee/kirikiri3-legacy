//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

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

/*! @note

Risa は wxWidgets と boost という２つのライブラリのスレッド実装の恩恵を
受けられる状態にあるが、それぞれは微妙に Risa の要求からはずれている。

ここでは、スレッド関連の実装の比較を行う。

以下の「問題あり」「問題なし」は、Risa の用途で問題があるか無いかということ
であり、それぞれの実装が単純に「ダメか」「良いか」ということを言っているの
ではない。

■ スレッドの実装

	- wxThread						問題あり
	- boost::thread					問題あり

	wxThread は設計上の問題があり、Risaの用途では使うことができない
	( Wait()やDelete()でメッセージループを動かしてしまう)

	boost::thread はスレッドの優先順位を指定できない。

	ここでは、wxThread の Wait() や Delete() でメッセージループを動かして
	しまうといった問題に対し、wxThread をベースに Wait の動作を独自に実装
	することで解決を図る。


■ CriticalSection の実装

	- wxCriticalSection				問題あり
	- boost::recursive_mutex		とくに問題なし
	- tRisseCiriticalSection		問題なし

	wxCriticalSection は、再帰的な(再入可能な)クリティカルセクションを実現できる
	保証がない。boost::recursive_mutex はその名の通り再入可能な mutex だが、
	たとえば Windows の提供する CriticalSection よりは効率が悪い。
	tRisseCiriticalSection は、プラットフォームネイティブなクリティカルセクショ
	ンを利用できる場合は利用するようになっているのでもっとも効率がよい。

	ここでは、tRisseCriticalSection を typedef したものを tRisaCriticalSection
	として用いて使うこととする。
*/


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
	tRisaLogger::Log(RISSE_WS("Unhandleded exception caught in sub-thread."), tRisaLogger::llCritical);
	tRisaLogger::Log(RISSE_WS("This should be considered as a fatal error!"), tRisaLogger::llCritical);
	tRisaLogger::Log(RISSE_WS("Thread: ") + ttstr(Owner->GetName()), tRisaLogger::llCritical);
	if(message_risse)
	{
		tRisaLogger::Log(RISSE_WS("Message: ") + ttstr(message_risse), tRisaLogger::llCritical);
	}
	if(message_char)
	{
		tRisaLogger::Log(RISSE_WS("Message: ") + ttstr(message_char), tRisaLogger::llCritical);
	}
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaThread::tRisaThread(wxString name) : Name(name)
{
	_Terminated = false;
	Internal = new tRisaThreadInternal(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaThread::~tRisaThread()
{
	Wait();

	// Internal は自分で自分自身を解放するのでここでは解放しない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スレッドの実行を開始する
//---------------------------------------------------------------------------
void tRisaThread::Run()
{
	Internal->Run();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スレッドが終了するまで待つ
//---------------------------------------------------------------------------
void tRisaThread::Wait()
{
	Terminate();
	wxMutexLocker lock(ThreadMutex); // スレッドが終了するまで待つ
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スレッドが終了すべきかどうかを得る
//! @return		スレッドが終了すべきであれば真
//---------------------------------------------------------------------------
volatile bool tRisaThread::ShouldTerminate()
{
	return Get_Terminated() || Internal->TestDestroy();
}
//---------------------------------------------------------------------------

