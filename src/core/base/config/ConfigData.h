//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

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

/*
@note
	<p>Risa の Config は以下の二つのrealmに分けられている。</p>
<dl>
	<dt>Variable</dt>
	<dd>
	  割と頻繁に書き換わる情報。基本的にシステムの起動ごとに値が変わるような
	  情報はこちらに入れる。デバッグ用 UI の情報などがここに入る。
	</dd>
	<dt>System</dt>
	<dd>
	  こちらの情報はユーザが自由に編集できるような UI を用意する。あまり変更
	  のない情報など。環境依存の不具合の微調整のためのオプションなどがここに
	  入る。
	 </dd>
</dl>
*/



class tRisaConfig;
//---------------------------------------------------------------------------
//! @brief		設定情報のデータを管理するクラス
//---------------------------------------------------------------------------
class tRisaConfigData : public wxFileConfig
{
	friend class tRisaConfig;

protected:

	//! @brief		コンストラクタ
	//! @param		filename ファイル名
	tRisaConfigData(const wxString & filename);

	//! @brief		デストラクタ
	~tRisaConfigData();

private:
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		設定情報管理クラス
//---------------------------------------------------------------------------
class tRisaConfig : public singleton_base<tRisaConfig>
{
	tRisaConfigData Variable; //!< Variable設定情報
	tRisaConfigData System; //!< System設定情報

public:
	//! @brief		コンストラクタ
	tRisaConfig();

	//! @brief		デストラクタ
	~tRisaConfig();

public:
	tRisaConfigData & GetVariableConfig() { return Variable; } //!< Variable設定情報を返す
	tRisaConfigData & GetSystemConfig()   { return System;   } //!< System設定情報を返す

private:
	//! @brief		設定ファイルのファイル名を得る
	//! @param		realm		レルム ( "system" か "variable" )
	//! @return		ファイル名
	static wxString GetConfigFileName(const wxString &realm);
};
//---------------------------------------------------------------------------



#endif
