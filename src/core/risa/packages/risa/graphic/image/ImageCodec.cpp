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
#include "risa/prec.h"
#include "risa/packages/risa/graphic/image/ImageCodec.h"
#include "risa/packages/risa/fs/FSManager.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(24692,13904,5736,19025,425,432,30553,23251);
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
tImageDecoder::tImageDecoder()
{
	Decoded = false;
	Image = NULL;
	ImageBuffer = NULL;
	BufferPointer = NULL;
	Descriptor = NULL;
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

		// TODO: Image-> の C++ メソッド呼び出しを Risse メソッド呼び出しに置き換えるように

		if(Image->GetPropertyDirect(tSS<'h','a','s','B','u','f','f','e','r'>()).operator bool() &&
			Image->GetDescriptor().PixelFormat != pixel_format)
		{
			// TODO: PixelFormat が違うとの例外
			return;
		}

		DesiredPixelFormat = pixel_format;
		ImageBuffer = Image->GetBuffer();
		if(ImageBuffer) BufferPointer = & ImageBuffer->GetBufferPointer();
		if(ImageBuffer) Descriptor = & ImageBuffer->GetDescriptor();
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
	if(dict) RISSE_ASSERT(dict->Invoke(tSS<'[',']'>(), tVariant(tSS<'_','d','u','m','m','y'>())).IsVoid());
#endif


		if(callback) callback->CallOnProgress(1, 1); // 100%
	}
	catch(...)
	{
		if(ImageBuffer) ImageBuffer->Release(), ImageBuffer = NULL;
		if(BufferPointer) BufferPointer->Release(), BufferPointer = NULL;
		throw;
	}
	if(ImageBuffer) ImageBuffer->Release(), ImageBuffer = NULL;
	if(BufferPointer) BufferPointer->Release(), BufferPointer = NULL;

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
		if(Descriptor->Width != w || Descriptor->Height != h)
		{
			// TODO: サイズが違うとの例外
			RISSE_ASSERT(Descriptor->Width == w && Descriptor->Height == h);
		}

		// イメージバッファを独立させる
		Image->Invoke(tSS<'i','n','d','e','p','e','n','t'>(), tVariant(false));
	}
	else
	{
		// メモリ上のバッファを image に割り当てる
		Image->Invoke(tSS<'a','l','l','o','c','a','t','e'>(), 
			tVariant(static_cast<risse_int64>(w)),
			tVariant(static_cast<risse_int64>(h)),
			tVariant(static_cast<risse_int64>(DesiredPixelFormat)));

		// イメージバッファとそのバッファポインタ、記述子を取得する
		ImageBuffer = Image->GetBuffer();
		BufferPointer = & ImageBuffer->GetBufferPointer();
		Descriptor = & ImageBuffer->GetDescriptor();
	}

	// デコーダが望むピクセル形式を保存
	DecoderPixelFormat = pixel_format;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void * tImageDecoder::StartLines(risse_size y, risse_size h, risse_offset * pitch)
{
	LastLineY = y;
	LastLineH = h;

	// デコーダが望むピクセル形式の場合はそのままイメージバッファのメモリ領域を返す
	if(DecoderPixelFormat == DesiredPixelFormat)
	{
		// デコーダが望むピクセル形式とイメージバッファのピクセル形式が同じ
		if(pitch) *pitch = BufferPointer->Pitch;
		return static_cast<risse_uint8*>(BufferPointer->Buffer) + BufferPointer->Pitch * y;
	}

	// そうでない場合は変換用バッファを作成してそれを返す
	// 変換用バッファのサイズは………
	const tPixel::tDescriptor & pixel_desc = tPixel::GetDescriptorFromFormat(Descriptor->PixelFormat);

	// んー
	// 横幅に必要なバイト数を決定(16bytes に整列)
	risse_size width_bytes = pixel_desc.Size * Descriptor->Width;
	LastConvertBufferPitch = (width_bytes + 15) & ~ 15;
	if(pitch) *pitch = LastConvertBufferPitch;
	// バッファサイズは？
	risse_size buffer_size = LastConvertBufferPitch * Descriptor->Height;
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
	for(risse_size y = 0; y < LastLineH; y++)
	{
		tPixel::Convert(
			static_cast<risse_uint8*>(BufferPointer->Buffer) + BufferPointer->Pitch * (y + LastLineY),
			DesiredPixelFormat,
			static_cast<risse_uint8*>(LastConvertBuffer) + LastConvertBufferPitch * y,
			DecoderPixelFormat,
			Descriptor->Width);
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
	if(Encoded) { RISSE_ASSERT(!Encoded); /* TODO: 例外 */ }
	Encoded = true;

	{
		// Image をロック
		volatile tObjectInterface::tSynchronizer sync(image); // sync

		ImageBuffer = image->GetBuffer();
		if(!ImageBuffer) { RISSE_ASSERT(ImageBuffer); /* TODO: 例外 */ }
		BufferPointer = & ImageBuffer->GetBufferPointer();
		Descriptor = & ImageBuffer->GetDescriptor();
	}

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
		BufferPointer->Release(), BufferPointer = NULL;
		throw;
	}
	ImageBuffer->Release(), ImageBuffer = NULL;
	BufferPointer->Release(), BufferPointer = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageEncoder::GetDimensions(risse_size *w, risse_size *h, tPixel::tFormat *pixel_format)
{
	if(w) *w = Descriptor->Width;
	if(h) *h = Descriptor->Height;
	if(pixel_format) *pixel_format = Descriptor->PixelFormat;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void * tImageEncoder::GetLines(void * buf, risse_size y, risse_size h,
	risse_offset * pitch, tPixel::tFormat pixel_format)
{
	risse_offset buffer_pitch = 0;
	if(buf || Descriptor->PixelFormat != pixel_format)
	{
		// バッファが与えられている、あるいは変換が必要な場合
		if(!buf)
		{
			// バッファが与えられていない場合
			// ピッチを計算する
			const tPixel::tDescriptor & pixel_desc =
				tPixel::GetDescriptorFromFormat(pixel_format);
			risse_size width_bytes = pixel_desc.Size * Descriptor->Width;
			buffer_pitch = (width_bytes + 15) & ~ 15;
			if(pitch) *pitch = buffer_pitch;
			risse_size buffer_size = buffer_pitch * h;
			// バッファを割り当てる
			if(LastConvertBufferSize < buffer_size)
			{
				// 足りないので割り当て直す
				LastConvertBuffer = AlignedMallocAtomicCollectee(buffer_size, 4);
				LastConvertBufferSize = buffer_size;
			}
			buf = LastConvertBuffer;
		}
		else
		{
			if(h > 1)
			{
				// 高さが2以上の場合はピッチの計算が必要になる
				RISSE_ASSERT(pitch);
				buffer_pitch = *pitch;
			}
		}
		// ピクセル形式の変換を行う
		for(risse_size yy = 0; yy < h; yy++)
		{
			tPixel::Convert(
				static_cast<risse_uint8*>(buf) + buffer_pitch * yy,
				pixel_format,
				static_cast<risse_uint8*>(BufferPointer->Buffer) + BufferPointer->Pitch * (yy + y),
				Descriptor->PixelFormat,
				Descriptor->Width);
		}
		return buf;
	}

	// バッファが与えられず、かつ、変換が必要ない場合
	// そのまま画像バッファを返す
	buffer_pitch = BufferPointer->Pitch;
	if(pitch) *pitch = buffer_pitch;
	return static_cast<risse_uint8*>(BufferPointer->Buffer) + BufferPointer->Pitch * y;
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
void tImageCodecFactoryManager::Register(const tString & extension, tImageDecoderFactory * factory)
{
	DecoderMap.insert(tDecoderMap::value_type(extension, factory));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageCodecFactoryManager::Register(const tString & extension, tImageEncoderFactory * factory)
{
	EncoderMap.insert(tEncoderMap::value_type(extension, factory));
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
tImageDecoder * tImageCodecFactoryManager::CreateDecoder(const tString & filename)
{
	// 拡張子を取り出す
	tString ext = tFileSystemManager::ExtractExtension(filename);
	ext.ToLowerCaseNC();

	// ファクトリを探す
	tDecoderMap::iterator factory = DecoderMap.find(ext);
	if(factory != DecoderMap.end())
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
	tEncoderMap::iterator factory = EncoderMap.find(ext);
	if(factory != EncoderMap.end())
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


