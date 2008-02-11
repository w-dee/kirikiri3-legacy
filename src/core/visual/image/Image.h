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
	void New(tPixel::tFormat format, risse_size w, risse_size h);

	//! @brief		イメージバッファを持っているかどうかを返す
	//! @return		イメージバッファを持っているかどうか
	bool HasBuffer() const { /* TODO: RESSE_ASSERT_CS_LOCKED */ return * ImageBuffer != NULL; }

	//! @brief		イメージバッファのデスクリプタを返す(イメージバッファを持っている場合のみに呼ぶこと)
	//! @return		イメージバッファのデスクリプタ
	const tImageBuffer::tDescriptor & GetDescriptor() const {
		return (*ImageBuffer)->GetDescriptor(); }

	//! @brief		書き込み用のイメージバッファのデスクリプタを返す(イメージバッファを持っている場合のみに呼ぶこと)
	//! @return		イメージバッファのデスクリプタ
	const tImageBuffer::tDescriptor & GetDescriptorForWrite() {
		Independ();
		return (*ImageBuffer)->GetDescriptor(); }

	//! @brief		イメージバッファを独立する
	//! @param		clone		独立する際、内容をコピーするかどうか
	//!							(偽を指定すると内容は不定になる)
	//! @note		イメージバッファが他と共有されている場合は内容をクローンして独立させる
	void Independ(bool clone = true);

};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risa


#endif
