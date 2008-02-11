//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief イメージバッファクラス
//---------------------------------------------------------------------------
#include "prec.h"
#include "visual/image/ImageBuffer.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(12015,30980,25352,17139,45454,47885,46776,11050);
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
template <typename pixel_t, int alignment, int pixel_format>
tMemoryImageBuffer<pixel_t, alignment, pixel_format>::tMemoryImageBuffer(risse_size w, risse_size h)
{
	PixelStore = new pixe_store_t(w, h);
	Descriptor.PixelFormat = static_cast<tPixel::tFormat>(pixel_format);
	Descriptor.Buffer = &*PixelStore->begin();
	Descriptor.Pitch = PixelStore->get_fragment_length();
	Descriptor.Width = w;
	Descriptor.Height = h;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
template <typename pixel_t, int alignment, int pixel_format>
tImageBuffer * tMemoryImageBuffer<pixel_t, alignment, pixel_format>::Clone(bool copy_image)
{
	self_type * new_ib = new self_type(Descriptor.Width, Descriptor.Height);
	if(copy_image)
	{
		const tPixel::tDescriptor & pixel_desc =
			tPixel::GetDescriptorFromFormat(static_cast<tPixel::tFormat>(pixel_format));
		// メモリイメージまんま全部コピーしても良いが、より汎用的な方法で。
		for(risse_size y = 0; y < Descriptor.Height; y++)
		{
			memcpy(
				static_cast<risse_uint8*>(new_ib->Descriptor.Buffer) + new_ib->Descriptor.Pitch * y,
				static_cast<const risse_uint8*>(Descriptor.Buffer) + Descriptor.Pitch * y,
				pixel_desc.Size * Descriptor.Width);
		}
	}
	return new_ib;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// 実体化
//---------------------------------------------------------------------------
template class tMemoryImageBuffer<risa_gl::pixel, 16, tPixel::pfARGB32>;
template class tMemoryImageBuffer<risse_uint8, 4, tPixel::pfGray8>;
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa


