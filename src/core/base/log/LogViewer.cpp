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
#include "Log.h"
#include "LogViewer.h"
#include "risseError.h"
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
class tRisaLogScrollView : public wxVScrolledWindow
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

public:

	void AddLine(const tRisaLogger::tItem & logger_item);

	wxCoord OnGetLineHeight(size_t n) const; // override
public:
	void OnIdle(wxIdleEvent& event);
	void OnPaint(wxPaintEvent& event);
	void OnScroll(wxScrollWinEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnMotion(wxMouseEvent & event);
	void OnLeaveWindow(wxMouseEvent & event);

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
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		イベントテーブルの定義
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tRisaLogScrollView, wxVScrolledWindow)
	EVT_IDLE			(tRisaLogScrollView::OnIdle)
	EVT_PAINT			(tRisaLogScrollView::OnPaint)
	EVT_SCROLLWIN		(tRisaLogScrollView::OnScroll)
	EVT_SIZE			(tRisaLogScrollView::OnSize)
	EVT_MOTION			(tRisaLogScrollView::OnMotion)
	EVT_LEAVE_WINDOW	(tRisaLogScrollView::OnLeaveWindow)
END_EVENT_TABLE()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaLogScrollView::tRisaLogScrollView(wxWindow * parent)	:
	wxVScrolledWindow(parent),
	CurrentFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT))
{
	LineHeight = 12; // TODO: 実際の行の高さの取得
	SetBackgroundColour(GetBackgroundColour());
	Show();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaLogScrollView::~tRisaLogScrollView()
{
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
	case tRisaLogger::tItem::itDebug:	//!< デバッグに関する物
		{ static wxColour c(0xa0, 0xa0, 0xa0); colour = c; bold = false; }
		break;
	case tRisaLogger::tItem::itInfo:		//!< 通知
		{ static wxColour c(0xff, 0xff, 0xff); colour = c; bold = false; }
		break;
	case tRisaLogger::tItem::itNotice:	//!< 通常状態だが大事な情報
		{ static wxColour c(0x00, 0xff, 0x00); colour = c; bold = false; }
		break;
	case tRisaLogger::tItem::itWarning:	//!< 警告
		{ static wxColour c(0xff, 0xff, 0x00); colour = c; bold = false; }
		break;
	case tRisaLogger::tItem::itError:	//!< 通常のエラー
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
	wxPaintDC dc(this);
	CurrentFont.SetWeight((!item.Bold)?wxFONTWEIGHT_NORMAL:wxFONTWEIGHT_BOLD);
	dc.SetFont(CurrentFont);

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
		int left_pixel = widths[left_char];

		// 横幅を超えるまで(一行に収まるまで)の文字列の長さを調べる
		for(; index < logline_len; index++)
		{
			if(widths[index] - left_pixel > cw) break;
		}
		int disp_len = index - left_char - 1;
		if(disp_len < 1) disp_len = 1;
		if(left_char + disp_len > logline_len)
			disp_len = logline_len - left_char;

		if(logline_len > 0)
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
//! @brief		一行分をDisplayLinesに追加し、レイアウトを行う
//! @param		log_index  tRisaLogger::tItem 型
//! @note		このメソッドは様々なスレッドから呼ばれる可能性があるので注意
//---------------------------------------------------------------------------
void tRisaLogScrollView::AddLine(const tRisaLogger::tItem & logger_item)
{
	volatile tRisseCriticalSectionHolder holder(CS);

	wxString line = CreateOneLineString(logger_item);
	wxString link = logger_item.Link.AsWxString();
	wxColour colour;
	bool bold;
	GetFontFromLogItemType(logger_item.Type, bold, colour);

	LogicalLines.push_back(tLogicalLine(line, link, colour, bold));

	LayoutOneLine(LogicalLines.size() - 1);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定行の高さを得る(基底クラスのオーバーライド)
//! @param		n   行
//! @return		その行の高さ (この実装では固定値を返す)
//---------------------------------------------------------------------------
wxCoord tRisaLogScrollView::OnGetLineHeight(size_t n) const
{
	return LineHeight;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		描画が必要なとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaLogScrollView::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);

	// 表示行を取得
	size_t lineFirst = GetFirstVisibleLine();

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
	}

	// draw_line_first と draw_line_last 間の全ての行を表示
	wxCoord y = draw_line_first * LineHeight;
	for(risse_int n = draw_line_first; n <= draw_line_last; n++)
	{
		const tDisplayLine & displayline = DisplayLines[n + lineFirst];
		const tLogicalLine & logicalline = LogicalLines[displayline.LogicalIndex];

		// フォントをデバイスコンテキストに選択
		wxPaintDC dc(this);
		CurrentFont.SetWeight((!logicalline.Bold)?wxFONTWEIGHT_NORMAL:wxFONTWEIGHT_BOLD);
		dc.SetFont(CurrentFont);

		// 部分文字列を取り出す
		wxString substr(logicalline.Line.c_str() + displayline.CharStart,
						displayline.CharLength);

		// 文字列を描画
		dc.DrawText(substr, 0, y);
		y += LineHeight;
	}
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaLogViewerFrame::tRisaLogViewerFrame()
{
	ScrollView = new tRisaLogScrollView(this);
}
//---------------------------------------------------------------------------







