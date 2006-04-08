//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 設定情報管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/config/ConfigData.h"
#include "base/exception/RisaException.h"
#include <wx/filename.h>

RISSE_DEFINE_SOURCE_ID(7464,23094,50611,20409,11701,31915,19285,41427);






//---------------------------------------------------------------------------
tRisaConfigData::tRisaConfigData(const wxString & filename) :
	wxFileConfig(wxEmptyString, wxEmptyString, filename,
							wxEmptyString, wxCONFIG_USE_LOCAL_FILE)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaConfigData::~tRisaConfigData()
{
}
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
tRisaConfig::tRisaConfig() :
	Variable(GetConfigFileName(wxT("variable"))),
	System  (GetConfigFileName(wxT("system")))
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaConfig::~tRisaConfig()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
wxString tRisaConfig::GetConfigFileName(const wxString &realm)
{
	// 設定ファイルはどこからとってくるかについては議論の余地があるが、
	// System   realm: (実行可能ファイルのパス)/../etc/k3.ini
	// Variable realm: (実行可能ファイルのパス)/../var/k3vars.ini
	// からとってくることにする。

	wxFileName appfilename(wxTheApp->argv[0]);
	wxString appdir = appfilename.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);

	wxString ret;

	if     (realm == wxT("system"  ))
		ret = appdir + wxT("../etc/k3.ini");
	else if(realm == wxT("variable"))
		ret = appdir + wxT("../var/k3vars.ini");

	if(ret != wxEmptyString)
	{
		// どうもここでパスの正規化を行わないと
		// wxFileConfig 内で呼んでいる wxFileName::CreateTempFileName が
		// エラーを起こすようなので正規化を行う
		wxFileName abspath(ret);
		abspath.Normalize();
		ret = abspath.GetFullPath();
	}

	wxFprintf(stderr, wxT("config '%s' realm filename: %s\n"), realm.c_str(), ret.c_str());

	return ret;
}
//---------------------------------------------------------------------------

