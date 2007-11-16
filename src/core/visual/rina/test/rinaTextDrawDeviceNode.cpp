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
#include "visual/rina/rinaProcess.h"


namespace Rina {
RISSE_DEFINE_SOURCE_ID(35503,37740,38367,18777,41870,21345,15082,43304);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tTextDrawDeviceNode::tTextDrawDeviceNode(tGraph * graph) : inherited(graph)
{
	
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tTextDrawDeviceNode::GetParentCount()
{
	return 0; // 親はない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tProcessNode * tTextDrawDeviceNode::GetParentAt(risse_size n)
{
	return NULL; // 親はない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextDrawDeviceNode::SetParentAt(risse_size n, tProcessNode * node)
{
	// 親は無いので設定できない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextDrawDeviceNode::InsertParentPinAt(risse_size n)
{
	// 親を追加することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextDrawDeviceNode::DeleteParentPinAt(risse_size n)
{
	// 親を削除することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tTextDrawDeviceNode::GetChildCount()
{
	return Children.size();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tInputPin * tTextDrawDeviceNode::GetChildAt(risse_size n)
{
	// XXX: 範囲外例外
	return Children[n];
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextDrawDeviceNode::SetChildAt(risse_size n, tProcessNode * node)
{
	// XXX: ピンのタイプのチェック
	// XXX: 範囲外例外
	if(!Children[n]) Childrenn] = new tTextInputPin();
	Children[n]->Attach(node);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextDrawDeviceNode::InsertChildPinAt(risse_size n)
{
	// XXX: 範囲外例外
	tTextInputPin * newpin = new tTextInputPin();
	newpin->Attatch(this);
	Children.insert(Children.begin() + n, newpin);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextDrawDeviceNode::DeleteChildPinAt(risse_size n)
{
	// XXX: 範囲外例外
	Children.erase(Children.begin() + n);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
}
