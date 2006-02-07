//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ログビューア
//---------------------------------------------------------------------------
#ifndef _LOGVIEWER_H
#define _LOGVIEWER_H

#include "risse/include/risse.h"
#include <deque>


//---------------------------------------------------------------------------
//! @brief		ログビューアのフレーム
//---------------------------------------------------------------------------
class tRisaLogScrollView;
class tRisaLogViewerFrame : public wxFrame
{
public:
	tRisaLogViewerFrame();

private:
	tRisaLogScrollView *ScrollView;

	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------


#endif
