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
#ifndef _OSFSBIND_H_
#define _OSFSBIND_H_

#include "base/fs/common/FSManagerBind.h"
#include "base/fs/osfs/OSFS.h"
#include "risse/include/risseNative.h"
#include "base/utils/Singleton.h"

//---------------------------------------------------------------------------
//! @brief OSFS ネイティブインスタンス
//---------------------------------------------------------------------------
class tRisseNI_OSFS : public tRisseNI_BaseFileSystem
{
public:
	//! @brief		コンストラクタ
	tRisseNI_OSFS();

	//! @brief		Risse コンストラクタ
	risse_error Construct(risse_int numparams,
		tRisseVariant **param, iRisseDispatch2 *risse_obj);

	//! @brief		Risse 無効化関数
	void Invalidate();

	tRisaOSFS * GetFileSystem() 
	{
		return reinterpret_cast<tRisaOSFS*>(
			tRisseNI_BaseFileSystem::GetFileSystem().get());
	} //!< ファイルシステムオブジェクトを得る

private:
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief OSFS ネイティブクラス
//---------------------------------------------------------------------------
class tRisseNC_OSFS : public tRisseNativeClass
{
public:
	//! @brief		コンストラクタ
	tRisseNC_OSFS();

	//! @brief		OSFS クラスID
	static risse_uint32 ClassID;

private:
	//! @brief		ネイティブインスタンスを作成して返す
	//! @return		ネイティブインスタンス
	tRisseNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief クラスレジストラ
//---------------------------------------------------------------------------
class tRisaOSFSRegisterer :
	public singleton_base<tRisaOSFSRegisterer>,
	protected depends_on<tRisaRisseScriptEngine>,
	protected depends_on<tRisaFileSystemRegisterer>
{
public:
	//! @brief		コンストラクタ
	tRisaOSFSRegisterer();
};
//---------------------------------------------------------------------------


#endif
