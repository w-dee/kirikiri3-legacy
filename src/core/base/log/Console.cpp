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
#include "prec.h"
#include "base/log/Log.h"
#include "base/log/LogViewer.h"
#include "base/log/Console.h"
#include "base/script/RisseEngine.h"
#include <stdlib.h>
#include <deque>
#include <wx/textctrl.h>

RISSE_DEFINE_SOURCE_ID(42206,11515,36168,20323,34721,49407,49922,37589);


//---------------------------------------------------------------------------
//! @brief		ログビューア用のカスタムステータスバー
//---------------------------------------------------------------------------
class tRisaLogViewerStatusBar : public wxStatusBar
{
	enum
	{
		ID_TextCtrl = 1
	};

	wxTextCtrl * TextCtrl;

public:
	tRisaLogViewerStatusBar(wxWindow *parent);
	virtual ~tRisaLogViewerStatusBar();

private:
	void AdjustControlSize();


	void OnSize(wxSizeEvent& event);
	void OnTextCtrlEnter(wxCommandEvent & event);
	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ログビューア用のカスタムステータスバー用のイベントテーブル
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tRisaLogViewerStatusBar, wxStatusBar)
	EVT_SIZE(					tRisaLogViewerStatusBar::OnSize)
	EVT_TEXT_ENTER(ID_TextCtrl,	tRisaLogViewerStatusBar::OnTextCtrlEnter)
END_EVENT_TABLE()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaLogViewerStatusBar::tRisaLogViewerStatusBar(wxWindow *parent)
		   : wxStatusBar(parent, wxID_ANY)
{
	SetFieldsCount(1);

	TextCtrl = new wxTextCtrl(
		this, ID_TextCtrl, wxEmptyString, wxDefaultPosition, wxDefaultSize,
		wxTE_PROCESS_ENTER);

    SetMinHeight(TextCtrl->GetSize().GetWidth() + 4);

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
//! @brief		テキストコントロールでEnterキーが押されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaLogViewerStatusBar::OnTextCtrlEnter(wxCommandEvent & event)
{
	// Risse に式を評価させ、結果をコンソールに表示する
	tRisaRisseScriptEngine::instance()->
		EvalExpresisonAndPrintResultToConsole(ttstr(event.GetString()));

	// これ以上イベントを処理しないように ...
	event.Skip(false);
}
//---------------------------------------------------------------------------




























//---------------------------------------------------------------------------
//! @brief		イベントテーブルの定義
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tRisaConsoleFrame, wxFrame)
END_EVENT_TABLE()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaConsoleFrame::tRisaConsoleFrame() :
	wxFrame(NULL, wxID_ANY, _("Console"))
{
	ScrollView = new tRisaLogScrollView(this);

	SetStatusBar(new tRisaLogViewerStatusBar(this));
}
//---------------------------------------------------------------------------








