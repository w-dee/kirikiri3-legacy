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
#include "base/script/RisseEngine.h"
#include "risse/include/risseDictionaryClass.h"

namespace Risa {
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		イメージインスタンス
//---------------------------------------------------------------------------
class tImageInstance : public tObjectBase
{
public:
	tGCReferencePtr<tImageBuffer> * ImageBuffer; //!< イメージバッファインスタンス

public:
	tImageInstance(); //!< コンストラクタ

public:
	//! @brief		明示的にイメージを破棄する
	//! @note		破棄というか参照カウンタを減じて参照を消すだけ。
	//!				他でイメージバッファを参照している場合は破棄されないと思う
	void Dispose();

	//! @brief		メモリ上にイメージバッファを新規作成する
	//! @param		format		ピクセル形式
	//! @param		w			横幅
	//! @param		h			縦幅
	void Allocate(tPixel::tFormat format, risse_size w, risse_size h);

	//! @brief		イメージバッファを持っているかどうかを返す
	//! @return		イメージバッファを持っているかどうか
	bool HasBuffer() const { /* TODO: RESSE_ASSERT_CS_LOCKED */ return * ImageBuffer != NULL; }

	//! @brief		イメージバッファを返す
	//! @note		注意！返されるイメージバッファの参照カウンタはここでインクリメントされる。
	tImageBuffer * GetBuffer() const { return ImageBuffer->get(); }

	//! @brief		イメージバッファのデスクリプタを返す(イメージバッファを持っている場合のみに呼ぶこと)
	//! @return		イメージバッファのデスクリプタ
	const tImageBuffer::tDescriptor & GetDescriptor() const {
		return (*ImageBuffer)->GetDescriptor(); }

	//! @brief		イメージバッファのバッファポインタ構造体を返す(イメージバッファを持っている場合のみに呼ぶこと)
	//! @return		イメージバッファのバッファポインタ構造体(使い終わったらRelease()を呼ぶこと)
	const tImageBuffer::tBufferPointer & GetBufferPointer() const {
		return (*ImageBuffer)->GetBufferPointer(); }

	//! @brief		書き込み用のイメージバッファのバッファポインタ構造体を返す(イメージバッファを持っている場合のみに呼ぶこと)
	//! @return		イメージバッファのバッファポインタ構造体(使い終わったらRelease()を呼ぶこと)
	const tImageBuffer::tBufferPointer & GetBufferPointerForWrite();

	//! @brief		イメージバッファを独立する
	//! @param		clone		独立する際、内容をコピーするかどうか
	//!							(偽を指定すると内容は不定になる)
	//! @note		イメージバッファが他と共有されている場合は内容をクローンして独立させる
	void Independ(bool clone = true);

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
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		"Image" クラス
//---------------------------------------------------------------------------
class tImageClass : public tClassBase
{
	typedef tClassBase inherited; //!< 親クラスの typedef

public:
	//! @brief		コンストラクタ
	//! @param		engine		スクリプトエンジンインスタンス
	tImageClass(tScriptEngine * engine);

	//! @brief		各メンバをインスタンスに追加する
	void RegisterMembers();

	//! @brief		newの際の新しいオブジェクトを作成して返す
	static tVariant ovulate();

public: // Risse 用メソッドなど
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
} // namespace Risa


#endif
