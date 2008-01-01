//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief スクリプトエディタフレーム
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/script/RisseEngine.h"
#include "base/exception/RisaException.h"
#include "base/ui/Editor/ScriptEditor.h"
#include "base/config/ConfigData.h"
#include <stdlib.h>
#include <deque>
#include <wx/textctrl.h>
#include <wx/bmpbuttn.h>

#if wxUSE_OWNER_DRAWN
#include <wx/artprov.h>
#endif


namespace Risa {
RISSE_DEFINE_SOURCE_ID(45447,29186,11918,17485,47798,28394,33256,47673);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		テキストエディタ用のカスタムテキストコントロール
//---------------------------------------------------------------------------
class tScriptEditorTextCtrl : public wxTextCtrl, depends_on<tConfig>
{
	enum
	{
		ID_First = tScriptEditorFrame::ID_Last,
		ID_Menu_Execute,
		ID_Menu_Undo,
		ID_Menu_Redo,
		ID_Menu_Cut,
		ID_Menu_Copy,
		ID_Menu_Paste,
		ID_Menu_SelectAll,
		ID_Menu_Open,
		ID_Menu_Save,
		ID_Menu_SaveAs,
		ID_Last
	};

	wxMenu ContextMenu; //!< ポップアップメニュー
	wxString FileName; //!< ファイル名
	wxString DefaultExt; //!< (ファイル選択ダイアログボックスにおける)デフォルトの拡張子
	wxString DefaultFilter; //!< (ファイル選択ダイアログボックスにおける)デフォルトのフィルタ

public:
	tScriptEditorTextCtrl(wxWindow *parent);
	~tScriptEditorTextCtrl();

private:
	void WriteConfig();

public:
	void GoToLine(long l);

#if wxUSE_ACCEL
	wxAcceleratorTable GetMenuAcceleratorTable();
#endif

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
public:
	void Execute();

private:
	void OnMenuUndo(wxCommandEvent & event);
	void OnMenuRedo(wxCommandEvent & event);
	void OnMenuCut(wxCommandEvent & event);
	void OnMenuCopy(wxCommandEvent & event);
	void OnMenuPaste(wxCommandEvent & event);
	void OnMenuSelectAll(wxCommandEvent & event);
	void OnMenuOpen(wxCommandEvent & event);
	void OnMenuSave(wxCommandEvent & event);
	void OnMenuSaveAs(wxCommandEvent & event);

	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		イベントテーブル
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tScriptEditorTextCtrl, wxTextCtrl)
#if USE_CONTEXT_MENU
	EVT_CONTEXT_MENU(					tScriptEditorTextCtrl::OnContextMenu)
#else
	EVT_RIGHT_UP(						tScriptEditorTextCtrl::OnRightUp)
#endif
	EVT_MENU(ID_Menu_Undo,				tScriptEditorTextCtrl::OnMenuUndo)
	EVT_MENU(ID_Menu_Redo,				tScriptEditorTextCtrl::OnMenuRedo)
	EVT_MENU(ID_Menu_Cut,				tScriptEditorTextCtrl::OnMenuCut)
	EVT_MENU(ID_Menu_Copy,				tScriptEditorTextCtrl::OnMenuCopy)
	EVT_MENU(ID_Menu_Paste,				tScriptEditorTextCtrl::OnMenuPaste)
	EVT_MENU(ID_Menu_SelectAll,			tScriptEditorTextCtrl::OnMenuSelectAll)
	EVT_MENU(ID_Menu_Open,				tScriptEditorTextCtrl::OnMenuOpen)
	EVT_MENU(ID_Menu_Save,				tScriptEditorTextCtrl::OnMenuSave)
	EVT_MENU(ID_Menu_SaveAs,			tScriptEditorTextCtrl::OnMenuSaveAs)
END_EVENT_TABLE()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		parent 親ウィンドウ
//---------------------------------------------------------------------------
tScriptEditorTextCtrl::tScriptEditorTextCtrl(wxWindow *parent):
	wxTextCtrl(parent, tScriptEditorFrame::ID_TextCtrl,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		wxTE_MULTILINE|wxTE_RICH|wxTE_DONTWRAP)
{
	// フィールドの初期化
	// for file dialog box default ext/filter of script editor
	DefaultExt = wxT("rs");
	DefaultFilter = wxString() + 
		_("Risse script (*.rs)|*.rs")	+ wxT("|") +
		_("Text file (*.txt)|*.txt")	+ wxT("|") +
		_("All files (*.*)|*.*"),

	// UI アップデートイベントの受け取り
	SetExtraStyle(GetExtraStyle()|wxWS_EX_PROCESS_UI_UPDATES);

	// ContextMenu の作成
	wxMenuItem * item;

	// context ContextMenu of script editor
	wxString acc_sep(wxT("\t"));
	item = new wxMenuItem(&ContextMenu, tScriptEditorFrame::ID_Command_Execute, 
											_("&Execute") + acc_sep + wxT("Ctrl+Enter"));
#if wxUSE_OWNER_DRAWN
	item->SetBitmap(wxArtProvider::GetBitmap(wxT("RightTriangle"), wxART_MENU));
#endif
	ContextMenu.Append(item);

	item = new wxMenuItem(&ContextMenu, ID_Menu_Undo, _("&Undo") + acc_sep + wxT("Ctrl+Z"));
#if wxUSE_OWNER_DRAWN
	item->SetBitmap(wxArtProvider::GetBitmap(wxART_UNDO, wxART_MENU));
#endif
	ContextMenu.Append(item);

	item = new wxMenuItem(&ContextMenu, ID_Menu_Redo, _("&Redo") + acc_sep + wxT("Ctrl+Y"));
#if wxUSE_OWNER_DRAWN
	item->SetBitmap(wxArtProvider::GetBitmap(wxART_REDO, wxART_MENU));
#endif
	ContextMenu.Append(item);

	item = new wxMenuItem(&ContextMenu, ID_Menu_Cut, _("Cu&t") + acc_sep + wxT("Ctrl+X"));
#if wxUSE_OWNER_DRAWN
	item->SetBitmap(wxArtProvider::GetBitmap(wxART_CUT, wxART_MENU));
#endif
	ContextMenu.Append(item);

	item = new wxMenuItem(&ContextMenu, ID_Menu_Copy, _("&Copy") + acc_sep + wxT("Ctrl+C"));
#if wxUSE_OWNER_DRAWN
	item->SetBitmap(wxArtProvider::GetBitmap(wxART_COPY, wxART_MENU));
#endif
	ContextMenu.Append(item);

	item = new wxMenuItem(&ContextMenu, ID_Menu_Paste, _("&Paste") + acc_sep + wxT("Ctrl+V"));
#if wxUSE_OWNER_DRAWN
	item->SetBitmap(wxArtProvider::GetBitmap(wxART_PASTE, wxART_MENU));
#endif
	ContextMenu.Append(item);

	item = new wxMenuItem(&ContextMenu, ID_Menu_SelectAll, _("Select &All") + acc_sep + wxT("Ctrl+A"));
	ContextMenu.Append(item);


	ContextMenu.AppendSeparator();

	item = new wxMenuItem(&ContextMenu, ID_Menu_Open, _("&Open...") + acc_sep + wxT("Ctrl+O"));
#if wxUSE_OWNER_DRAWN
	item->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_MENU));
