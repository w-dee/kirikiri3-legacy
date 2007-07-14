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
#ifndef _ConfigH_
#define _ConfigH_

#include "base/utils/Singleton.h"
#include <wx/fileconf.h>

namespace Risa {
//---------------------------------------------------------------------------

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



class tConfig;
//---------------------------------------------------------------------------
//! @brief		設定情報のデータを管理するクラス
//! @note		このクラスは GC 管理下ではないのでインスタンスを明示的にdeleteする必要あり
//---------------------------------------------------------------------------
class tConfigData : public wxFileConfig
{
	friend class tConfig;

protected:

	//! @brief		コンストラクタ
	//! @param		filename ファイル名
	tConfigData(const wxString & filename);

	//! @brief		デストラクタ
	~tConfigData();

private:
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		設定情報管理クラス
//---------------------------------------------------------------------------
class tConfig : public singleton_base<tConfig>
{
	tConfigData Variable; //!< Variable設定情報
	tConfigData System; //!< System設定情報

public:
	//! @brief		コンストラクタ
	tConfig();

	//! @brief		デストラクタ
	~tConfig();

public:
	tConfigData & GetVariableConfig() { return Variable; } //!< Variable設定情報を返す
	tConfigData & GetSystemConfig()   { return System;   } //!< System設定情報を返す

private:
	//! @brief		設定ファイルのファイル名を得る
	//! @param		realm		レルム ( "system" か "variable" )
	//! @return		ファイル名
	static wxString GetConfigFileName(const wxString &realm);
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif
