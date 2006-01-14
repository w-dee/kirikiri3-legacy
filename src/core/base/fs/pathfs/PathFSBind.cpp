//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief PathFSのRisseバインディング
//---------------------------------------------------------------------------
#include "prec.h"
#include "PathFS.h"
#include "PathFSBind.h"
#include "TVPException.h"

RISSE_DEFINE_SOURCE_ID(2013);



//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisseNI_PathFS::tRisseNI_PathFS()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Risse コンストラクタ
//---------------------------------------------------------------------------
risse_error tRisseNI_PathFS::Construct(risse_int numparams,
		tRisseVariant **param, iRisseDispatch2 *risse_obj)
{
	/*%
		@fn		PathFS.PathFS
		@brief	PathFSを構築する
	*/

	// filesystem オブジェクトの生成と登録
	RegisterFileSystemNativeInstance(risse_obj, new tTVPPathFS());
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Risse 無効化関数
//---------------------------------------------------------------------------
void tRisseNI_PathFS::Invalidate()
{
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		PathFS クラスID
//---------------------------------------------------------------------------
risse_uint32 tRisseNC_PathFS::ClassID = (risse_uint32)-1;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisseNC_PathFS::tRisseNC_PathFS() :
	tRisseNativeClass(RISSE_WS("PathFS"))
{
	// class constructor

	RISSE_BEGIN_NATIVE_MEMBERS(/*Risse class name*/PathFS)
	RISSE_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var. name*/_this, /*var. type*/tRisseNI_PathFS,
	/*Risse class name*/ PathFS)
{
	return RISSE_S_OK;
}
RISSE_END_NATIVE_CONSTRUCTOR_DECL(/*Risse class name*/PathFS)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/add)
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

	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_PathFS);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;
	ttstr dir = *param[0];
	bool recursive = RISSE_PARAM_EXIST(1) ? (risse_int)*param[1] : false;

	_this->GetFileSystem()->Add(dir, recursive);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/add)
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_METHOD_DECL(/*func. name*/remove)
{
	/*%
		@brief	パスからディレクトリを削除する
		@param	dir					削除したいディレクトリ
		@note
	*/

	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tRisseNI_PathFS);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;
	ttstr dir = *param[0];

	_this->GetFileSystem()->Remove(dir);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/remove)
//----------------------------------------------------------------------
	RISSE_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ネイティブインスタンスを作成して返す
//! @return		ネイティブインスタンス
//---------------------------------------------------------------------------
tRisseNativeInstance *tRisseNC_PathFS::CreateNativeInstance()
{
	return new tRisseNI_PathFS();
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tTVPPathFSRegisterer::tTVPPathFSRegisterer()
{
	// ファイルシステムクラスオブジェクトの下にクラスオブジェクトを登録する
	iRisseDispatch2 * cls = new tRisseNC_PathFS();
	try
	{
		tTVPFileSystemRegisterer::instance()->RegisterClassObject(
							RISSE_WS("PathFS"), cls);
	}
	catch(...)
	{
		cls->Release();
		throw;
	}
	cls->Release();
}
//---------------------------------------------------------------------------



