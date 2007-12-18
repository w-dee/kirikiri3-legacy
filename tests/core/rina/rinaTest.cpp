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
//! @brief テスト用プログラム
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/utils/Singleton.h"
#include "rinaWideTextProviderNode.h"
#include "rinaWideTextPin.h"
#include "rinaMultiTextProviderNode.h"
#include "rinaMultiTextPin.h"
#include "rinaWideTextDrawDeviceNode.h"
#include "rinaWideTextMixerNode.h"
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
	tWideTextProviderNode * provider_node1 = new tWideTextProviderNode();
	provider_node1->SetPosition(1);
	provider_node1->SetCaption(RISSE_WS("aaaaaa"));

	tWideTextProviderNode * provider_node2 = new tWideTextProviderNode();
	provider_node2->SetPosition(10);
	provider_node2->SetCaption(RISSE_WS("bbbbbbbb"));

	tWideTextDrawDeviceNode * dd_node = new tWideTextDrawDeviceNode();
	dd_node->InsertInputPinAt(0);
	dd_node->InsertInputPinAt(1);

	dd_node->GetInputPinAt(0)->Connect(provider_node1->GetOutputPinAt(0));
	dd_node->GetInputPinAt(1)->Connect(provider_node2->GetOutputPinAt(0));

	wxPrintf(wxT("%08x\n"), dd_node->GetInputPinAt(0)->GetAgreedType());

	tWideTextMixerNode * mixer_node = new tWideTextMixerNode();
	mixer_node->SetPosition(20);

	tWideTextProviderNode * provider_node3 = new tWideTextProviderNode();
	provider_node3->SetPosition(3);
	provider_node3->SetCaption(RISSE_WS("ccc"));

	tWideTextProviderNode * provider_node4 = new tWideTextProviderNode();
	provider_node4->SetPosition(12);
	provider_node4->SetCaption(RISSE_WS("dddd"));

	tMultiTextProviderNode * provider_node5 = new tMultiTextProviderNode();
	provider_node5->SetPosition(17);
	provider_node5->SetCaption(RISSE_WS("mmm"));

	mixer_node->InsertInputPinAt(0);
	mixer_node->InsertInputPinAt(1);
	mixer_node->InsertInputPinAt(2);

	mixer_node->GetInputPinAt(0)->Connect(provider_node3->GetOutputPinAt(0));
	mixer_node->GetInputPinAt(1)->Connect(provider_node4->GetOutputPinAt(0));
	mixer_node->GetInputPinAt(2)->Connect(provider_node5->GetOutputPinAt(0));

	dd_node->InsertInputPinAt(2);
	dd_node->GetInputPinAt(2)->Connect(mixer_node->GetOutputPinAt(0));


	tNarrowTextProviderNode * narrow_provider_node = new tNarrowTextProviderNode();
	narrow_provider_node->SetPosition(50);
	narrow_provider_node->SetCaption("narrow");

	tNarrowTextToWideTextConverterNode * converter_node1 = new tNarrowTextToWideTextConverterNode();
	converter_node1->GetInputPinAt(0)->Connect(narrow_provider_node->GetOutputPinAt(0));

	dd_node->InsertInputPinAt(3);
	dd_node->GetInputPinAt(3)->Connect(converter_node1->GetOutputPinAt(0));



	tMultiTextProviderNode * multi_provider_node = new tMultiTextProviderNode();
	multi_provider_node->SetPosition(60);
	multi_provider_node->SetCaption(RISSE_WS("multi"));

	tNarrowTextToWideTextConverterNode * converter_node2 = new tNarrowTextToWideTextConverterNode();
	converter_node2->GetInputPinAt(0)->Connect(multi_provider_node->GetOutputPinAt(0));

	dd_node->InsertInputPinAt(4);
	dd_node->GetInputPinAt(4)->Connect(converter_node2->GetOutputPinAt(0));


	// render
	tRenderState state;
	state.Render(dd_node);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
}