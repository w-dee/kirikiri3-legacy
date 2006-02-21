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


/*
	Risa の Config は以下の二つのrealmに分けられている。

	・Variable

	  割と頻繁に書き換わる情報。基本的にシステムの起動ごとに値が変わるような
	  情報はこちらに入れる。デバッグ用 UI の情報などがここに入る。

	・System

	  こちらの情報はユーザが自由に編集できるような UI を用意する。あまり変更
	  のない情報など。環境依存の不具合の微調整のためのオプションなどがここに
	  入る。
*/





//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		filename ファイル名
//---------------------------------------------------------------------------
tRisaConfigData::tRisaConfigData(const wxString & filename) :
	wxFileConfig(wxEmptyString, wxEmptyString, filename,
							wxEmptyString, wxCONFIG_USE_LOCAL_FILE)
{
}
//---------------------------------------------------------------------------


















//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaConfig::tRisaConfig() :
	Variable(GetConfigFileName(wxT("variable"))),
	System  (GetConfigFileName(wxT("system")))
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaConfig::~tRisaConfig()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		設定ファイルのファイル名を得る
//! @param		realm		レルム ( "system" か "variable" )
//! @return		ファイル名
//---------------------------------------------------------------------------
wxString tRisaConfig::GetConfigFileName(const wxString &realm)
{
	// 設定ファイルはどこからとってくるかについては議論の余地があるが、
	// System   realm: (実行可能ファイルのパス)/../etc/k3.ini
	// Variable realm: (実行可能ファイルのパス)/../var/k3vars.ini
	// からとってくることにする。

	wxFileName appfilename(wxTheApp->argv[0]);
	wxString appdir = appfilename.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);

	if(realm == wxT("system"  ))
	{
		return appdir + wxT("../etc/k3.ini");
	}
	if(realm == wxT("variable"))
	{
		return appdir + wxT("../var/k3vars.ini");
	}
	return wxEmptyString;
}
//---------------------------------------------------------------------------

