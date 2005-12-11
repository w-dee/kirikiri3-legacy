//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オンメモリストリームの実装
//---------------------------------------------------------------------------
#ifndef MemoryStreamH
#define MemoryStreamH



//---------------------------------------------------------------------------
//! @brief		メモリストリームで用いられるメモリブロック
//! @note		このメモリブロックは、複数のストリームで共有される可能性があり、
//!				複数スレッドから同時アクセスされる可能性がある
//---------------------------------------------------------------------------
class tTVPMemoryStreamBlock
{
	tTJSCriticalSection CS; //!< このメモリブロックへのアクセスを保護するクリティカルセクション
	void * Block;			//!< メモリブロック
	tjs_size Size;			//!< メモリブロックのデータが入っている部分のサイズ
	tjs_size AllocSize;		//!< メモリブロックのアロケートしているサイズ( Size <= AllocSize )
	tjs_uint RefCount;		//!< 参照カウント

public:
	tTVPMemoryStreamBlock();
protected:
	~tTVPMemoryStreamBlock();
public:
	void AddRef();
	void Release();

	void ChangeSize(tjs_size);
	void Fit();

	void * GetBlock() { return Block; } //!< ブロックを得る

	tTJSCriticalSection & GetCS( ) { return CS; } //!< クリティカルセクションオブジェクトを得る
	tjs_size GetSize() const { return Size; } //!< ブロックのサイズを得る
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		メモリストリーム
//---------------------------------------------------------------------------
/*
	this class provides a tTJSBinaryStream based access method for a memory block.
*/
class tTVPMemoryStream : public tTJSBinaryStream
{
protected:
	tTVPMemoryStreamBlock * Block; //!< メモリブロック
	tjs_size CurrentPos;		//!< 現在のポインタ
	tjs_uint32 Flags;			//!< アクセスフラグ

public:
	tTVPMemoryStream(tjs_uint32 flags);
	tTVPMemoryStream(tjs_uint32 flags, tTVPMemoryStreamBlock * block);
	~tTVPMemoryStream();

	tjs_uint64 Seek(tjs_int64 offset, tjs_int whence);
	tjs_size Read(void *buffer, tjs_size read_size);
	tjs_size Write(const void *buffer, tjs_size write_size);
	void SetEndOfFile();
	tjs_uint64 GetSize() { return Size; }

	// non-tTJSBinaryStream based methods
	void * GetInternalBuffer()  const { return Block->GetBlock(); }
};
//---------------------------------------------------------------------------


#endif
