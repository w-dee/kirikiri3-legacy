//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief コンソールフレーム
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/log/Log.h"
#include "base/ui/console/LogViewer.h"
#include "base/ui/console/Console.h"
#include "base/script/RisseEngine.h"
#include "base/config/ConfigData.h"
#include <stdlib.h>
#include <deque>
#include <wx/textctrl.h>
#include <wx/artprov.h>

RISSE_DEFINE_SOURCE_ID(42206,11515,36168,20323,34721,49407,49922,37589);



//---------------------------------------------------------------------------
//! @brief		コンソール用のヒストリ付きカスタムテキストコントロール
//---------------------------------------------------------------------------
class tRisaHistoryTextCtrl :  public wxTextCtrl, protected depends_on<tRisaConfig>
{
	const static size_t MaxNumHistoryItems = 100; //!< ヒストリの最大個数
	const static size_t InvalidIndex = static_cast<size_t>(-1L); //!< 無効なインデックス値
	std::deque<wxString> History; //!< ヒストリ(先頭が新、最後が旧)
	size_t HistoryIndex; //!< 現在選択中のヒストリインデックス

public:
	tRisaHistoryTextCtrl(wxWindow *parent);
	~tRisaHistoryTextCtrl();

private:
	void WriteConfig();

	void PushHistory(const wxString & item);

	void OnEnter(wxCommandEvent & event);
	void OnChar(wxKeyEvent & event);

	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		イベントテーブル
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tRisaHistoryTextCtrl, wxTextCtrl)
	EVT_TEXT_ENTER(wxID_ANY,	tRisaHistoryTextCtrl::OnEnter)
	EVT_CHAR(					tRisaHistoryTextCtrl::OnChar)
