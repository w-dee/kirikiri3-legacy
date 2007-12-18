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
//! @brief テスト用の複数形式をサポートするテキストプロバイダノード管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "rinaMultiTextProviderNode.h"
#include "rinaMultiTextPin.h"
#include "rinaWideTextPin.h"
#include "rinaWideTextProviderNode.h"
#include "rinaNarrowTextPin.h"
#include "rinaNarrowTextProviderNode.h"

namespace Rina {
RISSE_DEFINE_SOURCE_ID(47508,49325,57519,19588,18101,2946,50970,8610);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tMultiTextProviderNode::tMultiTextProviderNode()
{
	Position = 0;

	// 出力ピンを作成
	OutputPin = new tMultiTextOutputPin();
	OutputPin->Attach(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tMultiTextProviderNode::GetOutputPinCount()
{
	return 1; // 出力ピンは１個
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOutputPin * tMultiTextProviderNode::GetOutputPinAt(risse_size n)
{
	// TODO: 例外
	if(n == 0) return OutputPin;
	return NULL; // 出力ピンはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tMultiTextProviderNode::InsertOutputPinAt(risse_size n)
{
	// 出力ピンを追加することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tMultiTextProviderNode::DeleteOutputPinAt(risse_size n)
{
	// 出力ピンを削除することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tMultiTextProviderNode::GetInputPinCount()
{
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tInputPin * tMultiTextProviderNode::GetInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tMultiTextProviderNode::InsertInputPinAt(risse_size n)
{
	// XXX: 入力ピンを追加することはできない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tMultiTextProviderNode::DeleteInputPinAt(risse_size n)
{
	// XXX: 入力ピンを削除することはできない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tMultiTextProviderNode::BuildQueue(tRenderState * state)
{
	// 出力ピンの先に繋がってる入力ピンそれぞれについて
	for(tOutputPin::tInputPins::const_iterator i = OutputPin->GetInputPins().begin();
		i != OutputPin->GetInputPins().end(); i++)
	{
		// レンダリング世代が最新の物かどうかをチェック
		if((*i)->GetRenderGeneration() != state->GetRenderGeneration()) continue;

		// 入力ピンのタイプをチェックし、適切なキューノードを作成
		risse_uint32 target_type = (*i)->GetAgreedType();

		RISSE_ASSERT(target_type == WideTextEdgeType || target_type == NarrowTextEdgeType); // 暫定
		if(target_type == WideTextEdgeType)
			new tWideTextProviderQueueNode((*i)->GetParentQueueNode(), Position, Caption);
		else if(target_type == NarrowTextEdgeType)
			new tNarrowTextProviderQueueNode((*i)->GetParentQueueNode(), Position, Caption.AsNarrowString());
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}
