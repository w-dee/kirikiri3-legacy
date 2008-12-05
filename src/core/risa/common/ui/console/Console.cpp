//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief コンソールフレーム
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/log/Log.h"
#include "risa/common/ui/console/LogViewer.h"
#include "risa/common/ui/console/Console.h"
#include "risa/common/RisseEngine.h"
#include "risa/common/ConfigData.h"
#include <stdlib.h>
#include <deque>
#include <wx/textctrl.h>
#include <wx/artprov.h>


namespace Risa {
RISSE_DEFINE_SOURCE_ID(42206,11515,36168,20323,34721,49407,49922,37589);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * コンソール用のヒストリ付きカスタムテキストコントロール
 */
class tHistoryTextCtrl :  public wxTextCtrl, protected depends_on<tConfig>
{
	const static size_t MaxNumHistoryItems = 100; //!< ヒストリの最大個数
	const static size_t InvalidIndex = static_cast<size_t>(-1L); //!< 無効なインデックス値
	std::deque<wxString> History; //!< ヒストリ(先頭が新、最後が旧)
	size_t HistoryIndex; //!< 現在選択中のヒストリインデックス

public:
	tHistoryTextCtrl(wxWindow *parent);
	~tHistoryTextCtrl();

private:
	void WriteConfig();

	void PushHistory(const wxString & item);

