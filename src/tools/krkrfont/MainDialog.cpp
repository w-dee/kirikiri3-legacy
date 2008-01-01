//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

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
#include "WriteBFF.h"
#include <wx/filedlg.h>

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief		イベントハンドラ定義
//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE(wxMainDialog,wxMainDialog_Base)
	EVT_CLOSE(wxMainDialog::DialogClose)
	EVT_CHOICE(XRCID("SelectFontChoice"), wxMainDialog::SelectFontChoiceSelected)
	EVT_SPINCTRL(XRCID("SizeSpinEdit"), wxMainDialog::SizeSpinEditChange) 
	EVT_BUTTON(XRCID("PreviewButton"), wxMainDialog::PreviewButtonClick)
	EVT_BUTTON(XRCID("CloseButton"), wxMainDialog::CloseButtonClick)
	EVT_BUTTON(XRCID("GenerateButton"), wxMainDialog::GenerateButtonClick)
	EVT_RADIOBUTTON(XRCID("SelectFontRadioButton"), wxMainDialog::SelectFontRadioButtonClick)
	EVT_RADIOBUTTON(XRCID("SelectFileRadioButton"), wxMainDialog::SelectFileRadioButtonClick)
	EVT_CHOICE(XRCID("FaceNameChoice"), wxMainDialog::FaceNameChoiceSelected)
	EVT_CHECKBOX(XRCID("AntialiasedCheckBox"), wxMainDialog::AntialiasedCheckBoxClicked)
	EVT_CHECKBOX(XRCID("BoldCheckBox"), wxMainDialog::BoldCheckBoxClicked)
	EVT_CHECKBOX(XRCID("NoHintingCheckBox"), wxMainDialog::NoHintingCheckBoxClicked)
	EVT_CHECKBOX(XRCID("ForceAutoHintingCheckBox"), wxMainDialog::ForceAutoHintingCheckBoxClicked)
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
	tFreeTypeFontDriver::EnumerateFonts(array, 0);
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
		risse_int face_index = FaceNameChoice->GetSelection();
		if(face_index < 0) face_index = 0;
		risse_uint32 options =
			RISA__GET_FACE_INDEX_FROM_OPTIONS(face_index);
		if(BoldCheckBox->GetValue())
			options |= RISA__TF_BOLD;
		if(!AntialiasedCheckBox->GetValue())
			options |= RISA__FACE_OPTIONS_NO_ANTIALIASING;
		if(NoHintingCheckBox->GetValue())
			options |= RISA__FACE_OPTIONS_NO_HINTING;
		if(ForceAutoHintingCheckBox->GetValue())
			options |= RISA__FACE_OPTIONS_FORCE_AUTO_HINTING;
		if(SelectFileRadioButton->GetValue())
		{
			// ファイルによる指定
			Face = new tFreeTypeFace(
				SelectFileEdit->GetLabel(), options | RISA__FACE_OPTIONS_FILE);
		}
		else
		{
			// フォント名による指定
			Face = new tFreeTypeFace(
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
//! @brief		UpdateFace と UpdateFaceHeight と NotifyPreviewFrameFaceChanged の組み合わせ
//---------------------------------------------------------------------------
void wxMainDialog::Update()
{
	UpdateFace();
	UpdateFaceHeight();
	NotifyPreviewFrameFaceChanged();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		SelectFontChoice のアイテムが選択されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::SelectFontChoiceSelected(wxCommandEvent& event)
{
	SelectFontRadioButton->SetValue(true);
	Update();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		フォントサイズのspineditの内容が変更された場合
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::SizeSpinEditChange(wxSpinEvent& event)
{
	UpdateFaceHeight();
	NotifyPreviewFrameFaceChanged();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ダイアログが閉じるとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::DialogClose(wxCloseEvent& event)
{
	// Close() は Dialog では使わないこと; 代わりに Destroy を使う
	Destroy();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「プレビュー」ボタンが押されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::PreviewButtonClick(wxCommandEvent& event)
{
	ShowPreviewFrame(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「閉じる」ボタンが押されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::CloseButtonClick(wxCommandEvent& event)
{
	// Close() は Dialog では使わないこと; 代わりに Destroy を使う
	Destroy();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「作成」ボタンが押されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::GenerateButtonClick(wxCommandEvent& event)
{
	// BFF ファイルを書き出す
	if(Face)
	{
		if(OutputFileNameEdit->GetLabel().IsEmpty())
		{
			// 出力ファイル欄にまだ何も入力されていなければ
			// ファイル選択のダイアログボックスを開く
			OutputFileNameRefButtonClick(event);
			if(OutputFileNameEdit->GetLabel().IsEmpty()) return;
		}

		WriteGlyphBitmap(Face, OutputFileNameEdit->GetLabel(),
			!GenerateFontMetricsOnlyCheckBox->GetValue(), false,
			this);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「フォントファミリ名」の項目が選択された場合
//! @param		event イベントオブジェクト
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
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::SelectFontRadioButtonClick(wxCommandEvent& event)
{
	Update();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「ファイルを指定」ラジオボタンが押されたとき
//! @param		event イベントオブジェクト
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
		Update();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「アンチエイリアス」チェックボックスが押されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::AntialiasedCheckBoxClicked(wxCommandEvent & event)
{
	Update();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「太字」チェックボックスが押されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::BoldCheckBoxClicked(wxCommandEvent & event)
{
	Update();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「ヒンティングを行わない」チェックボックスが押されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::NoHintingCheckBoxClicked(wxCommandEvent & event)
{
	Update();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「自動ヒンティングを行う」チェックボックスが押されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::ForceAutoHintingCheckBoxClicked(wxCommandEvent & event)
{
	Update();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「Select a file」の横の ... ボタンが押されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::SelectFileRefButtonClick(wxCommandEvent& event)
{
	wxString filename = ::wxFileSelector(
		// messsage for select a font file
		_("Select a font file"), 
		// default_path
		::wxEmptyString,
		// default_filename
		SelectFileEdit->GetLabel(),
		// default extension for selecting a font file
		wxT("ttf"),
		// filename filter for selecting a font file
		wxString() + 
		_("All supported fonts|*.ttf;*.ttc;*.otf;*.fon")	+ wxT("|") +
		_("TrueType fonts (*.ttf)|*.ttf")					+ wxT("|") +
		_("TrueType Collection fonts (*.ttc)|*.ttc")		+ wxT("|") +
		_("OpenType fonts (*.otf)|*.otf")					+ wxT("|") +
		_("All files (*.*)|*.*"),
		// flags
		wxFD_OPEN|wxFD_FILE_MUST_EXIST
		);
	if(!filename.empty())
	{
		if(!SelectFileRadioButton->GetValue())
			SelectFileRadioButton->SetValue(true);
		SelectFileEdit->SetLabel(filename);
		Update();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		「出力ファイル」の横の ... ボタンが押されたとき
//! @param		event イベントオブジェクト
//---------------------------------------------------------------------------
void wxMainDialog::OutputFileNameRefButtonClick(wxCommandEvent& event)
{
	wxString filename = ::wxFileSelector(
		// messsage for select output file
		_("Select output file"), 
		// default_path
		::wxEmptyString,
		// default_filename
		OutputFileNameEdit->GetLabel(),
		// default extension for selecting output file (must be bff)
		wxT("bff"),
		// filename filter for selecting output file
		wxString() + 
		_("Kirikiri3 bitmap font (*.bff)|*.bff")	+ wxT("|") +
		_("All files (*.*)|*.*"),
		// flags
		wxFD_SAVE|wxFD_OVERWRITE_PROMPT
		);
	if(!filename.empty())
	{
		OutputFileNameEdit->SetLabel(filename);
	}
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
} // namespace Risa

