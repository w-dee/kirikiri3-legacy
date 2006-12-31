//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief スクリプトエディタフレーム
//---------------------------------------------------------------------------
#ifndef _SCRIPTEDITOR_H
#define _SCRIPTEDITOR_H

#include "risse/include/risse.h"
#include "base/ui/UIUtils.h"


class tRisaScriptEditorTextCtrl;
class tRisaScriptEditorStatusBar;
//---------------------------------------------------------------------------
//! @brief		スクリプトエディタのフレーム
//---------------------------------------------------------------------------
class tRisaScriptEditorFrame : public tRisaUIFrame
{
public:
	enum
	{
		ID_First = 100,
		ID_TextCtrl,
		ID_Command_Execute,
		ID_Last
	};

public:
	//! @brief		コンストラクタ
	tRisaScriptEditorFrame();

public:
	//! @brief		編集内容を設定する
	//! @param		content		編集内容
	void SetContent(const wxString & content);

	//! @brief		書き込み禁止かどうかを設定する
	//! @param		b		書き込み禁止かどうか
	void SetReadOnly(bool b);

	//! @brief		キャレットを指定行に移動する
	//! @param		pos		論理行
	void SetLinePosition(unsigned long pos);

	//! @brief		ステータスバーの文字列を設定する
	//! @param		status	ステータスバーに表示したい文字列
	void SetStatusString(const wxString & status);

private:
	//! @brief		「実行」メニューや実行ボタンが選択されたとき
	//! @param		event イベントオブジェクト
	tRisaScriptEditorTextCtrl *TextCtrl;

	//! @brief		TextCtrl の UI アップデートイベントが発生したとき
	//! @param		event イベントオブジェクト
	tRisaScriptEditorStatusBar *StatusBar;

	void OnCommandExecute(wxCommandEvent & event);
	void OnTextCtrlUpdateUI(wxUpdateUIEvent & event);

	//! @brief		イベントテーブルの定義
	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------


#endif
