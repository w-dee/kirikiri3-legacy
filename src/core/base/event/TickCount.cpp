//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief TickCount を提供するモジュール
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/event/TickCount.h"

RISSE_DEFINE_SOURCE_ID(37180,40935,47171,16902,29833,29636,3244,55820);

/*! @note
	tRisaTickCount は 64bit 整数によるミリ秒単位のティックカウントを提供する。
	wxWidgets は wxStopWatch でミリ秒単位の経過時刻情報を提供するが、
	返される値が long であり、long の範囲を超えた場合の動作の保証はいっさい
	無い。
	wxStopWatch では内部で wxGetLocalTimeMillis を 使用していて、これは
	64bit で値を返す。ただし、そもそもの用途がunix epochからの経過時間を
	ミリ秒単位で取得する、であって２点間の時間間隔を計る、ではないので必
	ずしも効率は良くない。

	Risa の用途では TickCount の取得は極めて頻繁に行われるため、なるべく
	効率のよい方法がほしい。

	かといってすべての状況に対応するのも難しいので、ここでは少々泥臭いが
	以下の方法で我慢をする。

	・Windows ファミリー

		Win32/Win64
			timeGetTime を使う (DWORDの周回の監視を行う)

		そのほか(WindowsMobileなど？)
			GetTickCount を使う (DWORDの周回の監視を行う)

		timeGettime/GetTickCount を使うときはタイマの分解能に注意する
		( TickCount.h にある tRisaWinTimerResolutionAdjuster を参照のこと )

	・それ以外

		wxGetLocalTimeMillis を使う
*/


#ifdef RISA_TICKCOUNT_NEED_WRAP_WATCH
//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		owner		このオブジェクトを所有する tRisaTickCount オブジェクト
//---------------------------------------------------------------------------
tRisaTickCount::tWatcher::tWatcher(tRisaTickCount & owner) :
	wxThread(wxTHREAD_JOINABLE),
	Semaphore(0, 1), Owner(owner)
{
	Terminated = false;
	Create();
	Run();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaTickCount::tWatcher::~tWatcher()
{
	// Terminated は Semapore.Post() よりも先に真にしておく必要がある
	// (セマフォでたたき起こされたスレッドが、Terminate されていないことを
	// 見つけるとまた眠ってしまうため)
	Terminate(); // スレッドの終了を伝える
	Semaphore.Post(); // セマフォ待ちを解除する
	Delete(); // スレッドの終了を待ち、スレッドを消す
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スレッドのエントリーポイント
//! @return		スレッドの終了コード
//---------------------------------------------------------------------------
wxThread::ExitCode tRisaTickCount::tWatcher::Entry()
{
	while(!Terminated && !TestDestroy())
	{
		// 60秒をまつか、セマフォが取得できるまで待つ。
		// 実際の所これはこれほど短い秒数を待つ必要は全くなく、
		// tick count の wraparound を検出できるほどに長ければよい
		Semaphore.WaitTimeout(60*1000); // 60秒待つ

		// tick count を取得する
		// この際、wrap around も正常に処理される
		(void) Owner.GetTickCount();
	}
	return 0;
}
//---------------------------------------------------------------------------
#endif














//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaTickCount::tRisaTickCount()
{
#ifdef RISA_TICKCOUNT_NEED_WRAP_WATCH
	// フィールドの初期化
	Value = 0;
	LastTick = GetTick();

	// スレッドの開始
	Watcher = new tWatcher(*this);
#endif
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaTickCount::~tRisaTickCount()
{
#ifdef RISA_TICKCOUNT_NEED_WRAP_WATCH
	// スレッドの停止
	delete Watcher;
#endif
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Tick Count を得る
//---------------------------------------------------------------------------
risse_uint64 tRisaTickCount::GetTickCount()
{
	volatile tRisseCriticalSection::tLocker cs_holder(CS);

#ifdef RISA_TICKCOUNT_NEED_WRAP_WATCH
	// 現在の tick と前回取得した tick の差を Value に追加する。
	// 現在の tick と前回取得した tick の差が tRisaTickCountBasicType型 の
	// 最大値を上回らないことは、それを上回らない程度に このメソッドが
	// Watcher のスレッドから定期的に呼ばれることで保証されている。
	tRisaTickCountBasicType nowtick = GetTick();
	Value += nowtick - LastTick;
	LastTick = nowtick;
	return Value;
#else
	return GetTick();
#endif

}
//---------------------------------------------------------------------------
