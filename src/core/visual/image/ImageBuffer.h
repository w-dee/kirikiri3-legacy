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

public:
	//! @brief		イメージバッファ記述子
	struct tDescriptor
	{
		tPixel::tFormat PixelFormat; //!< ピクセル形式
		void * Buffer; //!< バッファへの先頭
		risse_offset Pitch; //!< ピッチ(バイト単位)
		risse_size Width; //!< 横幅
		risse_size Height; //!< 縦幅
	};

public:
	//! @brief		コンストラクタ
	tImageBuffer() : RefCount(1L) {;}

	//! @brief		デストラクタ (他の多くのtCollectee派生クラスと違い、呼ばれることがある)
	virtual ~tImageBuffer() {;}

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
		if(-- RefCount)
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

public:
	//! @brief		記述子を得る
	//! @return		記述子
	virtual const tDescriptor & GetDescriptor() = 0;

	//! @brief		内容をクローンする
	//! @param		copy_image		内容をコピーするか(偽にするとコピーされたイメージバッファの内容は不定)
	//! @return		クローンされたイメージバッファ
	virtual tImageBuffer * Clone(bool copy_image = true) = 0;
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
	tDescriptor Descriptor; //!< 記述子

public:
	//! @brief		コンストラクタ
	//! @param		w		画像の横幅
	//! @param		h		画像の縦幅
	tMemoryImageBuffer(risse_size w, risse_size h);

public:
	//! @brief		記述子を得る
	//! @return		記述子
	virtual const tDescriptor & GetDescriptor() { return Descriptor; }

	//! @brief		内容をクローンする
	//! @param		copy_image		内容をコピーするか(偽にするとコピーされたイメージバッファの内容は不定)
	//! @return		クローンされたイメージバッファ
	virtual tImageBuffer * Clone(bool copy_image = true);
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
