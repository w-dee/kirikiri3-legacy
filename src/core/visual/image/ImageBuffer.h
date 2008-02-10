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
	//! @brief		ピクセル形式
	enum tPixelFormat
	{
		pfGray8, //!< 8bpp グレースケール
		pfARGB32, //!< 32bpp Alpha, R, G, B (もっとも一般的)
	};

	//! @brief		イメージバッファ記述子
	struct tDescriptor
	{
		tPixelFormat PixelFormat; //!< ピクセル形式
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

public:
	//! @brief		記述子を得る
	//! @return		記述子
	virtual const tDescriptor & GetDescriptor() = 0;
};
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
//! @brief		ARGB32メモリイメージバッファ
//---------------------------------------------------------------------------
class tARGB32MemoryImageBuffer : public tImageBuffer
{
	typedef risa_gl::pixel pixel_t;
	typedef gc_pixel_store<pixel_t,
		risa_gl::aligned_allocator<pixel_t, 16, pixel_store_allocate_implements> >
			pixe_store_t; //!< pfARGB32 用のピクセルストア

	pixe_store_t * PixelStore; //!< ピクセルストアへのポインタ
	tDescriptor Descriptor; //!< 記述子

public:
	//! @brief		コンストラクタ
	//! @param		w		画像の横幅
	//! @param		h		画像の縦幅
	tARGB32MemoryImageBuffer(risse_size w, risse_size h);

public:
	//! @brief		記述子を得る
	//! @return		記述子
	virtual const tDescriptor & GetDescriptor() { return Descriptor; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Gray8メモリイメージバッファ
//---------------------------------------------------------------------------
class tGray8MemoryImageBuffer : public tImageBuffer
{
	typedef risse_uint8 pixel_t;
	typedef gc_pixel_store<pixel_t,
		risa_gl::aligned_allocator<pixel_t, 4, pixel_store_allocate_implements> >
			pixe_store_t; //!< pfARGB32 用のピクセルストア

	pixe_store_t * PixelStore; //!< ピクセルストアへのポインタ
	tDescriptor Descriptor; //!< 記述子

public:
	//! @brief		コンストラクタ
	//! @param		w		画像の横幅
	//! @param		h		画像の縦幅
	tGray8MemoryImageBuffer(risse_size w, risse_size h);

public:
	//! @brief		記述子を得る
	//! @return		記述子
	virtual const tDescriptor & GetDescriptor() { return Descriptor; }
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace Risa


#endif
