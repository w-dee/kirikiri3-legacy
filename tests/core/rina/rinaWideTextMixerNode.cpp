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
//! @brief テスト用のテキストミキサノード管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "rinaWideTextMixerNode.h"
#include "rinaWideTextPin.h"
#include "rinaWideTextProviderNode.h"

namespace Rina {
RISSE_DEFINE_SOURCE_ID(8982,48844,33706,17807,17033,58515,58827,7512);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tWideTextMixerNode::tWideTextMixerNode() : inherited()
{
	Position = 0;

	// 出力ピンを作成
	OutputPin = new tWideTextOutputPin();
	OutputPin->Attach(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tWideTextMixerNode::GetOutputPinCount()
{
	return 1; // 出力ピンは1個
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOutputPin * tWideTextMixerNode::GetOutputPinAt(risse_size n)
{
	// TODO: 例外
	if(n == 0) return OutputPin;
	return NULL; // 出力ピンはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextMixerNode::InsertOutputPinAt(risse_size n)
{
	// 出力ピンを追加することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextMixerNode::DeleteOutputPinAt(risse_size n)
{
	// 出力ピンを削除することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tWideTextMixerNode::GetInputPinCount()
{
	return InputPins.size();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tInputPin * tWideTextMixerNode::GetInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	return InputPins[n];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextMixerNode::InsertInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	tWideTextInputPin * newpin = new tWideTextInputPin();
	newpin->Attach(this);
	InputPins.insert(InputPins.begin() + n, newpin);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextMixerNode::DeleteInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	InputPins.erase(InputPins.begin() + n);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextMixerNode::BuildQueue(tRenderState * state, tInputPin * input_pin, tQueueNode * parent)
{
	tQueueNode * new_parent = new tWideTextMixerQueueNode(parent, Position);

	// 入力ピンに再帰
	for(gc_vector<tInputPin *>::iterator i = InputPins.begin(); i != InputPins.end(); i++)
		state->PushNextBuildQueueNode(*i, new_parent);
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tWideTextMixerQueueNode::tWideTextMixerQueueNode(tQueueNode * parent, risse_int32 pos) :
	inherited(parent, pos, tString())
{
	Canvas = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextMixerQueueNode::BeginProcess()
{
	// キャンバス用にメモリを確保
	Canvas = (risse_char *)MallocAtomicCollectee(sizeof(risse_char) * (CanvasSize + 1));

	// キャンバスを空白で埋める
	for(risse_size i = 0; i < CanvasSize; i++) Canvas[i] = RISSE_WC(' ');
	Canvas[CanvasSize] = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextMixerQueueNode::EndProcess()
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

	// 結果をTextに格納
	Text = Canvas;
}
//---------------------------------------------------------------------------

}
