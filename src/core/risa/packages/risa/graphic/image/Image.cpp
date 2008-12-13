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
#include "risa/prec.h"
#include "risa/packages/risa/graphic/image/Image.h"
#include "risa/packages/risa/graphic/image/ImageCodec.h"
#include "risse/include/risseNativeBinder.h"
#include "risse/include/risseScriptEngine.h"
#include "risse/include/risseStaticStrings.h"
#include "risa/packages/risa/fs/FSManager.h"

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
const tImageBuffer::tBufferPointer & tImageInstance::GetBufferPointerForWrite()
{
	volatile tSynchronizer sync(this); // sync

	Independ();
	return (*ImageBuffer)->GetBufferPointer();
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
risse_uint32 tImageInstance::GetARGB32(risse_size x, risse_size y)
{
	// TODO: 範囲チェック
	volatile tSynchronizer sync(this); // sync

	RISSE_ASSERT(*ImageBuffer);

	return (*ImageBuffer)->GetARGB32(x, y);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageInstance::SetARGB32(risse_size x, risse_size y, risse_uint32 v)
{
	// TODO: 範囲チェック
	volatile tSynchronizer sync(this); // sync

	RISSE_ASSERT(*ImageBuffer);

	Independ(true);
	(*ImageBuffer)->SetARGB32(x, y, v);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageInstance::Load(const tString & filename,
		tPixel::tFormat format,
		tDictionaryInstance * dict,
		const tVariant & cbfunc)
{
	// ファイル名(の拡張子)に対応したデコーダを得る
	tImageDecoder * decoder = tImageCodecFactoryManager::instance()->CreateDecoder(filename);

	// ストリームを作成
	tStreamInstance * stream =
		tFileSystemManager::instance()->Open(filename, tFileOpenModes::omRead);

	try
	{
		// コールバックは？
		if(!cbfunc.IsVoid())
		{
			tRisseProgressCallback cb(GetRTTI()->GetScriptEngine(), cbfunc);
			decoder->Decode(stream, this, format, &cb, dict);
		}
		else
		{
			// コールバック無し
			decoder->Decode(stream, this, format, NULL, dict);
		}
	}
	catch(...)
	{
		tStreamAdapter(stream).Dispose();
		throw;
	}

	tStreamAdapter(stream).Dispose();
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
bool tImageInstance::get_hasBuffer()
{
	volatile tSynchronizer sync(this); // sync

	return HasBuffer();
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
void tImageInstance::load(const tString & filename, const tMethodArgument & args)
{
	// 辞書配列インスタンスを第２引数から取得
	tPixel::tFormat format = tPixel::pfARGB32; // デフォルトフォーマット
	tDictionaryInstance * dict = NULL;
	if(args.HasArgument(1))
	{
		dict = args[1].
			ExpectAndGetObjectInterface(
					GetRTTI()->GetScriptEngine()->DictionaryClass);
		// ピクセルフォーマット ('_pixel_format' を得る)
		tVariant val =
			dict->Invoke(
				tSS<'[',']'>(),
				tVariant(tSS<'_','p','i','x','e','l','_','f','o','r','m','a','t'>()));
		if(!val.IsVoid())
			format = (tPixel::tFormat)(risse_int64)val;
	}

	// コールバック
	tVariant cb;
	if(args.HasBlockArgument(0)) cb = args.GetBlockArgument(0);

	// ロードを行う
	Load(filename, format, dict, cb);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tImageInstance::save(const tString & filename, const tMethodArgument & args)
{
	// ファイル名(の拡張子)に対応したエンコーダを得る
	tImageEncoder * encoder = tImageCodecFactoryManager::instance()->CreateEncoder(filename);

	// 辞書配列インスタンスを第２引数から取得
	tDictionaryInstance * dict = NULL;
	if(args.HasArgument(1))
	{
		dict = args[1].
			ExpectAndGetObjectInterface(
					GetRTTI()->GetScriptEngine()->DictionaryClass);
	}

	// ストリームを作成
	tStreamInstance * stream =
		tFileSystemManager::instance()->Open(filename, tFileOpenModes::omWrite);

	try
	{
		// コールバックは？
		if(args.HasBlockArgument(0))
		{
			tRisseProgressCallback cb(GetRTTI()->GetScriptEngine(), args.GetBlockArgument(0));
			encoder->Encode(stream, this, &cb, dict);
		}
		else
		{
			// コールバック無し
			encoder->Encode(stream, this, NULL, dict);
		}
	}
	catch(...)
	{
		tStreamAdapter(stream).Dispose();
		throw;
	}

	tStreamAdapter(stream).Dispose();

}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tImageClass, (tSS<'I','m','a','g','e'>()),
		engine->ObjectClass)
	BindFunction(this, ss_ovulate, &tImageClass::ovulate);
	BindFunction(this, ss_construct, &tImageInstance::construct);
	BindFunction(this, ss_initialize, &tImageInstance::initialize);
	BindFunction(this, ss_dispose, &tImageInstance::dispose);
	BindProperty(this, tSS<'h','a','s','B','u','f','f','e','r'>(), &tImageInstance::get_hasBuffer);
	BindFunction(this, tSS<'a','l','l','o','c','a','t','e'>(), &tImageInstance::allocate);
	BindFunction(this, tSS<'d','e','a','l','l','o','c','a','t','e'>(), &tImageInstance::deallocate);
	BindFunction(this, tSS<'i','n','d','e','p','e','n','d'>(), &tImageInstance::independ);
	BindFunction(this, tSS<'l','o','a','d'>(), &tImageInstance::load);
	BindFunction(this, tSS<'s','a','v','e'>(), &tImageInstance::save);
	BindFunction(this, tSS<'g','e','t','A','R','G','B','3','2'>(), &tImageInstance::getARGB32);
	BindFunction(this, tSS<'s','e','t','A','R','G','B','3','2'>(), &tImageInstance::setARGB32);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * Image クラスレジストラ
 */
template class tClassRegisterer<
	tSS<'r','i','s','a','.','g','r','a','p','h','i','c','.','i','m','a','g','e'>,
	tImageClass>;
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
/**
 * risa.log のパッケージのメンバを初期化するためのシングルトンインスタンス
 */
class tRisaGraphicImagePackageMemberInitializer : public tPackageMemberInitializer,
	public singleton_base<tRisaGraphicImagePackageMemberInitializer>
{
public:
	/**
	 * コンストラクタ
	 */
	tRisaGraphicImagePackageMemberInitializer()
	{
		tPackageRegisterer<tSS<'r','i','s','a','.','g','r','a','p','h','i','c','.','i','m','a','g','e'> >::instance()->
			AddInitializer(this);
	}

	/**
	 * パッケージを初期化する
	 * @param engine	スクリプトエンジンインスタンス
	 * @param name		パッケージ名
	 * @param global	パッケージグローバル
	 */
	void Initialize(tScriptEngine * engine, const tString & name,
		const tVariant & global)
	{
		global.RegisterFinalConstMember(
			tSS<'p','f','G','r','a','y','8'>(),
			tVariant((risse_int64)tPixel::pfGray8));
		global.RegisterFinalConstMember(
			tSS<'p','f','A','R','G','B','3','2'>(),
			tVariant((risse_int64)tPixel::pfARGB32));
	}
};
//---------------------------------------------------------------------------














//---------------------------------------------------------------------------
} // namespace Risa


