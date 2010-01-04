//---------------------------------------------------------------------------
/*
	Risse [りせ]
	 stands for "Risse Is a Sweet Script Engine"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Risse用 "Stream" クラスの実装
//---------------------------------------------------------------------------
#include "../../prec.h"

#include "../../risseStream.h"
#include "risseStreamClass.h"
#include "../../risseExceptionClass.h"
#include "../../risseStaticStrings.h"
#include "../../risseScriptEngine.h"
#include "../../risseObjectClass.h"

RISSE_DEFINE_SOURCE_ID(42888,25529,18022,19240,53128,12574,40093,31909);


namespace Risse
{
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
tOctet tStreamInstance::read(const tMethodArgument &args)
{
	if(args.HasArgument(0))
	{
		// サイズが渡されているとき
		risse_size size = (risse_size)(risse_int64)args[0];
		tOctet buf;
		buf.Allocate(size);
		tVariant buf_v(buf);
		risse_size read_bytes = (risse_size)(risse_int64)Invoke(ss_get, buf_v);
		buf.SetLength(read_bytes);
		return buf;
	}
	else
	{
		// サイズが渡されていないとき
		// この場合は何回かに分けて読み込むしかない
		static const risse_size one_max_size = 64*1024; // 一度には64kbずつ
		tOctet buf;
		tOctet one_buf;
		for(;;)
		{
			if(one_buf.GetLength() != one_max_size)
				one_buf.Allocate(one_max_size);

			tVariant one_buf_v(one_buf);
			risse_size read_bytes = (risse_size)(risse_int64)Invoke(ss_get, one_buf_v);
			if(read_bytes == 0) break; // すでにすべて読み込んだ

			one_buf.SetLength(read_bytes);
			buf += one_buf;
		}
		return buf;
	}
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
RISSE_IMPL_CLASS_BEGIN(tStreamClass, ss_Stream, engine->ObjectClass)
	RISSE_BIND_CONSTRUCTORS
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
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------













//---------------------------------------------------------------------------
tStreamPackageInitializer::tStreamPackageInitializer() :
	tBuiltinPackageInitializer(ss_stream)
{
	StreamClass = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStreamPackageInitializer::Initialize(tScriptEngine * engine, const tString & name,
		const tVariant & global)
{
	StreamClass = new tStreamClass(engine);
	StreamClass->RegisterInstance(global);

	global.SetPropertyDirect(engine,
				ss_soSet,
				tOperateFlags::ofMemberEnsure|
					tOperateFlags::ofInstanceMemberOnly|
					tOperateFlags::ofUseClassMembersRule,
				tVariant((risse_int64)tStreamInstance::soSet));
	global.SetAttributeDirect(engine,
				ss_soSet,
				tMemberAttribute::GetDefault().Set(tMemberAttribute::mcConst));

	global.SetPropertyDirect(engine,
				ss_soCur,
				tOperateFlags::ofMemberEnsure|
					tOperateFlags::ofInstanceMemberOnly|
					tOperateFlags::ofUseClassMembersRule,
				tVariant((risse_int64)tStreamInstance::soCur));
	global.SetAttributeDirect(engine,
				ss_soCur,
				tMemberAttribute::GetDefault().Set(tMemberAttribute::mcConst));

	global.SetPropertyDirect(engine,
				ss_soEnd,
				tOperateFlags::ofMemberEnsure|
					tOperateFlags::ofInstanceMemberOnly|
					tOperateFlags::ofUseClassMembersRule,
				tVariant((risse_int64)tStreamInstance::soEnd));
	global.SetAttributeDirect(engine,
				ss_soEnd,
				tMemberAttribute::GetDefault().Set(tMemberAttribute::mcConst));
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
} // namespace Risse


