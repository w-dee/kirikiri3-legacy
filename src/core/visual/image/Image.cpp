//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief イメージクラス
//---------------------------------------------------------------------------
#include "prec.h"
#include "visual/image/Image.h"
#include "risse/include/risseNativeBinder.h"
#include "risse/include/risseScriptEngine.h"
#include "risse/include/risseStaticStrings.h"

namespace Risa {
RISSE_DEFINE_SOURCE_ID(59719,27864,5820,19638,31923,42718,35156,64208);
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tImageInstance::tImageInstance()
{
	ImageBuffer = new tGCReferencePtr<tImageBuffer>;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageInstance::Dispose()
{
	volatile tSynchronizer sync(this); // sync

	// 明示的に ImageBuffer を dispose しなくても、いずれ GC が回収してくれるが
	// 画像バッファは大きなリソースなので明示的な dispose() を推奨。
	ImageBuffer->dispose();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageInstance::Allocate(tPixel::tFormat format, risse_size w, risse_size h)
{
	volatile tSynchronizer sync(this); // sync

	tImageBuffer * buf = NULL;

	switch(format)
	{
	case tPixel::pfGray8:
		buf = new tGray8MemoryImageBuffer(w, h);
		break;
	case tPixel::pfARGB32:
		buf = new tARGB32MemoryImageBuffer(w, h);
		break;
	default:;
	}
	RISSE_ASSERT(buf != NULL);

	ImageBuffer->set(buf);
	buf->Release(); // ImageBuffer が後は参照カウンタを管理するのでここでは持っている必要なし
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageInstance::Independ(bool clone)
{
	volatile tSynchronizer sync(this); // sync

	RISSE_ASSERT(*ImageBuffer);

	tImageBuffer * buf = (*ImageBuffer)->Independ(clone);
	ImageBuffer->set(buf);
	buf->Release(); // ImageBuffer が後は参照カウンタを管理するのでここでは持っている必要なし
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageInstance::construct()
{

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageInstance::allocate(risse_size w, risse_size h, const tMethodArgument &args)
{
	// pixel format のデフォルトは今のところ tPixel::pfARGB32
	Allocate(args.HasArgument(2) ? (tPixel::tFormat)(risse_int64)args[2] : tPixel::pfARGB32, w, h);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageInstance::independ(const tMethodArgument &args)
{
	Independ(args.HasArgument(0) ? (bool)args[0] : true);
}
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
tImageClass::tImageClass(tScriptEngine * engine) :
	tClassBase(tSS<'I','m','a','g','e'>(), engine->ObjectClass)
{
	RegisterMembers();

	// PixelConsts を include する
	Do(ocFuncCall, NULL, ss_include, 0,
		tMethodArgument::New(
			tRisseModuleRegisterer<tPixelConstsModule>::instance()->GetModuleInstance()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tImageClass::ovulate);
	BindFunction(this, ss_construct, &tImageInstance::construct);
	BindFunction(this, ss_initialize, &tImageInstance::initialize);
	BindFunction(this, ss_dispose, &tImageInstance::dispose);
	BindFunction(this, tSS<'a','l','l','o','c','a','t','e'>(), &tImageInstance::allocate);
	BindFunction(this, tSS<'d','e','a','l','l','o','c','a','t','e'>(), &tImageInstance::deallocate);
	BindFunction(this, tSS<'i','n','d','e','p','e','n','d'>(), &tImageInstance::independ);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tImageClass::ovulate()
{
	return tVariant(new tImageInstance());
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		Image クラスレジストラ
template class tRisseClassRegisterer<tImageClass>;
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa


