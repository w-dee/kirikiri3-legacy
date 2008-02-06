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

#include <risagl/pixel_store.hpp>
#include <risagl/pixel.hpp>


namespace Risa {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		pixel_store のラッパ (tDestructee派生クラスにするため)
//---------------------------------------------------------------------------
template <typename pixel_type, int alignment>
class gc_pixel_store : public risa_gl::pixel_store<pixel_type, alignment>, public tDestructee
{
	typedef risa_gl::pixel_store<pixel_type, alignment> inherited;
public:
	gc_pixel_store(int w, int h) : inherited(w, h) {;}
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		イメージバッファ
//---------------------------------------------------------------------------
class tImageBuffer : public tCollectee
{
	typedef gc_pixel_store<risa_gl::pixel, 16> tPixelStore;

	tPixelStore * PixelStore;

public:
	//! @brief		コンストラクタ
	//! @param		type
	tImageBuffer();

	//! @brief		デストラクタ(おそらく呼ばれない)
	virtual ~tImageBuffer() {;}

public:
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace Risa


#endif
