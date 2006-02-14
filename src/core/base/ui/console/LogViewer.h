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
#ifndef _LOGVIEWER_H
#define _LOGVIEWER_H

#include <wx/wx.h>
#include <deque>


//---------------------------------------------------------------------------
//! @brief		ログビューアウィンドウ
//---------------------------------------------------------------------------
class tRisaLogScrollView : public wxPanel, public tRisaLogReceiver
{
	static const size_t RotateLimit = 2100; //!< この論理行数を超えるとローテーションを行う
	static const size_t RotateTo    = 2000; //!< 一回のローテーションではこの論理行数までにログを減らす

	//! @brief	メニューアイテムID
	enum
	{
		ID_First = 100,
		ID_Menu_Copy, //!< コピー
		ID_Menu_SelectAll,  //!< すべて選選択
		ID_Last
	};

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

		tCharacterPosition(size_t index, size_t pos)
			{ LogicalIndex = index, CharPosition = pos; }
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
	void GetFontFromLogItemLevel(tRisaLogger::tLevel level,
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
	bool AnySelected();
	wxString GetSelectionString();
	void SetSelection(const tCharacterPosition & pos1,
		const tCharacterPosition & pos2,
		const tCharacterPosition & pos3);
	void SelectAll();
	void RefreshSelection(const tCharacterPosition & pos1, const tCharacterPosition & pos2);

	void LogicalPositionToDisplayPosition(const tCharacterPosition & log_pos,
				tCharacterPosition & disp_pos);

	void ScrollByTimer();

	void ShowContextMenu(const wxPoint & pos);

public:
	void OnLog(const tRisaLogger::tItem & logger_item); // tRisaLogReceiver の override

private:
	void OnPaint(wxPaintEvent& event);
	void OnChar(wxKeyEvent & event);
	void OnScroll(wxScrollWinEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnLeftDown(wxMouseEvent & event);
	void OnLeftUp(wxMouseEvent & event);
	void OnMotion(wxMouseEvent & event);
	void OnMenuCopy(wxCommandEvent &event);
	void OnMenuSelectAll(wxCommandEvent &event);

#if USE_CONTEXT_MENU
	void OnContextMenu(wxContextMenuEvent& event);
#else
	void OnRightUp(wxMouseEvent& event)
		{ ShowContextMenu(event.GetPosition()); }
#endif

	DECLARE_EVENT_TABLE()

};
//---------------------------------------------------------------------------

#endif
