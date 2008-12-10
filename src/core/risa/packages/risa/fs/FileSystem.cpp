//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ファイルシステムマネージャ(ファイルシステムの根幹部分)
//---------------------------------------------------------------------------
#include "risa/prec.h"
#include "risa/packages/risa/fs/FSManager.h"
#include "risa/packages/risa/fs/osfs/OSFS.h"
#include "risa/common/RisaException.h"
#include "risse/include/builtin/stream/risseStreamClass.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(57835,14019,1274,20023,25994,43742,64617,60148);
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
void tFileSystemInstance::construct()
{
	// デフォルトではなにもしない
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemInstance::initialize(const tNativeCallInfo &info)
{
	volatile tSynchronizer sync(this); // sync

	// 親クラスの同名メソッドを呼び出す
	info.InitializeSuperClass();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tFileSystemInstance::walkAt(const tString & dirname,
	const tMethodArgument &args)
{
	// 実装されていない; 下位の Risse クラスで実装すること
	tUnsupportedOperationExceptionClass::ThrowOperationIsNotImplemented();
	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tFileSystemInstance::isFile(const tString & filename)
{
	// 実装されていない; 下位の Risse クラスで実装すること
	tUnsupportedOperationExceptionClass::ThrowOperationIsNotImplemented();
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tFileSystemInstance::isDirectory(const tString & dirname)
{
	// 実装されていない; 下位の Risse クラスで実装すること
	tUnsupportedOperationExceptionClass::ThrowOperationIsNotImplemented();
	return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemInstance::removeFile(const tString & filename)
{
	// 実装されていない; 下位の Risse クラスで実装すること
	tUnsupportedOperationExceptionClass::ThrowOperationIsNotImplemented();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemInstance::removeDirectory(const tString & dirname,
	const tMethodArgument &args)
{
	// 実装されていない; 下位の Risse クラスで実装すること
	tUnsupportedOperationExceptionClass::ThrowOperationIsNotImplemented();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemInstance::createDirectory(const tString & dirname)
{
	// 実装されていない; 下位の Risse クラスで実装すること
	tUnsupportedOperationExceptionClass::ThrowOperationIsNotImplemented();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tObjectInterface * tFileSystemInstance::stat(const tString & filename)
{
	// 実装されていない; 下位の Risse クラスで実装すること
	tUnsupportedOperationExceptionClass::ThrowOperationIsNotImplemented();
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tStreamInstance * tFileSystemInstance::open(const tString & filename,
	risse_uint32 flags)
{
	// 実装されていない; 下位の Risse クラスで実装すること
	tUnsupportedOperationExceptionClass::ThrowOperationIsNotImplemented();
	return NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemInstance::flush()
{
	// 標準の flush メソッドは何もしない
}
//---------------------------------------------------------------------------














//---------------------------------------------------------------------------
RISSE_IMPL_CLASS_BEGIN(tFileSystemClass,
		(tSS<'F','i','l','e','S','y','s','t','e','m'>()), engine->ObjectClass,
		ThrowCannotCreateInstanceFromThisClass())
	BindFunction(this, ss_ovulate, &tFileSystemClass::ovulate);
	BindFunction(this, ss_construct, &tFileSystemInstance::construct);
	BindFunction(this, ss_initialize, &tFileSystemInstance::initialize);

	BindFunction(this, tSS<'w','a','l','k','A','t'>(), &tFileSystemInstance::walkAt);
	BindFunction(this, tSS<'i','s','F','i','l','e'>(), &tFileSystemInstance::isFile);
	BindFunction(this, tSS<'i','s','D','i','r','e','c','t','o','r','y'>(), &tFileSystemInstance::isDirectory);
	BindFunction(this, tSS<'r','e','m','o','v','e','F','i','l','e'>(), &tFileSystemInstance::removeFile);
	BindFunction(this, tSS<'r','e','m','o','v','e','D','i','r','e','c','t','o','r','y'>(), &tFileSystemInstance::removeDirectory);
	BindFunction(this, tSS<'c','r','e','a','t','e','D','i','r','e','c','t','o','r','y'>(), &tFileSystemInstance::createDirectory);
	BindFunction(this, tSS<'s','t','a','t'>(), &tFileSystemInstance::stat);
	BindFunction(this, tSS<'o','p','e','n'>(), &tFileSystemInstance::open);
	BindFunction(this, tSS<'f','l','u','s','h'>(), &tFileSystemInstance::flush);
RISSE_IMPL_CLASS_END()
//---------------------------------------------------------------------------




















//---------------------------------------------------------------------------
} // namespace Risa

