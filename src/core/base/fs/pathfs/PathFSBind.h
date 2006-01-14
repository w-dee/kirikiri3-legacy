//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief PathFSのRisseバインディング
//---------------------------------------------------------------------------
#ifndef _PATHSFSBIND_H_
#define _PATHSFSBIND_H_

#include "FSManagerBind.h"
#include "PathFS.h"
#include "risseNative.h"

//---------------------------------------------------------------------------
//! @brief PathFS ネイティブインスタンス
//---------------------------------------------------------------------------
class tRisseNI_PathFS : public tRisseNI_BaseFileSystem
{
public:
	tRisseNI_PathFS();

	risse_error Construct(risse_int numparams,
		tRisseVariant **param, iRisseDispatch2 *risse_obj);
	void Invalidate();

	tTVPPathFS * GetFileSystem() 
	{
		return reinterpret_cast<tTVPPathFS*>(
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
	tRisseNC_PathFS();

	static risse_uint32 ClassID;

private:
	tRisseNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief クラスレジストラ
//---------------------------------------------------------------------------
class tTVPPathFSRegisterer
{
	tTVPSingleton<tTVPRisseScriptEngine> ref_tTVPRisseScriptEngine; //!< tTVPRisseScriptEngine に依存
	tTVPSingleton<tTVPFileSystemRegisterer> ref_tTVPFileSystemRegisterer; //!< tTVPFileSystemRegisterer に依存
public:
	tTVPPathFSRegisterer();

private:
	tTVPSingletonObjectLifeTracer<tTVPPathFSRegisterer> singleton_object_life_tracer;
public:
	static boost::shared_ptr<tTVPPathFSRegisterer> & instance() { return
		tTVPSingleton<tTVPPathFSRegisterer>::instance();
			} //!< このシングルトンのインスタンスを返す
};
//---------------------------------------------------------------------------



#endif
