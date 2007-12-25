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
//! @brief テスト用のナローテキストピン管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "rinaNarrowTextPin.h"


namespace Rina {
RISSE_DEFINE_SOURCE_ID(49275,58466,7520,16852,29087,44056,53196,61913);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tNarrowTextEdgeTypeRegisterer::tNarrowTextEdgeTypeRegisterer()
{
	tIdRegistry::tEdgeData data;
	data.Id = NarrowTextEdgeType;
	data.ShortName = RISSE_WS("narrowText");
	data.ShortDesc = RISSE_WS("narrow string");
	data.LongDesc = RISSE_WS("narrow text string (UTF-8)");
	tIdRegistry::instance()->RegisterEdgeData(data);
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tNarrowTextInputPin::tNarrowTextInputPin()
{
	Position = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const gc_vector<risse_uint32> & tNarrowTextInputPin::GetSupportedTypes()
{
	// 暫定実装、できれば static かシングルトン上の配列への参照を返した方がよい
	gc_vector<risse_uint32> * arr = new gc_vector<risse_uint32>();
	arr->push_back(NarrowTextEdgeType);
	return *arr;
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
tNarrowTextOutputPin::tNarrowTextOutputPin()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const gc_vector<risse_uint32> & tNarrowTextOutputPin::GetSupportedTypes()
{
	// 暫定実装、できれば static かシングルトン上の配列への参照を返した方がよい
	gc_vector<risse_uint32> * arr = new gc_vector<risse_uint32>();
	arr->push_back(NarrowTextEdgeType);
	return *arr;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
const tNarrowTextInheritableProperties & tNarrowTextInputPinQueueNode::GetInheritableProperties()
{
	RISSE_ASSERT(Children.size() == 1);
	InheritedProperty = reinterpret_cast<tNarrowTextQueueNode*>(Children[0])->GetInheritableProperties() + InheritableProperties;
	return InheritedProperty;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const char * tNarrowTextInputPinQueueNode::GetText()
{
	RISSE_ASSERT(Children.size() == 1);
	return reinterpret_cast<tNarrowTextQueueNode*>(Children[0])->GetText();
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
}
