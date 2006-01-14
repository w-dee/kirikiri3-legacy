//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OSFSのRisseバインディング
//---------------------------------------------------------------------------
#ifndef _OSFSBIND_H_
#define _OSFSBIND_H_

#include "FSManagerBind.h"
#include "OSFS.h"
#include "risseNative.h"
#include "Singleton.h"

//---------------------------------------------------------------------------
//! @brief OSFS ネイティブインスタンス
//---------------------------------------------------------------------------
class tRisseNI_OSFS : public tRisseNI_BaseFileSystem
{
public:
	tRisseNI_OSFS();

	risse_error Construct(risse_int numparams,
		tRisseVariant **param, iRisseDispatch2 *risse_obj);
	void Invalidate();

	tTVPOSFS * GetFileSystem() 
	{
		return reinterpret_cast<tTVPOSFS*>(
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
	tRisseNC_OSFS();

	static risse_uint32 ClassID;

private:
	tRisseNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief クラスレジストラ
//---------------------------------------------------------------------------
class tTVPOSFSRegisterer
{
	tTVPSingleton<tTVPRisseScriptEngine> ref_tTVPRisseScriptEngine; //!< tTVPRisseScriptEngine に依存
	tTVPSingleton<tTVPFileSystemRegisterer> ref_tTVPFileSystemRegisterer; //!< tTVPFileSystemRegisterer に依存
public:
	tTVPOSFSRegisterer();

private:
	tTVPSingletonObjectLifeTracer<tTVPOSFSRegisterer> singleton_object_life_tracer;
public:
	static boost::shared_ptr<tTVPOSFSRegisterer> & instance() { return
		tTVPSingleton<tTVPOSFSRegisterer>::instance();
			} //!< このシングルトンのインスタンスを返す
};
//---------------------------------------------------------------------------


#endif
