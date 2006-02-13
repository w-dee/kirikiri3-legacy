//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief スクリプトエディタフレーム
//---------------------------------------------------------------------------
#ifndef _SCRIPTEDITOR_H
#define _SCRIPTEDITOR_H

#include "risse/include/risse.h"


//---------------------------------------------------------------------------
//! @brief		コンソールのフレーム
//---------------------------------------------------------------------------
class tRisaScriptEditorTextCtrl;
class tRisaScriptEditorStatusBar;
class tRisaScriptEditorFrame : public wxFrame
{
public:
	tRisaScriptEditorFrame();

private:
	tRisaScriptEditorTextCtrl *TextCtrl;
	tRisaScriptEditorStatusBar *StatusBar;

	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------


#endif
