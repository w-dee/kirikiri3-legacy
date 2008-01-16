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
//! @brief ワイドテキストのエッジタイプ
//---------------------------------------------------------------------------
#include "prec.h"
#include "rina1DRegion.h"
#include "rinaWideTextEdge.h"
#include "rinaWideTextDrawDeviceNode.h"

namespace Rina {
RISSE_DEFINE_SOURCE_ID(52443,53248,53438,18214,2474,63937,55669,59383);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tWideTextEdgeTypeRegisterer::tWideTextEdgeTypeRegisterer()
{
	tIdRegistry::tEdgeData data;
	data.Id = WideTextEdgeType;
	data.ShortName = RISSE_WS("wideText");
	data.ShortDesc = RISSE_WS("wide string");
	data.LongDesc = RISSE_WS("wide text string (risse internal)");
	tIdRegistry::instance()->RegisterEdgeData(data);
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tWideTextInputPin::tWideTextInputPin()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const gc_vector<risse_uint32> & tWideTextInputPin::GetSupportedTypes()
{
	// 暫定実装、できれば static かシングルトン上の配列への参照を返した方がよい
	gc_vector<risse_uint32> * arr = new gc_vector<risse_uint32>();
	arr->push_back(WideTextEdgeType);
	return *arr;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
void tWideTextMixerInputPin::NotifyUpdate(const t1DArea & area)
{
	tWideTextDrawDeviceNode * dd_node = Risa::TryDownCast<tWideTextDrawDeviceNode*>(GetNode());
	if(dd_node) dd_node->NotifyUpdate(InheritableProperties.ToParent(area));
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tWideTextOutputPin::tWideTextOutputPin()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const gc_vector<risse_uint32> & tWideTextOutputPin::GetSupportedTypes()
{
	// 暫定実装、できれば static かシングルトン上の配列への参照を返した方がよい
	gc_vector<risse_uint32> * arr = new gc_vector<risse_uint32>();
	arr->push_back(WideTextEdgeType);
	return *arr;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextOutputPin::NotifyUpdate(const t1DArea & area)
{
	for(tInputPins::const_iterator i = GetInputPins().begin(); i != GetInputPins().end(); i++)
	{
		Risa::DownCast<tWideTextInputPin *>(*i)->NotifyUpdate(area);
	}
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
}
