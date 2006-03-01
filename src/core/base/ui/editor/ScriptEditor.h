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
#include "base/ui/UIUtils.h"


//---------------------------------------------------------------------------
//! @brief		スクリプトエディタのフレーム
//---------------------------------------------------------------------------
class tRisaScriptEditorTextCtrl;
class tRisaScriptEditorStatusBar;
class tRisaScriptEditorFrame : public tRisaUIFrame
{
public:
	enum
	{
		ID_First = 100,
		ID_TextCtrl,
		ID_Command_Execute,
		ID_Last
	};

public:
	tRisaScriptEditorFrame();

public:
	void SetContent(const wxString & content);
	void SetReadOnly(bool b);
	void SetLinePosition(unsigned long pos);
	void SetStatusString(const wxString & status);

private:
	tRisaScriptEditorTextCtrl *TextCtrl;
	tRisaScriptEditorStatusBar *StatusBar;

	void OnCommandExecute(wxCommandEvent & event);
	void OnTextCtrlUpdateUI(wxUpdateUIEvent & event);

	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------


#endif
