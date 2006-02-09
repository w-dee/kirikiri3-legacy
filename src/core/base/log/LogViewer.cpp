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

#ifdef wxUSE_DRAG_AND_DROP
#include <wx/dataobj.h>
#include <wx/dnd.h>
#endif

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
	あとは tRisaLogger にログが追加されれば、tRisaLogViewer もそのログを追加
	するといった同期動作を行う。再び非表示状態になれば同期動作をしなくなる。
*/


//---------------------------------------------------------------------------
//! @brief		ログビューアウィンドウ
//---------------------------------------------------------------------------
class tRisaLogScrollView : public wxPanel, public tRisaLogReceiver
{
	static const size_t RotateLimit = 2100; //!< この論理行数を超えるとローテーションを行う
	static const size_t RotateTo    = 2000; //!< 一回のローテーションではこの論理行数までにログを減らす

	tRisseCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

	struct tLogicalLine
	{
		wxString Line; //!< 行の内容
		wxString Link; //!< リンク先
		wxColour Colour; //!< 表示色
		size_t DisplayIndex; //!< この論理行に対応する表示行(の1行目)
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
			DisplayIndex = 0;
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
	size_t LinesPerWindow; //!< 一画面中の有効な行数(半端な行を含まず)
	size_t ViewWidth; //!< ビューのサイズ(横)
	size_t ViewHeight; //!< ビューのサイズ(縦)
	int ViewOriginX; //!< ビューのオフセット

	//! @brief 論理/表示行とその論理行中の文字位置を表すペア
	struct tCharacterPosition
	{
		union
		{
			size_t LogicalIndex; //!< 論理行
			size_t DisplayIndex; //!< 表示行
		};
		size_t CharPosition; //!< 文字位置

		tCharacterPosition() { Invalidate(); }
		bool IsInvalid() const { return LogicalIndex == static_cast<size_t>(-1L); }
		bool IsValid() const { return !IsInvalid(); }
		void Invalidate() { LogicalIndex = static_cast<size_t>(-1L); }

		bool operator < (const tCharacterPosition & rhs) const
		{
			if(LogicalIndex < rhs.LogicalIndex) return true;
			if(LogicalIndex > rhs.LogicalIndex) return false;
			if(CharPosition < rhs.CharPosition) return true;
			return false;
		}
		bool operator > (const tCharacterPosition & rhs) const
		{
			return rhs < *this;
		}
		bool operator == (const tCharacterPosition & rhs) const
		{
			return LogicalIndex == rhs.LogicalIndex &&
					CharPosition == rhs.CharPosition;
		}
		bool operator != (const tCharacterPosition & rhs) const
		{
			return !(*this == rhs);
		}
	};

	tCharacterPosition SelStart; //!< 選択領域の開始位置
	tCharacterPosition SelEnd; //!< 選択領域の終了位置
	bool MouseSelecting; //!< 範囲選択中は真
	tCharacterPosition MouseSelStart1; //!< マウスでの選択開始位置1
	tCharacterPosition MouseSelStart2; //!< マウスでの選択開始位置1

	//! @brief マウスで選択中にスクロールを行うためのタイマークラス
	class tScrollTimer : public wxTimer
	{
		tRisaLogScrollView * View;
	public:
		tScrollTimer(tRisaLogScrollView * view)
		{
			View = view;
			Start(100);
		}

	private:
		void Notify() // override
		{
			View->ScrollByTimer();
		}
	};
	tScrollTimer * ScrollTimer; //!< マウスで選択中にスクロールを行うためのタイマー
	int ScrollTimerScrollAmount; //!< ScrollTimer でスクロールすべき量

public:
	tRisaLogScrollView(wxWindow * parent);
	~tRisaLogScrollView();

private:
	void GetFontFromLogItemType(tRisaLogger::tType type,
		bool & bold, wxColour &colour);

	wxColour GetBackgroundColour();

	void Rotate();

	void LayoutOneLine(size_t buffer_offset);
	wxString CreateOneLineString(const tRisaLogger::tItem & item);
	void LayoutAllLines();
	void AddLine(const tRisaLogger::tItem & logger_item);

	void DoScroll(risse_int amount, bool absolute);
	void SetScrollBarInfo(risse_int anchor);
	risse_int GetScrollAnchor();
	void ScrollView(risse_int old_top, risse_int new_top);
	void RefreshDisplayLine(size_t index, risse_int range);

