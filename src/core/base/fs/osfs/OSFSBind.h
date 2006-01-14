//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
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

	tRisaOSFS * GetFileSystem() 
	{
		return reinterpret_cast<tRisaOSFS*>(
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
class tRisaOSFSRegisterer
{
	tRisaSingleton<tRisaRisseScriptEngine> ref_tRisaRisseScriptEngine; //!< tRisaRisseScriptEngine に依存
	tRisaSingleton<tRisaFileSystemRegisterer> ref_tRisaFileSystemRegisterer; //!< tRisaFileSystemRegisterer に依存
public:
	tRisaOSFSRegisterer();

private:
	tRisaSingletonObjectLifeTracer<tRisaOSFSRegisterer> singleton_object_life_tracer;
public:
	static boost::shared_ptr<tRisaOSFSRegisterer> & instance() { return
		tRisaSingleton<tRisaOSFSRegisterer>::instance();
			} //!< このシングルトンのインスタンスを返す
};
//---------------------------------------------------------------------------


#endif
