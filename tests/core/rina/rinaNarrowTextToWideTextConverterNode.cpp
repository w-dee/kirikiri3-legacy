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
//! @brief テスト用のナロー文字列→ワイド文字列変換ノード
//---------------------------------------------------------------------------
#include "prec.h"
#include "rinaNarrowTextToWideTextConverterNode.h"
#include "rinaWideTextPin.h"
#include "rinaNarrowTextPin.h"
#include "rinaNarrowTextProviderNode.h"

namespace Rina {
RISSE_DEFINE_SOURCE_ID(46687,10373,53420,18577,17050,14358,44584,58654);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tNarrowTextToWideTextConverterNode::tNarrowTextToWideTextConverterNode()
{
	// 出力ピンを作成
	OutputPin = new tWideTextOutputPin();
	OutputPin->Attach(this);
	InputPin = new tNarrowTextInputPin();
	InputPin->Attach(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tNarrowTextToWideTextConverterNode::GetOutputPinCount()
{
	return 1; // 出力ピンは１個
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOutputPin * tNarrowTextToWideTextConverterNode::GetOutputPinAt(risse_size n)
{
	// TODO: 例外
	if(n == 0) return OutputPin;
	return NULL; // 出力ピンはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNarrowTextToWideTextConverterNode::InsertOutputPinAt(risse_size n)
{
	// 出力ピンを追加することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNarrowTextToWideTextConverterNode::DeleteOutputPinAt(risse_size n)
{
	// 出力ピンを削除することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tNarrowTextToWideTextConverterNode::GetInputPinCount()
{
	return 1; // 入力ピンは一個
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tInputPin * tNarrowTextToWideTextConverterNode::GetInputPinAt(risse_size n)
{
	// TODO: 例外
	if(n == 0) return InputPin;
	return NULL; // 出力ピンはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNarrowTextToWideTextConverterNode::InsertInputPinAt(risse_size n)
{
	// XXX: 入力ピンを追加することはできない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNarrowTextToWideTextConverterNode::DeleteInputPinAt(risse_size n)
{
	// XXX: 入力ピンを削除することはできない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNarrowTextToWideTextConverterNode::BuildQueue(tQueueNode * parent)
{
	tQueueNode * new_parent = new tNarrowTextToWideTextConverterQueueNode(parent);

	// 入力ピンに再帰
	InputPin->BuildQueue(new_parent);
}
//---------------------------------------------------------------------------















//---------------------------------------------------------------------------
tNarrowTextToWideTextConverterQueueNode::tNarrowTextToWideTextConverterQueueNode(tQueueNode * parent) :
	inherited(parent, 0, tString())
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

	tNarrowTextProviderQueueNode * child = reinterpret_cast<tNarrowTextProviderQueueNode *>(Children[0]);

	// 結果をPositionとTextに格納
	Position = child->GetPosition();
	Text = tString(child->GetText());
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
}
