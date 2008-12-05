//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ログビューア
//---------------------------------------------------------------------------
#ifndef _LOGVIEWER_H
#define _LOGVIEWER_H


/*!
@note
	tLogger は Risa 内部でのログを保持する一方、tLogViewer は
	それを表示する役割を担う。tLogger は Risa 内の様々なコンポーネント
	から出力されるログを効率よく、確実に保持・記録することに特化している。
	一方、tLogViewer はその内容を画面に表示するにすぎない。
	tLogViewer の tLogger への影響がなるべく少なくなるよう、
	tLogViewer では tLogger とは別にログ内容を保持するように
	する。
	また、tLogViewer は非表示の場合はログ内容を保持しない。表示状態
	になったときにいったん tLogger からログ内容をすべて取得し、
	あとは tLogger にログが追加されれば、tLogViewer もそのログを追加
	するといった同期動作を行う。再び非表示状態になれば同期動作をしなくなる。
*/


#include "risa/common/RisaThread.h"
#include <wx/wx.h>
#include <deque>

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * ログビューアウィンドウ
 */
class tLogScrollView : public wxPanel, public tLogReceiver
{
	static const size_t RotateLimit = 2100; //!< この論理行数を超えるとローテーションを行う
	static const size_t RotateTo    = 2000; //!< 一回のローテーションではこの論理行数までにログを減らす

	/**
	 * メニューアイテムID
	 */
	enum
	{
		ID_First = 100,
		ID_Menu_Copy, //!< コピー
		ID_Menu_SelectAll,  //!< すべて選選択
		ID_Last
	};

	tCriticalSection CS; //!< このオブジェクトを保護するクリティカルセクション

	struct tLogicalLine
	{
		wxString Line; //!< 行の内容
		wxString Link; //!< リンク先
		wxColour Colour; //!< 表示色
		size_t DisplayIndex; //!< この論理行に対応する表示行(の1行目)
		bool Bold; //!< 太字？

		/**
		 * コンストラクタ
		 */
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


	/**
	 * 表示行(←→論理行)を表す構造体
	 */
	struct tDisplayLine
	{
		size_t	LogicalIndex; //!< この表示行に対応する論理行
		size_t	CharStart; //!< この表示行の、物理行中の表示開始位置(wxChar単位)
		size_t	CharLength; //!< この表示行の、物理行中の表示文字長さ(wxChar単位)

		/**
		 * コンストラクタ
		 */
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

	/**
	 * 論理/表示行とその論理行中の文字位置を表すペア
	 */
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
	tCharacterPosition MouseSelStart2; //!< マウスでの選択開始位置2

	bool LayoutRequested; //!< 行が追加されて、レイアウト待ちの行がある場合に真
	size_t LayoutRequestFirstLogicalLine; //!< レイアウトが必要な最初の論理行 (レイアウトはこれ以降について行われる)
	size_t LayoutRequestFirstDisplayLine; //!< レイアウトが必要な最初の表示行

	size_t RotationRefreshLostDisplayLines; //!< ローテーション中に失われた表示行(スクロールバーの調整に用いる)

	/**
	 * マウスで選択中にスクロールを行うためのタイマークラス
	 */
	class tScrollTimer : public wxTimer
	{
		tLogScrollView * View;
	public:
		tScrollTimer(tLogScrollView * view)
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
	/**
	 * コンストラクタ
	 */
	tLogScrollView(wxWindow * parent);

	/**
	 * デストラクタ
	 */
	~tLogScrollView();

private:
	/**
	 * ログアイテムの種別からフォントを得る
	 * @param level		ログレベル
	 * @param bold		太字にするかどうか
	 * @param colour	wxColor クラスへの参照
	 */
	void GetFontFromLogItemLevel(tLogger::tLevel level,
		bool & bold, wxColour &colour);

	/**
	 * 背景色を得る
	 * @return	背景色
	 */
	wxColour GetBackgroundColour();

	/**
	 * ログのローテーションを行う
	 */
	void Rotate();

	/**
	 * 選択範囲のfixupを行う(選択範囲をlog_num_delete_items分 切りつめる)
	 * @param sel1					選択始点
	 * @param sel2					選択終点
	 * @param log_num_delete_items	切りつめる行数
	 * @return	切りつめた結果、選択範囲が残っていれば真、消えれば偽
	 */
	static bool FixupSelection(tCharacterPosition &sel1, tCharacterPosition &sel2, size_t log_num_delete_items);

	/**
	 * 一行分のレイアウトを行い、DisplayLinesに追加する
	 * @param buffer_offset	バッファのオフセット
	 */
	void LayoutOneLine(size_t buffer_offset);

	/**
	 * 一行分の表示内容を作成する
	 * @param item	ログアイテム
	 * @return	表示内容
	 */
	wxString CreateOneLineString(const tLogger::tItem & item);

	/**
	 * 全てのログの行のレイアウトをし直す
	 */
	void LayoutAllLines();

	/**
	 * 一行分をDisplayLinesに追加し、レイアウトを行う
	 * @param logger_item	tLogger::tItem 型
	 */
	void AddLine(const tLogger::tItem & logger_item);

	/**
	 * スクロールを行う
	 * @param amount	スクロール量
	 * @param absolute	amount が絶対位置による指定かどうか
	 */
	void DoScroll(risse_int amount, bool absolute);

	/**
	 * スクロールバーの情報を設定する
	 * @param anchor	スクロール位置のアンカー (-1: ログの最後、それ以外は表示行インデックス)
	 */
	void SetScrollBarInfo(risse_int anchor);

