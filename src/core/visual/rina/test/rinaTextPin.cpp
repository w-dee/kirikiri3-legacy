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
	data.Id = WideTextEdgeType;
	data.ShortName = RISSE_WS("wideText");
	data.ShortDesc = RISSE_WS("wide string");
	data.LongDesc = RISSE_WS("wide text string as in Risse internal text format");
	tIdRegistry::instance()->RegisterEdgeData(data);

	data.Id = NarrowTextEdgeType;
	data.ShortName = RISSE_WS("narrowText");
	data.ShortDesc = RISSE_WS("narrow string");
	data.LongDesc = RISSE_WS("narrow text string (UTF-8)");
	tIdRegistry::instance()->RegisterEdgeData(data);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tTextInputPin::tTextInputPin()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const gc_vector<risse_uint32> & tTextInputPin::GetSupportedTypes()
{
	// 暫定実装、できれば static かシングルトン上の配列への参照を返した方がよい
	gc_vector<risse_uint32> * arr = new gc_vector<risse_uint32>();
	arr->push_back(WideTextEdgeType);
	return *arr;
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
tTextOutputPin::tTextOutputPin()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const gc_vector<risse_uint32> & tTextOutputPin::GetSupportedTypes()
{
	// 暫定実装、できれば static かシングルトン上の配列への参照を返した方がよい
	gc_vector<risse_uint32> * arr = new gc_vector<risse_uint32>();
	arr->push_back(WideTextEdgeType);
	return *arr;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
}
