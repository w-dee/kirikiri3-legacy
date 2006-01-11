//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OSFSのTJS3バインディング
//---------------------------------------------------------------------------
#include "prec.h"
#include "OSFS.h"
#include "OSFSBind.h"
#include "TVPException.h"

TJS_DEFINE_SOURCE_ID(2012);



//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTJSNI_OSFS::tTJSNI_OSFS()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		TJS コンストラクタ
//---------------------------------------------------------------------------
tjs_error tTJSNI_OSFS::Construct(tjs_int numparams,
		tTJSVariant **param, iTJSDispatch2 *tjs_obj)
{
	/*%
		@fn		OSFS.OSFS
		@brief	OSFSを構築する
		@param	basedir			このファイルシステムが参照するOS上のディレクトリ
		@param	checkcase		大文字小文字が区別されないファイルシステム上で
								大文字小文字を区別するか
	*/

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;

	ttstr basedir = *param[0];
	bool checkcase = TJS_PARAM_EXIST(1) ? (tjs_int)*param[1] : true;

	// filesystem オブジェクトの生成と登録
	RegisterFileSystemNativeInstance(tjs_obj, new tTVPOSFS(basedir, checkcase));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		TJS 無効化関数
//---------------------------------------------------------------------------
void tTJSNI_OSFS::Invalidate()
{
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		OSFS クラスID
//---------------------------------------------------------------------------
tjs_uint32 tTJSNC_OSFS::ClassID = (tjs_uint32)-1;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTJSNC_OSFS::tTJSNC_OSFS() :
	tTJSNativeClass(TJS_WS("OSFS"))
{
	// class constructor

	TJS_BEGIN_NATIVE_MEMBERS(/*TJS class name*/OSFS)
	TJS_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var. name*/_this, /*var. type*/tTJSNI_OSFS,
	/*TJS class name*/ OSFS)
{
	return TJS_S_OK;
}
TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/OSFS)
//----------------------------------------------------------------------
	TJS_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ネイティブインスタンスを作成して返す
//! @return		ネイティブインスタンス
//---------------------------------------------------------------------------
tTJSNativeInstance *tTJSNC_OSFS::CreateNativeInstance()
{
	return new tTJSNI_OSFS();
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTVPOSFSRegisterer::tTVPOSFSRegisterer()
{
	// ファイルシステムクラスオブジェクトの下にクラスオブジェクトを登録する
	iTJSDispatch2 * cls = new tTJSNC_OSFS();
	try
	{
		tTVPFileSystemRegisterer::instance()->RegisterClassObject(
							TJS_WS("OSFS"), cls);
	}
	catch(...)
	{
		cls->Release();
		throw;
	}
	cls->Release();
}
//---------------------------------------------------------------------------


