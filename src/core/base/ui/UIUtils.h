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
//! @brief		Risa UI 用フレームの基本基底クラス
//! @note		このクラスから派生するクラスがそのコンストラクタ～デストラクタ間
//!				すべてで config を参照できることを確実にする
//---------------------------------------------------------------------------
class tRisaUIFrameBase : public wxFrame
{
	tRisaSingleton<tRisaConfig> ref_tRisaConfig; //!< tRisaConfig に依存
public:
	//! @brief コンストラクタ
	tRisaUIFrameBase(wxWindow* parent,
		wxWindowID id,
		const wxString& title,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_FRAME_STYLE,
		const wxString& name = wxT("frame")) :
			wxFrame(parent, id, title, pos, size, style, name)
	{
		;
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Risa UI 用フレームの基本クラス
//---------------------------------------------------------------------------
class tRisaUIFrame : public tRisaUIFrameBase
{
	wxString FrameId; //!< フレームの Id (位置を記録したりするのに使う)

public:
	tRisaUIFrame(const wxString & id, const wxString & title);
	~tRisaUIFrame();

private:
	static wxPoint GetStoredPosition(const wxString & id);
	static wxSize GetStoredSize(const wxString & id);
};
//---------------------------------------------------------------------------


#endif
