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


namespace Risa {
RISSE_DEFINE_SOURCE_ID(59719,27864,5820,19638,31923,42718,35156,64208);
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
tImage::tImage()
{
	ImageBuffer = new tGCReferencePtr<tImageBuffer>;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImage::Dispose()
{
	// TODO: スレッド保護
	// 明示的に ImageBuffer を dispose しなくても、いずれ GC が回収してくれるが
	// 画像バッファは大きなリソースなので明示的な dispose() を推奨。
	ImageBuffer->dispose();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImage::New(tPixel::tFormat format, risse_size w, risse_size h)
{
	// TODO: スレッド保護
	tImageBuffer * buf = NULL;

	switch(format)
	{
	case tPixel::pfGray8:
		buf = new tGray8MemoryImageBuffer(w, h);
		break;
	case tPixel::pfARGB32:
		buf = new tARGB32MemoryImageBuffer(w, h);
		break;
	}
	RISSE_ASSERT(buf != NULL);

	ImageBuffer->set(buf);
	buf->Release(); // ImageBuffer が後は参照カウンタを管理するのでここでは持っている必要なし
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImage::Independ(bool clone)
{
	// TODO: スレッド保護
	RISSE_ASSERT(*ImageBuffer);

	tImageBuffer * buf = (*ImageBuffer)->Independ(clone);
	ImageBuffer->set(buf);
	buf->Release(); // ImageBuffer が後は参照カウンタを管理するのでここでは持っている必要なし
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace Risa


