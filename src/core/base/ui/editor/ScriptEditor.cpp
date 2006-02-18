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
#include "base/config/ConfigData.h"
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

	wxMenu ContextMenu; //!< ポップアップメニュー

public:
	tRisaScriptEditorTextCtrl(wxWindow *parent);
	~tRisaScriptEditorTextCtrl();

#if wxUSE_ACCEL
	wxAcceleratorTable GetMenuAcceleratorTable();
#endif

private:
	tRisaSingleton<tRisaConfig> ref_tRisaConfig; //!< tRisaConfig に依存

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

	void OnMenuExecute(wxCommandEvent & event);
	void OnMenuUndo(wxCommandEvent & event);
	void OnMenuRedo(wxCommandEvent & event);
	void OnMenuCut(wxCommandEvent & event);
	void OnMenuCopy(wxCommandEvent & event);
	void OnMenuPaste(wxCommandEvent & event);
	void OnMenuDelete(wxCommandEvent & event);
	void OnMenuSelectAll(wxCommandEvent & event);

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
	EVT_MENU(ID_Menu_Execute,			tRisaScriptEditorTextCtrl::OnMenuExecute)
	EVT_MENU(ID_Menu_Undo,				tRisaScriptEditorTextCtrl::OnMenuUndo)
	EVT_MENU(ID_Menu_Redo,				tRisaScriptEditorTextCtrl::OnMenuRedo)
	EVT_MENU(ID_Menu_Cut,				tRisaScriptEditorTextCtrl::OnMenuCut)
	EVT_MENU(ID_Menu_Copy,				tRisaScriptEditorTextCtrl::OnMenuCopy)
	EVT_MENU(ID_Menu_Paste,				tRisaScriptEditorTextCtrl::OnMenuPaste)
	EVT_MENU(ID_Menu_Delete,			tRisaScriptEditorTextCtrl::OnMenuDelete)
	EVT_MENU(ID_Menu_SelectAll,			tRisaScriptEditorTextCtrl::OnMenuSelectAll)
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
	wxMenuItem * item;

	// context ContextMenu of script editor
	item = new wxMenuItem(&ContextMenu, ID_Menu_Execute, _("&Execute\tCtrl+Enter"));
#if wxUSE_OWNER_DRAWN
	item->SetBitmap(wxArtProvider::GetBitmap(wxT("RisaRightTriangle"), wxART_MENU));
#endif
	ContextMenu.Append(item);

	item = new wxMenuItem(&ContextMenu, ID_Menu_Undo, _("&Undo\tCtrl+Z"));
#if wxUSE_OWNER_DRAWN
	item->SetBitmap(wxArtProvider::GetBitmap(wxART_UNDO, wxART_MENU));
#endif
	ContextMenu.Append(item);

	item = new wxMenuItem(&ContextMenu, ID_Menu_Redo, _("&Redo\tCtrl+Y"));
#if wxUSE_OWNER_DRAWN
	item->SetBitmap(wxArtProvider::GetBitmap(wxART_REDO, wxART_MENU));
#endif
	ContextMenu.Append(item);

	item = new wxMenuItem(&ContextMenu, ID_Menu_Cut, _("Cu&t\tCtrl+X"));
#if wxUSE_OWNER_DRAWN
	item->SetBitmap(wxArtProvider::GetBitmap(wxART_CUT, wxART_MENU));
#endif
	ContextMenu.Append(item);

	item = new wxMenuItem(&ContextMenu, ID_Menu_Copy, _("&Copy\tCtrl+C"));
#if wxUSE_OWNER_DRAWN
	item->SetBitmap(wxArtProvider::GetBitmap(wxART_COPY, wxART_MENU));
#endif
	ContextMenu.Append(item);

	item = new wxMenuItem(&ContextMenu, ID_Menu_Paste, _("&Paste\tCtrl+V"));
#if wxUSE_OWNER_DRAWN
	item->SetBitmap(wxArtProvider::GetBitmap(wxART_PASTE, wxART_MENU));
#endif
	ContextMenu.Append(item);

	item = new wxMenuItem(&ContextMenu, ID_Menu_Delete, _("&Delete\tDel"));
	ContextMenu.Append(item);

	item = new wxMenuItem(&ContextMenu, ID_Menu_SelectAll, _("Select &All\tCtrl+A"));
	ContextMenu.Append(item);


