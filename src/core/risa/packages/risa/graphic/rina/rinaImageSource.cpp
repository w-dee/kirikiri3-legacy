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
//! @brief RINA ラスタ画像用ソース
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/graphic/rina/rinaImageSource.h"
#include "risa/packages/risa/graphic/rina/rinaPin.h"
#include "risa/common/RisaThread.h"
#include "risse/include/risseNativeBinder.h"
#include "risse/include/risseObjectClass.h"
#include "risse/include/risseStaticStrings.h"
#include "risa/common/RisseEngine.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(7573,48746,61484,31356,16289,41410,60513,41447);
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
tImageSourceNodeInstance::tImageSourceNodeInstance()
{
	InputPinArrayInstance = NULL;
	OutputPinArrayInstance = NULL;
	OutputPinInstance = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tInputPinArrayInstance & tImageSourceNodeInstance::GetInputPinArrayInstance()
{
	return *InputPinArrayInstance;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOutputPinArrayInstance & tImageSourceNodeInstance::GetOutputPinArrayInstance()
{
	return *OutputPinArrayInstance;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageSourceNodeInstance::construct()
{
	// 入力ピンインスタンスを作成
	OutputPinInstance =
		tClassHolder<tImageOutputPinClass>::instance()->GetClass()->
			Invoke(ss_new).
		ExpectAndGetObjectInterface(
		tClassHolder<tImageOutputPinClass>::instance()->GetClass()
		);
	// 入力ピン配列と出力ピン配列を生成
	InputPinArrayInstance =
			tClassHolder<tInputPinArrayClass>::instance()->GetClass()->
				Invoke(ss_new, tVariant(this)).
			ExpectAndGetObjectInterface(
			tClassHolder<tInputPinArrayClass>::instance()->GetClass()
			);
	OutputPinArrayInstance =
			tClassHolder<tOneOutputPinArrayClass>::instance()->GetClass()->
				Invoke(ss_new, tVariant(this), tVariant(OutputPinInstance)).
			ExpectAndGetObjectInterface(
			tClassHolder<tOneOutputPinArrayClass>::instance()->GetClass()
			);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageSourceNodeInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	info.InitializeSuperClass(info.args); // 引数はそのまま渡す
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tImageSourceNodeClass,
		(tSS<'I','m','a','g','e','S','o','u','r','c','e'>()),
		tClassHolder<tNodeClass>::instance()->GetClass())
	RISSE_BIND_CONSTRUCTORS
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
/**
 * ImageSourceNode クラスレジストラ
 */
template class tClassRegisterer<
tSS<'r','i','s','a','.','g','r','a','p','h','i','c','.','r','i','n','a'>,
	tImageSourceNodeClass>;
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
}
