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
//! @brief テスト用のナロー文字列→ワイド文字列変換ノード
//---------------------------------------------------------------------------
#include "prec.h"
#include "rinaNarrowTextToWideTextConverterNode.h"
#include "rinaWideTextEdge.h"
#include "rinaNarrowTextEdge.h"
#include "rinaNarrowTextProviderNode.h"

namespace Rina {
RISSE_DEFINE_SOURCE_ID(46687,10373,53420,18577,17050,14358,44584,58654);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tPinDescriptor tNarrowTextToWideTextConverterNode::InputPinDescriptor(
	RISSE_WS("input"), RISSE_WS_TR("Input Pin") );
tPinDescriptor tNarrowTextToWideTextConverterNode::OutputPinDescriptor(
	RISSE_WS("output"), RISSE_WS_TR("Output Pin") );
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tNarrowTextToWideTextConverterNode::tNarrowTextToWideTextConverterNode(tGraph * graph) : inherited(graph),
	InputPins(this, InputPinDescriptor, new tNarrowTextInputPin()),
	OutputPins(this, OutputPinDescriptor, new tWideTextOutputPin())
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNarrowTextToWideTextConverterNode::BuildQueue(tQueueBuilder & builder)
{
	// 入力ピンの情報をクリアする
	InputPins.At(0)->SetRenderGeneration(builder.GetRenderGeneration());
	InputPins.At(0)->ClearRenderRequests();

	// 出力ピンの先に繋がってる入力ピンそれぞれについて
	const tOutputPin::tInputPins & input_pins = OutputPins.At(0)->GetInputPins();
	for(tOutputPin::tInputPins::const_iterator i = input_pins.begin();
		i != input_pins.end(); i++)
	{
		// レンダリング世代が最新の物かどうかをチェック
		if((*i)->GetRenderGeneration() != builder.GetRenderGeneration()) continue;

		// 入力ピンのタイプをチェック
		RISSE_ASSERT((*i)->GetAgreedType() == WideTextEdgeType);

		// すべてのリクエストのすべてのキューノードにそれぞれ別の子を作成する
		const tInputPin::tRenderRequests & requests = (*i)->GetRenderRequests();
		for(tInputPin::tRenderRequests::const_iterator i =
				requests.begin(); i != requests.end(); i ++)
		{
			tQueueNode * new_parent = new tNarrowTextToWideTextConverterQueueNode(Risa::DownCast<const tWideTextRenderRequest*>(*i));
			const tWideTextRenderRequest * wide_req = Risa::DownCast<const tWideTextRenderRequest*>(*i);
			tNarrowTextRenderRequest * narrow_req = new tNarrowTextRenderRequest(new_parent, 0, wide_req->GetArea());
			InputPins.At(0)->AddRenderRequest(narrow_req);
		}
	}
	builder.Push(InputPins.At(0)->GetOutputPin()->GetNode());

	// 出力ピンの先の入力ピンが要求している領域のみに対して変換を行うようにするなどの処置が
	// 必要かもしれないがここではそれは考えない(すべての要求をそのまま入力ピンにも渡す)
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNarrowTextToWideTextConverterNode::NotifyUpdate(const t1DArea & area)
{
	// そのまま出力ピンに情報を渡す
	OutputPins.At(0)->NotifyUpdate(area);
}
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
tNarrowTextToWideTextConverterQueueNode::tNarrowTextToWideTextConverterQueueNode(const tWideTextRenderRequest * request) :
					inherited(request, tString(), t1DArea(), 0)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNarrowTextToWideTextConverterQueueNode::BeginProcess()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNarrowTextToWideTextConverterQueueNode::EndProcess()
{
	// 子ノードを変換する
	RISSE_ASSERT(Children.size() == 1);

	tNarrowTextQueueNode * child = Risa::DownCast<tNarrowTextQueueNode*>(Children[0].GetChild());

	// 結果をTextとAreaとOffsetに格納
	Text = tString(child->GetText());
	Area = child->GetArea();
	Offset = child->GetOffset();
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
}