	/**
	 * スクロール位置のアンカー (どの表示部分を常に表示し続けるか) を得る
	 * @return	アンカー (-1: ログの最後、それ以外は表示行インデックス)
	 */
	risse_int GetScrollAnchor();

	/**
	 * ビューをスクロールする
	 * @param old_top	スクロール前の最上行の表示行インデックス
	 * @param new_top	スクロール後の最上行の表示行インデックス
	 */
	void ScrollView(risse_int old_top, risse_int new_top);

	/**
	 * 指定された表示行を再描画する
	 * @param index	表示行インデックス
	 * @param range	範囲
	 */
	void RefreshDisplayLine(size_t index, risse_int range);

	/**
	 * ビュー内の位置から文字位置を割り出す
	 * @param x				ビュー内の位置x
	 * @param y				ビュー内の位置y
	 * @param charpos		文字位置を格納する先(論理位置)
	 * @param charlength	その行の文字長さを格納する先(NULL可)
	 */
	void ViewPositionToCharacterPosition(risse_int x, risse_int y, tCharacterPosition & charpos, size_t *charlength = NULL);

	/**
	 * ビュー内の位置が選択範囲内かどうかを判断する
	 * @param x	ビュー内の位置x
	 * @param y	ビュー内の位置y
	 * @return	範囲内かどうか
	 */
	bool IsViewPositionInSelection(risse_int x, risse_int y);

	/**
	 * 選択が行われているかどうかを返す
	 * @return	選択が行われているかどうか
	 */
	bool AnySelected();

	/**
	 * 選択範囲の文字列を取得する
	 */
	wxString GetSelectionString();

	/**
	 * 選択範囲を設定する
	 * @param pos1	選択範囲の始点終点(論理位置)
	 * @param pos2	選択範囲の始点終点(論理位置)
	 * @param pos3	選択範囲の始点終点(論理位置)
	 * @note	min(pos1, pos2, pos3) ～ max(pos1, pos2, pos3) が選択される
	 */
	void SetSelection(const tCharacterPosition & pos1,
		const tCharacterPosition & pos2,
		const tCharacterPosition & pos3);

	/**
	 * すべてを選択する
	 */
	void SelectAll();

	/**
	 * 指定された範囲を再描画する
	 * @param pos1	範囲の始点(論理位置)
	 * @param pos2	範囲の終点(論理位置)
	 * @note	pos1 > pos2 でもかまわない(自動で判別する)
	 */
	void RefreshSelection(const tCharacterPosition & pos1, const tCharacterPosition & pos2);

	/**
	 * 論理位置から表示位置への変換
	 * @param log_pos	論理位置
	 * @param disp_pos	表示位置
	 */
	void LogicalPositionToDisplayPosition(const tCharacterPosition & log_pos,
				tCharacterPosition & disp_pos);

	/**
	 * タイマーにより定期的に呼び出され、スクロールを行うメソッド
	 */
	void ScrollByTimer();

	/**
	 * コンテキストメニューを表示する
	 * @param pos	表示位置
	 */
	void ShowContextMenu(const wxPoint & pos);

public:
	/**
	 * tLogReceiver::OnLog のオーバーライド
	 * @param logger_item	tLogger::tItem 型
	 * @note	このメソッドはメインスレッド以外から呼ばれる可能性がある
	 */
	void OnLog(const tLogger::tItem & logger_item); // tLogReceiver の override

private:
	/**
	 * 行が追加されたとき
	 * @param event	イベントオブジェクト
	 */
	void OnLineAdded(wxCommandEvent& event);

	/**
	 * 行が追加されたとき
	 * @param event	イベントオブジェクト
	 */
	void OnDoRotationRefresh(wxCommandEvent& event);

	/**
	 * 描画が必要なとき
	 * @param event	イベントオブジェクト
	 */
	void OnPaint(wxPaintEvent& event);

	/**
	 * キーが押されたとき
	 */
	void OnChar(wxKeyEvent & event);

	/**
	 * スクロールされたとき
	 * @param event	イベントオブジェクト
	 */
	void OnScroll(wxScrollWinEvent& event);

	/**
	 * マウスホイールが操作されたとき
	 * @param event	イベントオブジェクト
	 */
	void OnMouseWheel(wxMouseEvent& event);

	/**
	 * サイズが変更されたとき
	 * @param event	イベントオブジェクト
	 */
	void OnSize(wxSizeEvent& event);

	/**
	 * マウスの左ボタンが押されたとき
	 * @param event	イベントオブジェクト
	 */
	void OnLeftDown(wxMouseEvent & event);

	/**
	 * マウスの左ボタンが離されたとき
	 * @param event	イベントオブジェクト
	 */
	void OnLeftUp(wxMouseEvent & event);

	/**
	 * マウスが移動したとき
	 * @param event	イベントオブジェクト
	 */
	void OnMotion(wxMouseEvent & event);

	/**
	 * コンテキストメニューの Copy が選択されたとき
	 * @param event	イベントオブジェクト
	 */
	void OnMenuCopy(wxCommandEvent &event);

	/**
	 * コンテキストメニューの「すべて選択」が選択されたとき
	 * @param event	イベントオブジェクト
	 */
	void OnMenuSelectAll(wxCommandEvent &event);

#if USE_CONTEXT_MENU
	/**
	 * コンテキストメニューを表示するとき
	 * @param event	イベントオブジェクト
	 */
	void OnContextMenu(wxContextMenuEvent& event);
#else
	void OnRightUp(wxMouseEvent& event)
		{ ShowContextMenu(event.GetPosition()); }
#endif

	/**
	 * イベントテーブルの定義
	 */
	DECLARE_EVENT_TABLE()

};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif
