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

#if 0
#include "base/fs/common/FSManagerBind.h"
#include "base/fs/osfs/OSFS.h"
#include "risse/include/risseNative.h"
#include "base/utils/Singleton.h"

namespace Risa {
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! @brief OSFS ネイティブインスタンス
//---------------------------------------------------------------------------
class tNI_OSFS : public tNI_BaseFileSystem
{
public:
	//! @brief		コンストラクタ
	tNI_OSFS();

	//! @brief		Risse コンストラクタ
	risse_error Construct(risse_int numparams,
		tVariant **param, iRisseDispatch2 *risse_obj);

	//! @brief		Risse 無効化関数
	void Invalidate();

	tOSFS * GetFileSystem() 
	{
		return reinterpret_cast<tOSFS*>(
			tNI_BaseFileSystem::GetFileSystem().get());
	} //!< ファイルシステムオブジェクトを得る

private:
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief OSFS ネイティブクラス
//---------------------------------------------------------------------------
class tNC_OSFS : public tNativeClass
{
public:
	//! @brief		コンストラクタ
	tNC_OSFS();

	//! @brief		OSFS クラスID
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
class tOSFSRegisterer :
	public singleton_base<tOSFSRegisterer>,
	protected depends_on<tRisseScriptEngine>,
	protected depends_on<tFileSystemRegisterer>
{
public:
	//! @brief		コンストラクタ
	tOSFSRegisterer();
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif

#endif
