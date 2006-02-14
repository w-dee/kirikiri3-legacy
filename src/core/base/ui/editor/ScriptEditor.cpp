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

#if wxUSE_OWNER_DRAWN
#include <wx/artprov.h>
#endif


RISSE_DEFINE_SOURCE_ID(45447,29186,11918,17485,47798,28394,33256,47673);



//---------------------------------------------------------------------------
//! @brief		テキストエディタ用のカスタムテキストコントロール
//---------------------------------------------------------------------------
class tRisaScriptEditorTextCtrl : public wxTextCtrl
{
	enum
	{
		ID_First = 100,
		ID_Menu_Execute,
		ID_Menu_Undo,
		ID_Menu_Redo,
		ID_Menu_Cut,
		ID_Menu_Copy,
		ID_Menu_Paste,
		ID_Menu_Delete,
		ID_Menu_SelectAll,
		ID_Last
	};

public:
	tRisaScriptEditorTextCtrl(wxWindow *parent);
	~tRisaScriptEditorTextCtrl();

private:
	void ShowContextMenu(const wxPoint & pos);

#if USE_CONTEXT_MENU
	void OnContextMenu(wxContextMenuEvent& event)
	{
		wxPoint point = event.GetPosition();
		// If from keyboard
		if (point.x == -1 && point.y == -1)
		{
			wxSize size = GetSize();
			point.x = size.x / 2;
			point.y = size.y / 2;
		}
		else
		{
			point = ScreenToClient(point);
		}
		ShowContextMenu(point);
		event.Skip(false);
	}

#else
	void OnRightUp(wxMouseEvent& event)
	{
		SetFocus();
		ShowContextMenu(event.GetPosition());
		event.Skip(false);
	}
#endif

	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		イベントテーブル
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tRisaScriptEditorTextCtrl, wxTextCtrl)
#if USE_CONTEXT_MENU
	EVT_CONTEXT_MENU(					tRisaScriptEditorTextCtrl::OnContextMenu)
#else
	EVT_RIGHT_UP(						tRisaScriptEditorTextCtrl::OnRightUp)
#endif
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
//! @brief		コンテキストメニューを表示する
//! @param		pos  表示位置
//---------------------------------------------------------------------------
void tRisaScriptEditorTextCtrl::ShowContextMenu(const wxPoint & pos)
{
	// 切り取り、コピー、貼り付け、削除
	wxMenu menu;
	wxMenuItem * item;

	// context menu of script editor
	item = new wxMenuItem(&menu, ID_Menu_Execute, _("&Execute"));
#if wxUSE_OWNER_DRAWN
	item->SetBitmap(wxArtProvider::GetBitmap(wxT("RisaRightTriangle"), wxART_MENU));
#endif
	menu.Append(item);
//	item->Enable(AnySelected());

	item = new wxMenuItem(&menu, ID_Menu_Undo, _("&Undo"));
#if wxUSE_OWNER_DRAWN
	item->SetBitmap(wxArtProvider::GetBitmap(wxART_UNDO, wxART_MENU));
#endif
	menu.Append(item);
//	item->Enable(AnySelected());

	item = new wxMenuItem(&menu, ID_Menu_Redo, _("&Redo"));
#if wxUSE_OWNER_DRAWN
	item->SetBitmap(wxArtProvider::GetBitmap(wxART_REDO, wxART_MENU));
#endif
	menu.Append(item);
//	item->Enable(AnySelected());

	item = new wxMenuItem(&menu, ID_Menu_Cut, _("Cu&t"));
#if wxUSE_OWNER_DRAWN
	item->SetBitmap(wxArtProvider::GetBitmap(wxART_CUT, wxART_MENU));
#endif
	menu.Append(item);
//	item->Enable(AnySelected());

	item = new wxMenuItem(&menu, ID_Menu_Copy, _("&Copy"));
#if wxUSE_OWNER_DRAWN
	item->SetBitmap(wxArtProvider::GetBitmap(wxART_COPY, wxART_MENU));
#endif
	menu.Append(item);
//	item->Enable(AnySelected());

	item = new wxMenuItem(&menu, ID_Menu_Paste, _("&Paste"));
#if wxUSE_OWNER_DRAWN
	item->SetBitmap(wxArtProvider::GetBitmap(wxART_PASTE, wxART_MENU));
#endif
	menu.Append(item);
//	item->Enable(AnySelected());

	item = new wxMenuItem(&menu, ID_Menu_Delete, _("&Delete"));
	menu.Append(item);
//	item->Enable(AnySelected());

	item = new wxMenuItem(&menu, ID_Menu_SelectAll, _("Select &All"));
	menu.Append(item);
//	item->Enable(AnySelected());

	PopupMenu(&menu, pos.x, pos.y);
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


