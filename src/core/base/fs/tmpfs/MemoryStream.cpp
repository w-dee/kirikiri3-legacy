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

#include "MemoryStream.h"



//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTVPMemoryStreamBlock::tTVPMemoryStreamBlock()
{
	Block = NULL;
	Size = 0;
	AllocSize = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPMemoryStreamBlock::~tTVPMemoryStreamBlock()
{
	if(Block) free(Block);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//! @brief		メモリブロックのサイズを変更する
//! @note		サイズが拡張される場合、メモリブロックの内容は保たれるが、
//!				サイズが拡張された部分の内容は不定となる。縮小される場合、
//!				内容は最後が切りつめられる。
//---------------------------------------------------------------------------
void tTVPMemoryStreamBlock::ChangeSize(tjs_size size)
{
	tTJSCriticalSectionHolder holder(CS);

	if(Size < size && size <= AllocSize)
	{
		// サイズは拡張するが、すでに必要な容量は確保されている
		Size = size;
		return;
	}

	// サイズが縮小される場合、あるいはサイズを拡張しなければ
	// ならない場合

	// 確保するサイズを決定する
	tjs_size onesize;
	if(AllocSize < 64*1024) onesize = 4*1024;
	else if(AllocSize < 512*1024) onesize = 16*1024;
	else if(AllocSize < 4096*1024) onesize = 256*1024;
	else onesize = 2024*1024;
	AllocSize = size + onesize;

	// メモリを確保する
	Block = Realloc(Block, AllocSize);

	if(AllocSize && !Block)
		TVPThrowExceptionMessage(_("insufficient memory"));
		// this exception cannot be repaird; a fatal error.

	AllocSize = Size = size;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		メモリブロックのサイズをSizeぴったりのサイズに変更する
//---------------------------------------------------------------------------
void tTVPMemoryStreamBlock::Fit()
{
	// 通常、AllocSize は Size よりも大きくなるが、
	// Size よりも大きくて AllocSize よりも小さな部分は無駄である。
	// このメソッドは、メモリブロックのサイズを Size ぴったりにすることにより
	// この無駄な部分を解放する。

	if(Size != AllocSize)
	{
		Block = Realloc(Block, Size);
		if(Size && !Block)
			TVPThrowExceptionMessage(_("insufficient memory"));
		AllocSize = Size;
	}
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTVPMemoryStream::tTVPMemoryStream()
{
	Block = new tTVPMemoryStreamBlock();
	Reference = false;
	CurrentPos = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ(他のメモリブロックを参照する場合)
//---------------------------------------------------------------------------
tTVPMemoryStream::tTVPMemoryStream(tTVPMemoryStreamBlock * block)
{
	Block = block;
	Reference = true;
	CurrentPos = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		デストラクタ
//---------------------------------------------------------------------------
tTVPMemoryStream::~tTVPMemoryStream()
{
	if(!Reference) delete Block;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		シーク
//---------------------------------------------------------------------------
tjs_uint64 TJS_INTF_METHOD tTVPMemoryStream::Seek(tjs_int64 offset, tjs_int whence)
{
	tTJSCriticalSectionHolder holder(Block->GetCS());

	tjs_int64 newpos;
	switch(whence)
	{
	case TJS_BS_SEEK_SET:
		if(offset >= 0)
		{
			if(offset <= Block->GetSize()) CurrentPos = offset;
		}
		return CurrentPos;

	case TJS_BS_SEEK_CUR:
		if((newpos = offset + (tjs_int64)CurrentPos) >= 0)
		{
			tjs_uint np = (tjs_uint)newpos;
			if(np <= Block->GetSize()) CurrentPos = np;
		}
		return CurrentPos;

	case TJS_BS_SEEK_END:
		if((newpos = offset + (tjs_int64)Block->GetSize()) >= 0)
		{
			tjs_uint np = (tjs_uint)newpos;
			if(np <= Block->GetSize()) CurrentPos = np;
		}
		return CurrentPos;
	}
	return CurrentPos;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		読み込み
//---------------------------------------------------------------------------
tjs_size TJS_INTF_METHOD tTVPMemoryStream::Read(void *buffer, tjs_size read_size)
{
	tTJSCriticalSectionHolder holder(Block->GetCS());

	if(CurrentPos + read_size >= Block->GetSize())
	{
		read_size = Block->GetSize() - CurrentPos;
	}

	memcpy(buffer, (tjs_uint8*)Block->GetBlock() + CurrentPos, read_size);

	CurrentPos += read_size;

	return read_size;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		書き込み
//---------------------------------------------------------------------------
tjs_size TJS_INTF_METHOD tTVPMemoryStream::Write(const void *buffer, tjs_size write_size)
{
	tTJSCriticalSectionHolder holder(Block->GetCS());

	// writing may increase the internal buffer size.
	tjs_uint newpos = CurrentPos + write_size;
	if(newpos >= Block->GetSize())
	{
		// exceeds Size
		Block->ChangeSize(newpos);
	}

	memcpy((tjs_uint8*)Block->GetBlock() + CurrentPos, buffer, write_size);

	CurrentPos = newpos;

	return write_size;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ファイルの終わりを現在のポインタに設定する
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPMemoryStream::SetEndOfFile()
{
	tTJSCriticalSectionHolder holder(Block->GetCS());

	Block->ChangeSize(CurrentPos);
}
//---------------------------------------------------------------------------




