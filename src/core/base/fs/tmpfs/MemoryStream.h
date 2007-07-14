//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オンメモリストリームの実装
//---------------------------------------------------------------------------
#ifndef MemoryStreamH
#define MemoryStreamH

#include "base/utils/RisaThread.h"


//---------------------------------------------------------------------------
//! @brief		メモリストリームで用いられるメモリブロック
//! @note		このメモリブロックは、複数のストリームで共有される可能性があり、
//!				複数スレッドから同時アクセスされる可能性がある
//---------------------------------------------------------------------------
class tRisaMemoryStreamBlock
{
	tRisaCriticalSection CS; //!< このメモリブロックへのアクセスを保護するクリティカルセクション
	void * Block;			//!< メモリブロック
	risse_size Size;			//!< メモリブロックのデータが入っている部分のサイズ
	risse_size AllocSize;		//!< メモリブロックのアロケートしているサイズ( Size <= AllocSize )
	risse_uint RefCount;		//!< 参照カウント

public:
	//! @brief		コンストラクタ
	tRisaMemoryStreamBlock();
protected:
	//! @brief		デストラクタ
	~tRisaMemoryStreamBlock();
public:
	//! @brief		参照カウンタを一つ増やす
	void AddRef();

	//! @brief		参照カウンタを一つ減らす
	void Release();

	//! @brief		メモリブロックのサイズを変更する
	//! @param		size 新しいサイズ
	//! @note		サイズが拡張される場合、メモリブロックの内容は保たれるが、
	//!				サイズが拡張された部分の内容は不定となる。縮小される場合、
	//!				内容は最後が切りつめられる。
	void ChangeSize(risse_size);

	//! @brief		メモリブロックのサイズをSizeぴったりのサイズに変更する
	void Fit();

	void * GetBlock() { return Block; } //!< ブロックを得る

	tRisaCriticalSection & GetCS( ) { return CS; } //!< クリティカルセクションオブジェクトを得る
	risse_size GetSize() const { return Size; } //!< ブロックのサイズを得る
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		メモリストリーム
//---------------------------------------------------------------------------
/*
	this class provides a tBinaryStream based access method for a memory block.
*/
class tRisaMemoryStream : public tBinaryStream
{
protected:
	tRisaMemoryStreamBlock * Block; //!< メモリブロック
	risse_size CurrentPos;		//!< 現在のポインタ
	risse_uint32 Flags;			//!< アクセスフラグ

public:
	//! @brief		コンストラクタ
	//! @param		flags アクセスフラグ
	tRisaMemoryStream(risse_uint32 flags);

	//! @brief		コンストラクタ(他のメモリブロックを参照する場合)
	//! @param		flags アクセスフラグ
	//! @param		block メモリブロック
	tRisaMemoryStream(risse_uint32 flags, tRisaMemoryStreamBlock * block);

	//! @brief		デストラクタ
	~tRisaMemoryStream();

	//! @brief		シーク
	//! @param		offset 移動オフセット
	//! @param		whence 移動オフセットの基準 (RISSE_BS_SEEK_* 定数)
	//! @return		移動後のファイルポインタ
	risse_uint64 Seek(risse_int64 offset, risse_int whence);

	//! @brief		読み込み
	//! @param		buffer 読み込み先バッファ
	//! @param		read_size 読み込むバイト数
	//! @return		実際に読み込まれたバイト数
	risse_size Read(void *buffer, risse_size read_size);

	//! @brief		書き込み
	//! @param		buffer 書き込むバッファ
	//! @param		read_size 書き込みたいバイト数
	//! @return		実際に書き込まれたバイト数
	risse_size Write(const void *buffer, risse_size write_size);

	//! @brief		ファイルの終わりを現在のポインタに設定する
	void SetEndOfFile();

	risse_uint64 GetSize() { return Block->GetSize(); }

	// non-tBinaryStream based methods
	void * GetInternalBuffer()  const { return Block->GetBlock(); }
};
//---------------------------------------------------------------------------


#endif
