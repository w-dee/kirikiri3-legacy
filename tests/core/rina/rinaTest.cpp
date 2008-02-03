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
//! @brief テスト用プログラム
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/utils/Singleton.h"
#include "visual/rina/rinaGraph.h"
#include "rinaWideTextProviderNode.h"
#include "rinaWideTextEdge.h"
#include "rinaMultiTextProviderNode.h"
#include "rinaWideTextDrawDeviceNode.h"
#include "rinaWideTextMixerNode.h"
#include "rinaWideTextReverserNode.h"
#include "rinaNarrowTextToWideTextConverterNode.h"
#include "rinaNarrowTextProviderNode.h"

using namespace Risa;

namespace Rina {
RISSE_DEFINE_SOURCE_ID(32159,46024,56879,16658,9903,15595,38105,12016);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
class tTester : public singleton_base<tTester>
{
public:
	tTester();
private:
	void Test();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTester::tTester()
{
	Test();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTester::Test()
{
	tGraph * graph = new tGraph();
	tWideTextDrawDeviceNode * dd_node = new tWideTextDrawDeviceNode(graph);
	tWideTextProviderNode * provider_node1 = new tWideTextProviderNode(graph);
	tWideTextProviderNode * provider_node2 = new tWideTextProviderNode(graph);
	tWideTextMixerNode * mixer_node = new tWideTextMixerNode(graph);
	tWideTextProviderNode * provider_node3 = new tWideTextProviderNode(graph);
	tWideTextProviderNode * provider_node4 = new tWideTextProviderNode(graph);
	tMultiTextProviderNode * provider_node5 = new tMultiTextProviderNode(graph);
	tNarrowTextProviderNode * narrow_provider_node = new tNarrowTextProviderNode(graph);
	tNarrowTextToWideTextConverterNode * converter_node1 = new tNarrowTextToWideTextConverterNode(graph);
	tMultiTextProviderNode * multi_provider_node = new tMultiTextProviderNode(graph);
	tNarrowTextToWideTextConverterNode * converter_node2 = new tNarrowTextToWideTextConverterNode(graph);
	tWideTextReverserNode * rev_node = new tWideTextReverserNode(graph);

	// グラフをロックしてから……
	{
		Risa::tCriticalSection::tLocker lock(graph->GetCS());

		provider_node1->SetCaption(RISSE_WS("abc"));
		dd_node->GetInputPins().Insert(0);
		dd_node->GetInputPins().At(0)->Connect(provider_node1->GetOutputPins().At(0));
		Risa::DownCast<tWideTextMixerInputPin*>(dd_node->GetInputPins().At(0))->
			GetInheritableProperties().SetPosition(1);

		provider_node2->SetCaption(RISSE_WS("def"));
		dd_node->GetInputPins().Insert(1);
		dd_node->GetInputPins().At(1)->Connect(provider_node2->GetOutputPins().At(0));
		Risa::DownCast<tWideTextMixerInputPin*>(dd_node->GetInputPins().At(1))->
			GetInheritableProperties().SetPosition(5);

		wxFprintf(stderr, wxT("%08x\n"), dd_node->GetInputPins().At(0)->GetAgreedType());


		provider_node3->SetCaption(RISSE_WS("ghi"));
		mixer_node->GetInputPins().Insert(0);
		mixer_node->GetInputPins().At(0)->Connect(provider_node3->GetOutputPins().At(0));
		Risa::DownCast<tWideTextMixerInputPin*>(mixer_node->GetInputPins().At(0))->
			GetInheritableProperties().SetPosition(0);

		provider_node4->SetCaption(RISSE_WS("jkl"));
		mixer_node->GetInputPins().Insert(1);
		mixer_node->GetInputPins().At(1)->Connect(provider_node4->GetOutputPins().At(0));
		Risa::DownCast<tWideTextMixerInputPin*>(mixer_node->GetInputPins().At(1))->
			GetInheritableProperties().SetPosition(4);

		provider_node5->SetCaption(RISSE_WS("mno"));
		mixer_node->GetInputPins().Insert(2);
		mixer_node->GetInputPins().At(2)->Connect(provider_node5->GetOutputPins().At(0));
		Risa::DownCast<tWideTextMixerInputPin*>(mixer_node->GetInputPins().At(2))->
			GetInheritableProperties().SetPosition(8);

		dd_node->GetInputPins().Insert(2);
		dd_node->GetInputPins().At(2)->Connect(mixer_node->GetOutputPins().At(0));
		Risa::DownCast<tWideTextMixerInputPin*>(dd_node->GetInputPins().At(2))->
			GetInheritableProperties().SetPosition(9);


		narrow_provider_node->SetCaption("narrow");

		converter_node1->GetInputPins().At(0)->Connect(narrow_provider_node->GetOutputPins().At(0));

		dd_node->GetInputPins().Insert(3);
		dd_node->GetInputPins().At(3)->Connect(converter_node1->GetOutputPins().At(0));
		Risa::DownCast<tWideTextMixerInputPin*>(dd_node->GetInputPins().At(3))->
			GetInheritableProperties().SetPosition(21);

		multi_provider_node->SetCaption(RISSE_WS("multi"));

		converter_node2->GetInputPins().At(0)->Connect(multi_provider_node->GetOutputPins().At(0));

		dd_node->GetInputPins().Insert(4);
		dd_node->GetInputPins().At(4)->Connect(converter_node2->GetOutputPins().At(0));
		Risa::DownCast<tWideTextMixerInputPin*>(dd_node->GetInputPins().At(4))->
			GetInheritableProperties().SetPosition(28);

		dd_node->GetInputPins().Insert(5);
		dd_node->GetInputPins().At(5)->Connect(converter_node1->GetOutputPins().At(0));
		Risa::DownCast<tWideTextMixerInputPin*>(dd_node->GetInputPins().At(5))->
			GetInheritableProperties().SetPosition(46);

		dd_node->GetInputPins().Insert(6);
		dd_node->GetInputPins().At(6)->Connect(mixer_node->GetOutputPins().At(0));
		Risa::DownCast<tWideTextMixerInputPin*>(dd_node->GetInputPins().At(6))->
			GetInheritableProperties().SetPosition(34);

		rev_node->GetInputPins().At(0)->Connect(mixer_node->GetOutputPins().At(0));
		dd_node->GetInputPins().Insert(7);
		dd_node->GetInputPins().At(7)->Connect(rev_node->GetOutputPins().At(0));
		Risa::DownCast<tWideTextMixerInputPin*>(dd_node->GetInputPins().At(7))->
			GetInheritableProperties().SetPosition(64);
	}

	{
		// build queue render
		tQueueBuilder builder(graph);
		builder.Build(dd_node);

		// キューノードを作成し終わったということでレンダリングを開始する
		tQueue queue;
		queue.Process(builder.GetRootQueueNode());
	}

	provider_node1->SetCaption(RISSE_WS("ABC"));
	provider_node2->SetCaption(RISSE_WS("DE"));
	provider_node4->SetCaption(RISSE_WS("JKL"));
	narrow_provider_node->SetCaption("NARROW");

	{
		// build queue render
		tQueueBuilder builder(graph);
		builder.Build(dd_node);

		// キューノードを作成し終わったということでレンダリングを開始する
		tQueue queue;
		queue.Process(builder.GetRootQueueNode());
	}


}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
}
