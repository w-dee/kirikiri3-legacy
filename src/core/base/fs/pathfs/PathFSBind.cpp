//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief PathFSのTJS3バインディング
//---------------------------------------------------------------------------
#include "prec.h"
#include "PathFS.h"
#include "PathFSBind.h"
#include "TVPException.h"

TJS_DEFINE_SOURCE_ID(2013);



//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTJSNI_PathFS::tTJSNI_PathFS()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		TJS コンストラクタ
//---------------------------------------------------------------------------
tjs_error tTJSNI_PathFS::Construct(tjs_int numparams,
		tTJSVariant **param, iTJSDispatch2 *tjs_obj)
{
	/*%
		@fn		PathFS.PathFS
		@brief	PathFSを構築する
	*/

	// filesystem オブジェクトの生成と登録
	RegisterFileSystemNativeInstance(tjs_obj, new tTVPPathFS());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		TJS 無効化関数
//---------------------------------------------------------------------------
void tTJSNI_PathFS::Invalidate()
{
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		PathFS クラスID
//---------------------------------------------------------------------------
tjs_uint32 tTJSNC_PathFS::ClassID = (tjs_uint32)-1;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTJSNC_PathFS::tTJSNC_PathFS() :
	tTJSNativeClass(TJS_WS("PathFS"))
{
	// class constructor

	TJS_BEGIN_NATIVE_MEMBERS(/*TJS class name*/PathFS)
	TJS_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var. name*/_this, /*var. type*/tTJSNI_PathFS,
	/*TJS class name*/ PathFS)
{
	return TJS_S_OK;
}
TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/PathFS)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/add)
{
	/*%
		@brief	パスにディレクトリを追加する
		@param	dir					追加したいディレクトリ
		@param	recursive=false		再帰的にディレクトリを検索するか
		@note
		<p>
			パスにディレクトリを追加します。
			後から追加したディレクトリほど優先されて検索されます。
		</p>
		<p>
			\a recursive が真の場合は、\a dir で指定されたディレクトリ以下
			が再帰的に検索されて、それらのファイルがすべて自動検索パスに
			追加されます。
		</p>
		<p>
			\a recursive が偽の場合は、\a dir で指定されたディレクトリ直下
			のファイルのみが自動検索パスに追加されます。
		</p>
		<p>
			\a dir には、自分自身を含むディレクトリを指定しないでください。
		</p>
	*/

	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_PathFS);

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;
	ttstr dir = *param[0];
	bool recursive = TJS_PARAM_EXIST(1) ? (tjs_int)*param[1] : false;

	_this->GetFileSystem()->Add(dir, recursive);

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/add)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/remove)
{
	/*%
		@brief	パスからディレクトリを削除する
		@param	dir					削除したいディレクトリ
		@note
	*/

	TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_PathFS);

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;
	ttstr dir = *param[0];

	_this->GetFileSystem()->Remove(dir);

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/remove)
//----------------------------------------------------------------------
	TJS_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ネイティブインスタンスを作成して返す
//! @return		ネイティブインスタンス
//---------------------------------------------------------------------------
tTJSNativeInstance *tTJSNC_PathFS::CreateNativeInstance()
{
	return new tTJSNI_PathFS();
}
//---------------------------------------------------------------------------


