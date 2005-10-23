//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief メインダイアログの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include <wx/vscroll.h>
#include "PreviewWindow.h"
#include "MainDialog.h"


#define TVP_PREVIEW_MARGIN_RATIO 0.5
#define TVP_PREVIEW_MARGIN_MIN   1

//---------------------------------------------------------------------------
//! @brief		プレビューウィンドウのクライアントとなるスクロールウィンドウクラス
//---------------------------------------------------------------------------
class wxPreviewScrolledWindow : public wxVScrolledWindow
{
public:
	wxPreviewScrolledWindow(wxWindow * parent);
	~wxPreviewScrolledWindow();

private:
	tjs_int GetNCharsInLine() const;
public:
	void PushTopLeftIndex();
	void UpdateDisplayInfo();
	void SetMagnify(tjs_int magnify);
private:
	wxCoord OnGetLineHeight(size_t n) const;
	tjs_uint GetMargin() const;
	void PrintCharacter(tjs_char ch, int x, int y);

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
	typedef std::map<tjs_int, bool> tIndexToUpdate;
	tIndexToUpdate IndexToUpdate; //!< 更新すべき文字を含んだリスト
	tjs_int LastStatusedIndex; //!< 最後にステータスバーに表示した index 情報
	tjs_uint Magnify; //!< 拡大率
	tjs_uint LastTopLeftIndex; //!< 最後に左上に表示していた文字のインデックス
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		イベントテーブルの定義
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(wxPreviewScrolledWindow, wxVScrolledWindow)
	EVT_IDLE(wxPreviewScrolledWindow::OnIdle)
	EVT_PAINT(wxPreviewScrolledWindow::OnPaint)
	EVT_SCROLLWIN(wxPreviewScrolledWindow::OnScroll)
	EVT_SIZE(wxPreviewScrolledWindow::OnSize)
	EVT_MOTION(wxPreviewScrolledWindow::OnMotion)
	EVT_LEAVE_WINDOW(wxPreviewScrolledWindow::OnLeaveWindow)
END_EVENT_TABLE()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
wxPreviewScrolledWindow::wxPreviewScrolledWindow(wxWindow * parent)	:
	wxVScrolledWindow(parent)
{
	LastTopLeftIndex = 0;
	LastStatusedIndex = -1;
	Magnify = 1;
	SetBackgroundColour(*wxWHITE);
	UpdateDisplayInfo();
	Show();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
wxPreviewScrolledWindow::~wxPreviewScrolledWindow()
{
}
//---------------------------------------------------------------------------


#define FACE wxStaticCast(wxWindow::GetParent(), wxPreviewFrame)->GetMainDialog()->GetFace()


//---------------------------------------------------------------------------
//! @brief		一行に何文字はいるかを現在のクライアントサイズから計算する
//! @return		一行に入る文字数
//---------------------------------------------------------------------------
tjs_int wxPreviewScrolledWindow::GetNCharsInLine() const
{
	// 横に何文字入るかを計算
	if(!FACE) return 1;

	tjs_int cw = 0, ch = 0;
	GetClientSize(&cw, &ch);

	tjs_int nw = cw / (FACE->GetHeight() * Magnify + GetMargin() * 2);
	if(nw == 0) nw = 1;

	return nw;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		LastTopLeftIndex を更新する
//! @note		この情報は UpdateDisplayInfoで利用される
//---------------------------------------------------------------------------
void wxPreviewScrolledWindow::PushTopLeftIndex()
{
	tjs_int nw = GetNCharsInLine();
	size_t lineFirst = GetFirstVisibleLine();
	LastTopLeftIndex = lineFirst * nw;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		表示情報を更新する
//---------------------------------------------------------------------------
void wxPreviewScrolledWindow::UpdateDisplayInfo()
{
	// 横に何文字入るかを計算
	if(FACE)
	{
		tjs_int nw = GetNCharsInLine();

		// 縦に何行はいるかを計算
		tjs_int nh = (tjs_int)((FACE->GetGlyphCount() - 1) / nw) + 1;

		// スクロールバーを更新
		SetLineCount(nh);

		nw = GetNCharsInLine();
		ScrollToLine(LastTopLeftIndex / nw);
	}
	else
	{
		SetLineCount(1);
	}

	// 画面を再描画
	Refresh();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		倍率を設定する
//! @param		maginify: 倍率
//---------------------------------------------------------------------------
void wxPreviewScrolledWindow::SetMagnify(tjs_int magnify)
{
	PushTopLeftIndex();
	Magnify = magnify;
	UpdateDisplayInfo();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定行の高さを得る(基底クラスのオーバーライド)
//! @param		n 行
//! @return		その行の高さ (この実装では固定値を返す)
//---------------------------------------------------------------------------
wxCoord wxPreviewScrolledWindow::OnGetLineHeight(size_t n) const
{
	if(!FACE) return 12; // FACE が NULL の場合は特に意味のない値を返す
	return FACE->GetHeight() * Magnify + GetMargin() * 2;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		文字のセルの周りのマージンを得る
//! @return		マージン(ピクセル単位)
//---------------------------------------------------------------------------
tjs_uint wxPreviewScrolledWindow::GetMargin() const
{
	if(!FACE) return TVP_PREVIEW_MARGIN_MIN; // FACE が NULL の場合は特に意味のない値を返す
	tjs_size size = FACE->GetHeight();
	size *= (int)(TVP_PREVIEW_MARGIN_RATIO * 256);
	size /= 256;
	if(size < TVP_PREVIEW_MARGIN_MIN) size = TVP_PREVIEW_MARGIN_MIN;
	return size * Magnify;

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		指定位置にFreeTypeでレンダリングした文字を表示する
//! @param		ch: 表示したい文字
//! @param		x: 表示したいx位置
//! @param		y: 表示したいy位置
//---------------------------------------------------------------------------
void wxPreviewScrolledWindow::PrintCharacter(tjs_char ch, int x, int y)
{
	// 文字のグリフイメージを取得する
	// image の転送先を背景色でクリア
	if(!FACE) return; // FACE が NULL の場合はここで帰る

	tjs_uint margin = GetMargin();
	wxClientDC dc(this);
	dc.SetBrush(*wxWHITE_BRUSH);
	dc.SetPen(*wxTRANSPARENT_PEN);
	wxCoord char_size = FACE->GetHeight() * Magnify + margin * 2;
	dc.DrawRectangle(x, y, char_size, char_size);

	tTVPGlyphBitmap * srcbmp = FACE->GetGlyphFromCharcode(ch);
	if(!srcbmp) return; // 文字が不正か、グリフが空なので表示できない
	try
	{
		// wxImage を作成し、グリフイメージをコピーする
		// 入力のビットマップは 8bpp のグレースケール、出力の image の
		// ビットマップは 24bpp のフルカラー
		wxImage destimage ((int)srcbmp->GetBlackBoxW(),
			(int)srcbmp->GetBlackBoxH(), false);
		unsigned char * destimagedata = destimage.GetData();
		for(tjs_uint line = 0; line < srcbmp->GetBlackBoxH(); line++)
		{
			const tjs_uint8 * srcimagedata = srcbmp->GetData() +
										srcbmp->GetPitch() * line;
			for(tjs_uint x = 0; x < srcbmp->GetBlackBoxW(); x++)
			{
				tjs_uint8 v = 255 - *srcimagedata;
				destimagedata[0] = destimagedata[1] = destimagedata[2] = v;
				destimagedata += 3;
				srcimagedata ++;
			}
		}

		if(Magnify != 1)
		{
			// イメージの拡大を行う
			destimage.Rescale((int)srcbmp->GetBlackBoxW() * Magnify,
				(int)srcbmp->GetBlackBoxH() * Magnify);
		}

		// image をデバイスコンテキストに blt する
		dc.DestroyClippingRegion();
		dc.SetClippingRegion(x, y, char_size, char_size);
		dc.DrawBitmap(
			wxBitmap(destimage),
			margin + x + srcbmp->GetOriginX() * Magnify,
			margin + y + srcbmp->GetOriginY() * Magnify,
			false);
		dc.DestroyClippingRegion();
	}
	catch(...)
	{
		srcbmp->Release();
		throw;
	}
	srcbmp->Release();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		システムがアイドルなとき
//! @param		event: イベントオブジェクト
//---------------------------------------------------------------------------
void wxPreviewScrolledWindow::OnIdle(wxIdleEvent& event)
{
	// IndexToUpdate から一文字を取り出し、それをクライアントに描画する
	if(!FACE) return; // FACE が NULL の場合はここで帰る

	int nw = GetNCharsInLine();
	int glyph_count = FACE->GetGlyphCount();
	tIndexToUpdate::iterator i = IndexToUpdate.end();
	tjs_uint margin = GetMargin();
	int cell_size = (FACE->GetHeight() * Magnify + margin * 2);
	size_t lineFirst = GetFirstVisibleLine(), lineLast = GetLastVisibleLine();

	tjs_int index, cx, cy, vindex;

	while(!IndexToUpdate.empty())
	{
		// 表示可能な範囲内にあるインデックスを探す
		i = IndexToUpdate.begin();
		index = i->first;
		if((tjs_uint)index >= lineFirst * nw && (tjs_uint)index < (lineLast+1) * nw)
			break; // 表示できそう

		// あきらかに表示可能な範囲内を超えている
		// IndexToUpdate から削除し、continue
		IndexToUpdate.erase(i);
		i = IndexToUpdate.end();
	}

	if(i != IndexToUpdate.end())
	{
		// 表示位置を計算
		vindex = index - lineFirst * nw;
		cx = vindex % nw;
		cy = vindex / nw;

		// その位置に文字を表示
		tjs_char ch = index < glyph_count ? FACE->CharcodeFromGlyphIndex(index) : wxT('\0');
		PrintCharacter(ch, cx * cell_size , cy * cell_size );

		// 表示した文字を IndexToUpdate から削除
		IndexToUpdate.erase(i);
	}

	// まだ残りの文字がある場合は、もっとIdleイベントを発生させる
	if(!IndexToUpdate.empty()) event.RequestMore();

	// 現在の左上に表示されている文字のインデックスを保存
	PushTopLeftIndex();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		描画が必要なとき
//! @param		event: イベントオブジェクト
//---------------------------------------------------------------------------
void wxPreviewScrolledWindow::OnPaint(wxPaintEvent& event)
{
	if(!FACE)
	{
		event.Skip();
		// FACE が NULL の場合はここで帰る
		return;
	}

	wxPaintDC dc(this);
	wxMBConvUTF16 conv;

	// 横に何文字入るかを計算
	int nw = GetNCharsInLine();

	int glyph_count = FACE->GetGlyphCount();

	tjs_uint margin = GetMargin();
	int cell_size = (FACE->GetHeight() * Magnify + margin * 2);

	// 表示行を取得
	size_t lineFirst = GetFirstVisibleLine();

	// 更新矩形に従って内容を更新
	wxRegionIterator upd(GetUpdateRegion()); // get the update rect list

	while (upd)
	{
		wxRect rect(upd.GetRect());
		tjs_int cxs = rect.GetLeft() / cell_size;
		tjs_int cxe = (rect.GetRight() - 1) / cell_size;
		tjs_int cys = rect.GetTop() / cell_size;
		tjs_int cye = (rect.GetBottom() - 1) / cell_size;

		if(cxe >= nw) cxe = nw - 1;

		for(tjs_int y = cys; y <= cye; y++)
		{
			for(tjs_int x = cxs; x <= cxe; x++)
			{
				// rect が包含する位置にある文字はダメージを受けたとみなし、
				// IndexToUpdate に入れておく
				tjs_int index = y * nw + x + lineFirst * nw;
				IndexToUpdate.insert(std::pair<tjs_int, bool>(index, false));

				// また、その位置には仮の文字を表示しておく
				wchar_t wcharbuf[5]; // UTF-16 or UTF-32
				tjs_int32 utf32buf[2];
				utf32buf[0] = index < glyph_count ? FACE->CharcodeFromGlyphIndex(index) : wxT('\0');
				utf32buf[1] = wxT('\0');
				conv.MB2WC(wcharbuf, (const char *)&utf32buf, 4);
				wxCoord tw = 0, th = 0;
				dc.GetTextExtent(wcharbuf, &tw, &th);
				dc.DestroyClippingRegion();
				dc.SetClippingRegion(x * cell_size, y * cell_size,
					cell_size, cell_size);
				dc.DrawText(wcharbuf,
					x * cell_size + margin + (cell_size-margin*2) / 2 - tw / 2,
					y * cell_size + margin + (cell_size-margin*2) / 2 - th / 2);
			}
		}
		upd ++;
	}
	dc.DestroyClippingRegion();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		スクロールされるとき
//! @param		event: イベントオブジェクト
//---------------------------------------------------------------------------
void wxPreviewScrolledWindow::OnScroll(wxScrollWinEvent& event)
{
	event.Skip();
	PushTopLeftIndex();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		サイズが変更されたとき
//! @param		event: イベントオブジェクト
//---------------------------------------------------------------------------
void wxPreviewScrolledWindow::OnSize(wxSizeEvent& event)
{
	UpdateDisplayInfo();
	PushTopLeftIndex();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		マウスカーソルが移動したとき
//! @param		event: イベントオブジェクト
//---------------------------------------------------------------------------
void wxPreviewScrolledWindow::OnMotion(wxMouseEvent & event)
{
	// マウスカーソル下の文字をステータスバーに表示
	if(!FACE) return; // FACE が NULL の場合はここで帰る

	if(event.Moving())
	{
		long x = event.GetX();
		long y = event.GetY();
		int nw = GetNCharsInLine();
		int glyph_count = FACE->GetGlyphCount();
		tjs_uint margin = GetMargin();
		int cell_size = (FACE->GetHeight() * Magnify + margin * 2);
		size_t lineFirst = GetFirstVisibleLine();

		x /= cell_size;
		y /= cell_size;
		tjs_int index = y * nw + x + lineFirst * nw;
		if(index != LastStatusedIndex)
		{
			tjs_char ch = index < glyph_count ? FACE->CharcodeFromGlyphIndex(index) : wxT('\0');

			wxString status = wxString::Format(
				_("Index : %d   Charcode : U+%06X"), index, (tjs_int)ch);
			wxStaticCast(wxWindow::GetParent(), wxPreviewFrame)->GetStatusBar()->
				SetStatusText(status, 0);
			LastStatusedIndex = index;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		マウスカーソルがウィンドウから退出したとき
//! @param		event: イベントオブジェクト
//---------------------------------------------------------------------------
void wxPreviewScrolledWindow::OnLeaveWindow(wxMouseEvent & event)
{
	// ステータスバーの文字を消去
	wxStaticCast(wxWindow::GetParent(), wxPreviewFrame)->GetStatusBar()->
		SetStatusText(wxEmptyString, 0);
	LastStatusedIndex = -1;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
// グローバル変数など
//---------------------------------------------------------------------------
wxPreviewFrame * PreviewFrame = NULL;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		プレビューウィンドウを表示する
//! @param		maindialog: メインダイアログオブジェクトへのポインタ
//! @note		まだプレビューウィンドウが作成されていなければ作成し、
//!				既に作成されていたら最前面に表示する
//---------------------------------------------------------------------------
void ShowPreviewFrame(wxMainDialog *maindialog)
{
	if(!PreviewFrame)
		PreviewFrame = new wxPreviewFrame(maindialog);
	else
	{
		PreviewFrame->Raise();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		プレビューウィンドウにFaceやそのプロパティの変更を通知する
//---------------------------------------------------------------------------
void NotifyPreviewFrameFaceChanged()
{
	if(PreviewFrame)
	{
		PreviewFrame->FaceChanged();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		プレビューウィンドウを破棄する
//---------------------------------------------------------------------------
void DestroyPreviewFrame()
{
	if(PreviewFrame) PreviewFrame->Destroy();
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
//! @brief		イベントテーブル定義
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(wxPreviewFrame, wxPreviewFrame_Base)
	EVT_CLOSE(wxPreviewFrame::FrameClose)
	EVT_TOOL(XRCID("CloseButton"), wxPreviewFrame::CloseButtonClick)
	EVT_CHOICE(XRCID("MagnifyChoice"), wxPreviewFrame::MagnifyChoiceSelected)
END_EVENT_TABLE()
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
wxPreviewFrame::wxPreviewFrame(wxMainDialog * maindialog) : MainDialog(maindialog)
{
	PreviewScrolledWindow = NULL;
	SetToolBar(ToolBar);
	CreateStatusBar(2);
	PreviewScrolledWindow = new wxPreviewScrolledWindow(this);
	FaceChanged();
	Show();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
wxPreviewFrame::~wxPreviewFrame()
{
	PreviewFrame = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Faceやそのプロパティが変わったときにMainDialogから呼ばれる
//---------------------------------------------------------------------------
void wxPreviewFrame::FaceChanged()
{
	wxString status2;
	int count = GetMainDialog()->GetFace() ? GetMainDialog()->GetFace()->GetGlyphCount() : 0;
	status2.sprintf(_("Glyph count : %d"), count); 
	GetStatusBar()->SetStatusText(status2, 1);
	PreviewScrolledWindow->UpdateDisplayInfo();
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		ウィンドウが閉じるとき
//! @param		event: イベントオブジェクト
//---------------------------------------------------------------------------
void wxPreviewFrame::FrameClose(wxCloseEvent& event)
{
	Destroy();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		「閉じる」ボタンが押されたとき
//! @param		event: イベントオブジェクト
//---------------------------------------------------------------------------
void wxPreviewFrame::CloseButtonClick(wxCommandEvent& event)
{
	Destroy();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		「倍率」リストボックスが選択されたとき
//! @param		event: イベントオブジェクト
//---------------------------------------------------------------------------
void wxPreviewFrame::MagnifyChoiceSelected(wxCommandEvent& event)
{
	const char itemindextomag[] =
		{ 1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 14, 16 };
	PreviewScrolledWindow->SetMagnify(
		itemindextomag[MagnifyChoice->GetSelection()]);
}
//---------------------------------------------------------------------------




