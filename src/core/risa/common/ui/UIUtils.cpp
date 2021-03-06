//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief UI用ユーティリティ
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/common/ui/UIUtils.h"

namespace Risa {
RISSE_DEFINE_SOURCE_ID(23912,37381,23527,19276,1668,15249,42569,48188);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tUIFrame::tUIFrame(const wxString & id, const wxString & title) :
		wxFrame(NULL, wxID_ANY, title, GetStoredPosition(id), GetStoredSize(id),
		wxDEFAULT_FRAME_STYLE),
		FrameId(id)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tUIFrame::~tUIFrame()
{
	// サイズを config に格納する
	tConfigData & config = tConfig::instance()->GetVariableConfig();

	wxPoint pos = GetPosition();
	wxSize  size= GetSize();

	config.Write(FrameId + wxT("/left"),   static_cast<long>(pos.x));
	config.Write(FrameId + wxT("/top"),    static_cast<long>(pos.y));
	config.Write(FrameId + wxT("/width"),  static_cast<long>(size.GetWidth()));
	config.Write(FrameId + wxT("/height"), static_cast<long>(size.GetHeight()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
wxPoint tUIFrame::GetStoredPosition(const wxString & id)
{
	tConfigData & config = tConfig::instance()->GetVariableConfig();
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
wxSize tUIFrame::GetStoredSize(const wxString & id)
{
	tConfigData & config = tConfig::instance()->GetVariableConfig();
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


//---------------------------------------------------------------------------
} // namespace Risa


