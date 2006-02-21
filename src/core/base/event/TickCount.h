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
	#define RISA_DECLARE_GETTICK tRisaTickCountBasicType GetTick() { return ::GetTickCount(); }
	#define RISA_TICKCOUNT_NEED_WRAP_WATCH
	#include <windows.h>
	#include <wx/thread.h>
#else
	#define RISA_DECLARE_TYPEDEF typedef wxLongLong tRisaTickCountBasicType;
	#define RISA_DECLARE_GETTICK tRisaTickCountBasicType GetTick() { return ::wxGetLocalTimeMillis(); }
	#include <wx/timer.h>
#endif


//---------------------------------------------------------------------------
//! @brief		Tickcountを提供するクラス
//---------------------------------------------------------------------------
class tRisaTickCount : public singleton_base<tRisaTickCount>
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