#endif
	ContextMenu.Append(item);

	item = new wxMenuItem(&ContextMenu, ID_Menu_Save, _("&Save") + acc_sep + wxT("Ctrl+S"));
#if wxUSE_OWNER_DRAWN
	item->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_MENU));
#endif
	ContextMenu.Append(item);

	item = new wxMenuItem(&ContextMenu, ID_Menu_SaveAs, _("Save &As..."));
#if wxUSE_OWNER_DRAWN
	item->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS, wxART_MENU));
#endif
	ContextMenu.Append(item);


#if wxUSE_ACCEL
	SetAcceleratorTable(GetMenuAcceleratorTable());
#endif

	// 内容を設定情報から読み出す
	tConfigData & config =
		tConfig::instance()->GetVariableConfig();
	wxString content;
	if(config.Read(wxT("ui/editor/content"), &content))
		SetValue(content);
	long ins_pos;
	if(config.Read(wxT("ui/editor/insertion_point"), &ins_pos))
		SetInsertionPoint(ins_pos);
	SetSelection(0, 0);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tScriptEditorTextCtrl::~tScriptEditorTextCtrl()
{
	// 内容を設定情報に書き出す
	WriteConfig();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		内容を設定情報に書き出す
//---------------------------------------------------------------------------
void tScriptEditorTextCtrl::WriteConfig()
{
	// 内容を設定情報に書き出す
	tConfigData & config = tConfig::instance()->GetVariableConfig();

	config.Write(wxT("ui/editor/content"), GetValue());
	config.Write(wxT("ui/editor/insertion_point"), GetInsertionPoint());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定行に移動する
//! @param		l		移動したい行
//---------------------------------------------------------------------------
void tScriptEditorTextCtrl::GoToLine(long l)
{
	long pos = XYToPosition(0, l);
	SetInsertionPoint(pos);
	ShowPosition(pos);
}
//---------------------------------------------------------------------------


#if wxUSE_ACCEL
//---------------------------------------------------------------------------
//! @brief		アクセラレータテーブルを返す
//! @return		アクセラレータテーブル
//---------------------------------------------------------------------------
wxAcceleratorTable tScriptEditorTextCtrl::GetMenuAcceleratorTable()
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
void tScriptEditorTextCtrl::ShowContextMenu(const wxPoint & pos)
{
	ContextMenu.FindItem(ID_Menu_Undo)->Enable(CanUndo());
	ContextMenu.FindItem(ID_Menu_Redo)->Enable(CanRedo());
	ContextMenu.FindItem(ID_Menu_Cut)->Enable(CanCut());
	ContextMenu.FindItem(ID_Menu_Copy)->Enable(CanCopy());
	ContextMenu.FindItem(ID_Menu_Paste)->Enable(CanPaste());
	long selstart, selend;
	GetSelection(&selstart, &selend);

	PopupMenu(&ContextMenu, pos.x, pos.y);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		内容を Risse で実行する
//---------------------------------------------------------------------------
void tScriptEditorTextCtrl::Execute()
{
	// 内容をファイルに保存する
	WriteConfig();
	tConfig::instance()->GetVariableConfig().Flush();

	// Risse に実行させる
	tString block_name("Script Editor");

	try
	{
		tRisseScriptEngine::instance()->
			Evaluate(tString(GetValue()), block_name, 0, NULL, NULL, false);
	}
	RISA_CATCH_AND_SHOW_SCRIPT_EXCEPTION(block_name)
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「元に戻す」メニューが選択されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tScriptEditorTextCtrl::OnMenuUndo(wxCommandEvent & event)
{
	Undo();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「やり直し」メニューが選択されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tScriptEditorTextCtrl::OnMenuRedo(wxCommandEvent & event)
{
	Redo();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「切り取り」メニューが選択されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tScriptEditorTextCtrl::OnMenuCut(wxCommandEvent & event)
{
	Cut();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「コピー」メニューが選択されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tScriptEditorTextCtrl::OnMenuCopy(wxCommandEvent & event)
{
	Copy();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「貼り付け」メニューが選択されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tScriptEditorTextCtrl::OnMenuPaste(wxCommandEvent & event)
{
	Paste();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「すべて選択」メニューが選択されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tScriptEditorTextCtrl::OnMenuSelectAll(wxCommandEvent & event)
{
	SetSelection(-1, -1);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「開く」 メニューが選択されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tScriptEditorTextCtrl::OnMenuOpen(wxCommandEvent & event)
{
	wxString filename = ::wxFileSelector(
		// messsage for selecting an input file of script editor
		_("Select a file to open"), 
		// default_path
		::wxEmptyString,
		// default_filename
		FileName,
		// default extension for selecting a font file
		DefaultExt,
		// filename filter for selecting a font file
		DefaultFilter,
		// flags
		wxFD_OPEN|wxFD_FILE_MUST_EXIST
		);
	if(!filename.empty())
	{
		FileName = filename;
		LoadFile(FileName);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「保存」メニューが選択されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tScriptEditorTextCtrl::OnMenuSave(wxCommandEvent & event)
{
	if(FileName.empty())
		OnMenuSaveAs(event);
	else
		SaveFile(FileName);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「名前を付けて保存」メニューが選択されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tScriptEditorTextCtrl::OnMenuSaveAs(wxCommandEvent & event)
{
	wxString filename = ::wxFileSelector(
		// messsage for select output file
		_("Select an output file"), 
		// default_path
		::wxEmptyString,
		// default_filename
		FileName,
		// default extension for selecting output file (must be bff)
		DefaultExt,
		// filename filter for selecting output file
		DefaultFilter,
		// flags
		wxFD_SAVE|wxFD_OVERWRITE_PROMPT
		);
	if(!filename.empty())
	{
		FileName = filename;
		SaveFile(FileName);
	}
}
//---------------------------------------------------------------------------






















//---------------------------------------------------------------------------
//! @brief		スクリプトエディタ用のカスタムステータスバー
//---------------------------------------------------------------------------
class tScriptEditorStatusBar : public wxStatusBar
{
	enum
	{
		Col_ExecuteButton,
		Col_Position,
		Col_Message
	};

	wxBitmapButton * ExecuteButton; //!< 実行ボタン
	long LastX;
	long LastY;

public:
	tScriptEditorStatusBar(wxWindow *parent);
	virtual ~tScriptEditorStatusBar();

	wxString GetCaretPosStatusString(long x, long y);
	void SetCaretPosStatus(long x, long y);

private:
	void AdjustControlSize();

	void OnSize(wxSizeEvent& event);
	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ログビューア用のカスタムステータスバー用のイベントテーブル
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tScriptEditorStatusBar, wxStatusBar)
	EVT_SIZE(					tScriptEditorStatusBar::OnSize)
END_EVENT_TABLE()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		parent 親ウィンドウ
//---------------------------------------------------------------------------
tScriptEditorStatusBar::tScriptEditorStatusBar(wxWindow *parent)
		   : wxStatusBar(parent, wxID_ANY)
{
	// フィールドの初期化
	LastX = -1;
	LastY = -1;

	// create a bitmap button attached to the status bar
	ExecuteButton = new wxBitmapButton(this, tScriptEditorFrame::ID_Command_Execute,
		wxArtProvider::GetBitmap(wxT("RightTriangle"), wxART_MENU, wxSize(12,12)));
	ExecuteButton->SetToolTip(_("Execute"));

	SetMinHeight(ExecuteButton->GetSize().GetHeight() + 4);

	// ステータスバーの各フィールドのサイズを計算
	wxRect rect;
	GetFieldRect(Col_ExecuteButton, rect);

	wxClientDC dc(this);
	wxCoord tw=0, th=0;
	// 位置表示部分は、とりあえず 9999,9999 を表示させた場合のサイズにする
	dc.GetTextExtent(GetCaretPosStatusString(9999, 9999), &tw, &th);
	int widths[3] = { rect.height, tw, -1 };
	SetFieldsCount(sizeof(widths)/sizeof(*widths), widths);
	SetCaretPosStatus(-1, -1);

	// コントロールのサイズを調整
	AdjustControlSize();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tScriptEditorStatusBar::~tScriptEditorStatusBar()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		キャレットの位置を表す文字列を取得する
//---------------------------------------------------------------------------
wxString tScriptEditorStatusBar::GetCaretPosStatusString(long x, long y)
{
	if(x == -1 && y == -1)
	{
		return wxEmptyString;
	}
	else
	{
		// caret position status
		return wxString::Format(_("line %ld, col %ld"), y+1, x+1);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		キャレットの位置を表示する
//! @param		x   桁位置
//! @param		y   行位置
//---------------------------------------------------------------------------
void tScriptEditorStatusBar::SetCaretPosStatus(long x, long y)
{
	if(LastX != x || LastY != y)
	{
		LastX = x;
		LastY = y;
		SetStatusText(GetCaretPosStatusString(x, y), Col_Position);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コントロールのサイズを調整する
//---------------------------------------------------------------------------
void tScriptEditorStatusBar::AdjustControlSize()
{
	wxRect rect;
	GetFieldRect(Col_ExecuteButton, rect);

	ExecuteButton->SetSize(rect.x +2, rect.y +2, rect.height - 4, rect.height - 4);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		サイズが変更されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tScriptEditorStatusBar::OnSize(wxSizeEvent& event)
{
	AdjustControlSize();

	event.Skip();
}
//---------------------------------------------------------------------------









































//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tScriptEditorFrame, tUIFrame)
EVT_MENU(ID_Command_Execute,			tScriptEditorFrame::OnCommandExecute)
EVT_BUTTON(ID_Command_Execute,			tScriptEditorFrame::OnCommandExecute)
EVT_UPDATE_UI(ID_TextCtrl,				tScriptEditorFrame::OnTextCtrlUpdateUI) 
END_EVENT_TABLE()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tScriptEditorFrame::tScriptEditorFrame() :
	tUIFrame(wxT("ui/editor"), _("Script Editor"))
{
	TextCtrl = new tScriptEditorTextCtrl(this);
	StatusBar = new tScriptEditorStatusBar(this);

	SetStatusBar(StatusBar);

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tScriptEditorFrame::SetContent(const wxString & content)
{
	TextCtrl->SetValue(content);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tScriptEditorFrame::SetReadOnly(bool b)
{
	TextCtrl->SetEditable(!b);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tScriptEditorFrame::SetLinePosition(unsigned long pos)
{
	TextCtrl->GoToLine(pos);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tScriptEditorFrame::SetStatusString(const wxString & status)
{
	StatusBar->SetStatusText(status, 2);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tScriptEditorFrame::OnCommandExecute(wxCommandEvent & event)
{
	TextCtrl->Execute();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tScriptEditorFrame::OnTextCtrlUpdateUI(wxUpdateUIEvent & event)
{
	// ステータスバーのテキストを更新する
	long pos = TextCtrl->GetInsertionPoint();
	long x, y;
	TextCtrl->PositionToXY(pos, &x, &y);
	StatusBar->SetCaretPosStatus(x, y);

	event.Skip(false);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa

