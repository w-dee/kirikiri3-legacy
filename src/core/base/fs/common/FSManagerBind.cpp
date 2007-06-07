//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ファイルシステムマネージャのRisseバインディング
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/fs/common/FSManager.h"
#include "base/fs/common/FSManagerBind.h"
#include "base/exception/RisaException.h"
#include "base/script/RisseEngine.h"

RISSE_DEFINE_SOURCE_ID(57103,491,17401,17369,5283,30429,30396,3005);


#if 0



//---------------------------------------------------------------------------
tRisseNI_FileSystemNativeInstance::tRisseNI_FileSystemNativeInstance(
					boost::shared_ptr<tRisaFileSystem> filesystem,
					iRisseDispatch2 * owner) :
						FileSystem(filesystem),
						Owner(owner)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseNI_FileSystemNativeInstance::Invalidate()
{
	// ファイルシステムマネージャからこのファイルシステムをアンマウントする
	// ただしファイルシステムマネージャがシャットダウン中にこの関数が
	// 呼ばれるときは、すでにアンマウントされることが決定しているので
	// 呼ばない。
	if(tRisaFileSystemManager::pointer r = tRisaFileSystemManager::instance())
		r->Unmount(Owner);

	// FileSystem にこれ以上アクセスできないようにreset
	FileSystem.reset();

	// Owner にも NULL を代入しておく
	Owner = NULL;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
tRisseNI_BaseFileSystem::tRisseNI_BaseFileSystem()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseNI_BaseFileSystem::Invalidate()
{
	// FileSystem にこれ以上アクセスできないようにreset
	FileSystem.reset();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisseNI_BaseFileSystem::RegisterFileSystemNativeInstance(
		iRisseDispatch2 * risse_obj,
		tRisaFileSystem * fs_obj)
{
	boost::shared_ptr<tRisaFileSystem> filesystem(fs_obj);
	FileSystem = filesystem;

	// tRisseNI_FileSystemNativeInstance オブジェクトの生成
	tRisseNI_FileSystemNativeInstance *ni =
		new tRisseNI_FileSystemNativeInstance(filesystem, risse_obj);
	try
	{
		// tRisseNI_FileSystemNativeInstance オブジェクトを risse_obj に登録
		risse_obj->NativeInstanceSupport(RISSE_NIS_REGISTER,
			tRisseNI_FileSystemNativeInstance::ClassID,
				(iRisseNativeInstance**)&ni);
	}
	catch(...)
	{
		ni->Destruct();
		throw;
	}
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
risse_uint32 tRisseNC_FileSystem::ClassID = (risse_uint32)-1;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisseNC_FileSystem::tRisseNC_FileSystem() :
	tRisseNativeClass(RISSE_WS("FileSystem"))
{
	// class constructor

	RISSE_BEGIN_NATIVE_MEMBERS(/*Risse class name*/FileSystem)
	RISSE_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_CONSTRUCTOR_DECL_NO_INSTANCE(/*Risse class name*/ FileSystem)
{
	// このクラスはオブジェクトを持たないので (static class) 
	// ここでは何もしない
	return RISSE_S_OK;
}
RISSE_END_NATIVE_CONSTRUCTOR_DECL(/*Risse class name*/FileSystem)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/chopExt)
{
	/*%
		@brief	ファイル名から拡張子を取り去る
		@param	fn			ファイル名
		@return	拡張子が取り去られたファイル名が返ります。
				返されるファイル名の最後に . (ドット) は残りません。
		@note
		<p>
			たとえば <tt>/data/path/test.ogg</tt> ならば
			<tt>/data/path.test</tt> が返ります。
		</p>
	*/

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;
	if(result)
		*result = tRisaFileSystemManager::instance()->ChopExtension(*param[0]);
	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/chopExt)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/extractExt)
{
	/*%
		@brief	ファイル名から拡張子を取り出す
		@param	fn		ファイル名
		@return	拡張子が返ります。返される拡張子の最初に . (ドット)が含まれます。
				例外として、ファイル名が拡張子を持たない場合は空文字列が返ります。
		@note
		<p>
			たとえば <tt>/data/path/test.ogg</tt> ならば
			<tt>.ogg</tt> が返ります。
		</p>
		<p>
			<tt>/data/path/test</tt> ならば空文字列が返ります。
		</p>
	*/

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;
	if(result)
		*result = tRisaFileSystemManager::instance()->ExtractExtension(*param[0]);
	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/extractExt)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/extractName)
{
	/*%
		@brief	ファイル名からパス名を除いた部分を取り出す
		@param	fn		ファイル名
		@return	パス名が除かれたファイル名が返ります。
		@note
		<p>
			たとえば <tt>/data/path/test.ogg</tt> ならば
			<tt>test.ogg</tt> が返ります。
		</p>
	*/

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;
	if(result)
		*result = tRisaFileSystemManager::instance()->ExtractName(*param[0]);
	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/extractName)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/extractPath)
{
	/*%
		@brief	ファイル名からパス名を取り出す
		@param	fn		ファイル名
		@return	パス名が返ります。
		@note
		<p>
			たとえば <tt>/data/path/test.ogg</tt> ならば
			<tt>/data/path</tt> が返ります。
		</p>
	*/

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;
	if(result)
		*result = tRisaFileSystemManager::instance()->ExtractPath(*param[0]);
	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/extractPath)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/getFullPath)
{
	/*%
		@brief	フルパスを得る
		@param	fn		ファイル名
		@return	フルパスが返ります。
		@note
		<p>
			フルパスを得ます。ファイル名が相対パスだった場合 (先頭が
			'/' で始まってない場合) には <code>FileSystem.cwd</code> で
			指定されたカレントディレクトリが補完されます。
			<tt>..</tt> (一つ上のディレクトリを表す) などがパス名中に含まれている
			場合は圧縮されます。
		</p>
	*/

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;
	if(result)
		*result = tRisaFileSystemManager::instance()->GetFullPath(*param[0]);
	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/getFullPath)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/mount)
{
	/*%
		@brief	ファイルシステムをマウントする
		@param	mountpoint		マウントポイント
		@param	filesystem		ファイルシステムオブジェクト
		@note
		<p>
			ファイルシステムを、指定されたマウントポイントにマウントします。
			マウントポイントは、ファイルシステムをマウントしたい、マウント先の
			ディレクトリ名です。
		</p>
		<p>
			ファイルシステムオブジェクトは <code>new FileSystem.OSFS("data")</code> のように
			new で生成されたファイルシステムを表す Risse オブジェクトでなければ
			なりません。
		</p>
		<p>
			一度ファイルシステムをマウントポイントにマウントすると、その
			マウントポイント以下のディレクトリへのアクセスはすべてその
			ファイルシステムに対するアクセスであると見なされます。たとえば
			マウントポイント <tt>"/data"</tt> に
			<code>new FileSystem.OSFS("data")</code> を
			マウントすると、<tt>/data/file.txt</tt> のようなアクセスはすべて OSFS の
			<tt>"data"</tt> ディレクトリへのアクセスとなります。
		</p>
	*/

	if(numparams < 2) return RISSE_E_BADPARAMCOUNT;

	tRisseString mountpoint = *param[0];
	iRisseDispatch2 * dsp = param[1]->AsObjectNoAddRef();
	tRisaFileSystemManager::instance()->Mount(mountpoint, dsp);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/mount)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/unmount)
{
	/*%
		@brief	ファイルシステムをアンマウントする
		@param	mountpoint		マウントポイント
		@note
		<p>
			指定されたマウントポイントにマウントされているファイルシステムの
			マウントを解除(アンマウント)します。
		</p>
	*/
	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	tRisseString mountpoint = *param[0];

	tRisaFileSystemManager::instance()->Unmount(mountpoint);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/unmount)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_PROP_DECL(cwd)
{
	/*%
		@brief	作業ディレクトリ
		@note
		<p>
			現在の作業ディレクトリを表します。
			値を設定することもできます。デフォルトは "/" (ルート) になっています。
		</p>
		<p>
			値を設定すると現在の作業ディレクトリを変更できます。ただし、このプロパティを
			設定する時点ではそのディレクトリが実際に存在しているかどうかの
			チェックは行いません。
		</p>
	*/
	RISSE_BEGIN_NATIVE_PROP_GETTER
	{
		if(result) *result = tRisaFileSystemManager::instance()->GetCurrentDirectory();
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_GETTER

	RISSE_BEGIN_NATIVE_PROP_SETTER
	{
		tRisaFileSystemManager::instance()->SetCurrentDirectory(*param);
		return RISSE_S_OK;
	}
	RISSE_END_NATIVE_PROP_SETTER
}
RISSE_END_NATIVE_PROP_DECL(cwd)
//----------------------------------------------------------------------
	RISSE_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
tRisaFileSystemRegisterer::tRisaFileSystemRegisterer()
{
	FileSystemClass = new tRisseNC_FileSystem();
	try
	{
		depends_on<tRisaRisseScriptEngine>::locked_instance()->RegisterGlobalObject(RISSE_WS("FileSystem"), FileSystemClass);
	}
	catch(...)
	{
		FileSystemClass->Release();
		throw;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaFileSystemRegisterer::~tRisaFileSystemRegisterer()
{
	FileSystemClass->Release();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaFileSystemRegisterer::RegisterClassObject(const risse_char *name,
											iRisseDispatch2 * object)
{
	tRisseVariant val(object, NULL);
	risse_error er;
	er = FileSystemClass->PropSet(RISSE_MEMBERENSURE, name, NULL, &val, FileSystemClass);
	if(RISSE_FAILED(er))
		RisseThrowFrom_risse_error(er, name);
}
//---------------------------------------------------------------------------


#endif
