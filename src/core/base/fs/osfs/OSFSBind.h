//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief OSFSのTJS3バインディング
//---------------------------------------------------------------------------
#ifndef _OSFSBIND_H_
#define _OSFSBIND_H_

#include "FSManagerBind.h"
#include "OSFS.h"
#include "tjsNative.h"
#include "Singleton.h"

//---------------------------------------------------------------------------
//! @brief OSFS ネイティブインスタンス
//---------------------------------------------------------------------------
class tTJSNI_OSFS : public tTJSNI_BaseFileSystem
{
public:
	tTJSNI_OSFS();

	tjs_error Construct(tjs_int numparams,
		tTJSVariant **param, iTJSDispatch2 *tjs_obj);
	void Invalidate();

	tTVPOSFS * GetFileSystem() 
	{
		return reinterpret_cast<tTVPOSFS*>(
			tTJSNI_BaseFileSystem::GetFileSystem().get());
	} //!< ファイルシステムオブジェクトを得る

private:
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief OSFS ネイティブクラス
//---------------------------------------------------------------------------
class tTJSNC_OSFS : public tTJSNativeClass
{
public:
	tTJSNC_OSFS();

	static tjs_uint32 ClassID;

private:
	tTJSNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
//! @brief クラスレジストラ
//---------------------------------------------------------------------------
class tTVPOSFSRegisterer
{
	tTVPSingleton<tTVPTJS3ScriptEngine> ref_tTVPTJS3ScriptEngine; //!< tTVPTJS3ScriptEngine に依存
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
