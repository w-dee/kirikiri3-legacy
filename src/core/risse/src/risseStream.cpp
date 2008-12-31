//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief バイナリストリーム
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseStream.h"
#include "risseExceptionClass.h"
#include "builtin/stream/risseStreamClass.h"
#include "risseStaticStrings.h"
#include "risseExceptionClass.h"
#include "risseScriptEngine.h"

RISSE_DEFINE_SOURCE_ID(1709,46737,4438,20346,9893,20339,39302,25230);


namespace Risse
{
//---------------------------------------------------------------------------
tStreamAdapter::tStreamAdapter(const tVariant & stream)
{
	// stream が Stream クラスのインスタンスであることをチェックする
	// stream の vt が Object でないとスクリプトエンジンインスタンスが
	// わからないので、最初にそれをチェックする。
	if(stream.GetType() != tVariant::vtObject)
		tIllegalArgumentClassExceptionClass::ThrowSpecifyInstanceOfClass(ss_Stream);
	tScriptEngine * engine = stream.GetObjectInterface()->GetRTTI()->GetScriptEngine();
	stream.AssertClass(engine->StreamPackageInitializer->StreamClass);

	Stream = static_cast<tStreamInstance *>(stream.GetObjectInterface());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStreamAdapter::SetName(const tString & name)
{
	Stream->SetPropertyDirect(ss_name, 0, name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tString tStreamAdapter::GetName()
{
	return Stream->GetPropertyDirect(ss_name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStreamAdapter::Dispose()
{
	Stream->Do(ocFuncCall, NULL, ss_dispose, 0, tMethodArgument::Empty());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tStreamAdapter::Seek(risse_int64 offset, tOrigin whence)
{
	return Stream->Invoke(ss_seek, offset, (risse_int64)whence);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tStreamAdapter::Tell()
{
	return (risse_int64)Stream->Invoke(ss_tell);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tStreamAdapter::Read(void *buffer, risse_size read_size)
{
	tOctetBlock ref_blk(tOctetBlock::MakeReference(static_cast<risse_uint8*>(buffer), read_size));
	tOctet ref(&ref_blk);
	return (risse_int64)Stream->Invoke(ss_get, tVariant(ref));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tStreamAdapter::Write(const void *buffer, risse_size write_size)
{
	tOctetBlock ref_blk(tOctetBlock::MakeReference(
				reinterpret_cast<const risse_uint8*>(buffer), write_size));
	tOctet ref(&ref_blk);
	return (risse_int64)Stream->Invoke(ss_put, tVariant(ref));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStreamAdapter::Truncate()
{
	Stream->Do(ocFuncCall, NULL, ss_truncate, 0, tMethodArgument::Empty());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tStreamAdapter::GetSize()
{
	return (risse_int64)Stream->GetPropertyDirect(ss_size);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStreamAdapter::Flush()
{
	Stream->Do(ocFuncCall, NULL, ss_flush, 0, tMethodArgument::Empty());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tStreamAdapter::GetPosition()
{
	return (risse_int64)Stream->GetPropertyDirect(ss_position);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStreamAdapter::SetPosition(risse_uint64 pos)
{
	Stream->SetPropertyDirect(ss_position, 0, (risse_int64)pos);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStreamAdapter::ReadBuffer(void *buffer, risse_size read_size)
{
	if(Read(buffer, read_size) != read_size)
		tIOExceptionClass::ThrowReadError(GetName());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStreamAdapter::WriteBuffer(const void *buffer, risse_size write_size)
{
	if(Write(buffer, write_size) != write_size)
		tIOExceptionClass::ThrowWriteError(GetName());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tStreamAdapter::ReadI64LE()
{
	risse_uint64 buf;
	ReadBuffer(static_cast<void*>(&buf), sizeof(buf));
#if RISSE_HOST_IS_LITTLE_ENDIAN
	return buf;
#endif
#if RISSE_HOST_IS_BIG_ENDIAN
	risse_uint64 tmp = buf;
	tmp = ((tmp & RISSE_UI64_VAL(0xff00ff00ff00ff00)) >>  8) + ((tmp & RISSE_UI64_VAL(0x00ff00ff00ff00ff)) <<  8);
	tmp = ((tmp & RISSE_UI64_VAL(0xffff0000ffff0000)) >> 16) + ((tmp & RISSE_UI64_VAL(0x0000ffff0000ffff)) << 16);
	tmp = ((tmp & RISSE_UI64_VAL(0xffffffff00000000)) >> 32) + ((tmp & RISSE_UI64_VAL(0x00000000ffffffff)) << 32);
	return tmp;
#endif
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint32 tStreamAdapter::ReadI32LE()
{
	risse_uint32 buf;
	ReadBuffer(static_cast<void*>(&buf), sizeof(buf));
#if RISSE_HOST_IS_LITTLE_ENDIAN
	return buf;
#endif
#if RISSE_HOST_IS_BIG_ENDIAN
	risse_uint32 tmp = buf;
	tmp = ((tmp & 0xff00ff00L) >>  8) + ((tmp & 0x00ff00ffL) <<  8);
	tmp = ((tmp & 0xffff0000L) >> 16) + ((tmp & 0x0000ffffL) << 16);
	return tmp;
#endif
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint16 tStreamAdapter::ReadI16LE()
{
	risse_uint16 buf;
	ReadBuffer(static_cast<void*>(&buf), sizeof(buf));
#if RISSE_HOST_IS_LITTLE_ENDIAN
	return buf;
#endif
#if RISSE_HOST_IS_BIG_ENDIAN
	risse_uint16 tmp = buf;
	tmp = ((tmp & 0xff00) >>  8) + ((tmp & 0x00ff) <<  8);
	return tmp;
#endif
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint8 tStreamAdapter::ReadI8LE()
{
	risse_uint8 buf;
	ReadBuffer(static_cast<void*>(&buf), sizeof(buf));
	return buf;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStreamAdapter::Write64LE(risse_uint64 v)
{
	risse_uint64 tmp = v;
#if RISSE_HOST_IS_BIG_ENDIAN
	tmp = ((tmp & RISSE_UI64_VAL(0xff00ff00ff00ff00)) >>  8) + ((tmp & RISSE_UI64_VAL(0x00ff00ff00ff00ff)) <<  8);
	tmp = ((tmp & RISSE_UI64_VAL(0xffff0000ffff0000)) >> 16) + ((tmp & RISSE_UI64_VAL(0x0000ffff0000ffff)) << 16);
	tmp = ((tmp & RISSE_UI64_VAL(0xffffffff00000000)) >> 32) + ((tmp & RISSE_UI64_VAL(0x00000000ffffffff)) << 32);
#endif
	WriteBuffer(static_cast<void*>(&tmp), sizeof(tmp));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStreamAdapter::WriteI32LE(risse_uint32 v)
{
	risse_uint32 tmp = v;
#if RISSE_HOST_IS_BIG_ENDIAN
	tmp = ((tmp & 0xff00ff00L) >>  8) + ((tmp & 0x00ff00ffL) <<  8);
	tmp = ((tmp & 0xffff0000L) >> 16) + ((tmp & 0x0000ffffL) << 16);
#endif
	WriteBuffer(static_cast<void*>(&tmp), sizeof(tmp));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStreamAdapter::WriteI16LE(risse_uint16 v)
{
	risse_uint16 tmp = v;
#if RISSE_HOST_IS_BIG_ENDIAN
	tmp = ((tmp & 0xff00) >>  8) + ((tmp & 0x00ff) <<  8);
#endif
	WriteBuffer(static_cast<void*>(&tmp), sizeof(tmp));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStreamAdapter::WriteI8LE(risse_uint8 v)
{
	WriteBuffer(static_cast<void*>(&v), sizeof(v));
}
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
} // namespace Risse


