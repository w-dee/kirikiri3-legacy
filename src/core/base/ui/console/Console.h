//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief コンソールフレーム
//---------------------------------------------------------------------------
#ifndef _CONSOLE_H
#define _CONSOLE_H

#include "risse/include/risse.h"
#include "base/ui/UIUtils.h"
#include "base/event/Event.h"


//---------------------------------------------------------------------------
//! @brief		コンソールのフレーム
//---------------------------------------------------------------------------
class tRisaLogScrollView;
class tRisaLogViewerStatusBar;
class tRisaConsoleFrame : public tRisaUIFrame,
						depends_on<tRisaEventSystem>
{
	enum
	{
		ID_First = 100,
		ID_Event
	};
public:
	tRisaConsoleFrame();

	void SetFocusToLogViewer();
	void SetFocusToTextCtrl(int insert_code = 0);

private:
	tRisaLogScrollView *ScrollView;
	tRisaLogViewerStatusBar *StatusBar;

	void OnEventTool(wxCommandEvent & event);
	void OnUpdateUI(wxUpdateUIEvent & event);

	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------


#endif
