//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief TickCount を提供するモジュール
//---------------------------------------------------------------------------
#ifndef _TICKCOUNT_H
#define _TICKCOUNT_H

/*! @note
	tRisaTickCount は 64bit 整数によるミリ秒単位のティックカウントを
	提供する。

	ティックカウントは任意時間ごとにカウントアップされていく値であり、
	単位はミリ秒である。「任意時間」はOSや環境によって異なり、1msだったり
	10msだったりする。これは、連続してティックカウントを読み出していっても
	1msごとの増加とならずに10msごとにとびとびの値として読み出される可能性が
	あるということである。単位はミリ秒ではあるが、精度はミリ秒ではない、
	とも言える。
	ティックカウントの値はRisaの実行開始時からの経過時間であったり、
	UNIX epoch からの経過時間だったりする(実装依存)。
	基本的には、二点間の時間的距離を計測する用途に用いるが、
	tRisaTimerSchedulerなどではこのtRisaTickCountが返すティックカウント
	を「絶対TickCount」とよび、時間の基準としている。

	tRisaTickCount が拠り所としているクロック源はシステムのWall-Clock
	(日付時刻)とは異なる場合がある。長期間計測を行うと、tRisaTickCountが
	返す時間と日付時刻取得関数(たとえばRisseのDateクラスなど)が水晶発振子の
	誤差などによりずれてくる可能性があるということである。
	tRisaTickCountを拠り所としているtRisaTimerSchedulerもWall-Clockに対して
	ずれる可能性がある。

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


#include "basetypes.h"
#include "base/utils/Singleton.h"
#include "base/utils/RisaThread.h"

#ifdef __WXMSW__
	#define RISA_DECLARE_TYPEDEF typedef DWORD tRisaTickCountBasicType;
	#define RISA_TICKCOUNT_NEED_WRAP_WATCH
	#include <windows.h>
	#if defined(__WIN64__) || defined(__WIN32__)
		#define RISA_DECLARE_GETTICK tRisaTickCountBasicType GetTick() { return ::timeGetTime(); }
		#include <mmsystem.h>
		#define RISA_TICKCOUNT_DEPENDS_ON depends_on<tRisaWinTimerResolutionAdjuster>
		//! @brief Win32/Win64? でタイマの分解能を最低でも10msにするクラス
		//! @note
		//! いくつかの環境(おそらく性能の低いコンピュータ)ではタイマの分解能が
		//! デフォルトで20msぐらいになっている環境がある。
		//! サウンドなどのサービスの中にはタイマの分解能に敏感な
		//! 物があるため、最低でも10msの精度に合わせる。ちなみにこの分解能を
		//! 上げると、どうやらコンテキストスイッチングの時間制度も上がる模様。
		class tRisaWinTimerResolutionAdjuster :
			public singleton_base<tRisaWinTimerResolutionAdjuster>
		{
			UINT TimerPrecision;
		public:
			tRisaWinTimerResolutionAdjuster()
			{
				TimerPrecision = 10;
				// retrieve minimum timer resolution
				TIMECAPS tc;
				timeGetDevCaps(&tc, sizeof(tc));
				if(TimerPrecision < tc.wPeriodMin)
					TimerPrecision = tc.wPeriodMin;
				if(TimerPrecision > tc.wPeriodMax)
					TimerPrecision = tc.wPeriodMax;
				// set timer resolution
				timeBeginPeriod(TimerPrecision);
			}

			~tRisaWinTimerResolutionAdjuster()
			{
				timeEndPeriod(TimerPrecision);
			}
		};
	#else
		#define RISA_DECLARE_GETTICK tRisaTickCountBasicType GetTick() \
						{ return ::GetTickCount(); }
	#endif
#else
	#define RISA_DECLARE_TYPEDEF typedef risse_uint64 tRisaTickCountBasicType;
	#define RISA_DECLARE_GETTICK tRisaTickCountBasicType GetTick() \
						{ return ::wxGetLocalTimeMillis().GetValue() ; }
	#include <wx/timer.h>
#endif


//---------------------------------------------------------------------------
//! @brief		Tickcountを提供するクラス
//---------------------------------------------------------------------------
class tRisaTickCount : public singleton_base<tRisaTickCount>
	#ifdef RISA_TICKCOUNT_DEPENDS_ON
		, RISA_TICKCOUNT_DEPENDS_ON
	#endif
{
	tRisaCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

	RISA_DECLARE_TYPEDEF
	RISA_DECLARE_GETTICK

#ifdef RISA_TICKCOUNT_NEED_WRAP_WATCH
	//! @brief	tickcount の wraparound を検出するためのスレッドのクラス
	class tWatcher : public tRisaThread
	{
		tRisaThreadEvent Event; //!< イベントオブジェクト
		tRisaTickCount & Owner;
	public:

		//! @brief		コンストラクタ
		//! @param		owner		このオブジェクトを所有する tRisaTickCount オブジェクト
		tWatcher(tRisaTickCount & owner);

		//! @brief		デストラクタ
		~tWatcher();

		//! @brief		スレッドのエントリーポイント
		void Execute();
	};

	tWatcher *Watcher; //!< tickcount の wraparound を検出するためのスレッド

	risse_uint64 Value; //!< 現在の tick
	tRisaTickCountBasicType LastTick; //!< 最後に取得した リファレンスのtick
#endif

public:
	static const risse_uint64 InvalidTickCount = ~ static_cast<risse_uint64>(0);
		//!< 無効な tick を表す値

	//! @brief		コンストラクタ
	tRisaTickCount();

	//! @brief		デストラクタ
	~tRisaTickCount();

	//! @brief		Tick Count を得る
	risse_uint64 Get();
};
//---------------------------------------------------------------------------

#endif
