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
//! @brief テスト用のナローテキストプロバイダノード管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "rinaNarrowTextProviderNode.h"
#include "rinaNarrowTextEdge.h"

namespace Rina {
RISSE_DEFINE_SOURCE_ID(12623,61035,35256,19759,45714,27702,3711,32168);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tPinDescriptor tNarrowTextProviderNode::Descriptor(
	RISSE_WS("output"), RISSE_WS_TR("Output Pin") );
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tNarrowTextProviderNode::tNarrowTextProviderNode(tGraph * graph) :
	inherited(graph),
	InputPins(this),
	OutputPins(this, Descriptor, new tNarrowTextOutputPin())
{
	Caption = "";
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNarrowTextProviderNode::SetCaption(const char * caption)
{
	volatile tGraphLocker lock(*this); 

	risse_size length_was = strlen(Caption);

	size_t len = strlen(caption);
	char * newbuf = static_cast<char *>(MallocAtomicCollectee(len + 1));
	strcpy(newbuf, caption);
	Caption = newbuf;

	// キャプションが変わると前の長さと新しい長さのどちらか長い方分までが更新される
	risse_size length_is  = len;
	OutputPins.At(0)->NotifyUpdate(t1DArea(0, std::max(length_was, length_is)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNarrowTextProviderNode::BuildQueue(tQueueBuilder & builder)
{
	RISSE_ASSERT_CS_LOCKED(GetGraph()->GetCS());

	const tOutputPin::tInputPins & input_pins = OutputPins.At(0)->GetInputPins();
	for(tOutputPin::tInputPins::const_iterator i = input_pins.begin();
		i != input_pins.end(); i++)
	{
		// レンダリング世代が最新の物かどうかをチェック
		if((*i)->GetRenderGeneration() != builder.GetRenderGeneration()) continue;

		// 入力ピンのタイプをチェック
		RISSE_ASSERT((*i)->GetAgreedType() == NarrowTextEdgeType);

		// 入力ピンのすべてのリクエストに答えるためのキューノードを作成する
		const tInputPin::tRenderRequests & requests = (*i)->GetRenderRequests();
		for(tInputPin::tRenderRequests::const_iterator i =
			requests.begin(); i != requests.end(); i ++)
		{
			const tNarrowTextRenderRequest * req = Risa::DownCast<const tNarrowTextRenderRequest*>(*i);
			new tNarrowTextProviderQueueNode(
				req,
				Caption,
				t1DArea(req->GetArea(), t1DArea(0, strlen(Caption))),
				t1DArea(req->GetArea(), t1DArea(0, strlen(Caption))).GetStart());
		}
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}
