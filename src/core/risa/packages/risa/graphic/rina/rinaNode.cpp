//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief RINA ノード管理
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/graphic/rina/rinaNode.h"
#include "risa/packages/risa/graphic/rina/rinaPin.h"
#include "risse/include/risseNativeBinder.h"
#include "risse/include/risseObjectClass.h"
#include "risse/include/risseStaticStrings.h"
#include "risse/include/risseExceptionClass.h"
#include "risa/common/RisseEngine.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(35503,37740,38367,18777,41870,21345,15082,43304);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tNodeInstance::tNodeInstance()
{
	GraphInstance = NULL;
	LongestDistance = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNodeInstance::CalcLongestDistance()
{
	RISSE_ASSERT_CS_LOCKED(*GetGraphInstance()->GetCS());

	risse_size longest = risse_size_max;
	risse_size output_pincount = GetOutputPinArrayInstance().GetCount();
	for(risse_size i = 0; i < output_pincount; i++)
	{
		risse_size dist = GetOutputPinArrayInstance().At(i)->GetLongestDistance();
		if(longest == risse_size_max || longest < dist) longest = dist;
	}
	LongestDistance = longest + 1;

	// 子に再帰
	risse_size input_pincount = GetInputPinArrayInstance().GetCount();
	for(risse_size i = 0; i < input_pincount; i++)
	{
		GetInputPinArrayInstance().At(i)->GetOutputPinInstance()->
			GetNodeInstance()->CalcLongestDistance();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNodeInstance::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNodeInstance::initialize(const tVariant & graph, const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	SetGraphInstance(graph.ExpectAndGetObjectInterface(
					tClassHolder<tGraphClass>::instance()->GetClass()));

	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tNodeClass, (tSS<'G','r','a','p','h'>()), engine->ObjectClass)
	BindFunction(this, ss_ovulate, &tNodeClass::ovulate);
	BindFunction(this, ss_construct, &tNodeInstance::construct);
	BindFunction(this, ss_initialize, &tNodeInstance::initialize);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
/**
 * Node クラスレジストラ
 */
template class tClassRegisterer<
	tSS<'r','i','s','a','.','g','r','a','p','h','i','c','.','r','i','n','a'>,
	tNodeClass>;
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
}
