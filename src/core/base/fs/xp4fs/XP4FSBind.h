//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief XP4FSのTJS3バインディング
//---------------------------------------------------------------------------
#ifndef _XP4FSBIND_H_
#define _XP4FSBIND_H_

#include "FSManagerBind.h"
#include "XP4FS.h"
#include "tjsNative.h"
#include "Singleton.h"

//---------------------------------------------------------------------------
//! @brief XP4FS ネイティブインスタンス
//---------------------------------------------------------------------------
class tTJSNI_XP4FS : public tTJSNI_BaseFileSystem
{
public:
	tTJSNI_XP4FS();

	tjs_error Construct(tjs_int numparams,
		tTJSVariant **param, iTJSDispatch2 *tjs_obj);
	void Invalidate();

	tTVPXP4FS * GetFileSystem() 
	{
		return reinterpret_cast<tTVPXP4FS*>(
			tTJSNI_BaseFileSystem::GetFileSystem().get());
	} //!< ファイルシステムオブジェクトを得る

private:
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief XP4FS ネイティブクラス
//---------------------------------------------------------------------------
class tTJSNC_XP4FS : public tTJSNativeClass
{
public:
	tTJSNC_XP4FS();

	static tjs_uint32 ClassID;

private:
	tTJSNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
//! @brief クラスレジストラ
//---------------------------------------------------------------------------
class tTVPXP4FSRegisterer
{
	tTVPSingleton<tTVPTJS3ScriptEngine> ref_tTVPTJS3ScriptEngine; //!< tTVPTJS3ScriptEngine に依存
	tTVPSingleton<tTVPFileSystemRegisterer> ref_tTVPFileSystemRegisterer; //!< tTVPFileSystemRegisterer に依存
public:
	tTVPXP4FSRegisterer();

	static boost::shared_ptr<tTVPXP4FSRegisterer> & instance() { return
		tTVPSingleton<tTVPXP4FSRegisterer>::instance();
			} //!< このシングルトンのインスタンスを返す
};
//---------------------------------------------------------------------------



#endif
