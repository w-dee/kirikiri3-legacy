//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オンメモリストリームの実装
//---------------------------------------------------------------------------
#ifndef MemoryStreamH
#define MemoryStreamH

#include "risse/include/risseUtils.h"


//---------------------------------------------------------------------------
//! @brief		メモリストリームで用いられるメモリブロック
//! @note		このメモリブロックは、複数のストリームで共有される可能性があり、
//!				複数スレッドから同時アクセスされる可能性がある
//---------------------------------------------------------------------------
class tRisaMemoryStreamBlock
{
	tRisseCriticalSection CS; //!< このメモリブロックへのアクセスを保護するクリティカルセクション
	void * Block;			//!< メモリブロック
	risse_size Size;			//!< メモリブロックのデータが入っている部分のサイズ
	risse_size AllocSize;		//!< メモリブロックのアロケートしているサイズ( Size <= AllocSize )
	risse_uint RefCount;		//!< 参照カウント

public:
	tRisaMemoryStreamBlock();
protected:
	~tRisaMemoryStreamBlock();
public:
	void AddRef();
	void Release();

	void ChangeSize(risse_size);
	void Fit();

	void * GetBlock() { return Block; } //!< ブロックを得る

	tRisseCriticalSection & GetCS( ) { return CS; } //!< クリティカルセクションオブジェクトを得る
	risse_size GetSize() const { return Size; } //!< ブロックのサイズを得る
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		メモリストリーム
//---------------------------------------------------------------------------
/*
	this class provides a tRisseBinaryStream based access method for a memory block.
*/
class tRisaMemoryStream : public tRisseBinaryStream
{
protected:
	tRisaMemoryStreamBlock * Block; //!< メモリブロック
	risse_size CurrentPos;		//!< 現在のポインタ
	risse_uint32 Flags;			//!< アクセスフラグ

public:
	tRisaMemoryStream(risse_uint32 flags);
	tRisaMemoryStream(risse_uint32 flags, tRisaMemoryStreamBlock * block);
	~tRisaMemoryStream();

	risse_uint64 Seek(risse_int64 offset, risse_int whence);
	risse_size Read(void *buffer, risse_size read_size);
	risse_size Write(const void *buffer, risse_size write_size);
	void SetEndOfFile();
	risse_uint64 GetSize() { return Block->GetSize(); }

	// non-tRisseBinaryStream based methods
	void * GetInternalBuffer()  const { return Block->GetBlock(); }
};
//---------------------------------------------------------------------------


#endif
