//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief イメージコーデッククラス
//---------------------------------------------------------------------------
#include "prec.h"
#include "visual/image/ImageCodec.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(24692,13904,5736,19025,425,432,30553,23251);
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
tImageDecoder::tImageDecoder()
{
	Decoded = false;
	Image = NULL;
	LastConvertBuffer = NULL;
	LastConvertBufferSize = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageDecoder::Decode(tStreamInstance * stream, tImage * image,
	tPixel::tFormat pixel_format, tDictionaryInstance * dict)
{
	// TODO: image のロック
	// TODO: this のロック
	if(Decoded) { RISSE_ASSERT(!Decoded); /* TODO: 例外 */ }
	Decoded = true;
	Image = image;

	if(Image->HasBuffer() && Image->GetDescriptor().PixelFormat != pixel_format)
	{
		// TODO: PixelFormat が違うとの例外
		return;
	}

	// デコーダの本体処理を呼び出す
	DesiredPixelFormat = pixel_format;
	Process(stream, image, pixel_format, dict);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageDecoder::SetDimensions(risse_size w, risse_size h,
									tPixel::tFormat pixel_format)
{
	// Image にすでにバッファが割り当てられている場合はそのサイズをチェック
	if(Image->HasBuffer())
	{
		const tImageBuffer::tDescriptor & desc = Image->GetDescriptor();
		if(desc.Width != w || desc.Height != h)
		{
			// TODO: サイズが違うとの例外
			RISSE_ASSERT(desc.Width == w && desc.Height == h);
		}
	}

	// そうでなければイメージバッファを独立させる
	Image->Independ(false);

	// メモリ上のバッファを image に割り当てる
	Image->New(DesiredPixelFormat, w, h);

	// デコーダが望むピクセル形式を保存
	DecoderPixelFormat = pixel_format;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void * tImageDecoder::StartLines(risse_size y, risse_size h, risse_offset & pitch)
{
	LastLineY = y;
	LastLineH = h;

	// デコーダが望むピクセル形式の場合はそのままイメージバッファのメモリ領域を返す
	if(DecoderPixelFormat == DesiredPixelFormat)
	{
		// デコーダが望むピクセル形式とイメージバッファのピクセル形式が同じ
		const tImageBuffer::tDescriptor & desc = Image->GetDescriptorForWrite();
		pitch = desc.Pitch;
		return static_cast<risse_uint8*>(desc.Buffer) + pitch * y;
	}

	// そうでない場合は変換用バッファを作成してそれを返す
	// 変換用バッファのサイズは………
	const tImageBuffer::tDescriptor & image_desc = Image->GetDescriptorForWrite();
	const tPixel::tDescriptor & pixel_desc = tPixel::GetDescriptorFromFormat(image_desc.PixelFormat);

	// んー
	// 横幅に必要なバイト数を決定(16bytes に整列)
	risse_size width_bytes = pixel_desc.Size * image_desc.Width;
	LastCnvertBufferPitch = pitch = (width_bytes + 15) & ~ 15;
	// バッファサイズは？
	risse_size buffer_size = LastCnvertBufferPitch * image_desc.Height;
	// すでに割り当たってる？
	if(LastConvertBufferSize < buffer_size)
	{
		// 足りないので割り当て直す
		LastConvertBuffer = AlignedMallocCollectee(buffer_size, 4);
		LastConvertBufferSize = buffer_size;
	}

	return LastConvertBuffer;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageDecoder::DoneLines()
{
	// デコーダが望むピクセル形式の場合はなにもしない
	if(DecoderPixelFormat == DesiredPixelFormat) return;

	// そうでない場合は変換が必要です
	const tImageBuffer::tDescriptor & desc = Image->GetDescriptorForWrite();
	for(risse_size y = 0; y < LastLineH; y++)
	{
		tPixel::Convert(
			static_cast<risse_uint8*>(desc.Buffer) + desc.Pitch * (y + LastLineY),
			DesiredPixelFormat,
			static_cast<risse_uint8*>(LastConvertBuffer) + LastCnvertBufferPitch * y,
			DecoderPixelFormat,
			desc.Width);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa


