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
#include "prec.h"
#include "base/ui/UIUtils.h"

RISSE_DEFINE_SOURCE_ID(23912,37381,23527,19276,1668,15249,42569,48188);



//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaUIFrame::tRisaUIFrame(const wxString & id, const wxString & title) :
		tRisaUIFrameBase(NULL, wxID_ANY, title, GetStoredPosition(id), GetStoredSize(id),
		wxDEFAULT_FRAME_STYLE),
		FrameId(id)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaUIFrame::~tRisaUIFrame()
{
	// サイズを config に格納する
	tRisaConfigData & config = tRisaConfig::instance()->GetVariableConfig();

	wxPoint pos = GetPosition();
	wxSize  size= GetSize();

	config.Write(FrameId + wxT("/left"),   static_cast<long>(pos.x));
	config.Write(FrameId + wxT("/top"),    static_cast<long>(pos.y));
	config.Write(FrameId + wxT("/width"),  static_cast<long>(size.GetWidth()));
	config.Write(FrameId + wxT("/height"), static_cast<long>(size.GetHeight()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		config に格納された位置情報を読み出す
//---------------------------------------------------------------------------
wxPoint tRisaUIFrame::GetStoredPosition(const wxString & id)
{
	tRisaConfigData & config = tRisaConfig::instance()->GetVariableConfig();
	wxPoint ret;
	long x, y;
	if( config.Read(id + wxT("/left"), &x) &&
		config.Read(id + wxT("/top"),  &y))
	{
		// 位置が画面をはみ出ていないかどうかを調べる
		// はみ出ている場合は一部でもいいから画面内に収まるように
		// 調整する
		int cx, cy, cw, ch;
		::wxClientDisplayRect(&cx, &cy, &cw, &ch);
		if(x > (cx+cw))  x = (cx+cw) - 100;
		if(y > (cy+ch))  y = (cy+ch) - 100;
		ret.x = x;
		ret.y = y;
		return ret;
	}
	else
	{
		return wxDefaultPosition;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		config に格納されたサイズ情報を読み出す
//---------------------------------------------------------------------------
wxSize tRisaUIFrame::GetStoredSize(const wxString & id)
{
	tRisaConfigData & config = tRisaConfig::instance()->GetVariableConfig();
	wxSize ret;
	long w, h;
	if( config.Read(id + wxT("/width"),  &w) &&
		config.Read(id + wxT("/height"), &h))
	{
		ret.SetWidth(w);
		ret.SetHeight(h);
		return ret;
	}
	else
	{
		return wxDefaultSize;
	}
}
//---------------------------------------------------------------------------


