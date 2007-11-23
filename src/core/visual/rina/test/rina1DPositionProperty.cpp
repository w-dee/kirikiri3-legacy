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
#include "prec.h"
#include "visual/rina/test/rina1DPositionProperty.h"


namespace Rina {
RISSE_DEFINE_SOURCE_ID(50730,34952,26641,18840,23703,16989,3486,52532);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
t1DPositionPropertyInfo::t1DPositionPropertyInfo() :
	inherited()
	, ID_Position(Add(RISSE_WS("position"), RISSE_WS("1D position value")))
{
	
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
}
