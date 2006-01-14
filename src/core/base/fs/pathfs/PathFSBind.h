//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
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
	tRisseNC_PathFS();

	static risse_uint32 ClassID;

private:
	tRisseNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
//! @brief クラスレジストラ
//---------------------------------------------------------------------------
class tRisaPathFSRegisterer
{
	tRisaSingleton<tRisaRisseScriptEngine> ref_tRisaRisseScriptEngine; //!< tRisaRisseScriptEngine に依存
	tRisaSingleton<tRisaFileSystemRegisterer> ref_tRisaFileSystemRegisterer; //!< tRisaFileSystemRegisterer に依存
public:
	tRisaPathFSRegisterer();

private:
	tRisaSingletonObjectLifeTracer<tRisaPathFSRegisterer> singleton_object_life_tracer;
public:
	static boost::shared_ptr<tRisaPathFSRegisterer> & instance() { return
		tRisaSingleton<tRisaPathFSRegisterer>::instance();
			} //!< このシングルトンのインスタンスを返す
};
//---------------------------------------------------------------------------



#endif
