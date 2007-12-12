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
//! @brief テスト用のナローテキストプロバイダノード管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "rinaNarrowTextProviderNode.h"
#include "rinaNarrowTextPin.h"

namespace Rina {
RISSE_DEFINE_SOURCE_ID(12623,61035,35256,19759,45714,27702,3711,32168);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tNarrowTextProviderNode::tNarrowTextProviderNode()
{
	Position = 0;

	// 出力ピンを作成
	OutputPin = new tNarrowTextOutputPin();
	OutputPin->Attach(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNarrowTextProviderNode::SetCaption(const char * caption)
{
	size_t len = strlen(caption);
	char * newbuf = static_cast<char *>(MallocAtomicCollectee(len + 1));
	strcpy(newbuf, caption);
	Caption = newbuf;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tNarrowTextProviderNode::GetOutputPinCount()
{
	return 1; // 出力ピンは１個
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOutputPin * tNarrowTextProviderNode::GetOutputPinAt(risse_size n)
{
	// TODO: 例外
	if(n == 0) return OutputPin;
	return NULL; // 出力ピンはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNarrowTextProviderNode::InsertOutputPinAt(risse_size n)
{
	// 出力ピンを追加することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNarrowTextProviderNode::DeleteOutputPinAt(risse_size n)
{
	// 出力ピンを削除することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tNarrowTextProviderNode::GetInputPinCount()
{
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tInputPin * tNarrowTextProviderNode::GetInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNarrowTextProviderNode::InsertInputPinAt(risse_size n)
{
	// XXX: 入力ピンを追加することはできない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNarrowTextProviderNode::DeleteInputPinAt(risse_size n)
{
	// XXX: 入力ピンを削除することはできない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNarrowTextProviderNode::BuildQueue(tQueueNode * parent)
{
	new tNarrowTextProviderQueueNode(parent, Position, Caption);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}
