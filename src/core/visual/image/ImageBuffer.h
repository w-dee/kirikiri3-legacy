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
#ifndef ImageBufferH
#define ImageBufferH

#include <risa_gl/pixel_store.hpp>
#include <risa_gl/pixel.hpp>
#include "visual/image/PixelType.h"
#include "base/utils/RisaThread.h"

namespace Risa {
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		デフォルトの pixel_store 用 aligned_allocator の allocate_implement_type
//! @note		WIN32 の GDI は、メモリ上のビットマップから Blt するとき、メモリが
//!				GlobalAlloc(GMEM_FIXED) で確保されていないと表示がうまくできないという制限
//!				がある(仕様)。ImageBuffer は用途によってはそのまま GDI 経由で表示
//!				するかもしれないので、Windows では GlobalAlloc でメモリを確保する方法を使う。
// TODO: Win64でのチェック
//---------------------------------------------------------------------------
#ifdef _WIN32
	typedef risa_gl::default_allocate_implements pixel_store_allocate_implements;
#else
	typedef risa_gl::global_alloc_allocate_implements pixel_store_allocate_implements;
#endif
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		pixel_store のラッパ (tDestructee派生クラスにするため)
//---------------------------------------------------------------------------
template <typename pixel_t, typename allocator_t =
			  risa_gl::aligned_allocator<pixel_t, 16> >
class gc_pixel_store : public risa_gl::pixel_store<pixel_t, allocator_t>, public tDestructee
{
	typedef risa_gl::pixel_store<pixel_t, allocator_t> inherited;
public:
	gc_pixel_store(int w, int h) : inherited(w, h) {;}
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		イメージバッファのインターフェース
//---------------------------------------------------------------------------
class tImageBuffer : public tCollectee
{
	tAtomicCounter RefCount; //!< リファレンスカウンタ
	unsigned long BufferLockCount; //!< バッファのロックカウンタ
	tCriticalSection * CS; //!< このイメージバッファを保護するためのクリティカルセクション


public:
	//! @brief		イメージバッファ記述子
	struct tDescriptor
	{
		tPixel::tFormat PixelFormat; //!< ピクセル形式
		risse_size Width; //!< 横幅
		risse_size Height; //!< 縦幅
	};


	//! @brief		バッファポインタ構造体
	struct tBufferPointer
	{
		tImageBuffer * ImageBuffer; //!< イメージバッファへの参照
		void * Buffer; //!< バッファへの先頭
		risse_offset Pitch; //!< ピッチ(バイト単位)

		//! @brief		この情報をリリースする。
		//!				使い終わったら必ず呼ぶこと。
		void Release() const { ImageBuffer->DecBufferLockCount();ImageBuffer->Release(); }
	};

protected:
	tDescriptor Descriptor; //!< イメージバッファ記述子
	tBufferPointer BufferPointer;

public:
	//! @brief		コンストラクタ
	tImageBuffer() : RefCount(1L), BufferLockCount(0L)
		{CS=new tCriticalSection(); BufferPointer.ImageBuffer = this;}

	//! @brief		デストラクタ (他の多くのtCollectee派生クラスと違い、呼ばれることがある)
	virtual ~tImageBuffer()
	{
		// バッファはすべて開放されていなければならない
		RISSE_ASSERT( (long) BufferLockCount == 0 );
	}

protected:
	//! @brief		バッファのロックカウントを増やす
	void IncBufferLockCount();

	//! @brief		バッファのロックカウントを減らす
	void DecBufferLockCount();

public:
	//! @brief		参照カウンタをインクリメントする
	//! @return		インクリメントしたあとの参照カウント数
	long AddRef()
	{
		return ++ RefCount;
	}

	//! @brief		参照カウンタをデクリメントする
	//! @note		参照カウンタがゼロになれば delete this するようになっている
	void Release()
	{
		if(-- RefCount == 0)
		{
			// 参照カウンタがゼロになったので delete this する
			delete this;
		}
	}

	//! @brief		内容を独立させる
	//! @param		copy_image		独立させる際に内容をコピーするか(偽にするとコピーされたイメージバッファの内容は不定)
	//! @return		独立が行われた場合、独立されたイメージバッファ。そうでなければ this
	//! @note		返されるイメージバッファの参照カウンタはAddRef()されていると考えるべき
	tImageBuffer * Independ(bool copy_image)
	{
		// いったんリファレンスカウンタを増やす。
		if(AddRef() >= 3)
		{
			// 参照を増やした結果 3 以上になったと言うことは
			// 増やす前は 2 以上(つまり独立してない) だったということ
			Release();
			return Clone(copy_image);
		}
		return this;
	}