	void ViewPositionToCharacterPosition(risse_int x, risse_int y, tCharacterPosition & charpos, size_t *charlength = NULL);
	bool IsViewPositionInSelection(risse_int x, risse_int y);
	wxString GetSelectionString();
	void SetSelection(const tCharacterPosition & pos1, const tCharacterPosition & pos2, const tCharacterPosition & pos3);
	void RefreshSelection(const tCharacterPosition & pos1, const tCharacterPosition & pos2);

	void LogicalPositionToDisplayPosition(const tCharacterPosition & log_pos,
				tCharacterPosition & disp_pos);

	void ScrollByTimer();

public:
	void OnLog(const tRisaLogger::tItem & logger_item); // tRisaLogReceiver の override

private:
//	void OnIdle(wxIdleEvent& event);
	void OnPaint(wxPaintEvent& event);
	void OnScroll(wxScrollWinEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnLeftDown(wxMouseEvent & event);
	void OnLeftUp(wxMouseEvent & event);
	void OnMotion(wxMouseEvent & event);
//	void OnLeaveWindow(wxMouseEvent & event);
	void OnChar(wxKeyEvent &event);

	DECLARE_EVENT_TABLE()

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		イベントテーブルの定義
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tRisaLogScrollView, wxPanel)
//	EVT_IDLE			(tRisaLogScrollView::OnIdle)
	EVT_PAINT			(tRisaLogScrollView::OnPaint)
	EVT_SCROLLWIN		(tRisaLogScrollView::OnScroll)
	EVT_MOUSEWHEEL		(tRisaLogScrollView::OnMouseWheel) 
	EVT_SIZE			(tRisaLogScrollView::OnSize)
	EVT_LEFT_DOWN		(tRisaLogScrollView::OnLeftDown)
	EVT_LEFT_UP			(tRisaLogScrollView::OnLeftUp)
	EVT_MOTION			(tRisaLogScrollView::OnMotion)
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

	ViewWidth = 100; // あとで実際の文字の高さに調整する
	ViewHeight = 100; // あとで実際の文字の高さに調整する
	ViewOriginX = 8;

	SelStart.LogicalIndex = 0;
	SelStart.CharPosition = 3;
	SelEnd.LogicalIndex = 2;
	SelEnd.CharPosition = 11;

	MouseSelecting = false;
	ScrollTimer = NULL;
	ScrollTimerScrollAmount = 0;

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
	volatile tRisseCriticalSection::tLocker holder(CS);

	// レシーバとしてこのオブジェクトを登録解除する
	tRisaLogger::instance()->UnregisterReceiver(this);

	// キャプチャを保持している場合はリリース
	if(HasCapture()) ReleaseMouse();

