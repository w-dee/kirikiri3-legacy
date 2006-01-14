//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4FSのRisseバインディング
//---------------------------------------------------------------------------
#include "prec.h"
#include "XP4FS.h"
#include "XP4FSBind.h"
#include "RisaException.h"

RISSE_DEFINE_SOURCE_ID(2015);



//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisseNI_XP4FS::tRisseNI_XP4FS()
{
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Risse コンストラクタ
//---------------------------------------------------------------------------
risse_error tRisseNI_XP4FS::Construct(risse_int numparams,
		tRisseVariant **param, iRisseDispatch2 *risse_obj)
{
	/*%
		@fn		XP4FS.XP4FS
		@brief	XP4FSを構築する
		@param	filename		XP4アーカイブファイル名
	*/

	if(numparams < 1) return RISSE_E_BADPARAMCOUNT;

	ttstr filename = *param[0];

	// filesystem オブジェクトの生成と登録
	RegisterFileSystemNativeInstance(risse_obj, new tRisaXP4FS(filename));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		Risse 無効化関数
//---------------------------------------------------------------------------
void tRisseNI_XP4FS::Invalidate()
{
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief		XP4FS クラスID
//---------------------------------------------------------------------------
risse_uint32 tRisseNC_XP4FS::ClassID = (risse_uint32)-1;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisseNC_XP4FS::tRisseNC_XP4FS() :
	tRisseNativeClass(RISSE_WS("XP4FS"))
{
	// class constructor

	RISSE_BEGIN_NATIVE_MEMBERS(/*Risse class name*/XP4FS)
	RISSE_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
RISSE_BEGIN_NATIVE_CONSTRUCTOR_DECL(/*var. name*/_this, /*var. type*/tRisseNI_XP4FS,
	/*Risse class name*/ XP4FS)
{
	return RISSE_S_OK;
}
RISSE_END_NATIVE_CONSTRUCTOR_DECL(/*Risse class name*/XP4FS)
//----------------------------------------------------------------------
//----------------------------------------------------------------------
	RISSE_END_NATIVE_MEMBERS
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief		ネイティブインスタンスを作成して返す
//! @return		ネイティブインスタンス
//---------------------------------------------------------------------------
tRisseNativeInstance *tRisseNC_XP4FS::CreateNativeInstance()
{
	return new tRisseNI_XP4FS();
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief		コンストラクタ
//---------------------------------------------------------------------------
tRisaXP4FSRegisterer::tRisaXP4FSRegisterer()
{
	// ファイルシステムクラスオブジェクトの下にクラスオブジェクトを登録する
	iRisseDispatch2 * cls = new tRisseNC_XP4FS();
	try
	{
		tRisaFileSystemRegisterer::instance()->RegisterClassObject(
							RISSE_WS("XP4FS"), cls);
	}
	catch(...)
	{
		cls->Release();
		throw;
	}
	cls->Release();
}
//---------------------------------------------------------------------------
