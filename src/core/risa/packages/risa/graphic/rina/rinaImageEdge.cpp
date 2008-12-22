//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"

	Rina stands for "Rina is an Imaging Network Assembler"
*/
//---------------------------------------------------------------------------
/**
 * @file
 * RINA ラスタ画像用のピンなど
 */
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/graphic/rina/rinaImageEdge.h"
#include "risa/common/RisaThread.h"
#include "risseNativeBinder.h"
#include "risseObjectClass.h"
#include "risseStaticStrings.h"
#include "risa/common/RisseEngine.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(2075,61475,13040,16980,13187,37659,27738,36185);
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tImageEdgeTypeRegisterer::tImageEdgeTypeRegisterer()
{
	tIdRegistry::tEdgeData data;
	data.Id = ImageEdgeType;
	data.ShortName = RISSE_WS("image");
	data.ShortDesc = RISSE_WS("raster image");
	data.LongDesc = RISSE_WS("Raster 2-dimensional image");
	tIdRegistry::instance()->RegisterEdgeData(data);
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tImageInputPinInstance::tImageInputPinInstance()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const gc_vector<risse_uint32> & tImageInputPinInstance::GetSupportedTypes()
{
	// 暫定実装、できれば static かシングルトン上の配列への参照を返した方がよい
	gc_vector<risse_uint32> * arr = new gc_vector<risse_uint32>();
	arr->push_back(ImageEdgeType);
	return *arr;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageInputPinInstance::NotifyUpdate(const tTexturePolygon & rect)
{
	RISSE_ASSERT_CS_LOCKED(*GetNodeInstance()->GetGraphInstance()->GetCS());

	// TODO
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void tImageInputPinInstance::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageInputPinInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tImageInputPinClass,
		(tSS<'I','m','a','g','e','I','n','p','u','t','P','i','n'>()),
		tClassHolder<tInputPinClass>::instance()->GetClass())
	RISSE_BIND_CONSTRUCTORS
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * ImageInputPin クラスレジストラ
 */
//---------------------------------------------------------------------------
template class tClassRegisterer<
	tSS<'r','i','s','a','.','g','r','a','p','h','i','c','.','r','i','n','a'>,
	tImageInputPinClass>;
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
void tImageMixerInputPinInstance::NotifyUpdate(const tTexturePolygon & area)
{
	RISSE_ASSERT_CS_LOCKED(*GetNodeInstance()->GetGraphInstance()->GetCS());

	// TODO: 通常、ここでは変形先座標を通知する必要がある
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void tImageMixerInputPinInstance::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageMixerInputPinInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tImageMixerInputPinClass,
		(tSS<'I','m','a','g','e','M','i','x','e','r','I','n','p','u','t','P','i','n'>()),
		tClassHolder<tImageInputPinClass>::instance()->GetClass())
	RISSE_BIND_CONSTRUCTORS
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * ImageMixerInputPin クラスレジストラ
 */
//---------------------------------------------------------------------------
template class tClassRegisterer<
	tSS<'r','i','s','a','.','g','r','a','p','h','i','c','.','r','i','n','a'>,
	tImageMixerInputPinClass>;
//---------------------------------------------------------------------------




















//---------------------------------------------------------------------------
tImageOutputPinInstance::tImageOutputPinInstance()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const gc_vector<risse_uint32> & tImageOutputPinInstance::GetSupportedTypes()
{
	// 暫定実装、できれば static かシングルトン上の配列への参照を返した方がよい
	gc_vector<risse_uint32> * arr = new gc_vector<risse_uint32>();
	arr->push_back(ImageEdgeType);
	return *arr;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageOutputPinInstance::NotifyUpdate(const tTexturePolygon & area)
{
	RISSE_ASSERT_CS_LOCKED(*GetNodeInstance()->GetGraphInstance()->GetCS());

	tEnumerableIterator it(GetInputPins());
	while(it.Next())
	{
		// TODO: すべての入力ピンに対してアップデート通知を送る
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void tImageOutputPinInstance::construct()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageOutputPinInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tImageOutputPinClass,
		(tSS<'I','m','a','g','e','O','u','t','p','u','t','P','i','n'>()),
		tClassHolder<tOutputPinClass>::instance()->GetClass())
	RISSE_BIND_CONSTRUCTORS
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * ImageInputPin クラスレジストラ
 */
//---------------------------------------------------------------------------
template class tClassRegisterer<
	tSS<'r','i','s','a','.','g','r','a','p','h','i','c','.','r','i','n','a'>,
	tImageOutputPinClass>;
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
}