#if wxUSE_ACCEL
	SetAcceleratorTable(GetMenuAcceleratorTable());
#endif

	// 内容を設定情報から読み出す
	tRisaConfigData & config =
		tRisaSingleton<tRisaConfig>::instance()->GetVariableConfig();
	wxString content;
	if(config.Read(wxT("editor/content"), &content))
		SetValue(content);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaScriptEditorTextCtrl::~tRisaScriptEditorTextCtrl()
{
	// 内容を設定情報に書き出す
	tRisaConfigData & config =
		tRisaSingleton<tRisaConfig>::instance()->GetVariableConfig();

	config.Write(wxT("editor/content"), GetValue());
}
//---------------------------------------------------------------------------


#if wxUSE_ACCEL
//---------------------------------------------------------------------------
//! @brief		アクセラレータテーブルを返す
//! @return		アクセラレータテーブル
//---------------------------------------------------------------------------
wxAcceleratorTable tRisaScriptEditorTextCtrl::GetMenuAcceleratorTable()
{
	// メニューからアクセラレータの一覧を取得し、それをテーブルに格納して返す
	wxAcceleratorEntry * entries =
		new wxAcceleratorEntry[ContextMenu.GetMenuItemCount()];
	try
	{
		size_t item_count = 0;
		wxMenuItemList& items = ContextMenu.GetMenuItems();
		for(wxMenuItemList::iterator i = items.begin();
			i != items.end(); i++)
		{
			wxAcceleratorEntry *accel = (*i)->GetAccel();
			if(accel)
			{
				entries[item_count].Set(accel->GetFlags(),
							accel->GetKeyCode(), (*i)->GetId());
				item_count++;
				delete accel;
			}
		}
		wxAcceleratorTable table(item_count, entries);
		if(entries) delete [] entries, entries = NULL;
		return table;
	}
	catch(...)
	{
		if(entries) delete [] entries;
		throw;
	}
}
//---------------------------------------------------------------------------
#endif


//---------------------------------------------------------------------------
//! @brief		コンテキストメニューを表示する
//! @param		pos  表示位置
//---------------------------------------------------------------------------
void tRisaScriptEditorTextCtrl::ShowContextMenu(const wxPoint & pos)
{
	ContextMenu.FindItem(ID_Menu_Undo)->Enable(CanUndo());
	ContextMenu.FindItem(ID_Menu_Redo)->Enable(CanRedo());
	ContextMenu.FindItem(ID_Menu_Cut)->Enable(CanCut());
	ContextMenu.FindItem(ID_Menu_Copy)->Enable(CanCopy());
	ContextMenu.FindItem(ID_Menu_Paste)->Enable(CanPaste());
	long selstart, selend;
	GetSelection(&selstart, &selend);
	ContextMenu.FindItem(ID_Menu_Delete)->Enable(selstart != selend);

	PopupMenu(&ContextMenu, pos.x, pos.y);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		「実行」メニューが選択されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaScriptEditorTextCtrl::OnMenuExecute(wxCommandEvent & event)
{
	wxMessageBox(wxT("execute!"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「元に戻す」メニューが選択されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaScriptEditorTextCtrl::OnMenuUndo(wxCommandEvent & event)
{
	Undo();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「やり直し」メニューが選択されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaScriptEditorTextCtrl::OnMenuRedo(wxCommandEvent & event)
{
	Redo();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「切り取り」メニューが選択されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaScriptEditorTextCtrl::OnMenuCut(wxCommandEvent & event)
{
	Cut();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「コピー」メニューが選択されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaScriptEditorTextCtrl::OnMenuCopy(wxCommandEvent & event)
{
	Copy();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「貼り付け」メニューが選択されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaScriptEditorTextCtrl::OnMenuPaste(wxCommandEvent & event)
{
	Paste();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「削除」メニューが選択されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaScriptEditorTextCtrl::OnMenuDelete(wxCommandEvent & event)
{
	long from, to;
	GetSelection(&from, &to);
	Remove(from, to);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「すべて選択」メニューが選択されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaScriptEditorTextCtrl::OnMenuSelectAll(wxCommandEvent & event)
{
	SetSelection(-1, -1);
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


