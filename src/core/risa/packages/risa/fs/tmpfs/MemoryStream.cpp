//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief オンメモリストリームの実装
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/fs/tmpfs/MemoryStream.h"
#include "risa/common/RisaException.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(64253,18417,33137,18368,23694,7632,14591,23108);
//---------------------------------------------------------------------------


/*
	将来的にはここは別の、もっと効率のよい固定ブロックベースのアロケータに変える予定。
*/


//---------------------------------------------------------------------------
tMemoryStreamBlock::tMemoryStreamBlock()
{
	Block = NULL;
	Size = 0;
	AllocSize = 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tMemoryStreamBlock::~tMemoryStreamBlock()
{
	if(Block) FreeCollectee(Block), Block = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tMemoryStreamBlock::ChangeSize(risse_size size)
{
	volatile tCriticalSection::tLocker holder(CS);

	if(Size < size && size <= AllocSize)
	{
		// サイズは拡張するが、すでに必要な容量は確保されている
		Size = size;
		return;
	}

	// サイズが縮小される場合、あるいはサイズを拡張しなければ
	// ならない場合

	// 確保するサイズを決定する
	// XXX: おそらく間違いなく、ReallocCollectee では新しいメモリブロックの
	// 確保と、そこへのメモリブロックのコピーが発生する。
	// したがって非効率的。
	risse_size onesize;
	if(AllocSize < 64*1024) onesize = 4*1024;
	else if(AllocSize < 512*1024) onesize = 16*1024;
	else if(AllocSize < 4096*1024) onesize = 256*1024;
	else onesize = 512*1024;
	AllocSize = size + onesize;

	// メモリを確保する
	if(Block == NULL)
		Block = MallocAtomicCollectee(AllocSize);
	else
		Block = ReallocCollectee(Block, AllocSize);

	if(AllocSize && !Block)
		tIOExceptionClass::Throw(RISSE_WS_TR("insufficient memory"));
		// this exception cannot be repaird; a fatal error.

	AllocSize = Size = size;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tMemoryStreamBlock::Fit()
{
	// 通常、AllocSize は Size よりも大きくなるが、
	// Size よりも大きくて AllocSize よりも小さな部分は無駄である。
	// このメソッドは、メモリブロックのサイズを Size ぴったりにすることにより
	// この無駄な部分を解放する。
	volatile tCriticalSection::tLocker holder(CS);

	if(Size != AllocSize)
	{
		if(Block == NULL)
			Block = MallocAtomicCollectee(Size);
		else
			Block = ReallocCollectee(Block, Size);
		if(Size && !Block)
			tIOExceptionClass::Throw(RISSE_WS_TR("insufficient memory"));
		AllocSize = Size;
	}
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
tMemoryStreamInstance::tMemoryStreamInstance()
{
	Flags = 0;
	CurrentPos = 0;
	Block = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tMemoryStreamInstance::SeekEnd()
{
	volatile tSynchronizer sync(this); // sync
	{
		if(!Block) tInaccessibleResourceExceptionClass::Throw();

		volatile tCriticalSection::tLocker holder(Block->GetCS());

		CurrentPos = Block->GetSize();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tMemoryStreamInstance::initialize(risse_uint32 flags, const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync
	{
		info.InitializeSuperClass(); // スーパークラスのコンストラクタを呼ぶ

		bool open_stream = info.args.HasArgument(1) ? (bool)info.args[1] : true;

		Flags = flags;
		CurrentPos = 0;
		Block = NULL;

		if(open_stream)
		{
			// open_stream が true の場合は空のストリームを作る
			Block = new tMemoryStreamBlock();
		}

		if(Block)
		{
			volatile tCriticalSection::tLocker holder(Block->GetCS());
			CurrentPos = flags & tFileOpenModes::omAppendBit ? Block->GetSize() : 0;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tMemoryStreamInstance::dispose()
{
	volatile tSynchronizer sync(this); // sync
	{
		if(Block)
		{
			Block->Fit(); // メモリブロックのよけいな余裕を解放
			Block = NULL;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tMemoryStreamInstance::seek(risse_int64 offset, tOrigin whence)
{
	volatile tSynchronizer sync(this); // sync
	{
		if(!Block) tInaccessibleResourceExceptionClass::Throw();

		volatile tCriticalSection::tLocker holder(Block->GetCS());

		risse_int64 newpos;
		switch(whence)
		{
		case tStreamConstants::soSet:
			newpos = offset;
			break;

		case tStreamConstants::soCur:
			newpos = static_cast<risse_int64>(CurrentPos) + offset;
			break;

		case tStreamConstants::soEnd:
			newpos = static_cast<risse_int64>(Block->GetSize()) + offset;
			break;
		}

		if(newpos >= 0 && newpos <= Block->GetSize())
		{
			CurrentPos = newpos;
			return true;
		}
		else
		{
			return false;
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tMemoryStreamInstance::tell()
{
	volatile tSynchronizer sync(this); // sync
	{
		if(!Block) tInaccessibleResourceExceptionClass::Throw();

		return CurrentPos;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tMemoryStreamInstance::get(const tOctet & buf)
{
	volatile tSynchronizer sync(this); // sync
	{
		if(!Block) tInaccessibleResourceExceptionClass::Throw();

		volatile tCriticalSection::tLocker holder(Block->GetCS());

		if(!(Flags & tFileOpenModes::omReadBit))
			tIOExceptionClass::Throw(RISSE_WS_TR("access denied (stream has no read-access)"));

		if(CurrentPos >= Block->GetSize()) return 0; // can not read from there

		risse_size read_size = buf.GetLength();

		if(CurrentPos + read_size >= Block->GetSize())
		{
			read_size = Block->GetSize() - CurrentPos;
		}

		memcpy(const_cast<risse_uint8*>(buf.Pointer()),
			static_cast<risse_uint8*>(Block->GetBlock()) + CurrentPos,
			read_size);

		CurrentPos += read_size;

		return read_size;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tMemoryStreamInstance::put(const tOctet & buf)
{
	volatile tSynchronizer sync(this); // sync
	{
		if(!Block) tInaccessibleResourceExceptionClass::Throw();

		volatile tCriticalSection::tLocker holder(Block->GetCS());

		if(!(Flags & tFileOpenModes::omWriteBit))
			tIOExceptionClass::Throw(RISSE_WS_TR("access denied (stream has no write-access)"));

		// writing may increase the internal buffer size.
		risse_size write_size = buf.GetLength();

		risse_uint newpos = CurrentPos + write_size;
		if(newpos >= Block->GetSize())
		{
			// exceeds Size
			Block->ChangeSize(newpos);
		}

		memcpy(static_cast<risse_uint8*>(Block->GetBlock()) + CurrentPos,
			buf.Pointer(), write_size);

		CurrentPos = newpos;

		return write_size;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tMemoryStreamInstance::truncate()
{
	volatile tSynchronizer sync(this); // sync
	if(!Block) tInaccessibleResourceExceptionClass::Throw();

	volatile tCriticalSection::tLocker holder(Block->GetCS());

	if(!(Flags & tFileOpenModes::omWriteBit))
		tIOExceptionClass::Throw(RISSE_WS_TR("access denied (stream has no write-access)"));

	Block->ChangeSize(CurrentPos);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tMemoryStreamInstance::get_size()
{
	volatile tSynchronizer sync(this); // sync
	{
		if(!Block) tInaccessibleResourceExceptionClass::Throw();

		volatile tCriticalSection::tLocker holder(Block->GetCS());

		return Block->GetSize();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tMemoryStreamInstance::flush()
{
	// なにもしない
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tMemoryStreamClass::tMemoryStreamClass(tScriptEngine * engine) :
	tClassBase(tSS<'O','S','N','a','t','i','v','e','S','t','r','e','a','m'>(),
		static_cast<tClassBase*>(engine->GetPackageGlobal(tSS<'s','t','r','e','a','m'>()).
		GetPropertyDirect(engine, tSS<'S','t','r','e','a','m'>()).GetObjectInterface()))
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tMemoryStreamClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tMemoryStreamClass::ovulate);
	BindFunction(this, ss_construct, &tMemoryStreamInstance::construct);
	BindFunction(this, ss_initialize, &tMemoryStreamInstance::initialize);
	BindFunction(this, ss_dispose, &tMemoryStreamInstance::dispose);
	BindFunction(this, ss_seek, &tMemoryStreamInstance::seek);
	BindFunction(this, ss_tell, &tMemoryStreamInstance::tell);
	BindFunction(this, ss_get, &tMemoryStreamInstance::get);
	BindFunction(this, ss_put, &tMemoryStreamInstance::put);
	BindFunction(this, ss_truncate, &tMemoryStreamInstance::truncate);
	BindProperty(this, ss_size, &tMemoryStreamInstance::get_size);
	BindFunction(this, ss_flush, &tMemoryStreamInstance::flush);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tMemoryStreamClass::ovulate()
{
	return tVariant(new tMemoryStreamInstance());
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace Risa



