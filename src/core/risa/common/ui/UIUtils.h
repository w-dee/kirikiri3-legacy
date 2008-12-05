//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief UI用ユーティリティ
//---------------------------------------------------------------------------
#ifndef _UIUTILS_H
#define _UIUTILS_H

#include <wx/frame.h>
#include "risa/common/ConfigData.h"


namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * Risa UI 用フレームの基本クラス
 */
class tUIFrame : public wxFrame, protected depends_on<tConfig>
{
	wxString FrameId; //!< フレームの Id (位置を記録したりするのに使う)

public:
	/**
	 * コンストラクタ
	 */
	tUIFrame(const wxString & id, const wxString & title);

	/**
	 * デストラクタ
	 */
	~tUIFrame();

private:
	/**
	 * config に格納された位置情報を読み出す
	 */
	wxPoint GetStoredPosition(const wxString & id);

	/**
	 * config に格納されたサイズ情報を読み出す
	 */
	wxSize GetStoredSize(const wxString & id);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


#endif
