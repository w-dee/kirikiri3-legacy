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

#include "base/fs/common/FSManagerBind.h"
#include "base/fs/tmpfs/TmpFS.h"
#include "risse/include/risseNative.h"
#include "base/utils/Singleton.h"

//---------------------------------------------------------------------------
//! @brief TmpFS ネイティブインスタンス
//---------------------------------------------------------------------------
class tRisseNI_TmpFS : public tRisseNI_BaseFileSystem
{
public:
	//! @brief		コンストラクタ
	tRisseNI_TmpFS();

	//! @brief		Risse コンストラクタ
	risse_error Construct(risse_int numparams,
		tRisseVariant **param, iRisseDispatch2 *risse_obj);

	//! @brief		Risse 無効化関数
	void Invalidate();

	tRisaTmpFS * GetFileSystem() 
	{
		return reinterpret_cast<tRisaTmpFS*>(
			tRisseNI_BaseFileSystem::GetFileSystem().get());
	} //!< ファイルシステムオブジェクトを得る

private:
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief TmpFS ネイティブクラス
//---------------------------------------------------------------------------
class tRisseNC_TmpFS : public tRisseNativeClass
{
public:
	//! @brief		コンストラクタ
	tRisseNC_TmpFS();

	//! @brief		TmpFS クラスID
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
class tRisaTmpFSRegisterer :
	public singleton_base<tRisaTmpFSRegisterer>,
	protected depends_on<tRisaRisseScriptEngine>,
	protected depends_on<tRisaFileSystemRegisterer>
{
public:
	//! @brief		コンストラクタ
	tRisaTmpFSRegisterer();
};
//---------------------------------------------------------------------------



#endif
