//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief UI用ユーティリティ
//---------------------------------------------------------------------------
#ifndef _UIUTILS_H
#define _UIUTILS_H

#include <wx/frame.h>
#include "base/config/ConfigData.h"

//---------------------------------------------------------------------------
//! @brief		Risa UI 用フレームの基本クラス
//---------------------------------------------------------------------------
class tRisaUIFrame : public wxFrame, protected depends_on<tRisaConfig>
{
	wxString FrameId; //!< フレームの Id (位置を記録したりするのに使う)

public:
	tRisaUIFrame(const wxString & id, const wxString & title);
	~tRisaUIFrame();

private:
	wxPoint GetStoredPosition(const wxString & id);
	wxSize GetStoredSize(const wxString & id);
};
//---------------------------------------------------------------------------


#endif
