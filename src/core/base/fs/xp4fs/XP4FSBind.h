//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
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

	tRisaXP4FS * GetFileSystem() 
	{
		return reinterpret_cast<tRisaXP4FS*>(
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
class tRisaXP4FSRegisterer
{
	tRisaSingleton<tRisaRisseScriptEngine> ref_tRisaRisseScriptEngine; //!< tRisaRisseScriptEngine に依存
	tRisaSingleton<tRisaFileSystemRegisterer> ref_tRisaFileSystemRegisterer; //!< tRisaFileSystemRegisterer に依存
public:
	tRisaXP4FSRegisterer();

private:
	tRisaSingletonObjectLifeTracer<tRisaXP4FSRegisterer> singleton_object_life_tracer;
public:
	static boost::shared_ptr<tRisaXP4FSRegisterer> & instance() { return
		tRisaSingleton<tRisaXP4FSRegisterer>::instance();
			} //!< このシングルトンのインスタンスを返す
};
//---------------------------------------------------------------------------



#endif
