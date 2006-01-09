//---------------------------------------------------------------------------
/*
	TVP3 ( T Visual Presenter 3 )  A script authoring tool
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief PathFSのTJS3バインディング
//---------------------------------------------------------------------------
#ifndef _PATHSFSBIND_H_
#define _PATHSFSBIND_H_

#include "FSManagerBind.h"
#include "PathFS.h"
#include "tjsNative.h"

//---------------------------------------------------------------------------
//! @brief PathFS ネイティブインスタンス
//---------------------------------------------------------------------------
class tTJSNI_PathFS : public tTJSNI_BaseFileSystem
{
public:
	tTJSNI_PathFS();

	tjs_error Construct(tjs_int numparams,
		tTJSVariant **param, iTJSDispatch2 *tjs_obj);
	void Invalidate();

	tTVPPathFS * GetFileSystem() 
	{
		return reinterpret_cast<tTVPPathFS*>(
			tTJSNI_BaseFileSystem::GetFileSystem().get());
	} //!< ファイルシステムオブジェクトを得る

private:
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//! @brief PathFS ネイティブクラス
//---------------------------------------------------------------------------
class tTJSNC_PathFS : public tTJSNativeClass
{
public:
	tTJSNC_PathFS();

	static tjs_uint32 ClassID;

private:
	tTJSNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------


#endif