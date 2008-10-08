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
//! @brief RINA ラスタ画像用のピンなど
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/graphic/rina/rinaImageEdge.h"
#include "risa/common/RisaThread.h"
#include "risse/include/risseNativeBinder.h"
#include "risse/include/risseObjectClass.h"
#include "risse/include/risseStaticStrings.h"
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
tImageInputPinClass::tImageInputPinClass(tScriptEngine * engine) :
	inherited(tSS<'I','m','a','g','e','I','n','p','u','t','P','i','n'>(),
	tClassHolder<tInputPinClass>::instance()->GetClass())
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageInputPinClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tImageInputPinClass::ovulate);
	BindFunction(this, ss_construct, &tImageInputPinInstance::construct);
	BindFunction(this, ss_initialize, &tImageInputPinInstance::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tImageInputPinClass::ovulate()
{
	return tVariant(new tImageInputPinInstance());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ImageInputPin クラスレジストラ
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
tImageMixerInputPinClass::tImageMixerInputPinClass(tScriptEngine * engine) :
	inherited(tSS<'I','m','a','g','e','M','i','x','e','r','I','n','p','u','t','P','i','n'>(),
	tClassHolder<tImageInputPinClass>::instance()->GetClass())
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageMixerInputPinClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tImageMixerInputPinClass::ovulate);
	BindFunction(this, ss_construct, &tImageMixerInputPinInstance::construct);
	BindFunction(this, ss_initialize, &tImageMixerInputPinInstance::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tImageMixerInputPinClass::ovulate()
{
	return tVariant(new tImageMixerInputPinInstance());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ImageMixerInputPin クラスレジストラ
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
tImageOutputPinClass::tImageOutputPinClass(tScriptEngine * engine) :
	inherited(tSS<'I','m','a','g','e','O','u','t','p','u','t','P','i','n'>(),
	tClassHolder<tOutputPinClass>::instance()->GetClass())
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageOutputPinClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tImageOutputPinClass::ovulate);
	BindFunction(this, ss_construct, &tImageOutputPinInstance::construct);
	BindFunction(this, ss_initialize, &tImageOutputPinInstance::initialize);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tImageOutputPinClass::ovulate()
{
	return tVariant(new tImageOutputPinInstance());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ImageInputPin クラスレジストラ
//---------------------------------------------------------------------------
template class tClassRegisterer<
	tSS<'r','i','s','a','.','g','r','a','p','h','i','c','.','r','i','n','a'>,
	tImageOutputPinClass>;
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
}
