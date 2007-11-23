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
//! @brief テスト用の1次元位置プロパティ
//---------------------------------------------------------------------------
#ifndef RINA1DPOSITIONPROPERTY_H
#define RINA1DPOSITIONPROPERTY_H

#include "base/utils/Singleton.h"
#include "visual/rina/rinaProperty.h"

namespace Rina {
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		1次元位置プロパティ情報
//---------------------------------------------------------------------------
class t1DPositionPropertyInfo :
	public tPropertyInfo,
	public Risa::singleton_base<t1DPositionPropertyInfo>,
	Risa::manual_start<t1DPositionPropertyInfo>
{
	typedef tPropertyInfo inherited;

public:
	const risse_size ID_Position; // "position" プロパティの ID

	//! @brief		コンストラクタ
	t1DPositionPropertyInfo();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
}

#endif
