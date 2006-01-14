//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オンメモリストリームの実装
//---------------------------------------------------------------------------
#ifndef MemoryStreamH
#define MemoryStreamH

#include "risseUtils.h"


//---------------------------------------------------------------------------
//! @brief		メモリストリームで用いられるメモリブロック
//! @note		このメモリブロックは、複数のストリームで共有される可能性があり、
//!				複数スレッドから同時アクセスされる可能性がある
//---------------------------------------------------------------------------
class tTVPMemoryStreamBlock
{
	tRisseCriticalSection CS; //!< このメモリブロックへのアクセスを保護するクリティカルセクション
	void * Block;			//!< メモリブロック
	risse_size Size;			//!< メモリブロックのデータが入っている部分のサイズ
	risse_size AllocSize;		//!< メモリブロックのアロケートしているサイズ( Size <= AllocSize )
	risse_uint RefCount;		//!< 参照カウント

public:
	tTVPMemoryStreamBlock();
protected:
	~tTVPMemoryStreamBlock();
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
class tTVPMemoryStream : public tRisseBinaryStream
{
protected:
	tTVPMemoryStreamBlock * Block; //!< メモリブロック
	risse_size CurrentPos;		//!< 現在のポインタ
	risse_uint32 Flags;			//!< アクセスフラグ

public:
	tTVPMemoryStream(risse_uint32 flags);
	tTVPMemoryStream(risse_uint32 flags, tTVPMemoryStreamBlock * block);
	~tTVPMemoryStream();

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
