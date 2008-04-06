//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RINA パッケージ
//---------------------------------------------------------------------------
#ifndef RINA_H
#define RINA_H

#include "risa/common/Singleton.h"
#include "risa/common/RisseEngine.h"

namespace Risa {
//---------------------------------------------------------------------------


class tGraphClass;
class tNodeClass;
class tPinClass;
class tInputPinClass;
class tOutputPinClass;
class tInputPinArrayClass;
//---------------------------------------------------------------------------
//! @brief		risa.graphic.rina パッケージイニシャライザ
//---------------------------------------------------------------------------
class tRisaGraphicRinaPackageInitializer : public tBuiltinPackageInitializer
{
public:
	tGraphClass         * GraphClass;
	tNodeClass          * NodeClass;
	tPinClass           * PinClass;
	tInputPinClass      * InputPinClass;
	tOutputPinClass     * OutputPinClass;
	tInputPinArrayClass * InputPinArrayClass;

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tRisaGraphicRinaPackageInitializer(tScriptEngine * engine);

	//! @brief		パッケージを初期化する
	//! @param		engine		スクリプトエンジンインスタンス
	//! @param		name		パッケージ名
	//! @param		global		パッケージグローバル
	void Initialize(tScriptEngine * engine, const tString & name,
		const tVariant & global);
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
}

#endif
