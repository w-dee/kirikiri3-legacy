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
#include "visual/rina/test/rinaTextMixerNode.h"
#include "visual/rina/test/rinaTextPin.h"
#include "visual/rina/test/rinaTextProviderNode.h"
#include "visual/rina/test/rina1DPositionProperty.h"

namespace Rina {
RISSE_DEFINE_SOURCE_ID(8982,48844,33706,17807,17033,58515,58827,7512);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tTextMixerPropertySet::tTextMixerPropertySet()
{
	Add(t1DPositionPropertyInfo::instance());
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tTextMixerNode::tTextMixerNode() : inherited()
{
	// プロパティオブジェクトを作成
	SetProperty(new tProperty(tTextMixerPropertySet::instance()));

	// 出力ピンを作成
	OutputPin = new tTextOutputPin();
	OutputPin->Attach(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tTextMixerNode::GetOutputPinCount()
{
	return 1; // 出力ピンは1個
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOutputPin * tTextMixerNode::GetOutputPinAt(risse_size n)
{
	// TODO: 例外
	if(n == 0) return OutputPin;
	return NULL; // 出力ピンはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextMixerNode::InsertOutputPinAt(risse_size n)
{
	// 出力ピンを追加することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextMixerNode::DeleteOutputPinAt(risse_size n)
{
	// 出力ピンを削除することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tTextMixerNode::GetInputPinCount()
{
	return InputPins.size();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tInputPin * tTextMixerNode::GetInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	return InputPins[n];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextMixerNode::InsertInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	tTextInputPin * newpin = new tTextInputPin();
	newpin->Attach(this);
	InputPins.insert(InputPins.begin() + n, newpin);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextMixerNode::DeleteInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	InputPins.erase(InputPins.begin() + n);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextMixerNode::BuildQueue(tQueueNode * parent)
{
	tQueueNode * new_parent = new tTextMixerQueueNode(
		(risse_int32)(risse_int64)(
			GetProperty()->GetValueAt(t1DPositionPropertyInfo::instance()->ID_Position)));
	parent->AddChild(new_parent);

	// 入力ピンに再帰
	for(gc_vector<tInputPin *>::iterator i = InputPins.begin(); i != InputPins.end(); i++)
		(*i)->BuildQueue(new_parent);
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
tTextMixerQueueNode::tTextMixerQueueNode(risse_int32 pos) : inherited(pos, tString())
{
	Canvas = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextMixerQueueNode::BeginProcess()
{
	// キャンバス用にメモリを確保
	Canvas = (risse_char *)MallocAtomicCollectee(sizeof(risse_char) * (CanvasSize + 1));

	// キャンバスを空白で埋める
	for(risse_size i = 0; i < CanvasSize; i++) Canvas[i] = RISSE_WC(' ');
	Canvas[CanvasSize] = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextMixerQueueNode::EndProcess()
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

	// 結果をTextに格納
	Text = Canvas;
}
//---------------------------------------------------------------------------

}