END_EVENT_TABLE()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		parent 親ウィンドウ
//---------------------------------------------------------------------------
tRisaHistoryTextCtrl::tRisaHistoryTextCtrl(wxWindow *parent):
	wxTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
		wxTE_PROCESS_ENTER)
{
	HistoryIndex = InvalidIndex;
	DiscardEdits();

	// ヒストリなどを設定情報から読み出す
	tRisaConfigData & config = tRisaConfig::instance()->GetVariableConfig();
	for(int cnt = 0;;cnt++)
	{
		wxString item;
		if(config.Read(wxT("ui/console/history/") + wxString::Format(wxT("%d"), cnt),
			&item))
			History.push_back(item);
		else
			break;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaHistoryTextCtrl::~tRisaHistoryTextCtrl()
{
	// ヒストリなどを設定情報に書き出す
	WriteConfig();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ヒストリを設定情報に書き出す
//---------------------------------------------------------------------------
void tRisaHistoryTextCtrl::WriteConfig()
{
	tRisaConfigData & config = tRisaConfig::instance()->GetVariableConfig();

	config.DeleteGroup(wxT("ui/console/history"));
	int cnt = 0;
	for(std::deque<wxString>::iterator i = History.begin();
		i != History.end(); i++, cnt++)
	{
		config.Write(wxT("ui/console/history/") + wxString::Format(wxT("%d"), cnt), *i);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		式をヒストリに入れる
//! @param		item	式
//---------------------------------------------------------------------------
void tRisaHistoryTextCtrl::PushHistory(const wxString & item)
{
	History.push_front(item);

	if(History.size() > MaxNumHistoryItems)
	{
		// ヒストリの最大保持個数を超えているので最後を chop
		History.erase(History.begin() + MaxNumHistoryItems, History.end());
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Enterキーが押されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaHistoryTextCtrl::OnEnter(wxCommandEvent & event)
{
	// ヒストリの内容をファイルに保存する
	WriteConfig();
	tRisaConfig::instance()->GetVariableConfig().Flush();

	// Risse に式を評価させ、結果をコンソールに表示する
	wxString value = event.GetString();

	tRisaRisseScriptEngine::instance()->
		EvaluateExpresisonAndPrintResultToConsole(tRisseString(value));

	// 入力をヒストリに入れる
	if(History.size() == 0 || History[0] != value)
		PushHistory(value);

	// テキストを空にする
	SetValue(wxEmptyString);
	HistoryIndex = InvalidIndex;
	DiscardEdits();

	// これ以上イベントを処理しないように ...
	event.Skip(false);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字が入力されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaHistoryTextCtrl::OnChar(wxKeyEvent & event)
{
	switch ( event.GetKeyCode() )
	{
		case WXK_UP:  // 上キー
		{
			if(IsModified())
			{
				// もし内容が変更されていたらそれを最新位置に挿入する
				PushHistory(GetValue());
				HistoryIndex = 0;
			}

			if(HistoryIndex != InvalidIndex &&
				HistoryIndex + 1 >= History.size())
				break; // もうこれ以上古いヒストリがない
			if(HistoryIndex == InvalidIndex &&
				History.size() == 0)
				break; // もうこれ以上古いヒストリがない

			if(HistoryIndex == InvalidIndex)
				HistoryIndex = 0;
			else
				HistoryIndex++;

			// ヒストリをコントロールに入れる
			SetValue(History[HistoryIndex]);
			DiscardEdits();
			SetInsertionPointEnd();
			break;
		}

		case WXK_DOWN: // 下キー
		{
			if(IsModified())
			{
				// もし内容が変更されていたらそれを最新位置に挿入する
				PushHistory(GetValue());
				HistoryIndex = 0;
			}
			else
			{
				if(HistoryIndex == InvalidIndex)
					break; // これ以上新しいヒストリがない
			}

			wxString newvalue;
			if(HistoryIndex > 0)
			{
				HistoryIndex --;
				newvalue = History[HistoryIndex];
			}
			else
			{
				HistoryIndex = InvalidIndex;
			}

			// ヒストリをコントロールに入れる
			SetValue(newvalue);
			DiscardEdits();
			SetInsertionPointEnd();
			break;
		}

		case WXK_TAB: // TAB
			// ログビューアにフォーカスを合わせる
			wxWindow * top = GetParent();
			while(top->GetParent()) top = top->GetParent(); // トップレベルウィンドウを探す
			reinterpret_cast<tRisaConsoleFrame*>(top)->SetFocusToLogViewer();
			break;

		default:
			event.Skip(true);
			return;
	}

	event.Skip(false);
}
//---------------------------------------------------------------------------
























//---------------------------------------------------------------------------
//! @brief		コンソール用のカスタムステータスバー
//---------------------------------------------------------------------------
class tRisaLogViewerStatusBar : public wxStatusBar
{
	tRisaHistoryTextCtrl * TextCtrl; //!< テキストコントロール

public:
	tRisaLogViewerStatusBar(wxWindow *parent);
	virtual ~tRisaLogViewerStatusBar();

	void FocusToTextCtrl(int insert_code);

private:
	void AdjustControlSize();

	void OnSize(wxSizeEvent& event);
	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ログビューア用のカスタムステータスバー用のイベントテーブル
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tRisaLogViewerStatusBar, wxStatusBar)
	EVT_SIZE(					tRisaLogViewerStatusBar::OnSize)
END_EVENT_TABLE()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		parent 親ウィンドウ
//---------------------------------------------------------------------------
tRisaLogViewerStatusBar::tRisaLogViewerStatusBar(wxWindow *parent)
		   : wxStatusBar(parent, wxID_ANY)
{
	SetFieldsCount(1);

	wxClientDC dc(this);
	int lineheight = dc.GetCharHeight();

	TextCtrl = new tRisaHistoryTextCtrl(this);
	SetSize(-1, -1, -1, lineheight + 10); // XXX 10?

	AdjustControlSize();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaLogViewerStatusBar::~tRisaLogViewerStatusBar()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		テキストコントロールにフォーカスを合わせる
//! @param		insert_code		テキストコントロールにフォーカスを合わせた際に
//!								押されたキー (必要ならばこれを挿入する)
//---------------------------------------------------------------------------
void tRisaLogViewerStatusBar::FocusToTextCtrl(int insert_code)
{
	TextCtrl->SetFocus();
	if(insert_code >= 0x20)
		TextCtrl->WriteText(wxString(static_cast<wxChar>(insert_code)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コントロールのサイズを調整する
//---------------------------------------------------------------------------
void tRisaLogViewerStatusBar::AdjustControlSize()
{
	wxRect rect;
	GetFieldRect(0, rect);

	TextCtrl->SetSize(rect.x +2, rect.y +2, rect.width - 4, rect.height - 4);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		サイズが変更されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaLogViewerStatusBar::OnSize(wxSizeEvent& event)
{
	AdjustControlSize();

	event.Skip();
}
//---------------------------------------------------------------------------




























//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tRisaConsoleFrame, tRisaUIFrame)
	EVT_TOOL(ID_Event,				tRisaConsoleFrame::OnEventTool)
	EVT_UPDATE_UI(wxID_ANY,			tRisaConsoleFrame::OnUpdateUI)
END_EVENT_TABLE()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaConsoleFrame::tRisaConsoleFrame() :
	tRisaUIFrame(wxT("ui/console"), _("Console"))
{
	// UI アップデートイベントの受け取り
	SetExtraStyle(GetExtraStyle()|wxWS_EX_PROCESS_UI_UPDATES);

	// ツールバーを追加
	CreateToolBar();
	GetToolBar()->AddCheckTool(ID_Event, _("Deliver events"),
		wxArtProvider::GetBitmap(wxT("RisaEvent"), wxART_TOOLBAR),
		wxNullBitmap, _("Whether to deliver events"));
	GetToolBar()->Realize();

	// ログビューアを追加
	ScrollView = new tRisaLogScrollView(this);

	// ステータスバーを追加
	StatusBar = new tRisaLogViewerStatusBar(this);
	SetStatusBar(StatusBar);

	// ステータスバー内のテキストコントロールにフォーカスを設定
	SetFocusToTextCtrl();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaConsoleFrame::SetFocusToLogViewer()
{
	ScrollView->SetFocus();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaConsoleFrame::SetFocusToTextCtrl(int insert_code)
{
	StatusBar->FocusToTextCtrl(insert_code);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaConsoleFrame::OnEventTool(wxCommandEvent & event)
{
/*
	TODO: handle this
	bool event_enabled = GetToolBar()->GetToolState(ID_Event);
	tRisaEventSystem::instance()->SetCanDeliverEvents(event_enabled);
*/
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaConsoleFrame::OnUpdateUI(wxUpdateUIEvent & event)
{
/*
	TODO: handle this
	// "Event" ボタンの状態を更新
	bool event_enabled = GetToolBar()->GetToolState(ID_Event);
	if(event_enabled != tRisaEventSystem::instance()->GetCanDeliverEvents())
	{
		GetToolBar()->ToggleTool(ID_Event,
			tRisaEventSystem::instance()->GetCanDeliverEvents());
	}

	event.Skip(false);
*/
}
//---------------------------------------------------------------------------


