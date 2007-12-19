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
//! @brief テスト用のテキスト描画デバイスプロセスノード管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "rinaWideTextDrawDeviceNode.h"
#include "rinaWideTextPin.h"
#include "rinaWideTextProviderNode.h"


namespace Rina {
RISSE_DEFINE_SOURCE_ID(10207,53962,31748,17392,1438,46335,5173,19226);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tWideTextDrawDeviceNode::tWideTextDrawDeviceNode() : inherited()
{
	
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tWideTextDrawDeviceNode::GetOutputPinCount()
{
	return 0; // 出力ピンはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOutputPin * tWideTextDrawDeviceNode::GetOutputPinAt(risse_size n)
{
	// TODO: 例外
	return NULL; // 出力ピンはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextDrawDeviceNode::InsertOutputPinAt(risse_size n)
{
	// 出力ピンを追加することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextDrawDeviceNode::DeleteOutputPinAt(risse_size n)
{
	// 出力ピンを削除することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tWideTextDrawDeviceNode::GetInputPinCount()
{
	return InputPins.size();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tInputPin * tWideTextDrawDeviceNode::GetInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	return InputPins[n];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextDrawDeviceNode::InsertInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	tWideTextInputPin * newpin = new tWideTextInputPin();
	newpin->Attach(this);
	InputPins.insert(InputPins.begin() + n, newpin);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextDrawDeviceNode::DeleteInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	InputPins.erase(InputPins.begin() + n);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextDrawDeviceNode::BuildQueue(tRenderState * state)
{
	tQueueNode * new_parent = new tWideTextDrawDeviceQueueNode(NULL);

	// 入力ピンに再帰
	for(gc_vector<tInputPin *>::iterator i = InputPins.begin(); i != InputPins.end(); i++)
	{
		(*i)->SetRenderGeneration(state->GetRenderGeneration());
		tQueueNode * new_pin_node =
			new tWideTextInputPinQueueNode(new_parent, ((tWideTextInputPin*)(*i))->GetPosition());
		(*i)->SetParentQueueNode(new_pin_node);
		state->PushNextBuildQueueNode((*i)->GetOutputPin()->GetNode());
	}

	// ルートのキューノードはnew_parentであると主張(ここは後から修正するかも………)
	state->SetRootQueueNode(new_parent);
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tWideTextDrawDeviceQueueNode::tWideTextDrawDeviceQueueNode(tQueueNode * parent) : inherited(parent)
{
	Canvas = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextDrawDeviceQueueNode::BeginProcess()
{
	// キャンバス用にメモリを確保
	Canvas = (risse_char *)MallocAtomicCollectee(sizeof(risse_char) * (CanvasSize + 1));

	// キャンバスを空白で埋める
	for(risse_size i = 0; i < CanvasSize; i++) Canvas[i] = RISSE_WC(' ');
	Canvas[CanvasSize] = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextDrawDeviceQueueNode::EndProcess()
{
	// 子ノードを合成する
	for(tNodes::iterator i = Children.begin(); i != Children.end(); i++)
	{
		tWideTextProviderQueueNode * provider = reinterpret_cast<tWideTextProviderQueueNode *>(*i);
		const tString & text = provider->GetText();
		const risse_char *pbuf = text.c_str();
		risse_size text_size = text.GetLength();
		risse_int32 pos = provider->GetPosition();
		RISSE_ASSERT(pos >= 0);
		RISSE_ASSERT(pos + text_size < CanvasSize);
		for(risse_size i = 0 ; i < text_size; i++)
		{
			if(pbuf[i] != RISSE_WC(' '))
				Canvas[i + pos] = pbuf[i];
		}
	}

	// 結果を出力する
	wxFprintf(stdout, wxT("%s\n"), tString(Canvas).AsWxString().c_str());
}
//---------------------------------------------------------------------------

}
