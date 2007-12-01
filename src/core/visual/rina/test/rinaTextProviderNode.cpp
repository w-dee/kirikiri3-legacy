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
#include "visual/rina/test/rinaTextProviderNode.h"
#include "visual/rina/test/rinaTextProperty.h"
#include "visual/rina/test/rina1DPositionProperty.h"
#include "visual/rina/test/rinaTextPin.h"

namespace Rina {
RISSE_DEFINE_SOURCE_ID(55116,34738,34674,19554,62891,6474,53398,51508);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tTextPropertySet::tTextPropertySet()
{
	Add(tCaptionPropertyInfo::instance());
	Add(t1DPositionPropertyInfo::instance());
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
tTextProviderNode::tTextProviderNode()
{
	// プロパティオブジェクトを作成
	SetProperty(new tProperty(tTextPropertySet::instance()));

	// 出力ピンを作成
	OutputPin = new tTextOutputPin();
	OutputPin->Attach(this);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tTextProviderNode::GetOutputPinCount()
{
	return 1; // 出力ピンは１個
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOutputPin * tTextProviderNode::GetOutputPinAt(risse_size n)
{
	// TODO: 例外
	if(n == 0) return OutputPin;
	return NULL; // 出力ピンはない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextProviderNode::InsertOutputPinAt(risse_size n)
{
	// 出力ピンを追加することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextProviderNode::DeleteOutputPinAt(risse_size n)
{
	// 出力ピンを削除することはできない
	// TODO: 例外
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tTextProviderNode::GetInputPinCount()
{
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tInputPin * tTextProviderNode::GetInputPinAt(risse_size n)
{
	// XXX: 範囲外例外
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextProviderNode::InsertInputPinAt(risse_size n)
{
	// XXX: 入力ピンを追加することはできない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextProviderNode::DeleteInputPinAt(risse_size n)
{
	// XXX: 入力ピンを削除することはできない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTextProviderNode::BuildComandQueue(tQueueNode * parent)
{
	parent->AddChild(new tTextDrawDeviceQueueNode(
		(risse_int32)(risse_int64)(GetProperty()->GetValueAt(t1DPositionPropertyInfo::instance()->ID_Position)),
		                           GetProperty()->GetValueAt(tCaptionPropertyInfo::instance()->ID_Caption    )
		));

}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
}
