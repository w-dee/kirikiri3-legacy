//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief TmpFSのTJS3バインディング
//---------------------------------------------------------------------------
#ifndef _TMPFSBIND_H_
#define _TMPFSBIND_H_

#include "FSManagerBind.h"
#include "TmpFS.h"
#include "tjsNative.h"
#include "Singleton.h"

//---------------------------------------------------------------------------
//! @brief TmpFS ネイティブインスタンス
//---------------------------------------------------------------------------
class tTJSNI_TmpFS : public tTJSNI_BaseFileSystem
{
public:
	tTJSNI_TmpFS();

	tjs_error Construct(tjs_int numparams,
		tTJSVariant **param, iTJSDispatch2 *tjs_obj);
	void Invalidate();

	tTVPTmpFS * GetFileSystem() 
	{
		return reinterpret_cast<tTVPTmpFS*>(
			tTJSNI_BaseFileSystem::GetFileSystem().get());
	} //!< ファイルシステムオブジェクトを得る

private:
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief TmpFS ネイティブクラス
//---------------------------------------------------------------------------
class tTJSNC_TmpFS : public tTJSNativeClass
{
public:
	tTJSNC_TmpFS();

	static tjs_uint32 ClassID;

private:
	tTJSNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
//! @brief クラスレジストラ
//---------------------------------------------------------------------------
class tTVPTmpFSRegisterer
{
	tTVPSingleton<tTVPTJS3ScriptEngine> ref_tTVPTJS3ScriptEngine; //!< tTVPTJS3ScriptEngine に依存
	tTVPSingleton<tTVPFileSystemRegisterer> ref_tTVPFileSystemRegisterer; //!< tTVPFileSystemRegisterer に依存
public:
	tTVPTmpFSRegisterer();

	static boost::shared_ptr<tTVPTmpFSRegisterer> & instance() { return
		tTVPSingleton<tTVPTmpFSRegisterer>::instance();
			} //!< このシングルトンのインスタンスを返す
};
//---------------------------------------------------------------------------



#endif
