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
tNodeInstance::tNodeInstance(tGraphInstance * graph)
{
	GraphInstance = graph;
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
void tNodeInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tNodeClass::tNodeClass(tScriptEngine * engine) :
	inherited(tSS<'G','r','a','p','h'>(), engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNodeClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tNodeClass::ovulate);
	BindFunction(this, ss_construct, &tNodeInstance::construct);
	BindFunction(this, ss_initialize, &tNodeInstance::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tNodeClass::ovulate()
{
	// このクラスのインスタンスは生成できない
	tInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
	return tVariant();
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		Node クラスレジストラ
//---------------------------------------------------------------------------
template class tClassRegisterer<
	tSS<'r','i','s','a','.','g','r','a','p','h','i','c','.','r','i','n','a'>,
	tNodeClass>;
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
}
