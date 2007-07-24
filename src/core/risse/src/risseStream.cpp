//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief バイナリストリーム
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseStream.h"
#include "risseExceptionClass.h"
#include "risseStreamClass.h"
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
	stream.AssertClass(engine->StreamClass);

	Stream = reinterpret_cast<tStreamInstance *>(stream.GetObjectInterface());
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
	tOctetBlock ref_blk(tOctetBlock::MakeReference(reinterpret_cast<risse_uint8*>(buffer), read_size));
	tOctet ref(&ref_blk);
	return (risse_int64)Stream->Invoke(ss_read, tVariant(ref));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tStreamAdapter::Write(const void *buffer, risse_size write_size)
{
	tOctetBlock ref_blk(tOctetBlock::MakeReference(
				reinterpret_cast<const risse_uint8*>(buffer), write_size));
	tOctet ref(&ref_blk);
	return (risse_int64)Stream->Invoke(ss_write, tVariant(ref));
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
	tOctetBlock ref_blk(tOctetBlock::MakeReference(reinterpret_cast<risse_uint8*>(buffer), read_size));
	tOctet ref(&ref_blk);
	Stream->Do(ocFuncCall, NULL, ss_readBuffer, 0, tMethodArgument::New(tVariant(ref)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStreamAdapter::WriteBuffer(const void *buffer, risse_size write_size)
{
	tOctetBlock ref_blk(tOctetBlock::MakeReference(
				reinterpret_cast<const risse_uint8*>(buffer), write_size));
	tOctet ref(&ref_blk);
	Stream->Do(ocFuncCall, NULL, ss_writeBuffer, 0, tMethodArgument::New(tVariant(ref)));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tStreamAdapter::ReadI64LE()
{
	risse_uint64 buf;
	ReadBuffer(reinterpret_cast<void*>(&buf), sizeof(buf));
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
	ReadBuffer(reinterpret_cast<void*>(&buf), sizeof(buf));
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
	ReadBuffer(reinterpret_cast<void*>(&buf), sizeof(buf));
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
	ReadBuffer(reinterpret_cast<void*>(&buf), sizeof(buf));
	return buf;
}
//---------------------------------------------------------------------------















//---------------------------------------------------------------------------
} // namespace Risse


