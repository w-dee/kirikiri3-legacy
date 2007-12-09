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
//! @brief テスト用のテキストピン管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "visual/rina/test/rinaTextPin.h"


namespace Rina {
RISSE_DEFINE_SOURCE_ID(6749,17670,63856,19872,29832,12572,34125,64681);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tTextEdgeTypeRegisterer::tTextEdgeTypeRegisterer()
{
	tIdRegistry::tEdgeData data;
	data.Id = TextEdgeType;
	data.ShortName = RISSE_WS("wideText");
	data.ShortDesc = RISSE_WS("wide string");
	data.LongDesc = RISSE_WS("wide text string as in Risse internal text format");
	tIdRegistry::instance()->RegisterEdgeData(data);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tTextInputPin::tTextInputPin()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tTextInputPin::CanConnect(risse_uint32 type)
{
	return type == TextEdgeType;
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tTextOutputPin::tTextOutputPin()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tTextOutputPin::CanConnect(risse_uint32 type)
{
	return type == TextEdgeType;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
}
