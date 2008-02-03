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
//! @brief テスト用のテキストプロバイダノード管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "rinaWideTextProviderNode.h"
#include "rinaWideTextEdge.h"

namespace Rina {
RISSE_DEFINE_SOURCE_ID(55116,34738,34674,19554,62891,6474,53398,51508);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tPinDescriptor tWideTextProviderNode::Descriptor(
	RISSE_WS("output"), RISSE_WS_TR("Output Pin") );
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tWideTextProviderNode::tWideTextProviderNode(tGraph * graph) :
	inherited(graph),
	InputPins(this),
	OutputPins(this, Descriptor, new tWideTextOutputPin())
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextProviderNode::SetCaption(const tString & caption)
{
	volatile tGraphLocker lock(*this); 

	// キャプションが変わると前の長さと新しい長さのどちらか長い方分までが更新される
	risse_size length_was = Caption.GetLength();
	risse_size length_is  = caption.GetLength();
	Caption = caption;
	OutputPins.At(0)->NotifyUpdate(t1DArea(0, std::max(length_was, length_is)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextProviderNode::BuildQueue(tQueueBuilder & builder)
{
	RISSE_ASSERT_CS_LOCKED(GetGraph()->GetCS());

	const tOutputPin::tInputPins & input_pins = OutputPins.At(0)->GetInputPins();
	for(tOutputPin::tInputPins::const_iterator i = input_pins.begin();
		i != input_pins.end(); i++)
	{
		// レンダリング世代が最新の物かどうかをチェック
		if((*i)->GetRenderGeneration() != builder.GetRenderGeneration()) continue;

		// 入力ピンのタイプをチェック
		RISSE_ASSERT((*i)->GetAgreedType() == WideTextEdgeType);

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
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}
