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
#ifndef _RISATHREAD_H
#define _RISATHREAD_H

#include <wx/log.h>
/*! @note

Risseの実装を再利用する
*/



#include "risse/include/risseThread.h"


//---------------------------------------------------------------------------
//! @brief		クリティカルセクションクラス
//---------------------------------------------------------------------------
typedef Risse::tRisseCriticalSection tRisaCriticalSection;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スレッドの基本クラス
//---------------------------------------------------------------------------
typedef Risse::tRisseThread tRisaThread;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スレッドイベントクラス
//---------------------------------------------------------------------------
typedef Risse::tRisseThreadEvent tRisaThreadEvent;
//---------------------------------------------------------------------------



#endif
