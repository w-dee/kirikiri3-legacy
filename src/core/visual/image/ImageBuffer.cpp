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
tARGB32MemoryImageBuffer::tARGB32MemoryImageBuffer(risse_size w, risse_size h)
{
	PixelStore = new pixe_store_t(w, h);
	Descriptor.PixelFormat = pfARGB32;
	Descriptor.Buffer = &*PixelStore->begin();
	Descriptor.Pitch = PixelStore->get_fragment_length();
	Descriptor.Width = w;
	Descriptor.Height = h;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
tGray8MemoryImageBuffer::tGray8MemoryImageBuffer(risse_size w, risse_size h)
{
	PixelStore = new pixe_store_t(w, h);
	Descriptor.PixelFormat = pfGray8;
	Descriptor.Buffer = &*PixelStore->begin();
	Descriptor.Pitch = PixelStore->get_fragment_length();
	Descriptor.Width = w;
	Descriptor.Height = h;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
} // namespace Risa


