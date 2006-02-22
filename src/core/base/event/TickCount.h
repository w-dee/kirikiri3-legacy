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
#ifndef _TICKCOUNT_H
#define _TICKCOUNT_H

#include "base/utils/Singleton.h"

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


#ifdef RISA_TICKCOUNT_NEED_WRAP_WATCH
	#include <wx/thread.h>
#endif

//---------------------------------------------------------------------------
//! @brief		Tickcountを提供するクラス
//---------------------------------------------------------------------------
class tRisaTickCount : public singleton_base<tRisaTickCount>
	#ifdef RISA_TICKCOUNT_DEPENDS_ON
		, RISA_TICKCOUNT_DEPENDS_ON
	#endif
{
	tRisseCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

	RISA_DECLARE_TYPEDEF
	RISA_DECLARE_GETTICK

#ifdef RISA_TICKCOUNT_NEED_WRAP_WATCH
	//! @brief	tickcount の wraparound を検出するためのスレッド
	class tWatcher : public wxThread
	{
		wxSemaphore Semaphore; //!< セマフォ
		tRisaTickCount & Owner;
	public:
		tWatcher(tRisaTickCount & owner);
		~tWatcher();
		ExitCode Entry();
	};

	tWatcher *Watcher;

	risse_uint64 Value;
	tRisaTickCountBasicType LastTick;
#endif

public:
	tRisaTickCount();
	~tRisaTickCount();

	risse_uint64 GetTickCount();
};
//---------------------------------------------------------------------------

#endif
