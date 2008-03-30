//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OSFSの実装
//---------------------------------------------------------------------------
#include "prec.h"
#include "risa/packages/risa/file/fs/osfs/OSFS.h"
#include "base/exception/RisaException.h"
#include "risse/include/risseExceptionClass.h"
#include "risse/include/risseStaticStrings.h"
#include <wx/filename.h>
#include <wx/dir.h>



namespace Risa {
RISSE_DEFINE_SOURCE_ID(49572,65271,56057,18682,27296,33314,20965,8152);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSNativeStreamInstance::initialize(const tString & path, risse_uint32 flags, const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	info.InitializeSuperClass();

	// モードを決定する
	wxFile::OpenMode mode;
	switch(flags & tFileOpenModes::omAccessMask)
	{
	case tFileOpenModes::omRead:
		mode = wxFile::read; break;
	case tFileOpenModes::omWrite:
		mode = wxFile::write; break;
	case tFileOpenModes::omUpdate:
		mode = wxFile::read_write; break;
	default:
		mode = wxFile::read;
	}

	// ファイルを開く
	Internal = new tInternal();
	if(!Internal->File.Open(path.AsWxString(), mode))
	{
		delete Internal; Internal = NULL;
		tIOExceptionClass::Throw(
			tString(RISSE_WS_TR("can not open file %1"), path));
	}

	// 名前を

	// APPEND の場合はファイルポインタを最後に移動する
	if(flags & tFileOpenModes::omAppendBit)
		Internal->File.SeekEnd();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOSNativeStreamInstance::~tOSNativeStreamInstance()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSNativeStreamInstance::dispose()
{
	volatile tSynchronizer sync(this); // sync
	if(Internal)
		delete Internal, Internal = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tOSNativeStreamInstance::seek(risse_int64 offset, tOrigin whence)
{
	volatile tSynchronizer sync(this); // sync

	if(!Internal) tInaccessibleResourceExceptionClass::Throw();

	wxSeekMode mode = wxFromCurrent;
	switch(whence)
	{
	case soSet: mode = wxFromStart;   break;
	case soCur: mode = wxFromCurrent; break;
	case soEnd: mode = wxFromEnd;     break;
	default: offset = 0;
	}

	wxFileOffset newpos = Internal->File.Seek(offset, mode);
	if(newpos == wxInvalidOffset)
		return false;

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tOSNativeStreamInstance::tell()
{
	volatile tSynchronizer sync(this); // sync

	if(!Internal) tInaccessibleResourceExceptionClass::Throw();

	return Internal->File.Tell();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tOSNativeStreamInstance::get(const tOctet & buf)
{
	volatile tSynchronizer sync(this); // sync

	if(!Internal) tInaccessibleResourceExceptionClass::Throw();

	// buf の示すポインタに「直接」書き込みを行う。
	// tOctet をメモリバッファとして使うかなり危ない使い方だが、
	// これに限ってはこういう使い方をすると言うことになっている。
	// というかそうした。

	ssize_t read = Internal->File.Read(const_cast<risse_uint8*>(buf.Pointer()), buf.GetLength());

	if(read < 0 || read == wxInvalidOffset) read = 0; // エラーと見なす

	return read;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tOSNativeStreamInstance::put(const tOctet & buf)
{
	volatile tSynchronizer sync(this); // sync

	if(!Internal) tInaccessibleResourceExceptionClass::Throw();

	return Internal->File.Write(buf.Pointer(), buf.GetLength());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSNativeStreamInstance::truncate()
{
	// TODO: implement this 
	if(!Internal) tInaccessibleResourceExceptionClass::Throw();

	tIOExceptionClass::Throw(RISSE_WS_TR("tOSNativeStreamInstance::Truncate not implemented"));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tOSNativeStreamInstance::get_size()
{
	volatile tSynchronizer sync(this); // sync

	if(!Internal) tInaccessibleResourceExceptionClass::Throw();

	return Internal->File.Length();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSNativeStreamInstance::flush()
{
	volatile tSynchronizer sync(this); // sync

	if(!Internal) tInaccessibleResourceExceptionClass::Throw();

	Internal->File.Flush();
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tOSNativeStreamClass::tOSNativeStreamClass(tScriptEngine * engine) :
	tClassBase(tSS<'O','S','N','a','t','i','v','e','S','t','r','e','a','m'>(),
		static_cast<tClassBase*>(engine->GetPackageGlobal(tSS<'s','t','r','e','a','m'>()).
			GetPropertyDirect(engine, tSS<'S','t','r','e','a','m'>()).GetObjectInterface()))
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tOSNativeStreamClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tOSNativeStreamClass::ovulate);
	BindFunction(this, ss_construct, &tOSNativeStreamInstance::construct);
	BindFunction(this, ss_initialize, &tOSNativeStreamInstance::initialize);
	BindFunction(this, ss_dispose, &tOSNativeStreamInstance::dispose);
	BindFunction(this, ss_seek, &tOSNativeStreamInstance::seek);
	BindFunction(this, ss_tell, &tOSNativeStreamInstance::tell);
	BindFunction(this, ss_get, &tOSNativeStreamInstance::get);
	BindFunction(this, ss_put, &tOSNativeStreamInstance::put);
	BindFunction(this, ss_truncate, &tOSNativeStreamInstance::truncate);
	BindProperty(this, ss_size, &tOSNativeStreamInstance::get_size);
	BindFunction(this, ss_flush, &tOSNativeStreamInstance::flush);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tOSNativeStreamClass::ovulate()
{
	return tVariant(new tOSNativeStreamInstance());
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
} // namespace Risa