	// タイマーを削除
	delete ScrollTimer;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ログアイテムの種別からフォントを得る
//! @param		type		ログアイテムの種別
//! @param		bold		太字にするかどうか
//! @param		colour		wxColor クラスへの参照
//---------------------------------------------------------------------------
void tRisaLogScrollView::GetFontFromLogItemType(tRisaLogger::tType type,
		bool & bold, wxColour &colour)
{
	switch(type)
	{
	case tRisaLogger::itDebug:		//!< デバッグに関する物
		{ static wxColour c(0xa0, 0xa0, 0xa0); colour = c; bold = false; }
		break;
	case tRisaLogger::itInfo:		//!< 通知
		{ static wxColour c(0xff, 0xff, 0xff); colour = c; bold = false; }
		break;
	case tRisaLogger::itNotice:		//!< 通常状態だが大事な情報
		{ static wxColour c(0x00, 0xff, 0x00); colour = c; bold = false; }
		break;
	case tRisaLogger::itWarning:	//!< 警告
		{ static wxColour c(0xff, 0xff, 0x00); colour = c; bold = false; }
		break;
	case tRisaLogger::itError:		//!< 通常のエラー
		{ static wxColour c(0xff, 0x80, 0x80); colour = c; bold = false; }
		break;
	case tRisaLogger::itCritical:	//!< 致命的なエラー
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
//! @brief		ログのローテーションを行う
//---------------------------------------------------------------------------
void tRisaLogScrollView::Rotate()
{
	if(LogicalLines.size() < RotateLimit) return; // まだローテーションするほどではない
	if(MouseSelecting) return; // 範囲選択中はローテーションを行わない

	// これから先頭の (LogicalLines.size() - RotateTo) 個の論理行を削除する。
	size_t log_num_delete_items = LogicalLines.size() - RotateTo;

	// 削除されるべき表示行に対応する論理行を得る
	size_t disp_num_delete_items;
	for(disp_num_delete_items = 0; disp_num_delete_items < DisplayLines.size();
		disp_num_delete_items++)
	{
		if(DisplayLines[disp_num_delete_items].LogicalIndex >= log_num_delete_items)
			break;
	}

	// LogicalLines の fixup
	for(std::deque<tLogicalLine>::iterator i = LogicalLines.begin() + log_num_delete_items;
		i != LogicalLines.end(); i++)
	{
		i->DisplayIndex -= disp_num_delete_items;
	}

	// DisplayLines の fixup
	for(std::deque<tDisplayLine>::iterator i = DisplayLines.begin() + disp_num_delete_items;
		i != DisplayLines.end(); i++)
	{
		i->LogicalIndex -= log_num_delete_items;
	}

	// LogicalLines, DisplayLines の先頭部分を削除
	LogicalLines.erase(LogicalLines.begin(), LogicalLines.begin() + log_num_delete_items);
	DisplayLines.erase(DisplayLines.begin(), DisplayLines.begin() + disp_num_delete_items);

	// SelStart と SelEnd の fixup
	if(SelStart.LogicalIndex < log_num_delete_items)
	{
		SelStart.LogicalIndex = 0;
		SelStart.CharPosition = 0;
	}
	else
	{
		SelStart.LogicalIndex -= log_num_delete_items;
	}

	if(SelEnd.LogicalIndex < log_num_delete_items)
	{
		SelEnd.LogicalIndex = 0;
		SelEnd.CharPosition = 0;
	}
	else
	{
		SelEnd.LogicalIndex -= log_num_delete_items;
	}

	if(SelStart == SelEnd)
	{
		SelStart.Invalidate();
		SelEnd.Invalidate();
	}

	// スクロールバーの位置を調整
	int anchor = GetScrollAnchor();
	if(anchor != -1)
	{
		// 表示位置が最後部に固定されていない場合
		int top = GetScrollPos(wxVERTICAL);
		top -= disp_num_delete_items;
		if(top < 0)
		{
			Refresh();
			top = 0;
		}
		SetScrollPos(wxVERTICAL, top);
		anchor = top;
	}
	SetScrollBarInfo(anchor);

	// 表示を更新する
	Refresh();
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
	tLogicalLine & item = LogicalLines[log_index];
	item.DisplayIndex = DisplayLines.size();

	// フォントをデバイスコンテキストに選択
	wxClientDC dc(this);
	CurrentFont.SetWeight((!item.Bold)?wxFONTWEIGHT_NORMAL:wxFONTWEIGHT_BOLD);
	dc.SetFont(CurrentFont);
	LineHeight = dc.GetCharHeight();

	// 各文字位置での extentを得る
	wxArrayInt widths;
	dc.GetPartialTextExtents(item.Line, widths);

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
			if(widths[index] - left_pixel > static_cast<int>(ViewWidth)) break;
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
	case tRisaLogger::itDebug:
		logline += _("[D] "); break; // [D]ebug
	case tRisaLogger::itInfo:
		logline += _("[I] "); break; // [I]nformation
	case tRisaLogger::itNotice:
		logline += _("[N] "); break; // [N]otice
	case tRisaLogger::itWarning:
		logline += _("[W] "); break; // [W]arning
	case tRisaLogger::itError:
		logline += _("[E] "); break; // [E]rror
	case tRisaLogger::itCritical:
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
	// DisplayLines はいったん全てクリア
	DisplayLines.clear();

	// 全ての論理行に対してレイアウトを行う
	size_t num_log_lines = LogicalLines.size();
	for(size_t n = 0; n < num_log_lines; n++)
		LayoutOneLine(n);

	// スクロールバーの調整
	SetScrollBarInfo(GetScrollAnchor());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		一行分をDisplayLinesに追加し、レイアウトを行う
//! @param		logger_item  tRisaLogger::tItem 型
//---------------------------------------------------------------------------
void tRisaLogScrollView::AddLine(const tRisaLogger::tItem & logger_item)
{
	size_t old_num_displaylines = DisplayLines.size();

	wxString line = CreateOneLineString(logger_item);
	wxString link = logger_item.Link.AsWxString();
	wxColour colour;
	bool bold;
	GetFontFromLogItemType(logger_item.Type, bold, colour);
	LogicalLines.push_back(tLogicalLine(line, link, colour, bold));
	LayoutOneLine(LogicalLines.size() - 1);

	// 必要であればローテーションを行う
	Rotate();

	// スクロールバーの調整
	SetScrollBarInfo(GetScrollAnchor());

	// 新しく追加した領域の再描画
	RefreshDisplayLine(old_num_displaylines, DisplayLines.size() - old_num_displaylines);

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スクロールを行う
//! @param		amount スクロール量
//! @param		absolute amount が絶対位置による指定かどうか
//---------------------------------------------------------------------------
void tRisaLogScrollView::DoScroll(risse_int amount, bool absolute)
{
	if(!absolute && amount == 0) return ; // スクロールしない

	// 現在のスクロールバー位置を取得
	int old_pos = GetScrollPos(wxVERTICAL);
	int new_pos = old_pos;
	int pos_max = GetScrollRange(wxVERTICAL) - GetScrollThumb(wxVERTICAL);
	if(pos_max <= 0) return; // スクロールできない

	// スクロール量を加算
	if(absolute)
		new_pos = amount;
	else
		new_pos += amount;

	if(new_pos > pos_max) new_pos = pos_max;
	else if(new_pos < 0) new_pos = 0;

	// スクロールバーの位置を設定
	SetScrollPos(wxVERTICAL, new_pos);
	ScrollView(old_pos, new_pos);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スクロールバーの情報を設定する
//! @param		anchor スクロール位置のアンカー (-1: ログの最後、それ以外は表示行インデックス)
//---------------------------------------------------------------------------
void tRisaLogScrollView::SetScrollBarInfo(risse_int anchor)
{
	// スクロールバーの調整
	risse_int old_top = GetScrollPos(wxVERTICAL);

	size_t num_disp_lines = DisplayLines.size();

	risse_int new_top;
	if(anchor == -1)
		new_top = num_disp_lines > LinesPerWindow ? num_disp_lines - LinesPerWindow : 0; // ログの最後
	else
		new_top = anchor;

	SetScrollbar(wxVERTICAL, new_top, LinesPerWindow, num_disp_lines);

	ScrollView(old_top, new_top);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スクロール位置のアンカー (どの表示部分を常に表示し続けるか) を得る
//! @return		アンカー (-1: ログの最後、それ以外は表示行インデックス)
//---------------------------------------------------------------------------
risse_int tRisaLogScrollView::GetScrollAnchor()
{
	// ログの最後を表示しているかどうかを判定
	risse_int top = GetScrollPos(wxVERTICAL);
	if(top >= GetScrollRange(wxVERTICAL) - GetScrollThumb(wxVERTICAL))
		return -1; // ログの最後
	return top; // 表示行インデックス
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ビューをスクロールする
//! @param		old_top		スクロール前の最上行の表示行インデックス
//! @param		new_top		スクロール後の最上行の表示行インデックス
//---------------------------------------------------------------------------
void tRisaLogScrollView::ScrollView(risse_int old_top, risse_int new_top)
{
	if(old_top != new_top)
	{
		// スクロールを行う
		ScrollWindow(0, (old_top - new_top) * LineHeight);
		if(old_top < new_top)
		{
			// スクロールで新しく表示される部分が確実に再描画されるように
			// 再描画範囲を指定する
			wxRect r(ViewOriginX,
				(LinesPerWindow - (new_top-old_top))*LineHeight,
				ViewWidth,
				LineHeight*(new_top-old_top));
			RefreshRect(r);
		}
	}

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定された表示行を再描画する
//! @param		index 表示行インデックス
//! @param		range 範囲
//---------------------------------------------------------------------------
void tRisaLogScrollView::RefreshDisplayLine(size_t index, risse_int range)
{
	// index は表示可能範囲内かどうかはチェックしない
	size_t top = GetScrollPos(wxVERTICAL);

	index -= top;

	RefreshRect(wxRect(ViewOriginX, index * LineHeight, ViewWidth, LineHeight*range));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ビュー内の位置から文字位置を割り出す
//! @param		x ビュー内の位置x
//! @param		y ビュー内の位置y
//! @param		charpos 文字位置を格納する先(論理位置)
//! @param		charlength その行の文字長さを格納する先(NULL可)
//---------------------------------------------------------------------------
void tRisaLogScrollView::ViewPositionToCharacterPosition(risse_int x,
	risse_int y, tCharacterPosition & charpos, size_t *charlength)
{
	// あらかじめ charlength に 0 を入れておく
	if(charlength) *charlength = 0;

	// x から ViewOriginX を引く
	x -= ViewOriginX;

	// y からビュー内の行数に変換
	if(y < 0)
		y = y / static_cast<risse_int>(LineHeight) - 1;
	else
		y = y / static_cast<risse_int>(LineHeight);

	// 表示行を計算
	ptrdiff_t displayline_index = y + GetScrollPos(wxVERTICAL);

	// 表示行は範囲内？
	if(displayline_index < 0)
	{
		// 範囲外なので最初の要素を返す
		charpos.LogicalIndex = 0;
		charpos.CharPosition = 0;
		return;
	}

	if(static_cast<size_t>(displayline_index) >= DisplayLines.size())
	{
		// 範囲外なので最後の要素を返す
		if(DisplayLines.size() == 0) { charpos.Invalidate(); return; }
		const tDisplayLine & displayline = DisplayLines[DisplayLines.size() - 1];
		charpos.LogicalIndex = displayline.LogicalIndex;
		charpos.CharPosition = displayline.CharStart + displayline.CharLength;
		return;
	}

	// オブジェクトへの参照を取得
	const tDisplayLine & displayline = DisplayLines[displayline_index];
	const tLogicalLine & logicalline = LogicalLines[displayline.LogicalIndex];
	charpos.LogicalIndex = displayline.LogicalIndex;
	if(charlength) *charlength = displayline.CharLength;

	// フォントをデバイスコンテキストに選択
	wxClientDC dc(this);
	CurrentFont.SetWeight((!logicalline.Bold)?wxFONTWEIGHT_NORMAL:wxFONTWEIGHT_BOLD);
	dc.SetFont(CurrentFont);

	// 部分文字列を取り出す
	wxString substr(logicalline.Line.c_str() + displayline.CharStart,
					displayline.CharLength);

	// 各文字の幅を取得する
	wxArrayInt widths;
	dc.GetPartialTextExtents(substr, widths);

	// x がどの位置にあるのかを探す
	for(size_t i = 0; i < widths.GetCount(); i++)
	{
		int char_start = i>0?widths[i-1]:0; // iの位置にある文字の始点
		int char_end   = widths[i]; // iの位置にある文字の終点
		if(char_start <= x && x < char_end)
		{
			// この文字を指している
			charpos.CharPosition = displayline.CharStart + i;
			// 文字の右半分を指している場合は charpos.CharPosition に 1 を加算する
			if(x >= (char_start+char_end)/2) charpos.CharPosition++;
			return;
		}
	}

	// ループ中では見つからなかった
	if(x < 0)
		charpos.CharPosition = displayline.CharStart;
	if(x >= widths[widths.GetCount()-1])
		charpos.CharPosition = displayline.CharStart + displayline.CharLength;
	return;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ビュー内の位置が選択範囲内かどうかを判断する
//! @param		x ビュー内の位置x
//! @param		y ビュー内の位置y
//! @return		範囲内かどうか
//---------------------------------------------------------------------------
bool tRisaLogScrollView::IsViewPositionInSelection(risse_int x, risse_int y)
{
	if(!SelStart.IsValid() || !SelEnd.IsValid()) return false;
	tCharacterPosition charpos;
	ViewPositionToCharacterPosition(x, y, charpos);
	if(!charpos.IsValid()) return false;
	return !(SelStart > charpos) && charpos < SelEnd;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		選択範囲の文字列を取得する
//---------------------------------------------------------------------------
wxString tRisaLogScrollView::GetSelectionString()
{
	if(!SelStart.IsValid() || !SelEnd.IsValid()) return wxEmptyString;

	wxString ret;
	for(size_t log_index = SelStart.LogicalIndex;
		log_index <= SelEnd.LogicalIndex; log_index++)
	{
		if(log_index == SelStart.LogicalIndex &&
			log_index == SelEnd.LogicalIndex)
		{
			return wxString(LogicalLines[log_index].Line.c_str() +
						SelStart.CharPosition,
						SelEnd.CharPosition - SelStart.CharPosition);
		}

		if(log_index == SelStart.LogicalIndex)
			ret += wxString(LogicalLines[log_index].Line.c_str() + SelStart.CharPosition) + wxT("\n");
		else if(log_index == SelEnd.LogicalIndex)
			ret += wxString(LogicalLines[log_index].Line.c_str(), SelEnd.CharPosition);
		else
			ret += LogicalLines[log_index].Line + wxT("\n");
	}
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		選択範囲を設定する
//! @param		pos1   選択範囲の始点終点(論理位置)
//! @param		pos2   選択範囲の始点終点(論理位置)
//! @param		pos3   選択範囲の始点終点(論理位置)
//! @note		min(pos1, pos2, pos3) ～ max(pos1, pos2, pos3) が選択される
//---------------------------------------------------------------------------
void tRisaLogScrollView::SetSelection(
	const tCharacterPosition & pos1, const tCharacterPosition & pos2, const tCharacterPosition & pos3)
{
	// 新しい SelStart と SelEnd を決定
	tCharacterPosition new_selstart;
	tCharacterPosition new_selend;

	if(pos1.IsValid())
	{
		new_selstart = pos1;
		new_selend   = pos1;
	}

	if(pos2.IsValid())
	{
		if(new_selstart > pos2) new_selstart = pos2;
		if(new_selend   < pos2) new_selend   = pos2;
	}

	if(pos3.IsValid())
	{
		if(new_selstart > pos3) new_selstart = pos3;
		if(new_selend   < pos3) new_selend   = pos3;
	}

	// 更新された部分を Refresh
	if(new_selstart.IsValid() && !SelStart.IsValid())
		RefreshSelection(new_selstart, new_selend);
	else if(!new_selstart.IsValid() && SelStart.IsValid())
		RefreshSelection(SelStart, SelEnd);
	else if(new_selstart.IsValid() && SelStart.IsValid())
	{
		// 両方とも有効
		if(new_selstart != SelStart)
			RefreshSelection(new_selstart, SelStart);
		if(new_selend   != SelEnd  )
			RefreshSelection(new_selend,   SelEnd  );
	}

	// SelStart と SelEnd を更新
	SelStart = new_selstart;
	SelEnd   = new_selend;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定された範囲を再描画する
//! @param		pos1   範囲の始点(論理位置)
//! @param		pos2   範囲の終点(論理位置)
//! @note		pos1 > pos2 でもかまわない(自動で判別する)
//---------------------------------------------------------------------------
void tRisaLogScrollView::RefreshSelection(const tCharacterPosition & pos1, const tCharacterPosition & pos2)
{
	// 厳密には指定された領域を文字単位で再描画することになるが、
	// ここでは簡略化のため、行単位で再描画を行う
	tCharacterPosition start = pos1;
	tCharacterPosition end   = pos2;
	if(end < start) std::swap(start, end);

	tCharacterPosition disp_start, disp_end;
	LogicalPositionToDisplayPosition(start, disp_start);
	LogicalPositionToDisplayPosition(end  , disp_end);

	RefreshDisplayLine(disp_start.DisplayIndex,
		disp_end.DisplayIndex - disp_start.DisplayIndex + 1);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		論理位置から表示位置への変換
//! @param		log_pos  論理位置
//! @param		disp_pos 表示位置
//---------------------------------------------------------------------------
void tRisaLogScrollView::LogicalPositionToDisplayPosition(const tCharacterPosition & log_pos,
				tCharacterPosition & disp_pos)
{
	disp_pos.Invalidate();
	size_t disp_index = LogicalLines[log_pos.LogicalIndex].DisplayIndex;
	while(true)
	{
		const tDisplayLine & disp_item = DisplayLines[disp_index];

		if(!(disp_index < DisplayLines.size() &&
			log_pos.LogicalIndex == disp_item.LogicalIndex)) break;

		size_t char_start = disp_item.CharStart;
		size_t char_end   = disp_item.CharStart + disp_item.CharLength;
		if(char_start <= log_pos.CharPosition && log_pos.CharPosition <= char_end)
		{
			disp_pos.DisplayIndex = disp_index;
			disp_pos.CharPosition = log_pos.CharPosition - char_start;
			return;
		}

		disp_index ++;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		タイマーにより定期的に呼び出され、スクロールを行うメソッド
//---------------------------------------------------------------------------
void tRisaLogScrollView::ScrollByTimer()
{
	DoScroll(ScrollTimerScrollAmount, false);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		tRisaLogReceiver::OnLog のオーバーライド
//! @param		logger_item  tRisaLogger::tItem 型
//---------------------------------------------------------------------------
void tRisaLogScrollView::OnLog(const tRisaLogger::tItem & logger_item)
{
	volatile tRisseCriticalSection::tLocker holder(CS);

	AddLine(logger_item);
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

		printf("(%d,%d)-(%d,%d)\n",
			rect.GetLeft(),
			rect.GetTop(),
			rect.GetRight(),
			rect.GetBottom());

		// rect が影響している行を計算
		risse_int f = rect.GetTop() / LineHeight;
		risse_int l = rect.GetBottom() / LineHeight;

		// rect が影響している行と draw_line_first, draw_line_last を比較
		if(draw_line_first == -1)
			draw_line_first = f;
		else
			if(draw_line_first > f) draw_line_first = f;

		if(draw_line_last == -1)
			draw_line_last = l;
		else
			if(draw_line_last < l) draw_line_last = l;

		upd ++;
	}

	// draw_line_first と draw_line_last 間の全ての行を表示
	if(draw_line_first != -1 && draw_line_last != -1)
	{
		wxCoord y = draw_line_first * LineHeight;

		for(risse_int n = draw_line_first; n <= draw_line_last; n++, y += LineHeight)
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
			dc.DrawText(substr, ViewOriginX, y);

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
					dc.DrawRectangle(startx + ViewOriginX, y, endx - startx, LineHeight);
					dc.SetLogicalFunction(wxCOPY);
				}
			}
		}
	}

	event.Skip();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スクロールされたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaLogScrollView::OnScroll(wxScrollWinEvent& event)
{
	volatile tRisseCriticalSection::tLocker holder(CS);

	// イベントをスキップ
	event.Skip();

	// スクロール位置の最大値を取得
	int pos_max = GetScrollRange(wxVERTICAL) - GetScrollThumb(wxVERTICAL);

	// イベントの種類に従って分岐
	WXTYPE ev_type = event.GetEventType();
	if(ev_type == wxEVT_SCROLLWIN_THUMBRELEASE ||
			ev_type == wxEVT_SCROLLWIN_THUMBTRACK)
		DoScroll(event.GetPosition(), true);
	else if(ev_type == wxEVT_SCROLLWIN_TOP)
		DoScroll(0, true);
	else if(ev_type == wxEVT_SCROLLWIN_BOTTOM)
		DoScroll(pos_max, true);
	else if(ev_type == wxEVT_SCROLLWIN_LINEUP)
		DoScroll(-1, false);
	else if(ev_type == wxEVT_SCROLLWIN_LINEDOWN)
		DoScroll(+1, false);
	else if(ev_type == wxEVT_SCROLLWIN_PAGEUP)
		DoScroll(-LinesPerWindow, false);
	else if(ev_type == wxEVT_SCROLLWIN_PAGEDOWN)
		DoScroll(+LinesPerWindow, false);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		マウスホイールが操作されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaLogScrollView::OnMouseWheel(wxMouseEvent& event)
{
	volatile tRisseCriticalSection::tLocker holder(CS);

	// イベントをスキップ
	event.Skip();

	int amount = event.GetWheelRotation() / event.GetWheelDelta();
	if(event.IsPageScroll())
		DoScroll(-amount * LinesPerWindow, false);
	else
		DoScroll(-amount * event.GetLinesPerAction(), false);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		サイズが変更されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaLogScrollView::OnSize(wxSizeEvent& event)
{
	volatile tRisseCriticalSection::tLocker holder(CS);

	// LinesPerWindow などを計算
	risse_int cw = 0, ch = 0;
	GetClientSize(&cw, &ch);

	ViewWidth = cw - ViewOriginX;
	ViewHeight = ch;

	LinesPerWindow = ViewHeight / LineHeight;

	// 全ての行をレイアウト
	LayoutAllLines();
	Refresh();

	// イベントをスキップ
	event.Skip();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		マウスの左ボタンが押されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaLogScrollView::OnLeftDown(wxMouseEvent & event)
{
	volatile tRisseCriticalSection::tLocker holder(CS);

#if wxUSE_DRAG_AND_DROP
	if(IsViewPositionInSelection(event.GetX(), event.GetY()))
	{
		// ドラッグを開始する
		wxPoint start_mouse = ::wxGetMousePosition();
		wxTextDataObject text_data(GetSelectionString());
		wxDropSource source(text_data, this);
		source.DoDragDrop();
		// ドラッグされていないようならば選択範囲をクリアする
		if(start_mouse == ::wxGetMousePosition())
			SetSelection(tCharacterPosition(), tCharacterPosition(), tCharacterPosition());
	}
	else
#endif
	{
		tCharacterPosition pos;
		size_t charlength;
		ViewPositionToCharacterPosition(event.GetX(), event.GetY(), pos, &charlength);
		if(event.GetX() < ViewOriginX)
		{
			// 行選択
			MouseSelStart1 = pos;
			MouseSelStart2 = pos;
			MouseSelStart2.CharPosition += charlength;
			printf("charlength : %d\n", charlength);
			SetSelection(MouseSelStart1, MouseSelStart2, tCharacterPosition());
		}
		else
		{
			// 文字選択
			MouseSelStart1 = pos;
			MouseSelStart2 = pos;
			SetSelection(tCharacterPosition(), tCharacterPosition(), tCharacterPosition());
		}

		MouseSelecting = true;

		if(!HasCapture())
			CaptureMouse();

		ScrollTimerScrollAmount = 0;
		if(!ScrollTimer)
			ScrollTimer = new tScrollTimer(this);
	}

	event.Skip(true);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		マウスの左ボタンが離されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaLogScrollView::OnLeftUp(wxMouseEvent & event)
{
	volatile tRisseCriticalSection::tLocker holder(CS);

	tCharacterPosition pos;
	ViewPositionToCharacterPosition(event.GetX(), event.GetY(), pos);
	SetSelection(MouseSelStart1, MouseSelStart2, pos);

	MouseSelecting = false;

	if(HasCapture())
		ReleaseMouse();

	delete ScrollTimer, ScrollTimer = NULL;
	ScrollTimerScrollAmount = 0;

	event.Skip(true);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		マウスが移動したとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaLogScrollView::OnMotion(wxMouseEvent & event)
{
	volatile tRisseCriticalSection::tLocker holder(CS);

	// マウスカーソルの形状を決定
	if(event.GetX() < ViewOriginX)
	{
		SetCursor(wxCursor(wxCURSOR_RIGHT_ARROW));
	}
	else
	{
#if wxUSE_DRAG_AND_DROP
		// 範囲選択内を指していた場合は普通のカーソルにするが、
		// それ以外は I-Beam
		if(!MouseSelecting &&
			IsViewPositionInSelection(event.GetX(), event.GetY()))
			SetCursor(wxCursor(wxCURSOR_ARROW));
		else
			SetCursor(wxCursor(wxCURSOR_IBEAM));
#else
		SetCursor(wxCursor(wxCURSOR_IBEAM));
#endif
	}

	// マウスによる領域の選択中か
	if(MouseSelecting)
	{
		tCharacterPosition pos;
		ViewPositionToCharacterPosition(event.GetX(), event.GetY(), pos);
		SetSelection(MouseSelStart1, MouseSelStart2, pos);

		if(event.GetY() < 0)
			ScrollTimerScrollAmount = -1;
		else if(static_cast<size_t>(event.GetY()) > ViewHeight)
			ScrollTimerScrollAmount = +1;
		else
			ScrollTimerScrollAmount = 0;

		DoScroll(ScrollTimerScrollAmount, false);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		キーが押されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void tRisaLogScrollView::OnChar(wxKeyEvent &event)
{
	volatile tRisseCriticalSection::tLocker holder(CS);

	for(int n = 0; n < 100; n++)
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







