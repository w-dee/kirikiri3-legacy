//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief ファイルシステムマネージャのTJS3バインディング
//---------------------------------------------------------------------------
#include "prec.h"
#include "FSManager.h"
#include "FSManagerBind.h"
#include "TVPException.h"

TJS_DEFINE_SOURCE_ID(2011);






//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//! @param		filesystem  ファイルシステムオブジェクト
//! @param		owner       このインスタンスを保持している TJS オブジェクト
//---------------------------------------------------------------------------
tTJSNI_FileSystemNativeInstance::tTJSNI_FileSystemNativeInstance(
					boost::shared_ptr<tTVPFileSystem> filesystem,
					iTJSDispatch2 * owner) :
						FileSystem(filesystem),
						Owner(owner)
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		TJS 無効化関数
//---------------------------------------------------------------------------
void tTJSNI_FileSystemNativeInstance::Invalidate()
{
	// ファイルシステムマネージャからこのファイルシステムをアンマウントする
	tTVPFileSystemManager::instance().Unmount(Owner);

	// FileSystem にこれ以上アクセスできないようにreset
	FileSystem.reset();

	// Owner にも NULL を代入しておく
	Owner = NULL;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTJSNI_BaseFileSystem::tTJSNI_BaseFileSystem()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		TJS 無効化関数
//---------------------------------------------------------------------------
void tTJSNI_BaseFileSystem::Invalidate()
{
	// FileSystem にこれ以上アクセスできないようにreset
	FileSystem.reset();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		tTJSNI_FileSystemNativeInstance をファイルシステムから構築し、
//!             オブジェクトに登録する
//! @param		tjs_obj		登録先TJSオブジェクト
//! @param		fs_obj		ファイルシステムオブジェクト
//---------------------------------------------------------------------------
void tTJSNI_BaseFileSystem::RegisterFileSystemNativeInstance(
		iTJSDispatch2 * tjs_obj,
		tTVPFileSystem * fs_obj)
{
	boost::shared_ptr<tTVPFileSystem> filesystem(fs_obj);
	FileSystem = filesystem;

	// tTJSNI_FileSystemNativeInstance オブジェクトの生成
	tTJSNI_FileSystemNativeInstance *ni =
		new tTJSNI_FileSystemNativeInstance(filesystem, tjs_obj);
	try
	{
		// tTJSNI_FileSystemNativeInstance オブジェクトを tjs_obj に登録
		tjs_obj->NativeInstanceSupport(TJS_NIS_REGISTER,
			tTJSNI_FileSystemNativeInstance::ClassID,
				(iTJSNativeInstance**)&ni);
	}
	catch(...)
	{
		ni->Destruct();
		throw;
	}
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief   FileSystemクラスID
//---------------------------------------------------------------------------
tjs_uint32 tTJSNC_FileSystem::ClassID = (tjs_uint32)-1;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief   コンストラクタ
//---------------------------------------------------------------------------
tTJSNC_FileSystem::tTJSNC_FileSystem() :
	tTJSNativeClass(TJS_WS("FileSystem"))
{
	// class constructor

	TJS_BEGIN_NATIVE_MEMBERS(/*TJS class name*/FileSystem)
	TJS_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL_NO_INSTANCE(/*TJS class name*/ FileSystem)
{
	// このクラスはオブジェクトを持たないので (static class) 
	// ここでは何もしない
	return TJS_S_OK;
}
TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/FileSystem)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/chopExt)
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

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;
	if(result)
		*result = tTVPFileSystemManager::instance().ChopExtension(*param[0]);
	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/chopExt)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/extractExt)
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

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;
	if(result)
		*result = tTVPFileSystemManager::instance().ExtractExtension(*param[0]);
	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/extractExt)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/extractName)
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

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;
	if(result)
		*result = tTVPFileSystemManager::instance().ExtractName(*param[0]);
	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/extractName)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/extractPath)
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

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;
	if(result)
		*result = tTVPFileSystemManager::instance().ExtractPath(*param[0]);
	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/extractPath)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/getFullPath)
{
	/*%
		@brief	フルパスを得る
		@param	fn		ファイル名
		@return	フルパスが返ります。
		@note
		<p>
			フルパスを得ます。ファイル名が相対パスだった場合 (先頭が
			'/' で始まってない場合) にはカレントディレクトリが補完されます。
			<tt>..</tt> (一つ上のディレクトリを表す) などがパス名中に含まれている
			場合は圧縮されます。
		</p>
	*/

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;
	if(result)
		*result = tTVPFileSystemManager::instance().GetFullPath(*param[0]);
	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/getFullPath)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/mount)
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
			new で生成されたファイルシステムを表す TJS オブジェクトでなければ
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

	if(numparams < 2) return TJS_E_BADPARAMCOUNT;

	ttstr mountpoint = *param[0];
	iTJSDispatch2 * dsp = param[1]->AsObjectNoAddRef();
	tTVPFileSystemManager::instance().Mount(mountpoint, dsp);

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/mount)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/unmount)
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
	if(numparams < 1) return TJS_E_BADPARAMCOUNT;

	ttstr mountpoint = *param[0];

	tTVPFileSystemManager::instance().Unmount(mountpoint);

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/unmount)
//----------------------------------------------------------------------
	TJS_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------
