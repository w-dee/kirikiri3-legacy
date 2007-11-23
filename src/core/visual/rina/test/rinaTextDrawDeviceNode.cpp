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


namespace Rina {
RISSE_DEFINE_SOURCE_ID(35503,37740,38367,18777,41870,21345,15082,43304);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tTextDrawDeviceNode::tTextDrawDeviceNode(tGraph * graph) : inherited(graph)
{
	
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tTextDrawDeviceNode::GetOutputPinCount();
{
	return 0; // 出力ピンはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOutputPin * tTextDrawDeviceNode::GetOutputPinAt(risse_size n);
{
	// TODO: 例外
	return NULL; // 出力ピンはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextDrawDeviceNode::InsertOutputPinAt(risse_size n);
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
	newpin->Attatch(this);
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
}
