//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief TmpFSのRisseバインディング
//---------------------------------------------------------------------------
#ifndef _TMPFSBIND_H_
#define _TMPFSBIND_H_

#include "FSManagerBind.h"
#include "TmpFS.h"
#include "risseNative.h"
#include "Singleton.h"

//---------------------------------------------------------------------------
//! @brief TmpFS ネイティブインスタンス
//---------------------------------------------------------------------------
class tRisseNI_TmpFS : public tRisseNI_BaseFileSystem
{
public:
	tRisseNI_TmpFS();

	risse_error Construct(risse_int numparams,
		tRisseVariant **param, iRisseDispatch2 *risse_obj);
	void Invalidate();

	tTVPTmpFS * GetFileSystem() 
	{
		return reinterpret_cast<tTVPTmpFS*>(
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
	tRisseNC_TmpFS();

	static risse_uint32 ClassID;

private:
	tRisseNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief クラスレジストラ
//---------------------------------------------------------------------------
class tTVPTmpFSRegisterer
{
	tTVPSingleton<tTVPRisseScriptEngine> ref_tTVPRisseScriptEngine; //!< tTVPRisseScriptEngine に依存
	tTVPSingleton<tTVPFileSystemRegisterer> ref_tTVPFileSystemRegisterer; //!< tTVPFileSystemRegisterer に依存
public:
	tTVPTmpFSRegisterer();

private:
	tTVPSingletonObjectLifeTracer<tTVPTmpFSRegisterer> singleton_object_life_tracer;
public:
	static boost::shared_ptr<tTVPTmpFSRegisterer> & instance() { return
		tTVPSingleton<tTVPTmpFSRegisterer>::instance();
			} //!< このシングルトンのインスタンスを返す
};
//---------------------------------------------------------------------------



#endif
