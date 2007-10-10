//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief スタンダードIOとの入出力を行うクラス
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/exception/RisaException.h"
#include "risse/include/risseStreamClass.h"
#include "risse/include/risseExceptionClass.h"
#include "base/system/StandardIO.h"

namespace Risa {
RISSE_DEFINE_SOURCE_ID(36650,50749,64532,16674,7599,41607,50003,48070);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
tStandardIOStreamInstance::tStandardIOStreamInstance()
{
	Stream = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStandardIOStreamInstance::initialize(int index, const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	info.InitializeSuperClass(); // スーパークラスのコンストラクタを呼ぶ

	switch(index)
	{
	case 0:		Stream = stdin;		break;
	case 1:		Stream = stdout;	break;
	case 2:		Stream = stderr;	break;
	default:
		tIllegalArgumentExceptionClass::Throw(RISSE_WS_TR("stream index must be 0, 1 or 2"));
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStandardIOStreamInstance::dispose()
{
	volatile tSynchronizer sync(this); // sync

	Stream = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tStandardIOStreamInstance::get(const tOctet & buf)
{
	volatile tSynchronizer sync(this); // sync

	if(!Stream) tInaccessibleResourceExceptionClass::Throw();

	return fread(const_cast<risse_uint8 *>(buf.Pointer()), 1, buf.GetLength(), Stream);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_size tStandardIOStreamInstance::put(const tOctet & buf)
{
	volatile tSynchronizer sync(this); // sync

	if(!Stream) tInaccessibleResourceExceptionClass::Throw();

	return fwrite(buf.Pointer(), 1, buf.GetLength(), Stream);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStandardIOStreamInstance::flush()
{
	volatile tSynchronizer sync(this); // sync

	if(!Stream) tInaccessibleResourceExceptionClass::Throw();

	fflush(Stream);
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tStandardIOStreamClass::tStandardIOStreamClass(tScriptEngine * engine) :
	tClassBase(tSS<'S','t','a','n','d','a','r','d','I','O','S','t','r','e','a','m'>(), engine->StreamClass)
{
	RegisterMembers();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tStandardIOStreamClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

	BindFunction(this, ss_ovulate, &tStandardIOStreamClass::ovulate);
	BindFunction(this, ss_construct, &tStandardIOStreamInstance::construct);
	BindFunction(this, ss_initialize, &tStandardIOStreamInstance::initialize);
	BindFunction(this, ss_get, &tStandardIOStreamInstance::get);
	BindFunction(this, ss_put, &tStandardIOStreamInstance::put);
	BindFunction(this, ss_flush, &tStandardIOStreamInstance::flush);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tStandardIOStreamClass::ovulate()
{
	return tVariant(new tStandardIOStreamInstance());
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		System クラスレジストラ
template class tRisseClassRegisterer<tStandardIOStreamClass>;
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
} // namespace Risa



