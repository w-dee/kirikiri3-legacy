//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief テスト用の複数形式をサポートするテキストプロバイダノード管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "rinaMultiTextProviderNode.h"
#include "rinaWideTextEdge.h"
#include "rinaWideTextProviderNode.h"
#include "rinaNarrowTextEdge.h"
#include "rinaNarrowTextProviderNode.h"

namespace Rina {
RISSE_DEFINE_SOURCE_ID(47508,49325,57519,19588,18101,2946,50970,8610);
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tMultiTextOutputPin::tMultiTextOutputPin()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const gc_vector<risse_uint32> & tMultiTextOutputPin::GetSupportedTypes()
{
	// 暫定実装、できれば static かシングルトン上の配列への参照を返した方がよい
	gc_vector<risse_uint32> * arr = new gc_vector<risse_uint32>();
	arr->push_back(WideTextEdgeType);
	arr->push_back(NarrowTextEdgeType);
	return *arr;
}
//---------------------------------------------------------------------------














//---------------------------------------------------------------------------
tPinDescriptor tMultiTextProviderNode::Descriptor(
	RISSE_WS("output"), RISSE_WS_TR("Output Pin") );
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tMultiTextProviderNode::tMultiTextProviderNode(tGraph * graph) :
	inherited(graph),
	InputPins(this),
	OutputPins(this, Descriptor, new tMultiTextOutputPin())
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tMultiTextProviderNode::BuildQueue(tQueueBuilder & builder)
{
	RISSE_ASSERT_CS_LOCKED(GetGraph()->GetCS());

	// 出力ピンの先に繋がってる入力ピンそれぞれについて
	const tOutputPin::tInputPins & input_pins = OutputPins.At(0)->GetInputPins();
	for(tOutputPin::tInputPins::const_iterator i = input_pins.begin();
		i != input_pins.end(); i++)
	{
		// レンダリング世代が最新の物かどうかをチェック
		if((*i)->GetRenderGeneration() != builder.GetRenderGeneration()) continue;

		// 入力ピンのタイプをチェックし、適切なキューノードを作成
		risse_uint32 target_type = (*i)->GetAgreedType();

		RISSE_ASSERT(target_type == WideTextEdgeType || target_type == NarrowTextEdgeType); // 暫定
		if(target_type == WideTextEdgeType)
		{
			// 入力ピンのすべてのリクエストに答えるためのキューノードを作成する
			const tInputPin::tRenderRequests & requests = (*i)->GetRenderRequests();
			for(tInputPin::tRenderRequests::const_iterator i =
				requests.begin(); i != requests.end(); i ++)
			{
				const tWideTextRenderRequest * req = Risa::DownCast<const tWideTextRenderRequest*>(*i);
				new tWideTextProviderQueueNode(
					req,
					Caption,
					t1DArea(req->GetArea(), t1DArea(0, Caption.GetLength())),
					t1DArea(req->GetArea(), t1DArea(0, Caption.GetLength())).GetStart());
			}
		}
		else if(target_type == NarrowTextEdgeType)
		{
			// 入力ピンのすべてのリクエストに答えるためのキューノードを作成する
			const tInputPin::tRenderRequests & requests = (*i)->GetRenderRequests();
			for(tInputPin::tRenderRequests::const_iterator i =
				requests.begin(); i != requests.end(); i ++)
			{
				const char * caption = Caption.AsNarrowString();
				const tNarrowTextRenderRequest * req = Risa::DownCast<const tNarrowTextRenderRequest*>(*i);
				new tNarrowTextProviderQueueNode(
					req,
					caption,
					t1DArea(req->GetArea(), t1DArea(0, strlen(caption))),
					t1DArea(req->GetArea(), t1DArea(0, strlen(caption))).GetStart());
			}
		}
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}
