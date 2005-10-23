//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief プレビューウィンドウの実装
//---------------------------------------------------------------------------
#include "resource.h"

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
//! @brief プレビューウィンドウクラス
//---------------------------------------------------------------------------
class wxPreviewFrame:public wxPreviewFrame_Base
{
	wxMainDialog * MainDialog;
	wxPreviewScrolledWindow * PreviewScrolledWindow;
public:
	wxPreviewFrame(wxMainDialog * maindialog);
	~wxPreviewFrame();

	wxMainDialog * GetMainDialog() { return MainDialog; }
	void FaceChanged();

	void FrameClose(wxCloseEvent& event);
	void CloseButtonClick(wxCommandEvent& event);
	void MagnifyChoiceSelected(wxCommandEvent& event);

	DECLARE_EVENT_TABLE();
};
//---------------------------------------------------------------------------


