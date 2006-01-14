//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4FSのRisseバインディング
//---------------------------------------------------------------------------
#ifndef _XP4FSBIND_H_
#define _XP4FSBIND_H_

#include "FSManagerBind.h"
#include "XP4FS.h"
#include "risseNative.h"
#include "Singleton.h"

//---------------------------------------------------------------------------
//! @brief XP4FS ネイティブインスタンス
//---------------------------------------------------------------------------
class tRisseNI_XP4FS : public tRisseNI_BaseFileSystem
{
public:
	tRisseNI_XP4FS();

	risse_error Construct(risse_int numparams,
		tRisseVariant **param, iRisseDispatch2 *risse_obj);
	void Invalidate();

	tTVPXP4FS * GetFileSystem() 
	{
		return reinterpret_cast<tTVPXP4FS*>(
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
	tRisseNC_XP4FS();

	static risse_uint32 ClassID;

private:
	tRisseNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief クラスレジストラ
//---------------------------------------------------------------------------
class tTVPXP4FSRegisterer
{
	tTVPSingleton<tTVPRisseScriptEngine> ref_tTVPRisseScriptEngine; //!< tTVPRisseScriptEngine に依存
	tTVPSingleton<tTVPFileSystemRegisterer> ref_tTVPFileSystemRegisterer; //!< tTVPFileSystemRegisterer に依存
public:
	tTVPXP4FSRegisterer();

private:
	tTVPSingletonObjectLifeTracer<tTVPXP4FSRegisterer> singleton_object_life_tracer;
public:
	static boost::shared_ptr<tTVPXP4FSRegisterer> & instance() { return
		tTVPSingleton<tTVPXP4FSRegisterer>::instance();
			} //!< このシングルトンのインスタンスを返す
};
//---------------------------------------------------------------------------



#endif
