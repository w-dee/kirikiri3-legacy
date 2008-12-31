//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オンメモリストリームの実装
//---------------------------------------------------------------------------
#ifndef MemoryStreamH
#define MemoryStreamH

#include "risa/common/RisaThread.h"
#include "risa/packages/risa/fs/FSManager.h"
#include "risa/common/RisaGC.h"

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
 * メモリストリームで用いられるメモリブロック
 * @note	このメモリブロックは、複数のストリームで共有される可能性があり、
 *			複数スレッドから同時アクセスされる可能性がある
 */
class tMemoryStreamBlock : public tDestructee
{
	tCriticalSection CS;	//!< このメモリブロックへのアクセスを保護するクリティカルセクション
	void * Block;			//!< メモリブロック
	risse_size Size;		//!< メモリブロックのデータが入っている部分のサイズ
	risse_size AllocSize;	//!< メモリブロックのアロケートしているサイズ( Size <= AllocSize )

public:
	/**
	 * コンストラクタ
	 */
	tMemoryStreamBlock();
protected:
	/**
	 * デストラクタ
	 */
	~tMemoryStreamBlock();
public:

	/**
	 * メモリブロックのサイズを変更する
	 * @param size	新しいサイズ
	 * @note	サイズが拡張される場合、メモリブロックの内容は保たれるが、
	 *			サイズが拡張された部分の内容は不定となる。縮小される場合、
	 *			内容は最後が切りつめられる。
	 */
	void ChangeSize(risse_size);

	/**
	 * メモリブロックのサイズをSizeぴったりのサイズに変更する
	 */
	void Fit();

	void * GetBlock() { return Block; } //!< ブロックを得る

	tCriticalSection & GetCS( ) { return CS; } //!< クリティカルセクションオブジェクトを得る
	risse_size GetSize() const { return Size; } //!< ブロックのサイズを得る
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
/**
 * メモリストリーム
 */
/*
	this class provides a tBinaryStream based access method for a memory block.
*/
class tMemoryStreamInstance : public tStreamInstance
{
protected:
	tMemoryStreamBlock * Block; //!< メモリブロック
	risse_size CurrentPos;		//!< 現在のポインタ
	risse_uint32 Flags;			//!< アクセスフラグ

public:
	/**
	 * コンストラクタ
	 * @param flags	アクセスフラグ
	 */
	tMemoryStreamInstance();

	/**
	 * デストラクタ(おそらく呼ばれない)
	 */
	~tMemoryStreamInstance() {;}

	/**
	 * メモリブロックを設定する
	 * @param block	メモリブロック
	 */
	void SetMemoryBlock(tMemoryStreamBlock * block) { Block = block; }

	/**
	 * アクセスフラグを設定する
	 * @param flags	アクセスフラグ
	 */
	void SetAccessFlags(risse_uint32 flags) { Flags = flags; }

	/**
	 * ポインタを最後に移動する
	 */
	void SeekEnd();

public: // risse 用メソッドとか
	void construct() {;}
	void initialize(risse_uint32 flags, const tNativeCallInfo &info);

	/**
	 * ストリームを閉じる
	 * @note	基本的にはこれでストリームを閉じること。
	 *			このメソッドでストリームを閉じなかった場合の動作は
	 *			「未定義」である
	 */
	void dispose();

	/**
	 * 指定位置にシークする
	 * @param offset	基準位置からのオフセット (正の数 = ファイルの後ろの方)
	 * @param whence	基準位置
	 * @return	このメソッドは成功すれば真、失敗すれば偽を返す
	 */
	bool seek(risse_int64 offset, tOrigin whence);

	/**
	 * 現在位置を取得する
	 * @return	現在位置(先頭からのオフセット)
	 */
	risse_uint64 tell();

	/**
	 * ストリームから読み込む
	 * @param buf	読み込んだデータを書き込む先
	 * @return	実際に読み込まれたサイズ
	 */
	risse_size get(const tOctet & buf);

	/**
	 * ストリームに書き込む
	 * @param buf	書き込むデータ
	 * @return	実際に書き込まれたサイズ
	 */
	risse_size put(const tOctet & buf);

	/**
	 * ストリームを現在位置で切りつめる
	 */
	void truncate();

	/**
	 * サイズを得る
	 * @return	このストリームのサイズ
	 */
	risse_uint64 get_size();

	/**
	 * ストリームをフラッシュする(書き込みバッファをフラッシュする)
	 */
	void flush();

	// non-tBinaryStream based methods
	void * GetInternalBuffer()  const { return Block->GetBlock(); }
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
/**
 * "MemoryStream" クラス
 */
RISSE_DEFINE_CLASS_BEGIN(tMemoryStreamClass, tClassBase, tMemoryStreamInstance, itNormal)
RISSE_DEFINE_CLASS_END()
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif
