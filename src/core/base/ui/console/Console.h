//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief コンソールフレーム
//---------------------------------------------------------------------------
#ifndef _CONSOLE_H
#define _CONSOLE_H

#include "base/ui/UIUtils.h"
#include "base/event/Event.h"


class tRisaLogScrollView;
class tRisaLogViewerStatusBar;
//---------------------------------------------------------------------------
//! @brief		コンソールのフレーム
//---------------------------------------------------------------------------
class tRisaConsoleFrame : public tRisaUIFrame/*, TODO: handle this 
						depends_on<tRisaEventSystem> */
{
	enum
	{
		ID_First = 100,
		ID_Event
	};
public:
	//! @brief		コンストラクタ
	tRisaConsoleFrame();

	//! @brief		ログビューアにフォーカスを合わせる
	void SetFocusToLogViewer();

	//! @brief		テキストコントロールにフォーカスを合わせる
	//! @param		insert_code		テキストコントロールにフォーカスを合わせた際に
	//!								押されたキー (必要ならばこれを挿入する)
	void SetFocusToTextCtrl(int insert_code = 0);

private:
	tRisaLogScrollView *ScrollView; //!< スクロールエリア
	tRisaLogViewerStatusBar *StatusBar; //!< ステータスバー

	//! @brief		"Event" ボタンが押された
	//! @param		event イベントオブジェクト
	void OnEventTool(wxCommandEvent & event);

	//! @brief		UI アップデートイベントが発生したとき
	//! @param		event イベントオブジェクト
	void OnUpdateUI(wxUpdateUIEvent & event);

	//! @brief		イベントテーブルの定義
	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------


#endif
