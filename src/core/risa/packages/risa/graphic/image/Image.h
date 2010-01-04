//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief イメージクラス
//---------------------------------------------------------------------------
#ifndef ImageH
#define ImageH

#include "risa/common/RisaGC.h"
#include "risa/packages/risa/graphic/image/ImageBuffer.h"
#include "risa/common/RisseEngine.h"
#include "risseDictionaryClass.h"

namespace Risa {
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * イメージインスタンス
 */
class tImageInstance : public tObjectBase
{
public:
	tGCReferencePtr<tImageBuffer> * ImageBuffer; //!< イメージバッファインスタンス

public:
	tImageInstance(); //!< コンストラクタ

public:
	/**
	 * 明示的にイメージを破棄する
	 * @note	破棄というか参照カウンタを減じて参照を消すだけ。
	 *			他でイメージバッファを参照している場合は破棄されないと思う
	 */
	void Dispose();

	/**
	 * メモリ上にイメージバッファを新規作成する
	 * @param format	ピクセル形式
	 * @param w			横幅
	 * @param h			縦幅
	 */
	void Allocate(tPixel::tFormat format, risse_size w, risse_size h);

	/**
	 * イメージバッファを持っているかどうかを返す
	 * @return	イメージバッファを持っているかどうか
	 */
	bool HasBuffer() const { /* TODO: RESSE_ASSERT_CS_LOCKED */ return * ImageBuffer != NULL; }

	/**
	 * イメージバッファを返す
	 * @note	注意！返されるイメージバッファの参照カウンタはここでインクリメントされる。
	 */
	tImageBuffer * GetBuffer() const { return ImageBuffer->get(); }

	/**
	 * イメージバッファのデスクリプタを返す(イメージバッファを持っている場合のみに呼ぶこと)
	 * @return	イメージバッファのデスクリプタ
	 */
	const tImageBuffer::tDescriptor & GetDescriptor() const {
		return (*ImageBuffer)->GetDescriptor(); }

	/**
	 * イメージバッファのバッファポインタ構造体を返す(イメージバッファを持っている場合のみに呼ぶこと)
	 * @return	イメージバッファのバッファポインタ構造体(使い終わったらRelease()を呼ぶこと)
	 */
	const tImageBuffer::tBufferPointer & GetBufferPointer() const {
		return (*ImageBuffer)->GetBufferPointer(); }

	/**
	 * 書き込み用のイメージバッファのバッファポインタ構造体を返す(イメージバッファを持っている場合のみに呼ぶこと)
	 * @return	イメージバッファのバッファポインタ構造体(使い終わったらRelease()を呼ぶこと)
	 */
	const tImageBuffer::tBufferPointer & GetBufferPointerForWrite();

	/**
	 * イメージバッファを独立する
	 * @param clone	独立する際、内容をコピーするかどうか
	 *				(偽を指定すると内容は不定になる)
	 * @note	イメージバッファが他と共有されている場合は内容をクローンして独立させる
	 */
	void Independ(bool clone = true);

	/**
	 * 指定位置のピクセルを ARGB32 形式の整数で得る
	 * @note	きわめて低速。
	 * @param x	X位置
	 * @param y	Y位置
	 * @return	0xAARRGGBB 形式のピクセル値
	 */
	risse_uint32 GetARGB32(risse_size x, risse_size y);

	/**
	 * 指定位置のピクセルを ARGB32 形式の整数で設定する
	 * @note	きわめて低速。
	 * @param x	X位置
	 * @param y	Y位置
	 * @param v	0xAARRGGBB 形式のピクセル値
	 */
	void SetARGB32(risse_size x, risse_size y, risse_uint32 v);

	/**
	 * 画像をロードする
	 * @param filename	ファイル名
	 * @param format	読み込みたいフォーマット
	 * @param dict		ロードするパラメータの入った辞書配列(NULL=指定なし)
	 * @param cbfunc	コールバック(void = 指定なし)
	 */
	void Load(const tString & filename,
			tPixel::tFormat format = tPixel::pfARGB32,
			tDictionaryInstance * dict = NULL,
			const tVariant & cbfunc = tVariant::GetVoidObject());

public: // Risse用メソッドなど
	void construct();
	void initialize(const tNativeCallInfo &info);

	void dispose() { Dispose(); }
	bool get_hasBuffer();
	void allocate(risse_size w, risse_size h, const tMethodArgument &args);
	void deallocate() { Dispose(); }
	void independ(const tMethodArgument &args);
	void load(const tString & filename, const tMethodArgument & args);
	void save(const tString & filename, const tMethodArgument & args);
	risse_uint32 getARGB32(risse_size x, risse_size y) { return GetARGB32(x, y); }
	void setARGB32(risse_size x, risse_size y, risse_uint32 v) { SetARGB32(x, y, v); }
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
/**
 * "Image" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tImageClass, tClassBase, tImageInstance, itNormal)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


#endif
