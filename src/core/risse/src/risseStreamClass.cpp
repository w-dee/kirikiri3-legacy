//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Stream" クラスの実装
//---------------------------------------------------------------------------
#include "prec.h"

#include "risseStream.h"
#include "risseStreamClass.h"
#include "risseExceptionClass.h"
#include "risseStaticStrings.h"
#include "risseScriptEngine.h"
#include "risseObjectClass.h"

RISSE_DEFINE_SOURCE_ID(42888,25529,18022,19240,53128,12574,40093,31909);


namespace Risse
{
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
tStreamConstsModule::tStreamConstsModule(tScriptEngine * engine) :
	tModuleBase(ss_StreamConsts, engine)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStreamConstsModule::RegisterMembers()
{
	GetInstance()->RegisterFinalConstMember(ss_soSet, tVariant((risse_int64)tStreamInstance::soSet), true);
	GetInstance()->RegisterFinalConstMember(ss_soCur, tVariant((risse_int64)tStreamInstance::soCur), true);
	GetInstance()->RegisterFinalConstMember(ss_soEnd, tVariant((risse_int64)tStreamInstance::soEnd), true);
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
void tStreamInstance::truncate()
{
	tIOExceptionClass::ThrowTruncateError(Name, tStreamAdapter(this).GetPosition());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tStreamInstance::get_size()
{
	risse_uint64 orgpos = tStreamAdapter(this).GetPosition();
	tStreamAdapter(this).Seek(0, soEnd);
	risse_uint64 size = tStreamAdapter(this).Tell();
	tStreamAdapter(this).SetPosition(orgpos);
	return size;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStreamInstance::flush()
{
	// デフォルトでは何もしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_uint64 tStreamInstance::get_position()
{
	return tStreamAdapter(this).Tell();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStreamInstance::set_position(risse_uint64 pos)
{
	if(!tStreamAdapter(this).Seek(pos, soSet))
		tIOExceptionClass::ThrowSeekError(Name, pos);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOctet tStreamInstance::read(risse_size size)
{
	tOctet buf;
	buf.Allocate(size);
	tVariant buf_v(buf);
	risse_size read_bytes = (risse_size)(risse_int64)Invoke(ss_get, buf_v);
	buf.SetLength(read_bytes);
	return buf;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tStreamInstance::write(const tOctet & buf)
{
	return (risse_size)(risse_int64)Invoke(ss_put, tVariant(buf));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tOctet tStreamInstance::readBuffer(risse_size size)
{
	tOctet buf;
	buf.Allocate(size);
	tVariant buf_v(buf);
	risse_size read_bytes = (risse_size)(risse_int64)Invoke(ss_get, buf_v);
	if(read_bytes != size)
		tIOExceptionClass::ThrowReadError(Name);
	return buf;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStreamInstance::writeBuffer(const tOctet & buf)
{
	risse_size actual_written = (risse_int64)Invoke(ss_put, buf);
	if(actual_written != buf.GetLength())
		tIOExceptionClass::ThrowWriteError(Name);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStreamInstance::print(const tString & str)
{
	const risse_uint8 * ptr;
	risse_size size = 0;
	ptr = reinterpret_cast<const risse_uint8*>(str.AsNarrowString(&size));
	tStreamAdapter(this).WriteBuffer(ptr, size);
}
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
tStreamClass::tStreamClass(tScriptEngine * engine) :
	tClassBase(ss_Stream, engine->ObjectClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStreamClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tStreamClass::ovulate);
	BindFunction(this, ss_construct, &tStreamInstance::construct);
	BindFunction(this, ss_initialize, &tStreamInstance::initialize);
	BindFunction(this, ss_dispose, &tStreamInstance::dispose);
	BindProperty(this, ss_name, &tStreamInstance::get_name, &tStreamInstance::set_name);
	BindFunction(this, ss_seek, &tStreamInstance::seek);
	BindFunction(this, ss_tell, &tStreamInstance::tell);
	BindFunction(this, ss_read, &tStreamInstance::read);
	BindFunction(this, ss_write, &tStreamInstance::write);
	BindFunction(this, ss_get, &tStreamInstance::get);
	BindFunction(this, ss_put, &tStreamInstance::put);
	BindFunction(this, ss_truncate, &tStreamInstance::truncate);
	BindProperty(this, ss_size, &tStreamInstance::get_size);
	BindFunction(this, ss_flush, &tStreamInstance::flush);
	BindProperty(this, ss_position, &tStreamInstance::get_position, &tStreamInstance::set_position);
	BindFunction(this, ss_readBuffer, &tStreamInstance::readBuffer);
	BindFunction(this, ss_writeBuffer, &tStreamInstance::writeBuffer);
	BindFunction(this, ss_print, &tStreamInstance::print);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tStreamClass::ovulate()
{
	// このクラスのインスタンスは作成できないので例外を投げる
	tInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
	return tVariant();
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
} // namespace Risse

