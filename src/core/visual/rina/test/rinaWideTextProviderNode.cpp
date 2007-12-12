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
//! @brief テスト用のテキストプロバイダノード管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "visual/rina/test/rinaWideTextProviderNode.h"
#include "visual/rina/test/rinaWideTextPin.h"

namespace Rina {
RISSE_DEFINE_SOURCE_ID(55116,34738,34674,19554,62891,6474,53398,51508);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tWideTextProviderNode::tWideTextProviderNode()
{
	Position = 0;

	// 出力ピンを作成
	OutputPin = new tWideTextOutputPin();
	OutputPin->Attach(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tWideTextProviderNode::GetOutputPinCount()
{
	return 1; // 出力ピンは１個
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOutputPin * tWideTextProviderNode::GetOutputPinAt(risse_size n)
{
	// TODO: 例外
	if(n == 0) return OutputPin;
	return NULL; // 出力ピンはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextProviderNode::InsertOutputPinAt(risse_size n)
{
	// 出力ピンを追加することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextProviderNode::DeleteOutputPinAt(risse_size n)
{
	// 出力ピンを削除することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tWideTextProviderNode::GetInputPinCount()
{
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tInputPin * tWideTextProviderNode::GetInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextProviderNode::InsertInputPinAt(risse_size n)
{
	// XXX: 入力ピンを追加することはできない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextProviderNode::DeleteInputPinAt(risse_size n)
{
	// XXX: 入力ピンを削除することはできない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tWideTextProviderNode::BuildQueue(tQueueNode * parent)
{
	new tWideTextProviderQueueNode(parent, Position, Caption);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}
