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

namespace Risa {
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief XP4FS ネイティブインスタンス
//---------------------------------------------------------------------------
class tNI_XP4FS : public tNI_BaseFileSystem
{
public:
	//! @brief		コンストラクタ
	tNI_XP4FS();

	//! @brief		Risse コンストラクタ
	risse_error Construct(risse_int numparams,
		tVariant **param, iRisseDispatch2 *risse_obj);

	//! @brief		Risse 無効化関数
	void Invalidate();

	tXP4FS * GetFileSystem() 
	{
		return reinterpret_cast<tXP4FS*>(
			tNI_BaseFileSystem::GetFileSystem().get());
	} //!< ファイルシステムオブジェクトを得る

private:
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief XP4FS ネイティブクラス
//---------------------------------------------------------------------------
class tNC_XP4FS : public tNativeClass
{
public:
	//! @brief		コンストラクタ
	tNC_XP4FS();

	//! @brief		XP4FS クラスID
	static risse_uint32 ClassID;

private:
	//! @brief		ネイティブインスタンスを作成して返す
	//! @return		ネイティブインスタンス
	tNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief クラスレジストラ
//---------------------------------------------------------------------------
class tXP4FSRegisterer :
	public singleton_base<tXP4FSRegisterer>,
	protected depends_on<tRisseScriptEngine>,
	protected depends_on<tFileSystemRegisterer>
{
public:
	//! @brief		コンストラクタ
	tXP4FSRegisterer();
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif

#endif
