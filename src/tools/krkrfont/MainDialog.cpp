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
#include "MainDialog.h"
#include "FreeType.h"
#include "PreviewWindow.h"


//---------------------------------------------------------------------------
//! @brief		イベントハンドラ定義
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(wxMainDialog,wxMainDialog_Base)
	EVT_CLOSE(wxMainDialog::DialogClose)
	EVT_CHOICE(XRCID("SelectFontChoice"), wxMainDialog::SelectFontChoiceSelected)
	EVT_SPINCTRL(XRCID("SizeSpinEdit"), wxMainDialog::SizeSpinEditChange) 
	EVT_BUTTON(XRCID("PreviewButton"), wxMainDialog::PreviewButtonClick)
	EVT_BUTTON(XRCID("CloseButton"), wxMainDialog::CloseButtonClick)
	EVT_RADIOBUTTON(XRCID("SelectFontRadioButton"), wxMainDialog::SelectFontRadioButtonClick)
	EVT_RADIOBUTTON(XRCID("SelectFileRadioButton"), wxMainDialog::SelectFileRadioButtonClick)
	EVT_CHOICE(XRCID("FaceNameChoice"), wxMainDialog::FaceNameChoiceSelected)
	EVT_BUTTON(XRCID("SelectFileRefButton"), wxMainDialog::SelectFileRefButtonClick)
	EVT_BUTTON(XRCID("OutputFileNameRefButton"), wxMainDialog::OutputFileNameRefButtonClick)