	//! @brief		イメージバッファ記述子を得る
	//! @return		イメージバッファ記述子
	//! @note		イメージバッファ記述子は、いったんイメージバッファが構築されると以降内容が変わることはない。
	const tDescriptor & GetDescriptor()
	{
		IncBufferLockCount();
		AddRef();
		return Descriptor;
	}

	//! @brief		バッファポインタ構造体を得る
	//! @return		バッファポインタ構造体(使い終わったらtBufferPointer::Release() を呼ぶこと)
	const tBufferPointer & GetBufferPointer()
	{
		IncBufferLockCount();
		AddRef();
		return BufferPointer;
	}

	//! @brief		指定位置のピクセルを ARGB32 形式の整数で得る
	//! @note		きわめて低速。
	//! @param		x			X位置
	//! @param		y			Y位置
	//! @return		0xAARRGGBB 形式のピクセル値
	risse_uint32 GetARGB32(risse_size x, risse_size y);

	//! @brief		指定位置のピクセルを ARGB32 形式の整数で設定する
	//! @note		きわめて低速。
	//! @param		x			X位置
	//! @param		y			Y位置
	//! @param		v			0xAARRGGBB 形式のピクセル値
	void SetARGB32(risse_size x, risse_size y, risse_uint32 v);

public: // サブクラスで実装する
	//! @brief		内容をクローンする
	//! @param		copy_image		内容をコピーするか(偽にするとコピーされたイメージバッファの内容は不定)
	//! @return		クローンされたイメージバッファ
	virtual tImageBuffer * Clone(bool copy_image = true) = 0;

protected: // サブクラスで実装する

	//! @brief		バッファをロックする
	//! @note		バッファをロックするとともにこのクラスのBufferPointerに必要情報を書き込むこと。
	//!				(別にLockBufferで特にやることが無ければあらかじめコンストラクタなどでBufferPointerに
	//!				書き込んでおいてよい; tMemoryImageBuffer を参照)
	//!				GetBufferPointer() と tBufferPointer::Release() の呼び出し回数は釣り合ってなければならない。
	//!				ネストされた GetBufferPointer() の呼び出しではこのメソッドは呼ばれない
	//!				(すべての BufferPointer が Releaseされている状態で GetBufferPointer() が呼ばれた際に呼ばれる)
	virtual void LockBuffer() = 0;

	//! @brief		バッファのロックを解除する
	virtual void UnlockBuffer() = 0;

};
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
//! @brief		メモリイメージバッファ
//---------------------------------------------------------------------------
template <typename pixel_t, int alignment, int pixel_format>
class tMemoryImageBuffer : public tImageBuffer
{
	typedef tMemoryImageBuffer<pixel_t, alignment, pixel_format> self_type;

	typedef gc_pixel_store<pixel_t,
		risa_gl::aligned_allocator<pixel_t, alignment, pixel_store_allocate_implements> >
			pixe_store_t; //!< pfARGB32 用のピクセルストア

	pixe_store_t * PixelStore; //!< ピクセルストアへのポインタ

public:
	//! @brief		コンストラクタ
	//! @param		w		画像の横幅
	//! @param		h		画像の縦幅
	tMemoryImageBuffer(risse_size w, risse_size h);

public:
	//! @brief		内容をクローンする
	//! @param		copy_image		内容をコピーするか(偽にするとコピーされたイメージバッファの内容は不定)
	//! @return		クローンされたイメージバッファ
	virtual tImageBuffer * Clone(bool copy_image = true);

	//! @brief		バッファをロックする
	virtual void LockBuffer() {;}

	//! @brief		バッファのロックを解除する
	virtual void UnlockBuffer() {;}

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ARGB32メモリイメージバッファ
//---------------------------------------------------------------------------
typedef tMemoryImageBuffer<risa_gl::pixel, 16, tPixel::pfARGB32> tARGB32MemoryImageBuffer;

//---------------------------------------------------------------------------
//! @brief		Gray8メモリイメージバッファ
//---------------------------------------------------------------------------
typedef tMemoryImageBuffer<risse_uint8, 4, tPixel::pfGray8> tGray8MemoryImageBuffer;
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace Risa


#endif
