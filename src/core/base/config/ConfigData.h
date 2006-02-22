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
#ifndef _RisaConfigH_
#define _RisaConfigH_

#include "base/utils/Singleton.h"
#include <wx/fileconf.h>


class tRisaConfig;
//---------------------------------------------------------------------------
//! @brief		設定情報のデータを管理するクラス
//---------------------------------------------------------------------------
class tRisaConfigData : public wxFileConfig
{
	friend class tRisaConfig;

protected:
	tRisaConfigData(const wxString & filename);
	~tRisaConfigData();

private:
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		設定情報管理クラス
//---------------------------------------------------------------------------
class tRisaConfig : public singleton_base<tRisaConfig>
{
	tRisaConfigData Variable;
	tRisaConfigData System;

public:
	tRisaConfig();
	~tRisaConfig();

public:
	tRisaConfigData & GetVariableConfig() { return Variable; } //!< Variable設定情報を返す
	tRisaConfigData & GetSystemConfig()   { return System;   } //!< System設定情報を返す

private:
	static wxString GetConfigFileName(const wxString &realm);
};
//---------------------------------------------------------------------------



#endif
