//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief コンソールフレーム
//---------------------------------------------------------------------------
#ifndef _CONSOLE_H
#define _CONSOLE_H

#include "risa/common/ui/UIUtils.h"
#include "risa/packages/risa/event/Event.h"

namespace Risa {
//---------------------------------------------------------------------------

class tLogScrollView;
class tLogViewerStatusBar;
//---------------------------------------------------------------------------
/**
 * コンソールのフレーム
 */
class tConsoleFrame : public tUIFrame,
					 public tEventSystem::tStateListener,
						depends_on<tEventSystem>
{
	enum
	{
		ID_First = 100,
		ID_Event
	};
public:
	/**
	 * コンストラクタ
	 */
	tConsoleFrame();

	/**
	 * デストラクタ
	 */
	~tConsoleFrame();

	/**
	 * ログビューアにフォーカスを合わせる
	 */
	void SetFocusToLogViewer();

	/**
	 * テキストコントロールにフォーカスを合わせる
	 * @param insert_code	テキストコントロールにフォーカスを合わせた際に
	 *						押されたキー (必要ならばこれを挿入する)
	 */
	void SetFocusToTextCtrl(int insert_code = 0);

protected: // tEventSystem::tStateListener
	void OnCanDeliverEventsChanged(bool b);

private:
	tLogScrollView *ScrollView; //!< スクロールエリア
	tLogViewerStatusBar *StatusBar; //!< ステータスバー

	/**
	 * "Event" ボタンが押された
	 * @param event	イベントオブジェクト
	 */
	void OnEventTool(wxCommandEvent & event);

	/**
	 * イベントの状態が変化した
	 * @param event	イベントオブジェクト
	 */
	void OnEventStatusChanged(wxCommandEvent & event);

	/**
	 * イベントテーブルの定義
	 */
	DECLARE_EVENT_TABLE()
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


#endif
