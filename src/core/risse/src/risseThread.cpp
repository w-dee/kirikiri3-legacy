/*---------------------------------------------------------------------------*/
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief マルチスレッド関連ユーティリティ
//---------------------------------------------------------------------------
#ifdef RISSE_SUPPORT_THREADS

#include "prec.h"
#include "risseThread.h"

#include <wx/utils.h>

namespace Risse 
{
RISSE_DEFINE_SOURCE_ID(24795,6838,687,16805,21894,40786,6545,48673);

//---------------------------------------------------------------------------
//! @brief		tRisseThread の内部的な実装
//---------------------------------------------------------------------------
class tRisseThreadInternal : public wxThread
{
	tRisseThread * Owner;
public:
	tRisseThreadInternal(tRisseThread * owner);
	~tRisseThreadInternal(); // virtual

private:
	ExitCode Entry();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisseThreadInternal::tRisseThreadInternal(tRisseThread * owner) :
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
tRisseThreadInternal::~tRisseThreadInternal()
{
	// ミューテックスのロックを解除する
	Owner->Internal = NULL; // 自分自身への参照を消す
	Owner->ThreadMutex.Unlock();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スレッドのエントリポイント
//---------------------------------------------------------------------------
wxThread::ExitCode tRisseThreadInternal::Entry()
{
	// ミューテックスをロックする
	Owner->ThreadMutex.Lock();

	// スレッドが実際に開始したことを表す
	Owner->Started = true;

	// スレッドのメイン関数を実行する
	// 例外が発生するかも。
	Owner->Execute();


	return 0;
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tRisseThread::tRisseThread()
{
	Started = false;
	StartInitiated = false;
	_Terminated = false;
	Internal = new tRisseThreadInternal(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseThread::~tRisseThread()
{
	//注意	このデストラクタはメインスレッド以外から非同期に呼ばれる可能性がある
	if(Started) Wait();

	// Internal は自分で自分自身を解放するのでここでは解放しない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseThread::Run()
{
	{
		volatile tRisseCriticalSection::tLocker(&CS);
		if(StartInitiated) {  /* already running */ return; }
		StartInitiated = true;
	}
	Internal->Run();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseThread::Wait()
{
	// まだスレッドが開始していない場合は開始するまで待つ
	while(!Started) { ::wxMilliSleep(1); } // あまりよい実装とは言えないが……

	// 終了指示
	Terminate();

	// スレッドが終了するまで待つ
	wxMutexLocker lock(ThreadMutex);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
volatile bool tRisseThread::ShouldTerminate()
{
	return Get_Terminated() || Internal->TestDestroy();
}
//---------------------------------------------------------------------------


} // namespace Risse

#endif
