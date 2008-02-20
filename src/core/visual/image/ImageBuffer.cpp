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
#include "base/log/Log.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(12015,30980,25352,17139,45454,47885,46776,11050);
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
void tImageBuffer::IncBufferLockCount()
{
	volatile tCriticalSection::tLocker lock(*CS);
	if(++BufferLockCount == 1) LockBuffer();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageBuffer::DecBufferLockCount()
{
	volatile tCriticalSection::tLocker lock(*CS);
	if(--BufferLockCount == 0) UnlockBuffer();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint32 tImageBuffer::GetARGB32(risse_size x, risse_size y)
{
	const tBufferPointer & ptr = GetBufferPointer();
	risse_uint32 ret;
	try
	{
		ret = 
			tPixel::ConvertToARGB32(
				static_cast<const risse_uint8*>(ptr.Buffer) + ptr.Pitch * y +
					x * tPixel::GetDescriptorFromFormat(Descriptor.PixelFormat).Size,
				Descriptor.PixelFormat);
	}
	catch(...)
	{
		ptr.Release();
		throw;
	}
	ptr.Release();
	return ret;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageBuffer::SetARGB32(risse_size x, risse_size y, risse_uint32 v)
{
	const tBufferPointer & ptr = GetBufferPointer();
	try
	{
		tPixel::ConvertFromARGB32(
				static_cast<risse_uint8*>(ptr.Buffer) + ptr.Pitch * y +
					x * tPixel::GetDescriptorFromFormat(Descriptor.PixelFormat).Size,
				Descriptor.PixelFormat,
				v);
	}
	catch(...)
	{
		ptr.Release();
		throw;
	}
	ptr.Release();
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
template <typename pixel_t, int alignment, int pixel_format>
tMemoryImageBuffer<pixel_t, alignment, pixel_format>::tMemoryImageBuffer(risse_size w, risse_size h)
{
	int retry_count = 3; // 3 回までリトライ
retry:
	try
	{
		PixelStore = new pixe_store_t(w, h);
	}
	catch(std::bad_alloc)
	{
		if(retry_count--)
		{
			tLogger::Log(tString(
				RISSE_WS_TR("allocation of memory image buffer (%1x%2, %3) failed. retrying after GC ..."),
					tString::AsString((risse_int64)w), tString::AsString((risse_int64)h),
					tPixel::GetDescriptorFromFormat(static_cast<tPixel::tFormat>(pixel_format)).LongDesc),
				tLogger::llNotice);
			CollectGarbage();
			goto retry;
		}
		throw;
	}

	Descriptor.PixelFormat = static_cast<tPixel::tFormat>(pixel_format);
	Descriptor.Width = w;
	Descriptor.Height = h;

	BufferPointer.Buffer = &*PixelStore->begin();
	BufferPointer.Pitch = PixelStore->get_fragment_length();
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
				static_cast<risse_uint8*>(new_ib->BufferPointer.Buffer) + new_ib->BufferPointer.Pitch * y,
				static_cast<const risse_uint8*>(BufferPointer.Buffer) + BufferPointer.Pitch * y,
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


