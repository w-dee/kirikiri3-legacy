//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000-2007 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief PathFSのRisseバインディング
//---------------------------------------------------------------------------
#ifndef _PATHSFSBIND_H_
#define _PATHSFSBIND_H_

#include "base/fs/common/FSManagerBind.h"
#include "base/fs/pathfs/PathFS.h"
#include "risse/include/risseNative.h"

//---------------------------------------------------------------------------
//! @brief PathFS ネイティブインスタンス
//---------------------------------------------------------------------------
class tRisseNI_PathFS : public tRisseNI_BaseFileSystem
{
public:
	//! @brief		コンストラクタ
	tRisseNI_PathFS();

	//! @brief		Risse コンストラクタ
	risse_error Construct(risse_int numparams,
		tRisseVariant **param, iRisseDispatch2 *risse_obj);

	//! @brief		Risse 無効化関数
	void Invalidate();

	tRisaPathFS * GetFileSystem() 
	{
		return reinterpret_cast<tRisaPathFS*>(
			tRisseNI_BaseFileSystem::GetFileSystem().get());
	} //!< ファイルシステムオブジェクトを得る

private:
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief PathFS ネイティブクラス
//---------------------------------------------------------------------------
class tRisseNC_PathFS : public tRisseNativeClass
{
public:
	//! @brief		コンストラクタ
	tRisseNC_PathFS();

	//! @brief		PathFS クラスID
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
class tRisaPathFSRegisterer :
	public singleton_base<tRisaPathFSRegisterer>,
	protected depends_on<tRisaRisseScriptEngine>,
	protected depends_on<tRisaFileSystemRegisterer>
{
public:
	//! @brief		コンストラクタ
	tRisaPathFSRegisterer();
};
//---------------------------------------------------------------------------



#endif
