//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief プレビューウィンドウの実装
//---------------------------------------------------------------------------
#include "krkrfont_res.h"

namespace Risa {
//---------------------------------------------------------------------------

class wxMainDialog;
class wxPreviewScrolledWindow;

//---------------------------------------------------------------------------
// グローバル関数
//---------------------------------------------------------------------------
void ShowPreviewFrame(wxMainDialog *maindialog);
void DestroyPreviewFrame();
void NotifyPreviewFrameFaceChanged();
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * プレビューウィンドウクラス
 */
class wxFontPreviewFrame:public wxPreviewFrame_Base
{
	wxMainDialog * MainDialog;
	wxPreviewScrolledWindow * PreviewScrolledWindow;
public:
	wxFontPreviewFrame(wxMainDialog * maindialog);
	~wxFontPreviewFrame();

	wxMainDialog * GetMainDialog() { return MainDialog; }
	void FaceChanged();

	void FrameClose(wxCloseEvent& event);
	void CloseButtonClick(wxCommandEvent& event);
	void MagnifyChoiceSelected(wxCommandEvent& event);

	DECLARE_EVENT_TABLE();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


