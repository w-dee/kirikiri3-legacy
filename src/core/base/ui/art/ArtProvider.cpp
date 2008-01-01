//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risaのアイコンなどのプロバイダ
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/utils/Singleton.h"
#include <wx/artprov.h>

RISSE_DEFINE_SOURCE_ID(1864,60513,53990,19952,46254,61777,48308,42907);

namespace Risa {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// このプロバイダが提供するビットマップ
//---------------------------------------------------------------------------
#include "right_triangle.xpm"
#include "right_triangle_small.xpm"
#include "event.xpm"
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief Risa用ArtProvider
//---------------------------------------------------------------------------
class tArtProvider : public wxArtProvider
{
protected:
	virtual wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client,
									const wxSize& size);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief ビットマップを作成して返す
//---------------------------------------------------------------------------
wxBitmap tArtProvider::CreateBitmap(const wxArtID& id,
									 const wxArtClient& client,
									 const wxSize& size)
{
	if(id == wxT("RightTriangle"))
	{
		if(size != wxDefaultSize && size.GetWidth() <= 12 && size.GetHeight() <= 12)
			return wxBitmap(right_triangle_small_xpm);
		else
			return wxBitmap(right_triangle_xpm);
	}
	if(id == wxT("Event"))
	{
		return wxBitmap(event_xpm);
	}
	return wxNullBitmap;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tArtProvider を登録するためのシングルトン
//---------------------------------------------------------------------------
class tArtProviderRegisterer : public singleton_base<tArtProviderRegisterer>
{
	tArtProvider * Provider;
public:
	//! @brief コンストラクタ
	tArtProviderRegisterer()
	{
		Provider = new tArtProvider;
		wxArtProvider::Push(Provider);
	}
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


