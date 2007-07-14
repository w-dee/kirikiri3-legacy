//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ログビューア
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/log/Log.h"
#include "base/ui/console/LogViewer.h"
#include "base/ui/console/Console.h"
#include <stdlib.h>
#include <deque>
#include <wx/textctrl.h>
#include <wx/clipbrd.h>
#include <wx/datetime.h>

#if wxUSE_OWNER_DRAWN
#include <wx/artprov.h>
#endif

#ifdef wxUSE_DRAG_AND_DROP
#include <wx/dataobj.h>
#include <wx/dnd.h>
#endif

namespace Risa {
RISSE_DEFINE_SOURCE_ID(57288,52924,45855,20290,20385,24474,35332,13597);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// 行が追加されたことを表すイベントを定義
//---------------------------------------------------------------------------
DECLARE_EVENT_TYPE(wxEVT_LINE_ADDED, -1)
DEFINE_EVENT_TYPE(wxEVT_LINE_ADDED)
DECLARE_EVENT_TYPE(wxEVT_DO_ROTATION_REFRESH, -1)
DEFINE_EVENT_TYPE(wxEVT_DO_ROTATION_REFRESH)
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(tRisaLogScrollView, wxPanel)
	EVT_COMMAND(wxID_ANY,				wxEVT_LINE_ADDED, tRisaLogScrollView::OnLineAdded)
	EVT_COMMAND(wxID_ANY,				wxEVT_DO_ROTATION_REFRESH, tRisaLogScrollView::OnDoRotationRefresh)

	EVT_PAINT(							tRisaLogScrollView::OnPaint)
	EVT_CHAR(							tRisaLogScrollView::OnChar)
	EVT_SCROLLWIN(						tRisaLogScrollView::OnScroll)
	EVT_MOUSEWHEEL(						tRisaLogScrollView::OnMouseWheel)
	EVT_SIZE(							tRisaLogScrollView::OnSize)
	EVT_LEFT_DOWN(						tRisaLogScrollView::OnLeftDown)
	EVT_LEFT_UP(						tRisaLogScrollView::OnLeftUp)
	EVT_MOTION(							tRisaLogScrollView::OnMotion)
	EVT_CHAR(							tRisaLogScrollView::OnChar)

	EVT_MENU(ID_Menu_Copy,				tRisaLogScrollView::OnMenuCopy)
	EVT_MENU(ID_Menu_SelectAll,			tRisaLogScrollView::OnMenuSelectAll)

#if USE_CONTEXT_MENU
	EVT_CONTEXT_MENU(					tRisaLogScrollView::OnContextMenu)
#else
	EVT_RIGHT_UP(						tRisaLogScrollView::OnRightUp)
#endif

END_EVENT_TABLE()
//---------------------------------------------------------------------------


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

	MouseSelecting = false;

	LayoutRequested = false;
	LayoutRequestFirstLogicalLine = 0;
	LayoutRequestFirstDisplayLine = 0;
	RotationRefreshLostDisplayLines = 0;

	ScrollTimer = NULL;
	ScrollTimerScrollAmount = 0;

	// ウィンドウを表示する
	Show();

	// 既存のログをレイアウトする
	tRisaLogger::instance()->SendLogs(this, RotateTo);

