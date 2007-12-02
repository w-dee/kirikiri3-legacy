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
#include "visual/rina/test/rinaTextDrawDeviceNode.h"
#include "visual/rina/test/rinaTextPin.h"
#include "visual/rina/test/rinaTextProviderNode.h"


namespace Rina {
RISSE_DEFINE_SOURCE_ID(10207,53962,31748,17392,1438,46335,5173,19226);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tTextDrawDeviceNode::tTextDrawDeviceNode() : inherited()
{
	
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tTextDrawDeviceNode::GetOutputPinCount()
{
	return 0; // 出力ピンはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOutputPin * tTextDrawDeviceNode::GetOutputPinAt(risse_size n)
{
	// TODO: 例外
	return NULL; // 出力ピンはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextDrawDeviceNode::InsertOutputPinAt(risse_size n)
{
	// 出力ピンを追加することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextDrawDeviceNode::DeleteOutputPinAt(risse_size n)
{
	// 出力ピンを削除することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tTextDrawDeviceNode::GetInputPinCount()
{
	return InputPins.size();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tInputPin * tTextDrawDeviceNode::GetInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	return InputPins[n];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextDrawDeviceNode::InsertInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	tTextInputPin * newpin = new tTextInputPin();
	newpin->Attach(this);
	InputPins.insert(InputPins.begin() + n, newpin);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextDrawDeviceNode::DeleteInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	InputPins.erase(InputPins.begin() + n);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextDrawDeviceNode::BuildComandQueue(tQueueNode * parent)
{
	tQueueNode * new_parent = new tTextDrawDeviceQueueNode();
	parent->AddChild(new_parent);

	// 入力ピンに再帰
	for(gc_vector<tInputPin *>::iterator i = InputPins.begin(); i != InputPins.end(); i++)
		(*i)->BuildComandQueue(new_parent);
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tTextDrawDeviceQueueNode::tTextDrawDeviceQueueNode()
{
	Canvas = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextDrawDeviceQueueNode::BeginProcess()
{
	// キャンバス用にメモリを確保
	Canvas = (risse_char *)MallocAtomicCollectee(sizeof(risse_char) * (CanvasSize + 1));

	// キャンバスを空白で埋める
	for(risse_size i = 0; i < CanvasSize; i++) Canvas[i] = RISSE_WC(' ');
	Canvas[CanvasSize] = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextDrawDeviceQueueNode::EndProcess()
{
	// 子ノードを合成する
	for(tChildren::iterator i = Children.begin(); i != Children.end(); i++)
	{
		tTextProviderQueueNode * provider = reinterpret_cast<tTextProviderQueueNode *>(*i);
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
