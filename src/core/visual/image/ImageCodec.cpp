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
#include "base/fs/common/FSManager.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(24692,13904,5736,19025,425,432,30553,23251);
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
tImageDecoder::tImageDecoder()
{
	Decoded = false;
	Image = NULL;
	ImageBuffer = NULL;
	LastConvertBuffer = NULL;
	LastConvertBufferSize = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageDecoder::Decode(tStreamInstance * stream, tImageInstance * image,
	tPixel::tFormat pixel_format, tProgressCallback * callback, tDictionaryInstance * dict)
{
	if(Decoded) { RISSE_ASSERT(!Decoded); /* TODO: 例外 */ }
	Decoded = true;
	Image = image;

	{
		// Image をロック
		volatile tObjectInterface::tSynchronizer sync(Image); // sync

		if(Image->HasBuffer() && Image->GetDescriptor().PixelFormat != pixel_format)
		{
			// TODO: PixelFormat が違うとの例外
			return;
		}

		DesiredPixelFormat = pixel_format;
		ImageBuffer = Image->GetBuffer();
	}


	// デコーダの本体処理を呼び出す
	try
	{
		if(callback) callback->CallOnProgress(1, 0); // 0%


#ifdef RISSE_ASSERT_ENABLED
	// デコーダがちゃんと dict の中身をクリアする実装になってるかをチェックするためにダミーの要素を追加する
	if(dict) dict->Invoke(tSS<'[',']','='>(), tVariant(tSS<'d','u','m','m','y'>()), tVariant(tSS<'_','d','u','m','m','y'>()));
#endif


		Process(stream, pixel_format, callback, dict);


#ifdef RISSE_ASSERT_ENABLED
	// デコーダがちゃんと dict の中身をクリアする実装になってるかをチェックする
	if(dict) RISSE_ASSERT(dict->Invoke(tSS<'[',']'>(), tVariant(tSS<'_','d','u','m','m','y'>()).IsVoid()));
#endif


		if(callback) callback->CallOnProgress(1, 1); // 100%
	}
	catch(...)
	{
		ImageBuffer->Release(), ImageBuffer = NULL;
		throw;
	}
	ImageBuffer->Release(), ImageBuffer = NULL;

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageDecoder::SetDimensions(risse_size w, risse_size h,
									tPixel::tFormat pixel_format)
{
	// Image をロック
	volatile tObjectInterface::tSynchronizer sync(Image); // sync

	// Image にすでにバッファが割り当てられている場合はそのサイズをチェック
	if(ImageBuffer)
	{
		const tImageBuffer::tDescriptor & desc = Image->GetDescriptor();
		if(desc.Width != w || desc.Height != h)
		{
			// TODO: サイズが違うとの例外
			RISSE_ASSERT(desc.Width == w && desc.Height == h);
		}
	}
	else
	{
		// そうでなければイメージバッファを独立させる
		Image->Independ(false);

		// メモリ上のバッファを image に割り当てる
		Image->Allocate(DesiredPixelFormat, w, h);

		// イメージバッファを取得する
		ImageBuffer = Image->GetBuffer();
	}

	// デコーダが望むピクセル形式を保存
	DecoderPixelFormat = pixel_format;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void * tImageDecoder::StartLines(risse_size y, risse_size h, risse_offset & pitch)
{
	LastLineY = y;
	LastLineH = h;

	const tImageBuffer::tDescriptor & image_desc = ImageBuffer->GetDescriptor();

	// デコーダが望むピクセル形式の場合はそのままイメージバッファのメモリ領域を返す
	if(DecoderPixelFormat == DesiredPixelFormat)
	{
		// デコーダが望むピクセル形式とイメージバッファのピクセル形式が同じ
		pitch = image_desc.Pitch;
		return static_cast<risse_uint8*>(image_desc.Buffer) + pitch * y;
	}

	// そうでない場合は変換用バッファを作成してそれを返す
	// 変換用バッファのサイズは………
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
		LastConvertBuffer = AlignedMallocAtomicCollectee(buffer_size, 4);
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
	const tImageBuffer::tDescriptor & image_desc = ImageBuffer->GetDescriptor();
	for(risse_size y = 0; y < LastLineH; y++)
	{
		tPixel::Convert(
			static_cast<risse_uint8*>(image_desc.Buffer) + image_desc.Pitch * (y + LastLineY),
			DesiredPixelFormat,
			static_cast<risse_uint8*>(LastConvertBuffer) + LastCnvertBufferPitch * y,
			DecoderPixelFormat,
			image_desc.Width);
	}
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
tImageEncoder::tImageEncoder()
{
	Encoded = false;
	ImageBuffer = NULL;
	LastConvertBuffer = NULL;
	LastConvertBufferSize = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageEncoder::Encode(tStreamInstance * stream, tImageInstance * image,
				tProgressCallback * callback,
				tDictionaryInstance * dict)
{
	// TODO: image のロック
	// TODO: this のロック
	if(Encoded) { RISSE_ASSERT(!Encoded); /* TODO: 例外 */ }
	Encoded = true;
	ImageBuffer = image->GetBuffer();
	try
	{
		// デコーダの本体処理を呼び出す
		if(callback) callback->CallOnProgress(1, 0); // 0%
		Process(stream, callback, dict);
		if(callback) callback->CallOnProgress(1, 1); // 100%
	}
	catch(...)
	{
		ImageBuffer->Release(), ImageBuffer = NULL;
		throw;
	}
	ImageBuffer->Release(), ImageBuffer = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageEncoder::GetDimensions(risse_size *w, risse_size *h, tPixel::tFormat *pixel_format)
{
	const tImageBuffer::tDescriptor & image_desc = ImageBuffer->GetDescriptor();
	if(w) *w = image_desc.Width;
	if(h) *h = image_desc.Height;
	if(pixel_format) *pixel_format = image_desc.PixelFormat;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void * tImageEncoder::GetLines(void * buf, risse_size y, risse_size h,
	risse_offset & pitch, tPixel::tFormat pixel_format)
{
	const tImageBuffer::tDescriptor & image_desc = ImageBuffer->GetDescriptor();

	if(buf || image_desc.PixelFormat != pixel_format)
	{
		// バッファが与えられている、あるいは変換が必要な場合
		if(!buf)
		{
			// バッファが与えられていない場合
			// ピッチを計算する
			const tPixel::tDescriptor & pixel_desc =
				tPixel::GetDescriptorFromFormat(pixel_format);
			risse_size width_bytes = pixel_desc.Size * image_desc.Width;
			pitch = (width_bytes + 15) & ~ 15;
			risse_size buffer_size = pitch * h;
			// バッファを割り当てる
			if(LastConvertBufferSize < buffer_size)
			{
				// 足りないので割り当て直す
				LastConvertBuffer = buf = AlignedMallocAtomicCollectee(buffer_size, 4);
				LastConvertBufferSize = buffer_size;
			}
		}
		// ピクセル形式の変換を行う
		for(risse_size yy = 0; yy < h; yy++)
		{
			tPixel::Convert(
				static_cast<risse_uint8*>(buf) + pitch * yy,
				pixel_format,
				static_cast<risse_uint8*>(image_desc.Buffer) + image_desc.Pitch * (yy + y),
				image_desc.PixelFormat,
				image_desc.Width);
		}
		return buf;
	}

	// バッファが与えられず、かつ、変換が必要ない場合
	// そのまま画像バッファを返す
	pitch = image_desc.Pitch;
	return static_cast<risse_uint8*>(image_desc.Buffer) + image_desc.Pitch * y;
}
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
tImageCodecFactoryManager::tImageCodecFactoryManager()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tImageCodecFactoryManager::~tImageCodecFactoryManager()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageCodecFactoryManager::Register(const tString & extension, tImageCodecFactory * factory)
{
	Map.insert(tMap::value_type(extension, factory));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageCodecFactoryManager::Unregister(const tString & extension)
{
	Map.erase(extension);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tImageDecoder * tImageCodecFactoryManager::CreateDecoder(const tString & filename)
{
	// 拡張子を取り出す
	tString ext = tFileSystemManager::ExtractExtension(filename);
	ext.ToLowerCaseNC();

	// ファクトリを探す
	tMap::iterator factory = Map.find(ext);
	if(factory != Map.end())
	{
		// ファクトリが見つかった
		tImageDecoder * decoder;
		decoder = factory->second->CreateDecoder();
		return decoder;
	}
	else
	{
		// ファクトリは見つからなかった
		// TODO: これちゃんとした例外クラスにすること
		tIOExceptionClass::Throw(tString(
			RISSE_WS_TR("'%1' has non-supported file extension"), filename));
		return NULL; // これは実行されない
	}

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tImageEncoder * tImageCodecFactoryManager::CreateEncoder(const tString & filename)
{
	// 拡張子を取り出す
	tString ext = tFileSystemManager::ExtractExtension(filename);
	ext.ToLowerCaseNC();

	// ファクトリを探す
	tMap::iterator factory = Map.find(ext);
	if(factory != Map.end())
	{
		// ファクトリが見つかった
		tImageEncoder * encoder;
		encoder = factory->second->CreateEncoder();
		return encoder;
	}
	else
	{
		// ファクトリは見つからなかった
		// TODO: これちゃんとした例外クラスにすること
		tIOExceptionClass::Throw(tString(
			RISSE_WS_TR("'%1' has non-supported file extension"), filename));
		return NULL; // これは実行されない
	}

}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
} // namespace Risa


