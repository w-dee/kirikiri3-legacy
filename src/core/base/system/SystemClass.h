//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ファイルシステムマネージャ(ファイルシステムの根幹部分)
//---------------------------------------------------------------------------
#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

#include "base/gc/RisaGC.h"
#include "risse/include/risseNativeBinder.h"
#include "risse/include/risseClass.h"


namespace Risa {
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		"System" クラス
//---------------------------------------------------------------------------
class tSystemClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tSystemClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public: // Risse 用メソッドなど
	static void construct() { }
	static void initialize() { }

	static bool confirm(const tString & message, const tMethodArgument & args);
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
} // namespace Risa


#endif