END_EVENT_TABLE()
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
wxMainDialog::wxMainDialog()
{
	// フィールドのクリア
	Face = NULL;
	
	// 使用可能なフォントを列挙する
	wxArrayString array;
	tTVPFreeTypeFontDriver::EnumerateFonts(array, 0);
	int n = array.Count();
	for(int i = 0; i < n; i++)
		SelectFontChoice->Append(array[i]);

	// デフォルトのフォントを探し、選択させる

	// Default font initially selected in the main dialog.
	// You can specify font cadidates separated with '|' (vertline)
	// character, like "Tahoma|Times New Roman"
	wxString candidates = _("Tahoma");

	int index = 0;

	// candidates を | で分割し、目的のフォントがリスト内に有ればそれを選択する	
	while(true)
	{
		int vidx = candidates.Find(wxT('|'), true);
		wxString candidate;
		bool done = false;
		if(vidx == -1)
		{
			candidate = candidates;
			done = true;
		}
		else
		{
			candidate = candidates.Mid(vidx + 1);
			candidates = candidates.Mid(0, vidx);
		}

		index = SelectFontChoice->FindString(candidate);
		if(index != wxNOT_FOUND) break;
		if(done) break;
	}
	SelectFontChoice->SetSelection(index);
	UpdateFace();
	UpdateFaceHeight();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
wxMainDialog::~wxMainDialog()
{
	DestroyPreviewFrame(); // プレビューウィンドウを閉じる
	if(Face) delete Face;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Face オブジェクトを、現在選択されているフォント名に従い設定
//---------------------------------------------------------------------------
void wxMainDialog::UpdateFace()
{
	// フォント名、Faceインデックスに従ってFaceオブジェクトを設定
	UpdateFaceIndex();

	// FaceNameChoice に候補を設定
	FaceNameChoice->Clear();
	if(Face)
	{
		wxArrayString list;
		Face->GetFaceNameList(list);
		FaceNameChoice->Append(list);
		FaceNameChoice->SetSelection(0);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Face オブジェクトを、現在選択されているフォント名、Faceインデックスに従い設定
//---------------------------------------------------------------------------
void wxMainDialog::UpdateFaceIndex()
{
	if(Face) delete Face, Face = NULL;

	try
	{
		tjs_int face_index = FaceNameChoice->GetSelection();
		if(face_index < 0) face_index = 0;
		tjs_uint32 options =
			TVP_GET_FACE_INDEX_FROM_OPTIONS(face_index);
		if(SelectFileRadioButton->GetValue())
		{
			// ファイルによる指定
			Face = new tTVPFreeTypeFace(
				SelectFileEdit->GetLabel(), options | TVP_FACE_OPTIONS_FILE);
		}
		else
		{
			// フォント名による指定
			Face = new tTVPFreeTypeFace(
				SelectFontChoice->GetStringSelection(), options);
		}
	}
	catch(const wxString & errmsg)
	{
		wxMessageBox(errmsg);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Face の高さを、現在ダイアログボックスで指定されているサイズに設定
//---------------------------------------------------------------------------
void wxMainDialog::UpdateFaceHeight()
{
	if(Face) Face->SetHeight(SizeSpinEdit->GetValue());
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		SelectFontChoice のアイテムが選択されたとき
//! @param		event: イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::SelectFontChoiceSelected(wxCommandEvent& event)
{
	SelectFontRadioButton->SetValue(true);
	UpdateFace();
	UpdateFaceHeight();
	NotifyPreviewFrameFaceChanged();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		フォントサイズのspineditの内容が変更された場合
//! @param		event: イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::SizeSpinEditChange(wxSpinEvent& event)
{
	UpdateFaceHeight();
	NotifyPreviewFrameFaceChanged();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ダイアログが閉じるとき
//! @param		event: イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::DialogClose(wxCloseEvent& event)
{
	// Close() は Dialog では使わないこと; 代わりに Destroy を使う
	Destroy();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「プレビュー」ボタンが押されたとき
//! @param		event: イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::PreviewButtonClick(wxCommandEvent& event)
{
	ShowPreviewFrame(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「閉じる」ボタンが押されたとき
//! @param		event: イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::CloseButtonClick(wxCommandEvent& event)
{
	// Close() は Dialog では使わないこと; 代わりに Destroy を使う
	Destroy();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「フォントファミリ名」の項目が選択された場合
//! @param		event: イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::FaceNameChoiceSelected(wxCommandEvent& event)
{
	UpdateFaceIndex();
	UpdateFaceHeight();
	NotifyPreviewFrameFaceChanged();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「一覧から選択」ラジオボタンが押されたとき
//! @param		event: イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::SelectFontRadioButtonClick(wxCommandEvent& event)
{
	UpdateFace();
	UpdateFaceHeight();
	NotifyPreviewFrameFaceChanged();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「ファイルを指定」ラジオボタンが押されたとき
//! @param		event: イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::SelectFileRadioButtonClick(wxCommandEvent& event)
{
	if(SelectFileEdit->GetLabel().empty())
		SelectFileRefButtonClick(event);
	if(SelectFileEdit->GetLabel().empty())
	{
		SelectFontRadioButton->SetValue(true);
	}
	else
	{
		UpdateFace();
		UpdateFaceHeight();
		NotifyPreviewFrameFaceChanged();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「Select a file」の横の ... ボタンが押されたとき
//! @param		event: イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::SelectFileRefButtonClick(wxCommandEvent& event)
{
	wxString filename = ::wxFileSelector(
		// messsage for select a font file
		_("Select a font file"), 
		// default_path
		::wxEmptyString,
		// default_filename
		OutputFileNameEdit->GetLabel(),
		// default extension for selecting a font file
		wxT("ttf"),
		// filename filter for selecting a font file
		wxString() + 
		_("All supported fonts|*.ttf;*.ttc;*.otf;*.fon")	+ wxT("|") +
		_("TrueType fonts (*.ttf)|*.ttf")					+ wxT("|") +
		_("TrueType Collection fonts (*.ttc)|*.ttc")		+ wxT("|") +
		_("OpenType fonts (*.otf)|*.otf")					+ wxT("|") +
		_("OpenType fonts (*.otf)|*.otf")					+ wxT("|") +
		_("Windows fonts (*.fon)|*.fon")					+ wxT("|") +
		_("All files (*.*)|*.*"),
		// flags
		wxHIDE_READONLY|wxOPEN|wxFILE_MUST_EXIST
		);
	if(!filename.empty())
	{
		if(!SelectFileRadioButton->GetValue())
			SelectFileRadioButton->SetValue(true);
		SelectFileEdit->SetLabel(filename);
		UpdateFace();
		UpdateFaceHeight();
		NotifyPreviewFrameFaceChanged();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「出力ファイル」の横の ... ボタンが押されたとき
//! @param		event: イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::OutputFileNameRefButtonClick(wxCommandEvent& event)
{
}
//---------------------------------------------------------------------------






