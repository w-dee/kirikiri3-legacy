//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ログビューア
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/log/Log.h"
#include "base/log/LogViewer.h"
#include "risse/include/risseError.h"
#include <stdlib.h>
#include <deque>
#include <wx/vscroll.h>

RISSE_DEFINE_SOURCE_ID(57288,52924,45855,20290,20385,24474,35332,13597);

/*
	tRisaLogger は Risa 内部でのログを保持する一方、tRisaLogViewer は
	それを表示する役割を担う。tRisaLogger は Risa 内の様々なコンポーネント
	から出力されるログを効率よく、確実に保持・記録することに特化している。
	一方、tRisaLogViewer はその内容を画面に表示するにすぎない。
	tRisaLogViewer の tRisaLogger への影響がなるべく少なくなるよう、
	tRisaLogViewer では tRisaLogger とは別にログ内容を保持するように
	する。
	また、tRisaLogViewer は非表示の場合はログ内容を保持しない。表示状態
	になったときにいったん tRisaLogger からログ内容をすべて取得し、
	あとは tRisaLogger にログが追加されれば、そのログ tRisaLogViewer も追加
	するといった同期動作を行う。再び非表示状態になれば同期動作をしなくなる。
*/


//---------------------------------------------------------------------------
//! @brief		ログビューアウィンドウ
//---------------------------------------------------------------------------
class tRisaLogScrollView : public wxPanel, public tRisaLogReceiver
{
	tRisseCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

public:
	tRisaLogScrollView(wxWindow * parent);
	~tRisaLogScrollView();

public:

private:
	void GetFontFromLogItemType(tRisaLogger::tItem::tType type,
		bool & bold, wxColour &colour);

	wxColour GetBackgroundColour();

	void LayoutOneLine(size_t buffer_offset);
	wxString CreateOneLineString(const tRisaLogger::tItem & item);
	void LayoutAllLines();

public:
	void OnLog(const tRisaLogger::tItem & logger_item); // tRisaLogReceiver の override

protected:
	void AddLine(const tRisaLogger::tItem & logger_item);


public:
//	void OnIdle(wxIdleEvent& event);
	void OnPaint(wxPaintEvent& event);
	void OnScroll(wxScrollWinEvent& event);
	void OnSize(wxSizeEvent& event);
//	void OnMotion(wxMouseEvent & event);
//	void OnLeaveWindow(wxMouseEvent & event);
	void OnChar(wxKeyEvent &event);

private:
	DECLARE_EVENT_TABLE()

private:
	struct tLogicalLine
	{
		wxString Line; //!< 行の内容
		wxString Link; //!< リンク先
		wxColour Colour; //!< 表示色
		bool Bold; //!< 太字？

		//! @brief コンストラクタ
		tLogicalLine(
			const wxString & line, const wxString & link,
			wxColour colour, bool bold)
		{
			Line = line;
			Link = link;
			Colour = colour;
			Bold = bold;
		}
	};


	//! @brief 表示行(←→論理行)を表す構造体
	struct tDisplayLine
	{
		size_t	LogicalIndex; //!< この表示行に対応する論理行
		size_t	CharStart; //!< この表示行の、物理行中の表示開始位置(wxChar単位)
		size_t	CharLength; //!< この表示行の、物理行中の表示文字長さ(wxChar単位)

		//! @brief コンストラクタ
		tDisplayLine(
			size_t logicalindex,
			size_t charstart, size_t charlength)
		{
			LogicalIndex = logicalindex;
			CharStart = charstart;
			CharLength = charlength;
		}
	};

	std::deque<tLogicalLine> LogicalLines; //!< 論理行の deque
	std::deque<tDisplayLine> DisplayLines; //!< 表示行の deque
	wxFont CurrentFont; //!< フォント
	size_t LineHeight; //!< 一行の高さ(pixel単位)

	//! @brief 論理行とその論理行中の文字位置を表すペア
	struct tCharacterPosition
	{
		size_t LogicalIndex; //!< 論理行
		size_t CharPosition; //!< 文字位置

		tCharacterPosition() { Invalidate(); }
		bool IsInvalid() { return LogicalIndex == static_cast<size_t>(-1L); }
		bool IsValid() { return !IsInvalid(); }
		void Invalidate() { LogicalIndex = static_cast<size_t>(-1L); }
	};

	tCharacterPosition SelStart; //!< 選択領域の開始位置
	tCharacterPosition SelEnd; //!< 選択領域の終了位置
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		イベントテーブルの定義
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tRisaLogScrollView, wxPanel)
//	EVT_IDLE			(tRisaLogScrollView::OnIdle)
	EVT_PAINT			(tRisaLogScrollView::OnPaint)
	EVT_SCROLLWIN		(tRisaLogScrollView::OnScroll)
	EVT_SIZE			(tRisaLogScrollView::OnSize)
