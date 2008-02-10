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
#ifndef ImageH
#define ImageH

#include "base/gc/RisaGC.h"
#include "visual/image/ImageBuffer.h"

namespace Risa {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		イメージクラス
//---------------------------------------------------------------------------
class tImage : public tCollectee
{
public:
	tGCReferencePtr<tImageBuffer> * ImageBuffer; //!< イメージバッファインスタンス

public:
	tImage(); //!< コンストラクタ

public:
	//! @brief		明示的にイメージを破棄する
	//! @note		破棄というか参照カウンタを減じて参照を消すだけ。
	//!				他でイメージバッファを参照している場合は破棄されないと思う
	void Dispose();

	//! @brief		メモリ上にイメージバッファを新規作成する
	//! @param		format		ピクセル形式
	//! @param		w			横幅
	//! @param		h			縦幅
	void New(tImageBuffer::tPixelFormat format, risse_size w, risse_size h);

};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risa


#endif