	// レシーバとしてこのオブジェクトを登録する
	tRisaLogger::instance()->RegisterReceiver(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaLogScrollView::~tRisaLogScrollView()
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	// レシーバとしてこのオブジェクトを登録解除する
	tRisaLogger::instance()->UnregisterReceiver(this);

	// キャプチャを保持している場合はリリース
	if(HasCapture()) ReleaseMouse();

	// タイマーを削除
	delete ScrollTimer;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaLogScrollView::GetFontFromLogItemLevel(tRisaLogger::tLevel level,
		bool & bold, wxColour &colour)
{
	switch(level)
	{
	case tRisaLogger::llDebug:		//!< デバッグに関する物
		{ static wxColour c(0xa0, 0xa0, 0xa0); colour = c; bold = false; }
		break;
	case tRisaLogger::llInfo:		//!< 通知
		{ static wxColour c(0xff, 0xff, 0xff); colour = c; bold = false; }
		break;
	case tRisaLogger::llNotice:		//!< 通常状態だが大事な情報
		{ static wxColour c(0x00, 0xff, 0x00); colour = c; bold = false; }
		break;
	case tRisaLogger::llWarning:	//!< 警告
		{ static wxColour c(0xff, 0xff, 0x00); colour = c; bold = false; }
		break;
	case tRisaLogger::llError:		//!< 通常のエラー
		{ static wxColour c(0xff, 0x80, 0x80); colour = c; bold = false; }
		break;
	case tRisaLogger::llCritical:	//!< 致命的なエラー
		{ static wxColour c(0xff, 0x00, 0x00); colour = c; bold = true;  }
		break;
	default:
		{ static wxColour c(0xff, 0xff, 0xff); colour = c; bold = false; }
		break;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
wxColour tRisaLogScrollView::GetBackgroundColour()
{
		{ static wxColour c(0x00, 0x00, 0x00); return c; }
}
//---------------------------------------------------------------------------


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

	// LayoutRequestFirstLogicalLine の fixup
	if(LayoutRequestFirstLogicalLine >= log_num_delete_items)
		LayoutRequestFirstLogicalLine -= log_num_delete_items;
	else
		LayoutRequestFirstLogicalLine = 0;

	// LayoutRequestFirstDisplayLine の fixup
	if(LayoutRequestFirstDisplayLine >= log_num_delete_items)
		LayoutRequestFirstDisplayLine -= log_num_delete_items;
	else
		LayoutRequestFirstDisplayLine = 0;

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
	FixupSelection(SelStart, SelEnd, log_num_delete_items);

	// MouseSelStart1 と MouseSelStart2 の fixup
	if(!FixupSelection(MouseSelStart1, MouseSelStart2, log_num_delete_items))
		MouseSelecting = false;

	// メインスレッドでレイアウトを行うことが出来るように画面更新イベントをポスト
	if(RotationRefreshLostDisplayLines == 0)
	{
		wxCommandEvent evt(wxEVT_DO_ROTATION_REFRESH);
		GetEventHandler()->AddPendingEvent(evt);
	}

	RotationRefreshLostDisplayLines += disp_num_delete_items;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisaLogScrollView::FixupSelection(
	tCharacterPosition &sel1, tCharacterPosition &sel2, size_t log_num_delete_items)
{
	// GUI へのアクセスなし

	if(sel1.IsValid())
	{
		if(sel1.LogicalIndex < log_num_delete_items)
		{
			sel1.LogicalIndex = 0;
			sel1.CharPosition = 0;
		}
		else
		{
			sel1.LogicalIndex -= log_num_delete_items;
		}
	}

	if(sel2.IsValid())
	{
		if(sel2.LogicalIndex < log_num_delete_items)
		{
			sel2.LogicalIndex = 0;
			sel2.CharPosition = 0;
		}
		else
		{
			sel2.LogicalIndex -= log_num_delete_items;
		}
	}

	if(sel1 == sel2)
	{
		sel1.Invalidate();
		sel2.Invalidate();
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------



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
wxString tRisaLogScrollView::CreateOneLineString(const tRisaLogger::tItem & item)
{
	// 表示形式は
	// 時刻 ログレベル メッセージ

	// 時刻
	wxString logline = item.Timestamp.Format(wxT("%X")) + wxT(" ");

	// ログレベル
	switch(item.Level)
	{
	case tRisaLogger::llDebug:
		logline += _("[D] "); break; // [D]ebug
	case tRisaLogger::llInfo:
		logline += _("[I] "); break; // [I]nformation
	case tRisaLogger::llNotice:
		logline += _("[N] "); break; // [N]otice
	case tRisaLogger::llWarning:
		logline += _("[W] "); break; // [W]arning
	case tRisaLogger::llError:
		logline += _("[E] "); break; // [E]rror
	case tRisaLogger::llRecord:
		logline += _("[R] "); break; // [R]ecord
	case tRisaLogger::llCritical:
		logline += _("[C] "); break; // [C]ritical
	}

	// メッセージ
	logline += item.Content.AsWxString();

	return logline;
}
//---------------------------------------------------------------------------


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

	// レイアウトがすんだので LayoutRequested はいらない
	LayoutRequested = false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaLogScrollView::AddLine(const tRisaLogger::tItem & logger_item)
{
	// このメソッドはメインスレッド以外から呼ばれる可能性があるので注意すること

	size_t old_num_displaylines = DisplayLines.size();

	wxString line = CreateOneLineString(logger_item);
	wxString link = logger_item.Link.AsWxString();
	wxColour colour;
	bool bold;
	GetFontFromLogItemLevel(logger_item.Level, bold, colour);
	LogicalLines.push_back(tLogicalLine(line, link, colour, bold));

	// レイアウトをリクエストする
	if(!LayoutRequested)
	{
		LayoutRequestFirstLogicalLine = LogicalLines.size() - 1;
		LayoutRequestFirstDisplayLine = old_num_displaylines;
		LayoutRequested = true;

		// メインスレッドでレイアウトを行うことが出来るようにイベントをポスト
		wxCommandEvent evt(wxEVT_LINE_ADDED);
		GetEventHandler()->AddPendingEvent(evt);
	}

	// 必要であればローテーションを行う
	Rotate();
}
//---------------------------------------------------------------------------


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
void tRisaLogScrollView::RefreshDisplayLine(size_t index, risse_int range)
{
	// index は表示可能範囲内かどうかはチェックしない
	size_t top = GetScrollPos(wxVERTICAL);

	index -= top;

	RefreshRect(wxRect(ViewOriginX, index * LineHeight, ViewWidth, LineHeight*range));
}
//---------------------------------------------------------------------------


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
bool tRisaLogScrollView::AnySelected()
{
	return SelStart.IsValid() && SelEnd.IsValid();
}
//---------------------------------------------------------------------------


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

	if(new_selstart == new_selend)
	{
		// 始点と終点が同じ -> 選択されていないと見なす
		new_selstart.Invalidate();
		new_selend.Invalidate();
	}

	if(!new_selstart.IsValid() || !new_selend.IsValid())
	{
		// どちらかが Invalid -> 選択されていないと見なす
		new_selstart.Invalidate();
		new_selend.Invalidate();
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
void tRisaLogScrollView::SelectAll()
{
	if(LogicalLines.size() == 0) return;
	tCharacterPosition start(0, 0);
	tCharacterPosition end(LogicalLines.size() - 1,
		LogicalLines[LogicalLines.size()-1].Line.Length());
	SetSelection(start, end, tCharacterPosition());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaLogScrollView::RefreshSelection(const tCharacterPosition & pos1, const tCharacterPosition & pos2)
{
	// 厳密には指定された領域を文字単位で再描画することになるが、
	// ここでは簡略化のため、行単位で再描画を行う
	if(!pos1.IsValid() || !pos2.IsValid()) return;

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
void tRisaLogScrollView::ScrollByTimer()
{
	DoScroll(ScrollTimerScrollAmount, false);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaLogScrollView::ShowContextMenu(const wxPoint & pos)
{
	wxMenu menu;

	{
		volatile tRisaCriticalSection::tLocker holder(CS);

		// context menu of console log viewer
		wxMenuItem *item = new wxMenuItem(&menu, ID_Menu_Copy, _("&Copy"));
#if wxUSE_OWNER_DRAWN
		item->SetBitmap(wxArtProvider::GetBitmap(wxART_COPY, wxART_MENU));
#endif
		menu.Append(item);
		item->Enable(AnySelected());
		menu.Append(ID_Menu_SelectAll, _("Select &All"));
	}

	PopupMenu(&menu, pos.x, pos.y);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaLogScrollView::OnLog(const tRisaLogger::tItem & logger_item)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	AddLine(logger_item);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaLogScrollView::OnLineAdded(wxCommandEvent& event)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	if(LayoutRequested)
	{
		LayoutRequested = false;

		// LayoutRequestFirstLogicalLine から最終行までをレイアウトする
		for(size_t l = LayoutRequestFirstLogicalLine; l < LogicalLines.size(); l++)
			LayoutOneLine(l);

		// スクロールバーの調整
		SetScrollBarInfo(GetScrollAnchor());

		// 新しく追加した領域の再描画
		RefreshDisplayLine(LayoutRequestFirstDisplayLine, DisplayLines.size() - LayoutRequestFirstDisplayLine);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaLogScrollView::OnDoRotationRefresh(wxCommandEvent& event)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	// スクロールバーの位置を調整
	int anchor = GetScrollAnchor();
	if(anchor != -1)
	{
		// 表示位置が最後部に固定されていない場合
		int top = GetScrollPos(wxVERTICAL);
		top -= RotationRefreshLostDisplayLines;
		if(top < 0)
		{
			Refresh();
			top = 0;
		}
		SetScrollPos(wxVERTICAL, top);
		anchor = top;
	}
	SetScrollBarInfo(anchor);

	Refresh();

	RotationRefreshLostDisplayLines = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaLogScrollView::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);

	volatile tRisaCriticalSection::tLocker holder(CS);

	// 表示行を取得
	size_t lineFirst = GetScrollPos(wxVERTICAL);

	// 更新矩形に影響している行の範囲を取得
	risse_int draw_line_first = -1;
	risse_int draw_line_last  = -1;
	wxRegionIterator upd(GetUpdateRegion()); // get the update rect list
	while (upd)
	{
		wxRect rect(upd.GetRect());

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
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaLogScrollView::OnChar(wxKeyEvent & event)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	int keycode = event.GetKeyCode();

	switch (keycode)
	{
		case WXK_UP:  // 上キー
			DoScroll(-1, false);
			break;

		case WXK_DOWN: // 下キー
			DoScroll(+1, false);
			break;

		case WXK_PAGEUP: // Page Up
			DoScroll(-LinesPerWindow, false);
			break;

		case WXK_PAGEDOWN: // Page Down
			DoScroll(+LinesPerWindow, false);
			break;

		default:
			if(keycode == WXK_TAB || (keycode >= 0x20 && keycode < 256))
			{
				// 通常のキーが入力された場合
				// テキストコントロールにフォーカスを合わせる
				wxWindow * top = GetParent();
				while(top->GetParent()) top = top->GetParent(); // トップレベルウィンドウを探す
				reinterpret_cast<tRisaConsoleFrame*>(top)->SetFocusToTextCtrl(keycode);
				break;
			}
			else
			{
				event.Skip(true);
				return;
			}
	}

	event.Skip(false);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaLogScrollView::OnScroll(wxScrollWinEvent& event)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

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
void tRisaLogScrollView::OnMouseWheel(wxMouseEvent& event)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

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
void tRisaLogScrollView::OnSize(wxSizeEvent& event)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	// クライアントサイズを取得
	risse_int cw = 0, ch = 0;
	GetClientSize(&cw, &ch);

	ViewWidth = cw - ViewOriginX;
	ViewHeight = ch;

	// 全ての行をレイアウト
	LayoutAllLines();
	Refresh();

	// LineHeight と LinesPerWindow の計算
	// いったんフォントをデバイスコンテキストに選択
	wxClientDC dc(this);
	CurrentFont.SetWeight(wxFONTWEIGHT_NORMAL);
	dc.SetFont(CurrentFont);
	LineHeight = dc.GetCharHeight();
	LinesPerWindow = ViewHeight / LineHeight;

	// イベントをスキップ
	event.Skip();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaLogScrollView::OnLeftDown(wxMouseEvent & event)
{
#if wxUSE_DRAG_AND_DROP
	if(event.GetX() >= ViewOriginX &&
		 IsViewPositionInSelection(event.GetX(), event.GetY()))
	{
		// ドラッグを開始する
		wxPoint start_mouse = ::wxGetMousePosition();
		// 注意: DoDragDrop はドロップが終了するまで戻ってこない。
		// ここ全体をクリティカルセクションで保護してしまうことは避ける。
		// ただし、選択範囲のテキストをとってくるGetSelectionStringだけは保護しなければ
		// ならない。
		wxString selection;

		{
			volatile tRisaCriticalSection::tLocker holder(CS);
			selection = GetSelectionString();
		}

		// ドラッグの準備をする
		wxTextDataObject text_data(selection);
		wxDropSource source(text_data, this);
		source.DoDragDrop();
		// ドラッグされていないようならば選択範囲をクリアする
		if(start_mouse == ::wxGetMousePosition())
			SetSelection(tCharacterPosition(), tCharacterPosition(), tCharacterPosition());
	}
	else
#endif
	{
		volatile tRisaCriticalSection::tLocker holder(CS);

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
void tRisaLogScrollView::OnLeftUp(wxMouseEvent & event)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

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
void tRisaLogScrollView::OnMotion(wxMouseEvent & event)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

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
void tRisaLogScrollView::OnMenuCopy(wxCommandEvent &event)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	// 選択範囲をコピーする
	if (wxTheClipboard->Open())
	{
		// This data objects are held by the clipboard, 
		// so do not delete them in the app.

		// GetSelectionString はクリティカルセクションで保護すること。
		wxString selection;

		{
			volatile tRisaCriticalSection::tLocker holder(CS);
			selection = GetSelectionString();
		}

		wxTheClipboard->SetData(new wxTextDataObject(selection));
		wxTheClipboard->Close();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaLogScrollView::OnMenuSelectAll(wxCommandEvent &event)
{
	volatile tRisaCriticalSection::tLocker holder(CS);

	SelectAll();
}
//---------------------------------------------------------------------------


#if USE_CONTEXT_MENU
//---------------------------------------------------------------------------
void tRisaLogScrollView::OnContextMenu(wxContextMenuEvent& event)
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
}
//---------------------------------------------------------------------------
#endif


//---------------------------------------------------------------------------
} // namespace Risa
