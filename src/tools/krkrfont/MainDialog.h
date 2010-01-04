//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief メインダイアログの実装
//---------------------------------------------------------------------------
#ifndef RISA_MAIN_DIALOG_H
#define RISA_MAIN_DIALOG_H

#include "krkrfont_res.h"
#include "FreeType.h"

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * メインダイアログクラス
 */
class wxMainDialog:public wxMainDialog_Base
{
	tFreeTypeFace *Face; //!< Faceオブジェクト

public:
	wxMainDialog();
	~wxMainDialog();

	tFreeTypeFace * GetFace() { return Face; } 	

private:
	void UpdateFace();
	void UpdateFaceIndex();
	void UpdateFaceHeight();
	void Update();

public:
	// イベントハンドラ
	void SelectFontChoiceSelected(wxCommandEvent& event);
	void SizeSpinEditChange(wxSpinEvent& event);
	void DialogClose(wxCloseEvent& event);
	void PreviewButtonClick(wxCommandEvent& event);
	void CloseButtonClick(wxCommandEvent& event);
	void GenerateButtonClick(wxCommandEvent& event);
	void FaceNameChoiceSelected(wxCommandEvent& event);
	void SelectFontRadioButtonClick(wxCommandEvent& event);
	void SelectFileRadioButtonClick(wxCommandEvent& event);
	void AntialiasedCheckBoxClicked(wxCommandEvent & event);
	void BoldCheckBoxClicked(wxCommandEvent & event);
	void NoHintingCheckBoxClicked(wxCommandEvent & event);
	void ForceAutoHintingCheckBoxClicked(wxCommandEvent & event);

	void SelectFileRefButtonClick(wxCommandEvent& event);
	void OutputFileNameRefButtonClick(wxCommandEvent& event);

	DECLARE_EVENT_TABLE();

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


#endif
