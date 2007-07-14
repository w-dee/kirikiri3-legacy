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

#include "risseBinaryStream.h"
#include "risseExceptionClass.h"

RISSE_DEFINE_SOURCE_ID(1709,46737,4438,20346,9893,20339,39302,25230);


namespace Risse
{
//---------------------------------------------------------------------------
void tBinaryStream::Truncate()
{
	tIOExceptionClass::ThrowTruncateError(Name, GetPosition());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tBinaryStream::GetSize()
{
	risse_uint64 orgpos = GetPosition();
	Seek(0, soEnd);
	risse_uint64 size = Tell();
	SetPosition(orgpos);
	return size;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tBinaryStream::SetPosition(risse_uint64 pos)
{
	if(!Seek(pos, soSet))
		tIOExceptionClass::ThrowSeekError(Name, pos);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tBinaryStream::ReadBuffer(void *buffer, risse_size read_size)
{
	risse_size actual_read = Read(buffer, read_size);
	if(actual_read != read_size)
		tIOExceptionClass::ThrowReadError(Name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tBinaryStream::WriteBuffer(const void *buffer, risse_size write_size)
{
	risse_size actual_written = Write(buffer, write_size);
	if(actual_written != write_size)
		tIOExceptionClass::ThrowWriteError(Name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tBinaryStream::ReadI64LE()
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
risse_uint32 tBinaryStream::ReadI32LE()
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
risse_uint16 tBinaryStream::ReadI16LE()
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
risse_uint8 tBinaryStream::ReadI8LE()
{
	risse_uint8 buf;
	ReadBuffer(reinterpret_cast<void*>(&buf), sizeof(buf));
	return buf;
}
//---------------------------------------------------------------------------















//---------------------------------------------------------------------------
} // namespace Risse