	void OnEnter(wxCommandEvent & event);
	void OnChar(wxKeyEvent & event);

	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * イベントテーブル
 */
BEGIN_EVENT_TABLE(tHistoryTextCtrl, wxTextCtrl)
	EVT_TEXT_ENTER(wxID_ANY,	tHistoryTextCtrl::OnEnter)
	EVT_CHAR(					tHistoryTextCtrl::OnChar)
END_EVENT_TABLE()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * コンストラクタ
 * @param parent	親ウィンドウ
 */
tHistoryTextCtrl::tHistoryTextCtrl(wxWindow *parent):
	wxTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
		wxTE_PROCESS_ENTER)
{
	HistoryIndex = InvalidIndex;
	DiscardEdits();

	// ヒストリなどを設定情報から読み出す
	tConfigData & config = tConfig::instance()->GetVariableConfig();
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
/**
 * デストラクタ
 */
tHistoryTextCtrl::~tHistoryTextCtrl()
{
	// ヒストリなどを設定情報に書き出す
	WriteConfig();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * ヒストリを設定情報に書き出す
 */
void tHistoryTextCtrl::WriteConfig()
{
	tConfigData & config = tConfig::instance()->GetVariableConfig();

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
/**
 * 式をヒストリに入れる
 * @param item	式
 */
void tHistoryTextCtrl::PushHistory(const wxString & item)
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
/**
 * Enterキーが押されたとき
 * @param event	イベントオブジェクト
 */
void tHistoryTextCtrl::OnEnter(wxCommandEvent & event)
{
	// ヒストリの内容をファイルに保存する
	WriteConfig();
	tConfig::instance()->GetVariableConfig().Flush();

	// Risse に式を評価させ、結果をコンソールに表示する
	wxString value = event.GetString();

	tRisseScriptEngine::instance()->
		EvaluateExpresisonAndPrintResultToConsole(tString(value));

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
/**
 * 文字が入力されたとき
 * @param event	イベントオブジェクト
 */
void tHistoryTextCtrl::OnChar(wxKeyEvent & event)
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
		{
			// ログビューアにフォーカスを合わせる
			wxWindow * top = GetParent();
			while(top->GetParent()) top = top->GetParent(); // トップレベルウィンドウを探す
			static_cast<tConsoleFrame*>(top)->SetFocusToLogViewer();
			break;
		}

		default:
			event.Skip(true);
			return;
	}

	event.Skip(false);
}
//---------------------------------------------------------------------------
























//---------------------------------------------------------------------------
/**
 * コンソール用のカスタムステータスバー
 */
class tLogViewerStatusBar : public wxStatusBar
{
	tHistoryTextCtrl * TextCtrl; //!< テキストコントロール

public:
	tLogViewerStatusBar(wxWindow *parent);
	virtual ~tLogViewerStatusBar();

	void FocusToTextCtrl(int insert_code);

private:
	void AdjustControlSize();

	void OnSize(wxSizeEvent& event);
	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * ログビューア用のカスタムステータスバー用のイベントテーブル
 */
BEGIN_EVENT_TABLE(tLogViewerStatusBar, wxStatusBar)
	EVT_SIZE(					tLogViewerStatusBar::OnSize)
END_EVENT_TABLE()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * コンストラクタ
 * @param parent	親ウィンドウ
 */
tLogViewerStatusBar::tLogViewerStatusBar(wxWindow *parent)
		   : wxStatusBar(parent, wxID_ANY)
{
	SetFieldsCount(1);

	wxClientDC dc(this);
	int lineheight = dc.GetCharHeight();

	TextCtrl = new tHistoryTextCtrl(this);
	SetSize(-1, -1, -1, lineheight + 13); // XXX 13?

	AdjustControlSize();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * デストラクタ
 */
tLogViewerStatusBar::~tLogViewerStatusBar()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * テキストコントロールにフォーカスを合わせる
 * @param insert_code	テキストコントロールにフォーカスを合わせた際に
 *						押されたキー (必要ならばこれを挿入する)
 */
void tLogViewerStatusBar::FocusToTextCtrl(int insert_code)
{
	TextCtrl->SetFocus();
	if(insert_code >= 0x20)
		TextCtrl->WriteText(wxString(static_cast<wxChar>(insert_code)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * コントロールのサイズを調整する
 */
void tLogViewerStatusBar::AdjustControlSize()
{
	wxRect rect;
	GetFieldRect(0, rect);

	TextCtrl->SetSize(rect.x +1, rect.y +1, rect.width - 4, rect.height - 2);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * サイズが変更されたとき
 * @param event	イベントオブジェクト
 */
void tLogViewerStatusBar::OnSize(wxSizeEvent& event)
{
	AdjustControlSize();

	event.Skip();
}
//---------------------------------------------------------------------------
























//---------------------------------------------------------------------------
// イベントが配信可能かどうかが変化したことを伝えるイベントタイプを定義
//---------------------------------------------------------------------------
DECLARE_EVENT_TYPE(wxEVT_EVENT_STATUS_CHANGED, -1)
DEFINE_EVENT_TYPE(wxEVT_EVENT_STATUS_CHANGED)
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tConsoleFrame, tUIFrame)
	EVT_COMMAND(wxID_ANY,			wxEVT_EVENT_STATUS_CHANGED, tConsoleFrame::OnEventStatusChanged)
	EVT_TOOL(ID_Event,				tConsoleFrame::OnEventTool)
END_EVENT_TABLE()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tConsoleFrame::tConsoleFrame() :
	tUIFrame(wxT("ui/console"), _("Console"))
{
	// ツールバーを追加
	CreateToolBar();
	GetToolBar()->AddCheckTool(ID_Event, _("Deliver events"),
		wxArtProvider::GetBitmap(wxT("Event"), wxART_TOOLBAR),
		wxNullBitmap, _("Whether to deliver events"));
	GetToolBar()->Realize();

	// ログビューアを追加
	ScrollView = new tLogScrollView(this);

	// ステータスバーを追加
	StatusBar = new tLogViewerStatusBar(this);
	SetStatusBar(StatusBar);

	// ステータスバー内のテキストコントロールにフォーカスを設定
	SetFocusToTextCtrl();

	// イベント状態を反映
	GetToolBar()->ToggleTool(ID_Event,
		tEventSystem::instance()->GetCanDeliverEvents());

	// イベント状態リスナに登録
	tEventSystem::instance()->GetStateListeners().add(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tConsoleFrame::~tConsoleFrame()
{
	// イベント状態リスナから削除
	tEventSystem::instance()->GetStateListeners().remove(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tConsoleFrame::SetFocusToLogViewer()
{
	ScrollView->SetFocus();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tConsoleFrame::SetFocusToTextCtrl(int insert_code)
{
	StatusBar->FocusToTextCtrl(insert_code);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tConsoleFrame::OnCanDeliverEventsChanged(bool b)
{
	// ここは様々なスレッドから呼ばれるが、UI の処理は UI 用の
	// スレッドで行わなければならない。ここでは UI の状態を更新するために
	// いったん UI 用のイベントキューにイベントを post する
	wxCommandEvent evt(wxEVT_EVENT_STATUS_CHANGED);
	GetEventHandler()->AddPendingEvent(evt);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tConsoleFrame::OnEventTool(wxCommandEvent & event)
{
	bool event_enabled = GetToolBar()->GetToolState(ID_Event);
	tEventSystem::instance()->SetCanDeliverEvents(event_enabled);
	// これは OnCanDeliverEventsChanged を呼び、結果的に後に
	// OnEventStatusChanged も呼ぶことになるが、おそらく問題ない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tConsoleFrame::OnEventStatusChanged(wxCommandEvent & event)
{
	// イベントの状態が変化した場合に、OnCanDeliverEventsChanged が Post した
	// イベントに呼応して呼ばれる
	GetToolBar()->ToggleTool(ID_Event,
		tEventSystem::instance()->GetCanDeliverEvents());
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa


