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
#include "prec.h"
#include "risa/packages/risa/file/FSManager.h"
#include "risa/packages/risa/file/fs/osfs/OSFS.h"
#include "base/exception/RisaException.h"
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
tFileSystemClass::tFileSystemClass(tScriptEngine * engine) :
	tClassBase(tSS<'F','i','l','e','S','y','s','t','e','m'>(), engine->ObjectClass)
{
	RegisterMembers();

	// FileOpenModeConsts を include する
	Do(ocFuncCall, NULL, ss_include, 0,
		tMethodArgument::New(
			tRisseModuleRegisterer<tFileOpenModeConstsModule>::instance()->GetModuleInstance()));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tFileSystemClass::RegisterMembers()
{
	// 親クラスの RegisterMembers を呼ぶ
	inherited::RegisterMembers();

	// クラスに必要なメソッドを登録する
	// 基本的に ss_construct と ss_initialize は各クラスごとに
	// 記述すること。たとえ construct の中身が空、あるいは initialize の
	// 中身が親クラスを呼び出すだけだとしても、記述すること。

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
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tVariant tFileSystemClass::ovulate()
{
	// このクラスのインスタンスは作成できないので例外を投げる
	tInstantiationExceptionClass::ThrowCannotCreateInstanceFromThisClass();
	return tVariant();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		FileSystem クラスレジストラ
template class tRisseClassRegisterer<tFileSystemClass>;
//---------------------------------------------------------------------------


















//---------------------------------------------------------------------------
} // namespace Risa

