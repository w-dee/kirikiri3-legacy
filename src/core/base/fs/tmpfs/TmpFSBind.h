//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief TmpFSのRisseバインディング
//---------------------------------------------------------------------------
#ifndef _TMPFSBIND_H_
#define _TMPFSBIND_H_

#if 0

namespace Risa {
//---------------------------------------------------------------------------

#include "base/fs/common/FSManagerBind.h"
#include "base/fs/tmpfs/TmpFS.h"
#include "risse/include/risseNative.h"
#include "base/utils/Singleton.h"

//---------------------------------------------------------------------------
//! @brief TmpFS ネイティブインスタンス
//---------------------------------------------------------------------------
class tNI_TmpFS : public tNI_BaseFileSystem
{
public:
	//! @brief		コンストラクタ
	tNI_TmpFS();

	//! @brief		Risse コンストラクタ
	risse_error Construct(risse_int numparams,
		tVariant **param, iRisseDispatch2 *risse_obj);

	//! @brief		Risse 無効化関数
	void Invalidate();

	tTmpFS * GetFileSystem() 
	{
		return reinterpret_cast<tTmpFS*>(
			tNI_BaseFileSystem::GetFileSystem().get());
	} //!< ファイルシステムオブジェクトを得る

private:
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief TmpFS ネイティブクラス
//---------------------------------------------------------------------------
class tNC_TmpFS : public tNativeClass
{
public:
	//! @brief		コンストラクタ
	tNC_TmpFS();

	//! @brief		TmpFS クラスID
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
class tTmpFSRegisterer :
	public singleton_base<tTmpFSRegisterer>,
	protected depends_on<tRisseScriptEngine>,
	protected depends_on<tFileSystemRegisterer>
{
public:
	//! @brief		コンストラクタ
	tTmpFSRegisterer();
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
} // namespace Risa


#endif

#endif
