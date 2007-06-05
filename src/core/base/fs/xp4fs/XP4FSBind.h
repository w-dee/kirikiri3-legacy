//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4FSのRisseバインディング
//---------------------------------------------------------------------------
#ifndef _XP4FSBIND_H_
#define _XP4FSBIND_H_

#if 0

#include "base/fs/common/FSManagerBind.h"
#include "base/fs/xp4fs/XP4FS.h"
#include "risse/include/risseNative.h"
#include "base/utils/Singleton.h"

//---------------------------------------------------------------------------
//! @brief XP4FS ネイティブインスタンス
//---------------------------------------------------------------------------
class tRisseNI_XP4FS : public tRisseNI_BaseFileSystem
{
public:
	//! @brief		コンストラクタ
	tRisseNI_XP4FS();

	//! @brief		Risse コンストラクタ
	risse_error Construct(risse_int numparams,
		tRisseVariant **param, iRisseDispatch2 *risse_obj);

	//! @brief		Risse 無効化関数
	void Invalidate();

	tRisaXP4FS * GetFileSystem() 
	{
		return reinterpret_cast<tRisaXP4FS*>(
			tRisseNI_BaseFileSystem::GetFileSystem().get());
	} //!< ファイルシステムオブジェクトを得る

private:
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief XP4FS ネイティブクラス
//---------------------------------------------------------------------------
class tRisseNC_XP4FS : public tRisseNativeClass
{
public:
	//! @brief		コンストラクタ
	tRisseNC_XP4FS();

	//! @brief		XP4FS クラスID
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
class tRisaXP4FSRegisterer :
	public singleton_base<tRisaXP4FSRegisterer>,
	protected depends_on<tRisaRisseScriptEngine>,
	protected depends_on<tRisaFileSystemRegisterer>
{
public:
	//! @brief		コンストラクタ
	tRisaXP4FSRegisterer();
};
//---------------------------------------------------------------------------

#endif

#endif
