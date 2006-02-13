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
#include "prec.h"
#include "base/script/RisseEngine.h"
#include "base/ui/Editor/ScriptEditor.h"
#include <stdlib.h>
#include <deque>
#include <wx/textctrl.h>

RISSE_DEFINE_SOURCE_ID(45447,29186,11918,17485,47798,28394,33256,47673);



//---------------------------------------------------------------------------
//! @brief		テキストエディタ用のカスタムテキストコントロール
//---------------------------------------------------------------------------
class tRisaScriptEditorTextCtrl : public wxTextCtrl
{

public:
	tRisaScriptEditorTextCtrl(wxWindow *parent);
	~tRisaScriptEditorTextCtrl();
private:

	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		イベントテーブル
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tRisaScriptEditorTextCtrl, wxTextCtrl)
END_EVENT_TABLE()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		parent 親ウィンドウ
//---------------------------------------------------------------------------
tRisaScriptEditorTextCtrl::tRisaScriptEditorTextCtrl(wxWindow *parent):
	wxTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
		wxTE_MULTILINE|wxTE_RICH)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaScriptEditorTextCtrl::~tRisaScriptEditorTextCtrl()
{
}
//---------------------------------------------------------------------------





















//---------------------------------------------------------------------------
//! @brief		スクリプトエディタ用のカスタムステータスバー
//---------------------------------------------------------------------------
class tRisaScriptEditorStatusBar : public wxStatusBar
{
//	tRisaHistoryTextCtrl * TextCtrl; //!< テキストコントロール

public:
	tRisaScriptEditorStatusBar(wxWindow *parent);
	virtual ~tRisaScriptEditorStatusBar();


private:
	void AdjustControlSize();

	void OnSize(wxSizeEvent& event);
	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ログビューア用のカスタムステータスバー用のイベントテーブル
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tRisaScriptEditorStatusBar, wxStatusBar)
	EVT_SIZE(					tRisaScriptEditorStatusBar::OnSize)
END_EVENT_TABLE()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		parent 親ウィンドウ
//---------------------------------------------------------------------------
tRisaScriptEditorStatusBar::tRisaScriptEditorStatusBar(wxWindow *parent)
		   : wxStatusBar(parent, wxID_ANY)
{
	SetFieldsCount(1);

//	TextCtrl = new tRisaHistoryTextCtrl(this);

//	SetMinHeight(TextCtrl->GetSize().GetWidth() + 4);

	AdjustControlSize();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaScriptEditorStatusBar::~tRisaScriptEditorStatusBar()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コントロールのサイズを調整する
//---------------------------------------------------------------------------
void tRisaScriptEditorStatusBar::AdjustControlSize()
{
	wxRect rect;
	GetFieldRect(0, rect);

//	TextCtrl->SetSize(rect.x +2, rect.y +2, rect.width - 4, rect.height - 4);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		サイズが変更されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaScriptEditorStatusBar::OnSize(wxSizeEvent& event)
{
	AdjustControlSize();

	event.Skip();
}
//---------------------------------------------------------------------------




























//---------------------------------------------------------------------------
//! @brief		イベントテーブルの定義
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tRisaScriptEditorFrame, wxFrame)
END_EVENT_TABLE()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaScriptEditorFrame::tRisaScriptEditorFrame() :
	wxFrame(NULL, wxID_ANY, _("Script Editor"), wxDefaultPosition, wxDefaultSize,
		wxDEFAULT_FRAME_STYLE)
{
	TextCtrl = new tRisaScriptEditorTextCtrl(this);
	StatusBar = new tRisaScriptEditorStatusBar(this);

	SetStatusBar(StatusBar);
}
//---------------------------------------------------------------------------