//	EVT_MOTION			(tRisaLogScrollView::OnMotion)
//	EVT_LEAVE_WINDOW	(tRisaLogScrollView::OnLeaveWindow)
	EVT_CHAR			(tRisaLogScrollView::OnChar)
END_EVENT_TABLE()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaLogScrollView::tRisaLogScrollView(wxWindow * parent)	:
	wxPanel(parent, wxID_ANY,
                            wxDefaultPosition, wxDefaultSize,
                            wxSUNKEN_BORDER|wxVSCROLL),
	CurrentFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT))
{
	LineHeight = 12; // あとで実際の文字の高さに調整する
	SetBackgroundColour(GetBackgroundColour());

	SelStart.LogicalIndex = 0;
	SelStart.CharPosition = 3;
	SelEnd.LogicalIndex = 2;
	SelEnd.CharPosition = 11;

	// ウィンドウを表示する
	Show();

	// 既存のログをレイアウトする
	tRisaLogger::instance()->SendLogs(this);

	// レシーバとしてこのオブジェクトを登録する
	tRisaLogger::instance()->RegisterReceiver(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaLogScrollView::~tRisaLogScrollView()
{
	// レシーバとしてこのオブジェクトを登録解除する
	tRisaLogger::instance()->UnregisterReceiver(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ログアイテムの種別からフォントを得る
//! @param		type		ログアイテムの種別
//! @param		bold		太字にするかどうか
//! @param		colour		wxColor クラスへの参照
//---------------------------------------------------------------------------
void tRisaLogScrollView::GetFontFromLogItemType(tRisaLogger::tItem::tType type,
		bool & bold, wxColour &colour)
{
	switch(type)
	{
	case tRisaLogger::tItem::itDebug:		//!< デバッグに関する物
		{ static wxColour c(0xa0, 0xa0, 0xa0); colour = c; bold = false; }
		break;
	case tRisaLogger::tItem::itInfo:		//!< 通知
		{ static wxColour c(0xff, 0xff, 0xff); colour = c; bold = false; }
		break;
	case tRisaLogger::tItem::itNotice:		//!< 通常状態だが大事な情報
		{ static wxColour c(0x00, 0xff, 0x00); colour = c; bold = false; }
		break;
	case tRisaLogger::tItem::itWarning:		//!< 警告
		{ static wxColour c(0xff, 0xff, 0x00); colour = c; bold = false; }
		break;
	case tRisaLogger::tItem::itError:		//!< 通常のエラー
		{ static wxColour c(0xff, 0x80, 0x80); colour = c; bold = false; }
		break;
	case tRisaLogger::tItem::itCritical:	//!< 致命的なエラー
		{ static wxColour c(0xff, 0x00, 0x00); colour = c; bold = true;  }
		break;
	default:
		{ static wxColour c(0xff, 0xff, 0xff); colour = c; bold = false; }
		break;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		背景色を得る
//! @return		背景色
//---------------------------------------------------------------------------
wxColour tRisaLogScrollView::GetBackgroundColour()
{
		{ static wxColour c(0x00, 0x00, 0x00); return c; }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		一行分のレイアウトを行い、DisplayLinesに追加する
//! @param		buffer_offset  バッファのオフセット
//---------------------------------------------------------------------------
void tRisaLogScrollView::LayoutOneLine(size_t log_index)
{
	// TODO: Right to Left (like Arabic) support, complex layout support

	// buffer_offset に対応するログアイテムを得る
	const tLogicalLine & item = LogicalLines[log_index];

	// フォントをデバイスコンテキストに選択
	wxClientDC dc(this);
	CurrentFont.SetWeight((!item.Bold)?wxFONTWEIGHT_NORMAL:wxFONTWEIGHT_BOLD);
	dc.SetFont(CurrentFont);
	LineHeight = dc.GetCharHeight();

	// 各文字位置での extentを得る
	wxArrayInt widths;
	dc.GetPartialTextExtents(item.Line, widths);

	// ウィンドウの横幅を得る
	risse_int cw = 0, ch = 0;
	GetClientSize(&cw, &ch);

	// 横幅位置でloglineを区切る
	const wxString & logline = item.Line;
	int logline_len = logline.Length();
	int index = 0;
	while(index < logline_len)
	{
		int left_char = index;
		int left_pixel = left_char == 0 ? 0 : widths[left_char - 1];

		// 横幅を超えるまで(一行に収まらなくなるまで)の文字列の長さを調べる
		index ++;
		while(index < logline_len)
		{
			if(widths[index] - left_pixel > cw) break;
			index ++;
		}

		int disp_len = index - left_char;
		if(disp_len < 1) disp_len = 1;
		if(left_char + disp_len > logline_len)
			disp_len = logline_len - left_char;

		if(disp_len > 0)
		{
			// その横幅に収まる文字で表示行を作る
			DisplayLines.push_back(tDisplayLine(log_index, left_char, disp_len));
		}

		index = left_char + disp_len;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		一行分の表示内容を作成する
//! @param		item  ログアイテム
//! @return		表示内容
//---------------------------------------------------------------------------
wxString tRisaLogScrollView::CreateOneLineString(const tRisaLogger::tItem & item)
{
	// 表示形式は
	// 時刻 ログレベル メッセージ

	// 時刻
	wxString logline = item.Timestamp.FormatTime() + wxT(" ");

	// ログレベル
	switch(item.Type)
	{
	case tRisaLogger::tItem::itDebug:
		logline += _("[D] "); break; // [D]ebug
	case tRisaLogger::tItem::itInfo:
		logline += _("[I] "); break; // [I]nformation
	case tRisaLogger::tItem::itNotice:
		logline += _("[N] "); break; // [N]otice
	case tRisaLogger::tItem::itWarning:
		logline += _("[W] "); break; // [W]arning
	case tRisaLogger::tItem::itError:
		logline += _("[E] "); break; // [E]rror
	case tRisaLogger::tItem::itCritical:
		logline += _("[C] "); break; // [C]ritical
	}

	// メッセージ
	logline += item.Content.AsWxString();

	return logline;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		全てのログの行のレイアウトをし直す
//---------------------------------------------------------------------------
void tRisaLogScrollView::LayoutAllLines()
{
	volatile tRisseCriticalSection::tLocker holder(CS);

	// DisplayLines はいったん全てクリア
	size_t num_disp_lines_before = DisplayLines.size();
	DisplayLines.clear();

	// 全ての論理行に対してレイアウトを行う
	size_t num_log_lines = LogicalLines.size();
	for(size_t n = 0; n < num_log_lines; n++)
		LayoutOneLine(n);

	// 全体の行数を設定
	// スクロールバーの調整
	size_t num_disp_lines = DisplayLines.size();
	risse_int cw = 0, ch = 0;
	GetClientSize(&cw, &ch);
	size_t lines_per_window = ch / LineHeight;

	risse_int new_top = num_disp_lines > lines_per_window ? num_disp_lines - lines_per_window : 0;

	SetScrollbar(wxVERTICAL, new_top, lines_per_window - 1, LogicalLines.size() + 1);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tRisaLogReceiver::OnLog のオーバーライド
//! @param		logger_item  tRisaLogger::tItem 型
//---------------------------------------------------------------------------
void tRisaLogScrollView::OnLog(const tRisaLogger::tItem & logger_item)
{
	AddLine(logger_item);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		一行分をDisplayLinesに追加し、レイアウトを行う
//! @param		logger_item  tRisaLogger::tItem 型
//! @note		このメソッドは様々なスレッドから呼ばれる可能性があるので注意
//---------------------------------------------------------------------------
void tRisaLogScrollView::AddLine(const tRisaLogger::tItem & logger_item)
{
	volatile tRisseCriticalSection::tLocker holder(CS);

	wxString line = CreateOneLineString(logger_item);
	wxString link = logger_item.Link.AsWxString();
	wxColour colour;
	bool bold;
	GetFontFromLogItemType(logger_item.Type, bold, colour);

	LogicalLines.push_back(tLogicalLine(line, link, colour, bold));

	LayoutOneLine(LogicalLines.size() - 1);

	// スクロールバーの調整
	risse_int old_top = GetScrollPos(wxVERTICAL);

	size_t num_disp_lines = DisplayLines.size();
	risse_int cw = 0, ch = 0;
	GetClientSize(&cw, &ch);
	size_t lines_per_window = ch / LineHeight;

	risse_int new_top = num_disp_lines > lines_per_window ? num_disp_lines - lines_per_window : 0;

	SetScrollbar(wxVERTICAL, new_top, lines_per_window - 1, LogicalLines.size() + 1);

	if(old_top != new_top)
	{
		// スクロールを行う
		ScrollWindow(0, (old_top - new_top) * LineHeight);
		Refresh();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		描画が必要なとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaLogScrollView::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);

	volatile tRisseCriticalSection::tLocker holder(CS);

	// 表示行を取得
	size_t lineFirst = GetScrollPos(wxVERTICAL);

	// 更新矩形を取得
	wxRegionIterator upd(GetUpdateRegion()); // get the update rect list

	// 更新矩形に影響している行の範囲を取得
	risse_int draw_line_first = -1;
	risse_int draw_line_last  = -1;
	while (upd)
	{
		wxRect rect(upd.GetRect());

		// rect が影響している行を計算
		risse_int f = rect.GetTop() / LineHeight;
		risse_int l = (rect.GetBottom()-1) / LineHeight;

		// rect が影響している行と draw_line_first, draw_line_last を比較
		if(draw_line_first == -1)
			draw_line_first = f;
		else
			if(draw_line_first > f) draw_line_first = f;

		if(draw_line_last == -1)
			draw_line_last = l;
		else
			if(draw_line_last < f) draw_line_last = f;

		upd ++;
	}

	// draw_line_first と draw_line_last 間の全ての行を表示
	if(draw_line_first != -1 && draw_line_last != -1)
	{
		wxCoord y = draw_line_first * LineHeight;
		for(risse_int n = draw_line_first; n <= draw_line_last; n++)
		{
			if(n+lineFirst < 0 ||
				static_cast<size_t>(n+lineFirst) >= DisplayLines.size())
			{
				// 配列の範囲外
				continue;
			}

			const tDisplayLine & displayline =
					DisplayLines[n + lineFirst];
			const tLogicalLine & logicalline =
					LogicalLines[displayline.LogicalIndex];

			// フォントをデバイスコンテキストに選択
			CurrentFont.SetWeight((!logicalline.Bold)?wxFONTWEIGHT_NORMAL:wxFONTWEIGHT_BOLD);
			dc.SetTextForeground(logicalline.Colour);
			dc.SetFont(CurrentFont);

			// 部分文字列を取り出す
			wxString substr(logicalline.Line.c_str() + displayline.CharStart,
							displayline.CharLength);

			// 文字列を描画
			dc.DrawText(substr, 0, y);

			// 選択範囲がある場合
			if(SelStart.IsValid() &&
				displayline.LogicalIndex >= SelStart.LogicalIndex &&
				displayline.LogicalIndex <= SelEnd.LogicalIndex)
			{
				// 反転を行う範囲を探す
				size_t inv_start = displayline.CharStart;
				size_t inv_end   = displayline.CharStart + displayline.CharLength;
				if(displayline.LogicalIndex == SelStart.LogicalIndex)
					if(inv_start < SelStart.CharPosition)
						inv_start = SelStart.CharPosition;
				if(displayline.LogicalIndex == SelEnd.LogicalIndex)
					if(inv_end > SelEnd.CharPosition)
						inv_end = SelEnd.CharPosition;

				if(inv_end > inv_start)
				{
					inv_start -= displayline.CharStart;
					inv_end   -= displayline.CharStart;

					// 反転を行う範囲をピクセル単位に変換する
					wxArrayInt widths;
					dc.GetPartialTextExtents(substr, widths);
					int startx = inv_start == 0 ? 0 : widths[inv_start - 1];
					int endx   = inv_end   == 0 ? 0 : widths[inv_end   - 1];

					// 範囲を反転する
					dc.SetLogicalFunction(wxINVERT);
					dc.DrawRectangle(startx, y, endx - startx, LineHeight);
					dc.SetLogicalFunction(wxCOPY);
				}
			}

			// y 位置を下に
			y += LineHeight;
		}
	}

	event.Skip();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スクロールされたとき
//---------------------------------------------------------------------------
void tRisaLogScrollView::OnScroll(wxScrollWinEvent& event)
{
	WXTYPE ev_type = event.GetEventType();
	if(ev_type == wxEVT_SCROLLWIN_THUMBRELEASE || ev_type == wxEVT_SCROLLWIN_THUMBTRACK)
	{
		int pos = event.GetPosition();
		SetScrollPos(wxVERTICAL, pos);
		Refresh();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		サイズが変更されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaLogScrollView::OnSize(wxSizeEvent& event)
{
	LayoutAllLines();
	Refresh();

	event.Skip();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		キーが押されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaLogScrollView::OnChar(wxKeyEvent &event)
{
	// テスト
	// ランダムな文字列をランダムな長さで作成

	size_t len = rand() % 50 + 10;
	risse_char buf[100];
	for(size_t i = 0; i < len; i++)
		buf[i] = rand() % 27 + 'A';
	buf[len] = 0;

	tRisaLogger::instance()->Log(buf);
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief		イベントテーブルの定義
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tRisaLogViewerFrame, wxFrame)
END_EVENT_TABLE()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaLogViewerFrame::tRisaLogViewerFrame() :
	wxFrame(NULL, wxID_ANY, _("Console"))
{
	ScrollView = new tRisaLogScrollView(this);
}
//---------------------------------------------------------------------------







