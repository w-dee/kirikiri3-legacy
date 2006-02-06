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
#include "prec.h"
#include "MemoryStream.h"
#include "RisaException.h"

RISSE_DEFINE_SOURCE_ID(64253,18417,33137,18368,23694,7632,14591,23108);


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaMemoryStreamBlock::tRisaMemoryStreamBlock()
{
	Block = NULL;
	Size = 0;
	AllocSize = 0;
	RefCount = 1;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaMemoryStreamBlock::~tRisaMemoryStreamBlock()
{
	if(Block) free(Block);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		参照カウンタを一つ増やす
//---------------------------------------------------------------------------
void tRisaMemoryStreamBlock::AddRef()
{
	volatile tRisseCriticalSection::tLocker holder(CS);

	RefCount ++;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		参照カウンタを一つ減らす
//---------------------------------------------------------------------------
void tRisaMemoryStreamBlock::Release()
{
	risse_uint decremented_count;

	{
		volatile tRisseCriticalSection::tLocker holder(CS);

		RefCount --;
		decremented_count = RefCount;
	}

	if(decremented_count == 0) delete this;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		メモリブロックのサイズを変更する
//! @param		size 新しいサイズ
//! @note		サイズが拡張される場合、メモリブロックの内容は保たれるが、
//!				サイズが拡張された部分の内容は不定となる。縮小される場合、
//!				内容は最後が切りつめられる。
//---------------------------------------------------------------------------
void tRisaMemoryStreamBlock::ChangeSize(risse_size size)
{
	volatile tRisseCriticalSection::tLocker holder(CS);

	if(Size < size && size <= AllocSize)
	{
		// サイズは拡張するが、すでに必要な容量は確保されている
		Size = size;
		return;
	}

	// サイズが縮小される場合、あるいはサイズを拡張しなければ
	// ならない場合

	// 確保するサイズを決定する
	risse_size onesize;
	if(AllocSize < 64*1024) onesize = 4*1024;
	else if(AllocSize < 512*1024) onesize = 16*1024;
	else if(AllocSize < 4096*1024) onesize = 256*1024;
	else onesize = 2024*1024;
	AllocSize = size + onesize;

	// メモリを確保する
	Block = realloc(Block, AllocSize);

	if(AllocSize && !Block)
		eRisaException::Throw(RISSE_WS_TR("insufficient memory"));
		// this exception cannot be repaird; a fatal error.

	AllocSize = Size = size;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		メモリブロックのサイズをSizeぴったりのサイズに変更する
//---------------------------------------------------------------------------
void tRisaMemoryStreamBlock::Fit()
{
	// 通常、AllocSize は Size よりも大きくなるが、
	// Size よりも大きくて AllocSize よりも小さな部分は無駄である。
	// このメソッドは、メモリブロックのサイズを Size ぴったりにすることにより
	// この無駄な部分を解放する。
	volatile tRisseCriticalSection::tLocker holder(CS);

	if(Size != AllocSize)
	{
		Block = realloc(Block, Size);
		if(Size && !Block)
			eRisaException::Throw(RISSE_WS_TR("insufficient memory"));
		AllocSize = Size;
	}
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		flags アクセスフラグ
//---------------------------------------------------------------------------
tRisaMemoryStream::tRisaMemoryStream(risse_uint32 flags)
{
	Flags = flags;
	Block = new tRisaMemoryStreamBlock();

	volatile tRisseCriticalSection::tLocker holder(Block->GetCS());
	CurrentPos = flags & RISSE_BS_ACCESS_APPEND_BIT ? Block->GetSize() : 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ(他のメモリブロックを参照する場合)
//! @param		flags アクセスフラグ
//! @param		block メモリブロック
//---------------------------------------------------------------------------
tRisaMemoryStream::tRisaMemoryStream(risse_uint32 flags, tRisaMemoryStreamBlock * block)
{
	Flags = flags;
	Block = block;

	volatile tRisseCriticalSection::tLocker holder(Block->GetCS());
	Block->AddRef();
	CurrentPos = flags & RISSE_BS_ACCESS_APPEND_BIT ? Block->GetSize() : 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tRisaMemoryStream::~tRisaMemoryStream()
{
	Block->Fit(); // メモリブロックのよけいな余裕を解放
	Block->Release();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		シーク
//! @param		offset 移動オフセット
//! @param		whence 移動オフセットの基準 (RISSE_BS_SEEK_* 定数)
//! @return		移動後のファイルポインタ
//---------------------------------------------------------------------------
risse_uint64 tRisaMemoryStream::Seek(risse_int64 offset, risse_int whence)
{
	volatile tRisseCriticalSection::tLocker holder(Block->GetCS());

	risse_int64 newpos;
	switch(whence)
	{
	case RISSE_BS_SEEK_SET:
		if(offset >= 0)
		{
			if(offset <= Block->GetSize()) CurrentPos = offset;
		}
		return CurrentPos;

	case RISSE_BS_SEEK_CUR:
		if((newpos = offset + static_cast<risse_int64>(CurrentPos)) >= 0)
		{
			risse_uint np = static_cast<risse_uint>(newpos);
			if(np <= Block->GetSize()) CurrentPos = np;
		}
		return CurrentPos;

	case RISSE_BS_SEEK_END:
		if((newpos = offset + static_cast<risse_int64>(Block->GetSize())) >= 0)
		{
			risse_uint np = static_cast<risse_uint>(newpos);
			if(np <= Block->GetSize()) CurrentPos = np;
		}
		return CurrentPos;
	}
	return CurrentPos;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		読み込み
//! @param		buffer 読み込み先バッファ
//! @param		read_size 読み込むバイト数
//! @return		実際に読み込まれたバイト数
//---------------------------------------------------------------------------
risse_size tRisaMemoryStream::Read(void *buffer, risse_size read_size)
{
	volatile tRisseCriticalSection::tLocker holder(Block->GetCS());

	if(!(Flags & RISSE_BS_ACCESS_READ_BIT))
		eRisaException::Throw(RISSE_WS_TR("access denied (stream has no read-access)"));

	if(CurrentPos > Block->GetSize()) return 0; // can not read from there

	if(CurrentPos + read_size >= Block->GetSize())
	{
		read_size = Block->GetSize() - CurrentPos;
	}

	memcpy(buffer, reinterpret_cast<risse_uint8*>(Block->GetBlock()) + CurrentPos, read_size);

	CurrentPos += read_size;

	return read_size;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		書き込み
//! @param		buffer 書き込むバッファ
//! @param		read_size 書き込みたいバイト数
//! @return		実際に書き込まれたバイト数
//---------------------------------------------------------------------------
risse_size tRisaMemoryStream::Write(const void *buffer, risse_size write_size)
{
	volatile tRisseCriticalSection::tLocker holder(Block->GetCS());

	if(!(Flags & RISSE_BS_ACCESS_WRITE_BIT))
		eRisaException::Throw(RISSE_WS_TR("access denied (stream has no write-access)"));

	// adjust current file pointer
	if(CurrentPos > Block->GetSize()) return 0; // can not write there

	// writing may increase the internal buffer size.
	risse_uint newpos = CurrentPos + write_size;
	if(newpos >= Block->GetSize())
	{
		// exceeds Size
		Block->ChangeSize(newpos);
	}

	memcpy(reinterpret_cast<risse_uint8*>(Block->GetBlock()) + CurrentPos, buffer, write_size);

	CurrentPos = newpos;

	return write_size;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルの終わりを現在のポインタに設定する
//---------------------------------------------------------------------------
void tRisaMemoryStream::SetEndOfFile()
{
	volatile tRisseCriticalSection::tLocker holder(Block->GetCS());

	if(!(Flags & RISSE_BS_ACCESS_WRITE_BIT))
		eRisaException::Throw(RISSE_WS_TR("access denied (stream has no write-access)"));

	Block->ChangeSize(CurrentPos);
}
//---------------------------------------------------------------------------




