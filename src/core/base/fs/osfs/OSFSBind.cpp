//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OSFSのRisseバインディング
//---------------------------------------------------------------------------
#include "prec.h"
#include "base/fs/osfs/OSFS.h"
#include "base/fs/osfs/OSFSBind.h"
#include "base/exception/RisaException.h"


namespace Risa {
RISSE_DEFINE_SOURCE_ID(5731,26202,12507,18005,22157,59190,3112,30254);
//---------------------------------------------------------------------------

#if 0
//---------------------------------------------------------------------------
tNI_OSFS::tNI_OSFS()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
risse_error tNI_OSFS::Construct(risse_int numparams,
		tVariant **param, iRisseDispatch2 *risse_obj)
{
	/*%
		@fn		OSFS.OSFS
		@brief	OSFSを構築する
		@param	basedir			このファイルシステムが参照するOS上のディレクトリ
		@param	checkcase		大文字小文字が区別されないファイルシステム上で
								大文字小文字を区別するか
	*/

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	tString basedir = *param[0];
	bool checkcase = RISSE_PARAM_EXIST(1) ? (risse_int)*param[1] : true;

	// filesystem オブジェクトの生成と登録
	RegisterFileSystemNativeInstance(risse_obj, new tRisaOSFS(basedir, checkcase));

	return RISSE_S_OK;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tNI_OSFS::Invalidate()
{
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
risse_uint32 tNC_OSFS::ClassID = (risse_uint32)-1;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tNC_OSFS::tNC_OSFS() :
	tNativeClass(RISSE_WS("OSFS"))
{
	// class constructor

	RISSE_BEGIN_NATIVE_MEMBERS(/*Risse class name*/OSFS)
	RISSE_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var. name*/_this, /*var. type*/tNI_OSFS,
	/*Risse class name*/ OSFS)
{
	return RISSE_S_OK;
}
RISSE_END_NATIVE_CONSTRUCTOR_DECL(/*Risse class name*/OSFS)
//----------------------------------------------------------------------
	RISSE_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tNativeInstance *tNC_OSFS::CreateNativeInstance()
{
	return new tNI_OSFS();
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
tRisaOSFSRegisterer::tRisaOSFSRegisterer()
{
	// ファイルシステムクラスオブジェクトの下にクラスオブジェクトを登録する
	iRisseDispatch2 * cls = new tNC_OSFS();
	try
	{
		depends_on<tRisaFileSystemRegisterer>::locked_instance()->RegisterClassObject(
							RISSE_WS("OSFS"), cls);
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
