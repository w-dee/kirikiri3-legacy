//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief テスト用のテキストプロパティ
//---------------------------------------------------------------------------
#ifndef RINATEXTPROPERTY_H
#define RINATEXTPROPERTY_H

#include "visual/rina/rinaProperty.h"
#include "visual/rina/test/rina1DPositionProperty.h"

namespace Rina {
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		キャプションプロパティ情報
//---------------------------------------------------------------------------
class tCaptionPropertyInfo :
	public tPropertyInfo,
	public Risa::singleton_base<tCaptionPropertyInfo>,
	Risa::manual_start<tCaptionPropertyInfo>
{
	typedef tPropertyInfo inherited;

public:
	const risse_size ID_Caption; // "caption" プロパティの ID

	//! @brief		コンストラクタ
	tCaptionPropertyInfo();
};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
}

#endif
