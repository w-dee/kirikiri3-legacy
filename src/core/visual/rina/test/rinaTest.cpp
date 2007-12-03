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
#include "visual/rina/test/rinaTextProviderNode.h"
#include "visual/rina/test/rinaTextProperty.h"
#include "visual/rina/test/rina1DPositionProperty.h"
#include "visual/rina/test/rinaTextPin.h"
#include "visual/rina/test/rinaTextDrawDeviceNode.h"

using namespace Risa;

namespace Rina {
RISSE_DEFINE_SOURCE_ID(32159,46024,56879,16658,9903,15595,38105,12016);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
class tTester : public singleton_base<tTester>
{
public:
	tTester();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTester::tTester()
{
	tTextProviderNode * provider_node1 = new tTextProviderNode();
	provider_node1->GetProperty()->SetValueAt(
		t1DPositionPropertyInfo::instance()->ID_Position, (risse_int64)1);
	provider_node1->GetProperty()->SetValueAt(
		tCaptionPropertyInfo::instance()->ID_Caption, RISSE_WS("aaaaaa"));

	tTextProviderNode * provider_node2 = new tTextProviderNode();
	provider_node2->GetProperty()->SetValueAt(
		t1DPositionPropertyInfo::instance()->ID_Position, (risse_int64)10);
	provider_node2->GetProperty()->SetValueAt(
		tCaptionPropertyInfo::instance()->ID_Caption, RISSE_WS("bbbbbbbb"));

	wxFprintf(stdout, wxT("provider1: pos=%s cap=%s\n"),
		provider_node1->GetProperty()->GetValueAt(t1DPositionPropertyInfo::instance()->ID_Position).AsHumanReadable().AsWxString().c_str(),
		provider_node1->GetProperty()->GetValueAt(tCaptionPropertyInfo::instance()->ID_Caption    ).AsHumanReadable().AsWxString().c_str() );
	wxFprintf(stdout, wxT("provider2: pos=%s cap=%s\n"),
		provider_node2->GetProperty()->GetValueAt(t1DPositionPropertyInfo::instance()->ID_Position).AsHumanReadable().AsWxString().c_str(),
		provider_node2->GetProperty()->GetValueAt(tCaptionPropertyInfo::instance()->ID_Caption    ).AsHumanReadable().AsWxString().c_str() );

	tTextDrawDeviceNode * dd_node = new tTextDrawDeviceNode();
	dd_node->InsertInputPinAt(0);
	dd_node->InsertInputPinAt(1);

	dd_node->GetInputPinAt(0)->Connect(provider_node1->GetOutputPinAt(0));
	dd_node->GetInputPinAt(1)->Connect(provider_node2->GetOutputPinAt(0));

	// render
	tRootQueueNode * rootqueue = new tRootQueueNode();
	dd_node->BuildQueue(rootqueue);
	rootqueue->Process();

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
}
