//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief スクリプトエディタフレーム
//---------------------------------------------------------------------------
#ifndef _SCRIPTEDITOR_H
#define _SCRIPTEDITOR_H

#include "risa/common/ui/UIUtils.h"

namespace Risa {
//---------------------------------------------------------------------------

class tScriptEditorTextCtrl;
class tScriptEditorStatusBar;
//---------------------------------------------------------------------------
/**
 * スクリプトエディタのフレーム
 */
class tScriptEditorFrame : public tUIFrame
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
	/**
	 * コンストラクタ
	 */
	tScriptEditorFrame();

public:
	/**
	 * 編集内容を設定する
	 * @param content	編集内容
	 */
	void SetContent(const wxString & content);

	/**
	 * 書き込み禁止かどうかを設定する
	 * @param b	書き込み禁止かどうか
	 */
	void SetReadOnly(bool b);

	/**
	 * キャレットを指定行に移動する
	 * @param pos	論理行
	 */
	void SetLinePosition(unsigned long pos);

	/**
	 * ステータスバーの文字列を設定する
	 * @param status	ステータスバーに表示したい文字列
	 */
	void SetStatusString(const wxString & status);

private:
	/**
	 * 「実行」メニューや実行ボタンが選択されたとき
	 * @param event	イベントオブジェクト
	 */
	tScriptEditorTextCtrl *TextCtrl;

	/**
	 * TextCtrl の UI アップデートイベントが発生したとき
	 * @param event	イベントオブジェクト
	 */
	tScriptEditorStatusBar *StatusBar;

	void OnCommandExecute(wxCommandEvent & event);
	void OnTextCtrlUpdateUI(wxUpdateUIEvent & event);

	/**
	 * イベントテーブルの定義
	 */
	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa



#endif
