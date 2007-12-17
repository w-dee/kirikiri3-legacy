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
//! @brief RINA ノード管理
//---------------------------------------------------------------------------
#include "prec.h"
#include "visual/rina/rinaNode.h"
#include "visual/rina/rinaPin.h"


namespace Rina {
RISSE_DEFINE_SOURCE_ID(35503,37740,38367,18777,41870,21345,15082,43304);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tProcessNode::tProcessNode() : inherited()
{
	LongestDistance = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tProcessNode::CalcLongestDistance()
{
	risse_size longest = risse_size_max;
	risse_size output_pincount = GetOutputPinCount();
	for(risse_size i = 0; i < output_pincount; i++)
	{
		risse_size dist = GetOutputPinAt(i)->GetLongestDistance();
		if(longest == risse_size_max || longest < dist) longest = dist;
	}
	LongestDistance = longest;

	// 子に再帰
	risse_size input_pincount = GetInputPinCount();
	for(risse_size i = 0; i < input_pincount; i++)
	{
		GetInputPinAt(i)->GetOutputPin()->GetNode()->CalcLongestDistance();
	}
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
}
