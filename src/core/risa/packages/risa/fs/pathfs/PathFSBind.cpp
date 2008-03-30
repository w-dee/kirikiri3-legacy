//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2008 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief PathFSのRisseバインディング
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/fs/pathfs/PathFS.h"
#include "base/fs/pathfs/PathFSBind.h"
#include "base/exception/RisaException.h"



#if 0
namespace Risa {
RISSE_DEFINE_SOURCE_ID(7556,48904,60264,18196,46767,10992,26980,15305);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tNI_PathFS::tNI_PathFS()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_error tNI_PathFS::Construct(risse_int numparams,
		tVariant **param, iRisseDispatch2 *risse_obj)
{
	/*%
		@fn		PathFS.PathFS
		@brief	PathFSを構築する
	*/

	// filesystem オブジェクトの生成と登録
	RegisterFileSystemNativeInstance(risse_obj, new tPathFS());

	return RISSE_S_OK;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNI_PathFS::Invalidate()
{
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
risse_uint32 tNC_PathFS::ClassID = (risse_uint32)-1;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tNC_PathFS::tNC_PathFS() :
	tNativeClass(RISSE_WS("PathFS"))
{
	// class constructor

	RISSE_BEGIN_NATIVE_MEMBERS(/*Risse class name*/PathFS)
	RISSE_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var. name*/_this, /*var. type*/tNI_PathFS,
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

	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tNI_PathFS);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;
	tString dir = *param[0];
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

	RISSE_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tNI_PathFS);

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;
	tString dir = *param[0];

	_this->GetFileSystem()->Remove(dir);

	return RISSE_S_OK;
}
RISSE_END_NATIVE_METHOD_DECL(/*func. name*/remove)
//----------------------------------------------------------------------
	RISSE_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tNativeInstance *tNC_PathFS::CreateNativeInstance()
{
	return new tNI_PathFS();
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
tPathFSRegisterer::tPathFSRegisterer()
{
	// ファイルシステムクラスオブジェクトの下にクラスオブジェクトを登録する
	iRisseDispatch2 * cls = new tNC_PathFS();
	try
	{
		depends_on<tFileSystemRegisterer>::locked_instance()->RegisterClassObject(
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

//---------------------------------------------------------------------------
} // namespace Risa

#endif


